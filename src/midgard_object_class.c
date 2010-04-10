/* 
 * Copyright (C) 2005, 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_object_class.h"
#include "midgard_object.h"
#include "midgard_error.h"
#include "schema.h"
#include <libgda/libgda.h>
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_user.h"
#include "guid.h"
#include "midgard_core_object_class.h"

#define _GET_TYPE_ATTR(__klass) \
	MgdSchemaTypeAttr *type_attr = \
		midgard_core_class_get_type_attr(MIDGARD_DBOBJECT_CLASS(__klass))

const gchar *midgard_object_class_get_primary_property(
		MidgardObjectClass *klass)
{	
	g_assert(klass != NULL);
	
	_GET_TYPE_ATTR(klass);
	return type_attr->primary;
} 


/**
 * midgard_object_class_get_property_parent:
 * @klass: #MidgardObjectClass
 *
 * Returned string is a pointer to object's parent property, and you should not 
 * free it. Parent property is a property which holds value of field which points to 
 * parent object's identifier. Parent property always points to object's 
 * identifier which is not the same type as given MidgardObjectClass klass.
 *
 * This is static method.
 * NULL is returned if parent property is not found.
 *
 * Returns: the name of object's parent property or %NULL.
 */
const gchar *midgard_object_class_get_property_parent(
		MidgardObjectClass *klass)
{
	g_assert(klass != NULL);

	_GET_TYPE_ATTR(klass);
	return type_attr->property_parent;
}


/**
 * midgard_object_class_get_property_up:
 * @klass: #MidgardObjectClass
 * 
 * Returned string is a pointer to object's up property.
 * Up property is a property which holds value of field which points to 
 * "upper" object's identifier. Up property always points to object's
 * identifier which is the same type as given MidgardObjectClass klass.
 *
 * This is static method.
 *
 * NULL is returned if up property is not registered for given class.
 *
 * Returns: the name of object's up property or %NULL.
 */
const gchar *midgard_object_class_get_property_up(
		MidgardObjectClass *klass)
{	
	g_assert(klass != NULL);
	
	_GET_TYPE_ATTR(klass);
	return type_attr->property_up;
}

/**
 * midgard_object_class_get_property_unique:
 * @klass: #MidgardObjectClass
 * 
 * Returns: name of the property or %NULL
 */
const gchar *midgard_object_class_get_property_unique(MidgardObjectClass *klass)
{
	g_assert(klass != NULL);

	_GET_TYPE_ATTR(klass);
	return type_attr->unique_name;
}

/**
 * midgard_object_class_list_children:
 * @klass: MidgardObjectClass
 *
 * Returns newly allocated and NULL terminated children ( in midgard tree ) classes' pointers . 
 * Returned array should be freed if no longer needed without freeing array's elements. 
 * Elements are static pointers owned by GLib.
 *
 * This is static method.
 *
 * Returns: array of childreen ( in midgard tree ) classes of the given MidgardObjectClass.
 */
MidgardObjectClass **midgard_object_class_list_children(
		        MidgardObjectClass *klass)
{
	g_assert(klass != NULL);

	_GET_TYPE_ATTR(klass);
	if(!type_attr->children)
		return NULL;

	GSList *list;	
	GSList *children = type_attr->children;
	guint i = 0;
	MidgardObjectClass **children_class = 
		g_new(MidgardObjectClass *, g_slist_length(children)+1);

	for(list = children; list != NULL; list = list->next){

		children_class[i] = MIDGARD_OBJECT_GET_CLASS_BY_NAME((const gchar *) list->data);
		i++;
	}

	children_class[i] = NULL;

	return children_class;
}

/**
 * midgard_object_class_get_object_by_guid:
 * @mgd: #MidgardConnection handler 
 * @guid: guid string which should identify an object
 *
 * #MidgardError set by this function:
 *   - MGD_ERR_NOT_EXISTS: object is not found
 *   - MGD_ERR_OBJECT_DELETED: object is already deleted
 *   - MGD_ERR_OBJECT_PURGED: object is purged
 *
 * This is static method.
 *
 * Returns: #MgdObject object identified by given @guid
 */
MgdObject *midgard_object_class_get_object_by_guid (	MidgardConnection *mgd,
							const gchar *guid)
{
	g_assert(mgd != NULL);
	g_assert(guid);

	if(guid == NULL) {
		g_warning("Can not get object by guid = NULL");
		return NULL;
	}

	MgdObject *object;
	
	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);
	GString *sql = g_string_new("SELECT ");
	g_string_append_printf(sql,
			"typename, object_action FROM repligard "
			"WHERE guid = '%s' ",
			guid);
	
	gchar *query = g_string_free(sql, FALSE);
	GdaDataModel *model =
		midgard_core_query_get_model(mgd, query);
	g_free(query);
	GValue pval = {0, };

	if(!model) {

		MIDGARD_ERRNO_SET(mgd, MGD_ERR_NOT_EXISTS);
		return NULL;
	}

	gint rows = gda_data_model_get_n_rows(model);

	if(rows == 0) {

		g_object_unref (model);
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_NOT_EXISTS);
		return NULL;
	}

	const GValue *action_value =
		gda_data_model_get_value_at_col_name(model, "object_action", 0);
	
	const GValue *type_value =
		gda_data_model_get_value_at_col_name(model, "typename", 0);

	if (action_value == NULL 
			|| type_value == NULL ) {

		g_warning("NULL value returned from correctly allocated data model.");
		return NULL;
	}

	if (!type_value 
			|| (type_value && !G_VALUE_HOLDS_STRING(type_value))) {

		g_warning("Database inconsistency!. Expected classname string in repligard typename (%s)", guid);
		return NULL;
	}

	guint aval = 0;
	MIDGARD_GET_UINT_FROM_VALUE(aval, action_value);

	/* workaround for incorrect repligard.object_action which holds varchar type in mjolnir 9.09.0 */
	/* TODO: Remove in Ratatoskr+1 */
	if (G_VALUE_HOLDS_STRING (action_value)) {

		const gchar *strval = g_value_get_string (action_value);
		if (strval)
			aval = atoi (strval);
	}

	switch(aval) {
		
		case MGD_OBJECT_ACTION_DELETE:
			MIDGARD_ERRNO_SET(mgd, MGD_ERR_OBJECT_DELETED);
			g_object_unref(model);
			return NULL;
			break;

		case MGD_OBJECT_ACTION_PURGE:
			MIDGARD_ERRNO_SET(mgd, MGD_ERR_OBJECT_PURGED);
			g_object_unref(model);
			return NULL;
			break;

		default:
			g_value_init(&pval, G_TYPE_STRING);
			g_value_set_string(&pval, guid);
			object = midgard_object_new(mgd, 
					g_value_get_string((GValue*)type_value),
					&pval);
			g_value_unset(&pval);
			g_object_unref(model);
			if(object){
				return object;
			} else {
				MIDGARD_ERRNO_SET(mgd, MGD_ERR_NOT_EXISTS);
				return NULL;
			}
			break;

	}
	return NULL;

}

static void __get_parentclass_name(const gchar *classname, GList **list)
{
	MidgardObjectClass *klass = 
		MIDGARD_OBJECT_GET_CLASS_BY_NAME(classname);

	_GET_TYPE_ATTR(klass);
	const gchar *pname = type_attr->parent;

	if(pname != NULL) {
	
		*list = g_list_prepend(*list, (gpointer) pname);
		__get_parentclass_name(pname, list);
	}

	return;
}

static gboolean __get_id_from_path_element(MidgardConnection *mgd,
		const gchar *table, const gchar *name, const gchar *field, 
		guint val, MidgardObjectClass *klass, guint *rval)
{	
	const gchar *unique_name = midgard_object_class_get_property_unique(klass);

	GString *where = g_string_new("");
	g_string_append_printf(where, 
			"%s = '%s' AND %s = %d AND metadata_deleted = 0 ",
			unique_name, name, field, val);

	GValue *value = 
		midgard_core_query_get_field_value(mgd, "id", table, where->str);

	g_string_free(where, TRUE);

	if(!value)
		return FALSE;

	guint _id = 0;

	if(G_VALUE_HOLDS_UINT(value))
		_id = g_value_get_uint(value);
	if(G_VALUE_HOLDS_INT(value))
		_id = g_value_get_int(value);

	g_value_unset(value);
	g_free(value);

	*rval = _id;

	return TRUE;
}

/** 
 * midgard_object_class_get_object_by_path:
 * @mgd: #MidgardConnection holder
 * @classname: name of the class for which object should be returned
 * @object_path: a path, at which object should be found
 *
 * Empty name is accepted for @object_path. For example, if you need to get by path 
 * object with empty name use "/" as path. If one of object in path has empty name, 
 * the full path could look like this: "/A/B//D".
 *
 * This is static method.
 *
 * Cases to set NULL:
 *  - object is not found by path
 *  - object's class has neither name nor id member
 *  - object's class doesn't have both: parent and up property
 *
 * MidgardError set by this method:
 *  - MGD_ERR_INTERNAL 
 *  - MGD_ERR_NOT_EXISTS
 * 
 * Returns: #MgdObject instance of @classname or %NULL if object is not found
 */
MgdObject *midgard_object_class_get_object_by_path(MidgardConnection *mgd,
		const gchar *classname, const gchar *object_path)
{
	g_assert(mgd != NULL);
	g_assert(classname != NULL);
	g_assert(object_path != NULL);

	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	gchar *path = NULL;

	if(g_str_has_prefix(object_path, "//")){
		g_warning("Empty element in object's path '%s'", object_path);
	}    
	    
	if(g_str_has_prefix(object_path, "/")){
		
		path = g_strdup(object_path); 
	
	} else  {
		
		path = g_strconcat("/", object_path, NULL);
	}

	/* Add classname to list */
	GList *plist = NULL;
	plist = g_list_prepend(plist, NULL);
	plist = g_list_prepend(plist, (gpointer) classname);

	/* "Walk" tree till NULL parent found */
	__get_parentclass_name(classname, &plist);

	gchar **pelts = g_strsplit(path, "/", 0);
	g_free(path);
	
	guint i = 1;	
	const gchar *tablename;
	gchar *_cname;
	MidgardObjectClass *klass;
	const gchar *property_up, *property_parent;
	guint oid, _oid;
	gboolean id_exists = FALSE;

	do {
		if(i == 1)
			oid = 0;
		else 
			oid = _oid;

		if(plist->data == NULL) {			
			g_strfreev(pelts);
			g_list_free(plist);
			MIDGARD_ERRNO_SET(mgd, MGD_ERR_NOT_EXISTS);
			return NULL;
		}

		_cname = (gchar *)plist->data;	
		klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(_cname);
		tablename = midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(klass));
		property_parent = midgard_object_class_get_property_parent(klass);
		property_up = midgard_object_class_get_property_up(klass);
		
		if(!property_parent && !property_up) {
			
			g_strfreev(pelts);
			g_list_free(plist);
			g_warning("%s doesn't support tree functionality", _cname);
			MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);
			return NULL;
		}

		GParamSpec *namespec, *idspec;
		namespec = g_object_class_find_property(G_OBJECT_CLASS(klass), "name");
		idspec = g_object_class_find_property(G_OBJECT_CLASS(klass), "id");

		if(!namespec || !idspec) {
			
			 g_strfreev(pelts);
			 g_list_free(plist);
			 g_warning("%s class has no 'name' or 'id' member registered", _cname);
			 MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);
			 return NULL;
		}

		if(property_parent) {
			
			id_exists = __get_id_from_path_element(mgd,
					tablename, (const gchar *)pelts[i], 
					property_parent, oid, klass, &_oid);
		
			if(id_exists)
				goto _GET_NEXT_PATH_ELEMENT;
		}

		/* Try to get object using up property */
		if(property_up) {
			
			id_exists = __get_id_from_path_element(mgd,
					tablename, (const gchar *)pelts[i],
					property_up, oid, klass, &_oid);
		
			if(id_exists > 0)
				goto _GET_NEXT_PATH_ELEMENT;
		}

		if(!id_exists) {
			
			plist = plist->next;
			continue;
		}

_GET_NEXT_PATH_ELEMENT:

		i++;

		if(pelts[i] == NULL) {
			
			GValue idval = {0, };
			g_value_init(&idval, G_TYPE_UINT);
			g_value_set_uint(&idval, _oid);

			MgdObject *object = midgard_object_new(mgd, _cname, &idval);
			
			g_value_unset(&idval);
			g_list_free(plist);
			g_strfreev(pelts);

			return object;
		}

	} while(pelts[i] != NULL);

	return NULL;
}

/**
 * midgard_object_class_undelete:
 * @mgd: MidgardConnection object
 * @guid: string which should identify object
 *
 * Undelete midgard object identified by given @guid
 *
 * MidgardError set by this method:
 *  - MGD_ERR_NOT_EXISTS
 *  - MGD_ERR_OBJECT_PURGED
 *  - MGD_ERR_INTERNAL
 *
 * Returns: %TRUE on success, %FALSE otherwise
 */
gboolean midgard_object_class_undelete(MidgardConnection *mgd, const gchar *guid)
{
	g_assert(mgd);
	g_assert(guid);

	MidgardObjectClass *klass;
	gint rv;
	

	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);
	GString *sql = g_string_new("SELECT ");
	g_string_append_printf(sql,
			"typename, object_action FROM repligard "
			"WHERE guid = '%s'",
			guid);

	gchar *query = g_string_free(sql, FALSE);
	GdaDataModel *model = 
		midgard_core_query_get_model(mgd, query);
	g_free(query);
	
	if (!model) {	
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_NOT_EXISTS);
		return FALSE;
	}

	const GValue *action_value = 
		midgard_data_model_get_value_at (model, 1, 0);
	
	const GValue *type_value = 
		midgard_data_model_get_value_at (model, 0, 0);

	g_object_unref(model);

	switch(g_value_get_uint((GValue*)action_value)) {

		case MGD_OBJECT_ACTION_DELETE:
			klass = 
				MIDGARD_OBJECT_GET_CLASS_BY_NAME(
						g_value_get_string(
							(GValue *)type_value));
			const gchar *tablename = 
				midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(klass));

			sql = g_string_new("UPDATE ");
			g_string_append_printf(sql, 
					"%s SET metadata_deleted=FALSE "
					"WHERE guid = '%s'",
					tablename,
					guid);

			query = g_string_free(sql, FALSE);
			rv = midgard_core_query_execute(mgd, query, FALSE);
			g_free(query);

			if(rv == 0) {
				MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);
				return FALSE;
			}
			return TRUE;			
			break;

		case MGD_OBJECT_ACTION_PURGE:
			MIDGARD_ERRNO_SET(mgd, MGD_ERR_OBJECT_PURGED);
			return FALSE;

		case MGD_OBJECT_ACTION_NONE:
			midgard_set_error(mgd,
					MGD_GENERIC_ERROR,
					MGD_ERR_USER_DATA,
					"Requested object is not deleted");
			g_clear_error(&mgd->err);
			return FALSE;
	}
	return FALSE;
}

/**
  * midgard_object_class_factory
  * @mgd: #MidgardConnection instance
  * @klass: #MidgardObjectClass 
  * @val: #GValue of string or uint type
  *
  * @val should be initialized as %G_TYPE_STRING or %G_TYPE_UINT and should hold 
  * database id or guid value. Empty object (not fecthed from database) is returned
  * if value is explicitly set to NULL.
  *
  * Returns: newly allocated #MgdObject object, or %NULL on failure.
  */
MgdObject *midgard_object_class_factory(
		MidgardConnection *mgd, MidgardObjectClass *klass, const GValue *val)
{
	g_assert(mgd != NULL);
	g_assert(klass != NULL);

	const gchar *klassname = G_OBJECT_CLASS_NAME(klass);

	return midgard_object_new(mgd, klassname, (GValue *)val);
}

/**
 * midgard_object_class_has_metadata:
 * @klass: #MidgardObjectClass pointer
 *
 * Checks whether given class provides metadata.
 *
 * Returns: %TRUE or %FALSE if metadata is not declared for given class.
 *
 * Since: 9.09
 */ 
gboolean 
midgard_object_class_has_metadata (MidgardObjectClass *klass)
{
	g_return_val_if_fail (klass != NULL, FALSE);

	return MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->has_metadata;
}

/**
 * midgard_object_class_get_metadata_class:
 * @klass: #MidgardObjectClass pointer
 *
 * Get metadata class pointer if declared for given #MidgardObjectClass
 *
 * Returns: Pointer to GObject derived class or %NULL
 *
 * Since: 9.09
 */
GObjectClass *
midgard_object_class_get_metadata_class (MidgardObjectClass *klass)
{
	g_return_val_if_fail (klass != NULL, NULL);
	MgdSchemaTypeAttr *type_attr = midgard_core_class_get_type_attr (MIDGARD_DBOBJECT_CLASS (klass));	

	if (!type_attr)
		return NULL;

	if (type_attr->metadata_class == NULL)
		return NULL;

	return g_type_class_peek(g_type_from_name(type_attr->metadata_class));
}

/**
 * midgard_object_class_get_schema_value:
 * @klass: MidgardObjectClass derived class pointer
 * @name: node's name declared for given @klass
 *
 * Returns: value for given node's @name 
 *
 * Since: 9.09.1
 */
const gchar *
midgard_object_class_get_schema_value (MidgardObjectClass *klass, const gchar *name)
{
	g_return_val_if_fail (klass != NULL, NULL);
	g_return_val_if_fail (name != NULL, NULL);

	MgdSchemaTypeAttr *type_attr = midgard_core_class_get_type_attr (MIDGARD_DBOBJECT_CLASS (klass));	

	if (!type_attr)
		return NULL;

	return g_hash_table_lookup (type_attr->user_values, (gpointer) name);
}

