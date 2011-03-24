/* 
 * Copyright (C) 2006, 2007, 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_core_object_class.h"
#include "midgard_error.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "midgard_reflection_property.h"
#include "guid.h"
#include "midgard_core_object.h"
#include "midgard_core_config.h"
#include "midgard_core_metadata.h"
#include "midgard_dbobject.h"
#include "midgard_metadata.h"
#include "midgard_reflector_object.h"

#define get_varchar_size(str) \
	        if (str == NULL) \
                size++; \
        else \
                size = strlen((const gchar *)str) + 1 + size; \
        g_free(str); \
	str = NULL;	

#define get_datetime_size(str) \
	        size = 8 + size; \
        g_free(str);


gboolean midgard_core_object_prop_type_is_valid(GType src_type, GType dst_type)
{
	if (src_type == dst_type)
		return TRUE;

	return FALSE;
}

gboolean midgard_core_object_prop_link_is_valid(GType ltype)
{
	if (ltype == MGD_TYPE_UINT) {

		return TRUE;

	} else if (ltype == MGD_TYPE_GUID) {

		return TRUE;

	} else if (ltype == MGD_TYPE_STRING) {

		return TRUE;

	} else {

		g_warning("Invalid property type. Expected uint, guid or string");
		return FALSE;
	}
}

gboolean midgard_core_object_prop_parent_is_valid(GType ptype)
{
	return  midgard_core_object_prop_link_is_valid(ptype);
}

gboolean midgard_core_object_prop_up_is_valid(GType ptype)
{
	return  midgard_core_object_prop_link_is_valid(ptype);
}

GType 
midgard_core_object_get_property_parent_type (MidgardObjectClass *klass)
{
	g_assert(klass != NULL);	

	const gchar *prop = MGD_DBCLASS_PROPERTY_PARENT (klass);
	if (prop == NULL) {
		g_warning("Failed to get parent property");
		return 0;
	}

	GParamSpec *pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), prop);
	if (pspec == NULL) {
		g_error("Failed to find GParamSpec for parent '%s' property", prop);
		return 0;
	}

	if (!midgard_core_object_prop_parent_is_valid(pspec->value_type))
		return 0;

	return pspec->value_type;
}

GType 
midgard_core_object_get_property_up_type (MidgardObjectClass *klass)
{
	g_assert(klass != NULL);

	const gchar *prop = MGD_DBCLASS_PROPERTY_UP (klass);

	if (prop == NULL) {
		g_warning("Failed to get up property");
		return 0;
	}

	GParamSpec *pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), prop);
	if (pspec == NULL) {
		g_error("Failed to find GParamSpec for up '%s' property", prop);
		return 0;
	}

	if (!midgard_core_object_prop_parent_is_valid(pspec->value_type))
		return 0;

	return pspec->value_type;
}

gboolean midgard_core_object_prop_parent_is_set(MidgardObject *object)
{
	g_assert(object != NULL);

	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS(object);
	GType ptype = midgard_core_object_get_property_parent_type(klass);

	if (ptype == 0) 
		return FALSE;

	GValue gval = {0, };
	g_value_init(&gval, ptype);
	const gchar *prop = MGD_DBCLASS_PROPERTY_PARENT (klass);

	g_object_get_property(G_OBJECT(object), prop, &gval);

	if (ptype == G_TYPE_UINT) {
		
		guint i = g_value_get_uint(&gval);
		g_value_unset (&gval);
		if (i == 0)
			return FALSE;

		return TRUE;
	
	} else if (ptype == G_TYPE_STRING) {

		const gchar *s = g_value_get_string(&gval);
		gboolean _rv = TRUE;

		if (s == NULL || *s == '\0')
			_rv = FALSE;

		g_value_unset (&gval);

		return _rv;
	}

	if (G_IS_VALUE (&gval))
		g_value_unset (&gval);

	return FALSE;
}

gboolean 
midgard_core_object_prop_up_is_set (MidgardObject *object)
{
	g_assert(object != NULL);

	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS(object);
	GType ptype = midgard_core_object_get_property_up_type(klass);

	if (ptype == 0) 
		return FALSE;

	GValue gval = {0, };
	g_value_init(&gval, ptype);
	const gchar *prop = MGD_DBCLASS_PROPERTY_UP (klass);

	g_object_get_property(G_OBJECT(object), prop, &gval);

	if (ptype == G_TYPE_UINT) {
		
		guint i = g_value_get_uint(&gval);
		if (i == 0)
			return FALSE;

		return TRUE;
	
	} else if (ptype == G_TYPE_STRING) {

		const gchar *s = g_value_get_string(&gval);
		if (s == NULL || *s == '\0')
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

gboolean midgard_core_object_is_valid(MidgardObject *object)
{	
	guint propn, i, size = 0;
	GType prop_type;
	gchar *prop_str = NULL;
	gchar *blobpath, *location = NULL;
	struct stat statbuf; /* GLib 2.6 is not available everywhere, g_stat not used */
	const gchar *typename = G_OBJECT_TYPE_NAME(object);
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS(object);	

	GParamSpec **props = g_object_class_list_properties(
			G_OBJECT_GET_CLASS(G_OBJECT(object)), &propn);
	if (!props){
		midgard_set_error(MGD_OBJECT_CNC (object),
				MGD_GENERIC_ERROR,
				MGD_ERR_INVALID_OBJECT,
				"Object %s has no properties",
				G_OBJECT_TYPE_NAME(object));
		g_warning("%s", MGD_OBJECT_CNC (object)->err->message);	
		g_free(props);
		return FALSE; 
	}

	/* Check if object is dependent */
	const gchar *parent_prop = MGD_DBCLASS_PROPERTY_PARENT (klass);
	const gchar *up = MGD_DBCLASS_PROPERTY_UP (klass);

	/* Make sure parent and up are integers */
	GParamSpec *parent_pspec = NULL;
	GParamSpec *up_pspec = NULL;
	if (parent_prop)	
		parent_pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), parent_prop);
	if (up_pspec)
		up_pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), up);

	/* Parent declared, up not. Parent can not be 0 */
	if (parent_pspec && !up_pspec) {
	
		if (!midgard_core_object_prop_parent_is_set(object)) {
			
			midgard_set_error(MGD_OBJECT_CNC(object),
					MGD_GENERIC_ERROR,
					MGD_ERR_INVALID_PROPERTY_VALUE,
					"Parent '%s' property's value is 0. ",
					parent_prop);
			
			g_free(props);
			return FALSE;
		}
	}

	/* Parent declared, up declared. Parent might be 0, only if up is not 0. */
	if (parent_pspec && up_pspec) {

		if (!midgard_core_object_prop_parent_is_set(object)
				&& !midgard_core_object_prop_up_is_set(object)) {
			
			midgard_set_error(MGD_OBJECT_CNC(object),
					MGD_GENERIC_ERROR,
					MGD_ERR_INVALID_PROPERTY_VALUE,
					"Parent '%s' and up '%s' property's value is 0. ",
					parent_prop, up);
			
			g_free(props);
		}
	}

	/* Attachment, we need to get file size */
	if (g_str_equal(typename, "midgard_attachment")){
		
		gint statv;
		MidgardConnection *mgd = MGD_OBJECT_CNC(object);
		const gchar *blobdir = mgd->priv->config->blobdir;

	      	if (blobdir == NULL) {
			g_warning ("NULL blobdir for current configuration");
			g_error ("Invalid blobdir");
			return FALSE;
		}

		if (!g_file_test((const gchar *)blobdir,
					G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
			g_warning ("Configured blobdir '%s' is not a directory!", blobdir);
			g_error ("Invalid blobdir");
			return FALSE;
		}

		g_object_get(G_OBJECT(object), "location", &location, NULL);

		if (location != NULL && *location != '\0') {
				
			blobpath = g_build_path(G_DIR_SEPARATOR_S, blobdir, location, NULL);

				statv = g_stat(blobpath, &statbuf);

				if (statv == 0) {
					
					if (g_file_test((const gchar *)blobpath,
								G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {

						size = statbuf.st_size;
					}
				}

				g_free(blobpath);
				g_free(location);
		}
	}
	
	MidgardReflectionProperty *mrp = 
		midgard_reflection_property_new(MIDGARD_DBOBJECT_CLASS(klass));
	
	for(i = 0; i < propn; i++) {
		
		prop_type = midgard_reflection_property_get_midgard_type (mrp, props[i]->name);

		if (prop_type == MGD_TYPE_GUID) {
			
			g_object_get(G_OBJECT(object), props[i]->name, &prop_str, NULL);

			if (prop_str != NULL && *prop_str != '\0') {
				
				if (!midgard_is_guid(prop_str)) { 
				
					midgard_set_error(MGD_OBJECT_CNC (object),
							MGD_GENERIC_ERROR,
							MGD_ERR_INVALID_PROPERTY_VALUE,
							"'%s' property's value is not a guid. ",
							props[i]->name);
					g_warning("%s", MGD_OBJECT_CNC (object)->err->message);
					g_clear_error(&MGD_OBJECT_CNC (object)->err);
					g_free(props);
					g_free(prop_str);
					g_object_unref (mrp);
					return FALSE;
				}
			}

			get_varchar_size(prop_str);
		
		} else if (prop_type == MGD_TYPE_LONGTEXT) {
					
			g_object_get(G_OBJECT(object), props[i]->name, &prop_str, NULL);
			if (prop_str == NULL) prop_str = g_strdup("");
			size = (strlen(prop_str)) + 4 + size;
			g_free (prop_str);
		
		} else if (prop_type == MGD_TYPE_TIMESTAMP) {
			
			/* TODO, Check if timestamp value is valid */
			size =  8 + size;
		
		} else if (prop_type == MGD_TYPE_STRING) {
			
			g_object_get(G_OBJECT(object), props[i]->name, &prop_str, NULL);
			get_varchar_size(prop_str);

		} else if (prop_type == MGD_TYPE_UINT 
				&& prop_type == MGD_TYPE_INT
				&& prop_type == MGD_TYPE_FLOAT
				&& prop_type == MGD_TYPE_BOOLEAN){
			size = size + 4;
		
		} else {
			/* Do nothing now */
		}
	}

	g_object_unref (mrp);
	g_free(props);

	/* Object metadata */
	MidgardMetadata *metadata = MGD_DBOBJECT_METADATA (object);
	if (metadata)
		metadata->priv->size = size + midgard_core_metadata_get_size (metadata);

	return TRUE;
}

gboolean midgard_core_object_has_dependents(MidgardObject *self, const gchar *classname)
{
	g_return_val_if_fail(self != NULL, FALSE);
	g_return_val_if_fail(classname != NULL, FALSE);

	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(classname);

	if (klass == NULL) {

		g_warning("Can not find class pointer for given '%s' name", classname);
		return TRUE;
	}

	const gchar *up_property = NULL;

	/* Do we check the same type? */
	if (G_OBJECT_TYPE(self) == g_type_from_name(classname)) {
		
		up_property = MGD_DBCLASS_PROPERTY_UP (klass);
	
	} else {
		
		up_property = MGD_DBCLASS_PROPERTY_PARENT (klass);
	}
	
	if (up_property == NULL)
		return FALSE;

	GParamSpec *pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), up_property);

	GValue val = {0, };

	if (pspec->value_type == G_TYPE_STRING) {

		g_value_init(&val, G_TYPE_STRING);
		g_object_get_property(G_OBJECT(self), "guid", &val);
		
	} else if (pspec->value_type == G_TYPE_UINT) {
	
		g_value_init(&val, G_TYPE_UINT);
		g_object_get_property(G_OBJECT(self), "id", &val);
	
	} else {

		g_warning("Confused with unexpected '%s' property type", g_type_name(pspec->value_type));
		/* No idea what to do in such case, so make it safe and return TRUE */
		return TRUE;
	}
	
	MidgardQueryBuilder *builder = midgard_query_builder_new(MGD_OBJECT_CNC(self), classname);
	
	if (builder == NULL)
		return FALSE;

	midgard_query_builder_add_constraint(builder, up_property, "=", &val);	
	midgard_query_builder_set_limit(builder, 1);

	guint i = midgard_query_builder_count(builder);

	g_object_unref(builder);

	if (i == 0)
		return FALSE;

	return TRUE;
}

/* DATABASE DATA MACROS AND ROUTINE FUNCTIONS */

void midgard_core_object_class_set_midgard(
		MidgardConnection *mgd, GObject *object)
{
	g_assert(mgd != NULL);
	g_assert(object != NULL);

	GObjectClass *klass = G_OBJECT_GET_CLASS(object);

	MidgardConnection *_mgd = MGD_OBJECT_CNC(object);
	if (_mgd != NULL) {
		
		g_warning("Connection pointer already associated with object");
		return;
	}

	if (MIDGARD_IS_DBOBJECT_CLASS(klass))
		MGD_OBJECT_CNC (object) = mgd;

	return;
}

void midgard_core_object_class_set_guid(
		GObject *object, const gchar *guid)
{
	g_assert(object != NULL);

	GObjectClass *klass = G_OBJECT_GET_CLASS(object);

	if (MIDGARD_IS_OBJECT_CLASS (klass)) {
		g_free((gchar *) MGD_OBJECT_GUID (object));
		MGD_OBJECT_GUID (object) = g_strdup(guid);
	}	
	
	return;
}

MgdSchemaTypeAttr *midgard_core_class_get_type_attr(MidgardDBObjectClass *klass)
{
	if (!G_TYPE_IS_CLASSED(G_TYPE_FROM_CLASS(klass))){
		g_warning("Class pointer is not a GObjectClass pointer");
		return NULL;
	}

	MgdSchemaTypeAttr *type_attr = klass->dbpriv->storage_data;

	return type_attr;
}

const gchar *midgard_core_class_get_table(MidgardDBObjectClass *klass)
{
	MgdSchemaTypeAttr *type_attr = NULL;
	type_attr = midgard_core_class_get_type_attr(klass);
	
	if (!type_attr)
		return NULL;

	return type_attr->table;
}

const gchar *midgard_core_class_get_tables(MidgardDBObjectClass *klass)
{
	MgdSchemaTypeAttr *type_attr = NULL;
	type_attr = midgard_core_class_get_type_attr(klass);
	
	if (!type_attr)
		return NULL;
	
	return type_attr->tables;
}

MgdSchemaPropertyAttr *midgard_core_class_get_property_attr(
		MidgardDBObjectClass *klass, const gchar *name)
{
	g_assert(klass != NULL);
	g_assert(name != NULL);

	MgdSchemaPropertyAttr *prop_attr = NULL;	
	prop_attr = g_hash_table_lookup(klass->dbpriv->storage_data->prophash, name);

	if (prop_attr == NULL)
		return NULL;
	
	return prop_attr;
}

const gchar *midgard_core_class_get_property_table(
		MidgardDBObjectClass *klass, const gchar *name)
{
	g_assert(klass != NULL);
	g_assert(name != NULL);
	
	MgdSchemaPropertyAttr *prop_attr = NULL;
	prop_attr = midgard_core_class_get_property_attr(klass, name);

	if (prop_attr == NULL)
		return NULL;
	
	return prop_attr->table;
}

const gchar *midgard_core_class_get_property_colname(
		MidgardDBObjectClass *klass, const gchar *name)
{
	g_assert(klass != NULL);
	g_assert(name != NULL);
	
	MgdSchemaPropertyAttr *prop_attr = NULL;
	prop_attr = midgard_core_class_get_property_attr(klass, name);
	
	if (prop_attr == NULL)
		return NULL;
	
	return prop_attr->field;
}

const gchar *midgard_core_class_get_property_tablefield(
		MidgardDBObjectClass *klass, const gchar *name)
{
	g_assert(klass != NULL);
	g_assert(name != NULL);

	MgdSchemaPropertyAttr *prop_attr = NULL;
	prop_attr = midgard_core_class_get_property_attr(klass, name);

	if (prop_attr == NULL)
		return NULL;
	return prop_attr->tablefield;
}
