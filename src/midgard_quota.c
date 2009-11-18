/* 
 * Copyright (C) 2005 Piotr Pokora <piotr.pokora@infoglob.pl>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <midgard_quota.h>
#include <midgard_error.h>
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include "midgard_metadata.h"
#include "midgard_user.h"

gboolean midgard_quota_size_is_reached(MgdObject *object, gint size)
{
	GString *query;
	guint limit_tmp_size = 0, tmp_size = 0;	
	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	guint rows = 0;
	const GValue *tmp_size_value;
	const GValue *limit_tmp_size_value;	

	/* Check global quota */
	gchar *qstr = "SELECT limit_sg_size, sg_size FROM quota WHERE typename = '' AND limit_sg_size > 0 ";

	GdaDataModel *model = 
		midgard_core_query_get_model(mgd, qstr);

	/* Sitegroup limit is not set */
	if(!model)
		return FALSE;
	
	limit_tmp_size_value = midgard_data_model_get_value_at(model, 0, 0); 
	MIDGARD_GET_UINT_FROM_VALUE(limit_tmp_size, limit_tmp_size_value);

	tmp_size_value = midgard_data_model_get_value_at(model, 0, 1); 
	MIDGARD_GET_UINT_FROM_VALUE(tmp_size, tmp_size_value);

	g_object_unref(model);

	if(((tmp_size + size) > limit_tmp_size) && limit_tmp_size > 0){
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_QUOTA);
		return TRUE;
	}
	
	/* Check quota for class size */
	query = g_string_new("SELECT limit_type_size, type_size FROM quota ");
	g_string_append_printf(query,
			"WHERE typename = '%s' AND limit_type_size > 0 ", G_OBJECT_TYPE_NAME(object));

	model = midgard_core_query_get_model(mgd, query->str);
	rows = gda_data_model_get_n_rows(model);
	g_string_free(query, TRUE);
	
	/* Class limit is not set */
	if(rows == 0) {
		g_object_unref(model);
		return FALSE;
	}
	
	limit_tmp_size_value = midgard_data_model_get_value_at(model, 0, 0); 
	MIDGARD_GET_UINT_FROM_VALUE(limit_tmp_size, limit_tmp_size_value);

	tmp_size_value = midgard_data_model_get_value_at(model, 0, 1); 
	MIDGARD_GET_UINT_FROM_VALUE(tmp_size, tmp_size_value);

	g_object_unref(model);

	if(((tmp_size + size) > limit_tmp_size) && limit_tmp_size > 0){
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_QUOTA);
		return TRUE;
	}

	return FALSE;
}


/*
 * Returns object's disk usage size 
 */ 
guint midgard_quota_get_object_size(MgdObject *object)
{
	g_assert(object != NULL);

	const gchar *tablename = 
		midgard_core_class_get_table(MIDGARD_DBOBJECT_GET_CLASS(object));
	
	gchar *where = 
		midgard_core_query_where_guid(tablename, MGD_OBJECT_GUID (object));
	
	GValue *value = midgard_core_query_get_field_value(
			MGD_OBJECT_CNC (object),
			"metadata_size", 
			tablename, 
			where);
	
	g_free(where);

	guint size = 0;

	if(G_VALUE_HOLDS_UINT(value))
		size = g_value_get_uint(value);

	if(G_VALUE_HOLDS_INT(value))
		size = (guint) g_value_get_int(value);

	g_value_unset(value);
	g_free(value);

	return size;
}

/*
 * Updates object's storage setting object's size
 * WARNING, this is MySQL optimized!	
 */
gboolean midgard_quota_update(MgdObject *object, guint32 init_size)
{
	g_assert(object != NULL);
	g_assert(MGD_OBJECT_CNC (object) != NULL);

	guint32 size = 0;
	guint32 qsize = 0;
	const gchar *typename = G_OBJECT_TYPE_NAME(object);	
	gchar *tmpstr;

	if (!object->metadata)
		return TRUE;

	size = object->metadata->priv->size;
	
	/* nothing to update, this should never happen */
	if(size == 0) return TRUE;

	/* Get difference */
	/* difference is important to allow multiple updates without wrong size 
	 * diff_size may be less then , equal or greater than 0 */
	qsize = init_size;
	gint diff_size = size - qsize;	

	if(midgard_quota_size_is_reached(object, diff_size)){
		MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (object), MGD_ERR_QUOTA);
		return FALSE;
	}

	/* diff size is 0 , so no need update record without changing its value */
	if(diff_size == 0) 
		return TRUE;

	/* Update type size */
	GString *quota_query;
	quota_query = g_string_new("UPDATE quota ");
	g_string_append_printf(quota_query, 
			"SET type_size=type_size+%d WHERE typename='%s' ", 
			diff_size, typename);
        tmpstr = g_string_free(quota_query, FALSE);
	midgard_core_query_execute(MGD_OBJECT_CNC (object), tmpstr, TRUE);
	g_free(tmpstr);
	
	return TRUE;
}

gboolean midgard_quota_create(MgdObject *object)
{
	g_assert(object != NULL);

	GString *query;		
	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	gchar *tmpstr;

	/* Bit of hack, but there's no hook to store object's size */
	if (!object->metadata)
		return TRUE;

	guint32 size = object->metadata->priv->size;
	if(midgard_quota_size_is_reached(object, size)){
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_QUOTA);
		return FALSE;
	}

	/* Update type's records */
	query = g_string_new("UPDATE quota ");
	g_string_append_printf(query,
			"SET type_records=type_records+1"
			" WHERE typename='%s' ", G_OBJECT_TYPE_NAME(object)); 

	tmpstr = g_string_free(query, FALSE);
	midgard_core_query_execute(MGD_OBJECT_CNC (object), tmpstr, TRUE);
	g_free(tmpstr);
	
	return TRUE;
}

void midgard_quota_remove(MgdObject *object, guint size){

	g_assert(object != NULL);

	GString *query;
	gchar *tmpstr;
	const gchar *typename = G_OBJECT_TYPE_NAME(object);

	/* Update type's records */
	query = g_string_new("UPDATE quota ");
	g_string_append_printf(query,
			"SET type_records=type_records-1"
			" WHERE typename='%s' AND type_records>0",
			typename);
	tmpstr = g_string_free(query, FALSE);
	midgard_core_query_execute(MGD_OBJECT_CNC (object), tmpstr, TRUE);
	g_free(tmpstr);

	query = g_string_new("UPDATE quota ");
	g_string_append_printf(query,
			"SET type_size=type_size-%d "
			"WHERE typename='%s' AND type_size>0",
			size, typename);
	
	tmpstr = g_string_free(query, FALSE);
	midgard_core_query_execute(MGD_OBJECT_CNC (object), tmpstr, TRUE);
	g_free(tmpstr);
}

