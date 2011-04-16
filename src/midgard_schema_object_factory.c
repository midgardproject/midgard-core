/* 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_schema_object_factory.h"
#include "midgard_object.h"
#include "guid.h"
#include "midgard_error.h"
#include <libgda/libgda.h>
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include "midgard_repligard.h"

/**
 * midgard_schema_object_factory_get_object_by_guid:
 * @mgd: #MidgardConnection instance
 * @guid: guid which identifies object to look for 
 * 
 * Creates new instance of the class defined in Midgard Schema.
 *
 * Cases to return %NULL:
 * <itemizedlist>
 * <listitem><para>
 * Given guid is not a valid guid (MGD_ERR_NOT_EXISTS)
 * </para></listitem>
 * <listitem><para>
 * There's no object identified by given guid (MGD_ERR_NOT_EXISTS)
 * </para></listitem>
 * <listitem><para>
 * Object identified by given guid is deleted (MGD_ERR_OBJECT_DELETED)
 * </para></listitem>
 * <listitem><para>
 * Object identified by given guid is purged (MGD_ERR_OBJECT_PURGED)
 * </para></listitem>
 * <listitem><para>
 * Object identified by given guid doesn't provide 'metadata' or 'deleted' property (MGD_ERR_INVALID_PROPERTY)
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: (transfer full): #MidgardObject derived new instance or %NULL on failure
 * Since: 10.05
 */ 
MidgardObject*
midgard_schema_object_factory_get_object_by_guid (MidgardConnection *mgd, const gchar *guid)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (guid != NULL, NULL);

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	if (!midgard_is_guid (guid)) {
		MIDGARD_ERRNO_SET (mgd, MGD_ERR_NOT_EXISTS);
		return NULL;
	}

	MidgardObject *object = NULL;

	MidgardDBObject *repligard = NULL;
	GError *error = NULL;
	GValue gval = {0, };
	g_value_init (&gval, G_TYPE_STRING);
	g_value_set_string (&gval, guid);
	midgard_core_query_get_object (mgd, g_type_name (MIDGARD_TYPE_REPLIGARD), &repligard, &error, "reference", &gval, NULL);
	g_value_unset (&gval);
	
	if (error) {
		MIDGARD_ERRNO_SET_STRING (mgd, error->code, "%s", error->message);
		g_clear_error (&error);
		if (repligard)
			g_object_unref (G_OBJECT (repligard));
		return FALSE;
	}

	guint action = 0;
	gchar *classname;
	g_object_get (repligard,
			"action", &action,
			"type", &classname, NULL);
	g_object_unref (repligard);

	if (!classname || (classname && *classname == '\0')) {
		g_warning("Database inconsistency!. Expected classname string in repligard typename (%s)", guid);
		return NULL;
	}

	switch (action) {

		case MGD_OBJECT_ACTION_DELETE:
			MIDGARD_ERRNO_SET (mgd, MGD_ERR_OBJECT_DELETED);
			break;
	
		case MGD_OBJECT_ACTION_PURGE:
			MIDGARD_ERRNO_SET (mgd, MGD_ERR_OBJECT_PURGED);
			break;

		default:
			g_value_init(&gval, G_TYPE_STRING);
    			g_value_set_string (&gval, guid);
			object = midgard_object_new (mgd, g_value_get_string ((GValue*)classname), &gval);
	   		g_value_unset (&gval);
			if (!object) {
				MIDGARD_ERRNO_SET (mgd, MGD_ERR_NOT_EXISTS);
		       	}
			break;
	}

	g_free (classname);

	return object;	
}

static void 
__get_parentclass_name (const gchar *classname, GList **list)
{
 	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME (classname);
	MgdSchemaTypeAttr *type_attr = midgard_core_class_get_type_attr (MIDGARD_DBOBJECT_CLASS (klass));
	const gchar *pname = (const gchar *)type_attr->parent;

	if (pname != NULL) {
		*list = g_list_prepend (*list, (gpointer) pname);
		__get_parentclass_name (pname, list);
	}
	return;
}

static gboolean 
__get_id_from_path_element (MidgardConnection *mgd,
		const gchar *table, const gchar *name, const gchar *field,
		guint val, MidgardObjectClass *klass, guint *rval)
{
	const gchar *unique_name = MGD_DBCLASS_PROPERTY_UNIQUE (klass);
	const gchar *deleted_field = midgard_core_object_get_deleted_field (MIDGARD_DBOBJECT_CLASS (klass));
	GString *where = g_string_new ("");
	if (deleted_field)
		g_string_append_printf (where, 
				"%s = '%s' AND %s = %d AND %s = 0 ",
				unique_name, name, field, val, deleted_field);
	else 
		g_string_append_printf (where,
				"%s = '%s' AND %s = %d  ",
				unique_name, name, field, val);

	GValue *value = midgard_core_query_get_field_value (mgd, "id", table, where->str);
        g_string_free (where, TRUE);

	if (!value)
		return FALSE;

	guint _id = 0;
	if (G_VALUE_HOLDS_UINT (value))
		_id = g_value_get_uint (value);
	if (G_VALUE_HOLDS_INT (value))
		_id = g_value_get_int (value);
	g_value_unset (value);
	g_free (value);
	*rval = _id;
	return TRUE;
}

/**
 * midgard_schema_object_factory_get_object_by_path:
 * @mgd: #MidgardConnection instance
 * @classname: name of the class, new instance should be created for
 * @path: path which identifies object
 *
 * Get object by path. Path elements are objects' names.
 * To get top object with empty name use "/" path. 
 * Also empty name is allowed in given path: "/A/B//D".
 *
 * Cases to return %NULL:
 * <itemizedlist>
 * <listitem><para>
 * Object identified by given path doesn't exist (MGD_ERR_NOT_EXISTS)
 * </para></listitem>
 * <listitem><para>
 * Given @classname doesn't support tree functionality (MGD_ERR_NOT_INTERNAL)
 * </para></listitem>
 * <listitem><para>
 * Given @classname doesn't provide 'id' or unique named property (MGD_ERR_NOT_INTERNAL)
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: (transfer full): #MidgardObject derived, new @classname instance or %NULL 
 * Since: 10.05
 */ 
MidgardObject*
midgard_schema_object_factory_get_object_by_path (MidgardConnection *mgd, const gchar *classname, const gchar *path)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (classname != NULL, NULL);
	g_return_val_if_fail (path != NULL, NULL);

	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	gchar *object_path = NULL;
	
	if (g_str_has_prefix (path, "//")) 
		g_warning ("Empty element in object's path '%s'", path);
	
	if (g_str_has_prefix (path, "/")) {
		object_path = g_strdup (path);
	} else  {
		object_path = g_strconcat ("/", path, NULL);
	}

	/* Add classname to list */
	GList *plist = NULL;
	plist = g_list_prepend (plist, NULL);
	plist = g_list_prepend (plist, (gpointer) classname);
	/* "Walk" tree till NULL parent found */
	__get_parentclass_name (classname, &plist);

      	gchar **pelts = g_strsplit (object_path, "/", 0);
	g_free (object_path);

	guint i = 1;
	const gchar *tablename;
	gchar *_cname;
	MidgardObjectClass *klass;
	const gchar *property_up, *property_parent;
	guint oid, _oid;
	gboolean id_exists = FALSE;

	do {
		if (i == 1)
			oid = 0;
		else
			oid = _oid;

		if(plist->data == NULL) {
			g_strfreev (pelts);
			g_list_free (plist);
			MIDGARD_ERRNO_SET(mgd, MGD_ERR_NOT_EXISTS);
			return NULL;
		}

		_cname = (gchar *)plist->data;
		klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME (_cname);
		tablename = midgard_core_class_get_table (MIDGARD_DBOBJECT_CLASS(klass));
		property_parent = MGD_DBCLASS_PROPERTY_PARENT (klass);
		property_up = MGD_DBCLASS_PROPERTY_UP (klass);
		
		if (!property_parent && !property_up) {
			g_strfreev (pelts);
			g_list_free (plist);
			MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INTERNAL, "%s doesn't support tree functionality", _cname);
			return NULL;
		}
	
		/* FIXME, replace 'name' with unique property */
		GParamSpec *namespec, *idspec;
		namespec = g_object_class_find_property (G_OBJECT_CLASS(klass), "name");
     		idspec = g_object_class_find_property (G_OBJECT_CLASS(klass), "id");

		if (!namespec || !idspec) {
			g_strfreev (pelts);
			g_list_free (plist);
    			MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INTERNAL, "%s class has no 'name' or 'id' member registered", _cname);
			return NULL;
		}

		if (property_parent) {
			id_exists = __get_id_from_path_element (mgd,
					tablename, (const gchar *)pelts[i],
					property_parent, oid, klass, &_oid);
			if (id_exists)
				goto _GET_NEXT_PATH_ELEMENT;
		}

		/* Try to get object using up property */
		if (property_up) {
			id_exists = __get_id_from_path_element (mgd,
					tablename, (const gchar *)pelts[i],
					property_up, oid, klass, &_oid);
			if (id_exists > 0)
				goto _GET_NEXT_PATH_ELEMENT;
		}

		if (!id_exists) {
			plist = plist->next;
			continue;
		}

_GET_NEXT_PATH_ELEMENT:
		
		i++;
  		if (pelts[i] == NULL) {
			GValue idval = {0, };
			g_value_init (&idval, G_TYPE_UINT);
			g_value_set_uint (&idval, _oid);
			MidgardObject *object = midgard_object_new (mgd, _cname, &idval);
 			g_value_unset (&idval);
			g_list_free (plist);
			g_strfreev (pelts);

			return object;
		}

	} while (pelts[i] != NULL);

	if (plist)
		g_list_free (plist);

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_NOT_EXISTS);
 	return NULL;
}

/**
 * midgard_schema_object_factory_object_undelete:
 * @mgd: #MidgardConnection instance
 * @guid: guid which identifies object to undelete
 * 
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * Object identified by given guid doesn't exist (MGD_ERR_NOT_EXISTS)
 * </para></listitem>
 * <listitem><para>
 * Object identified by given guid is purged (MGD_ERR_OBJECT_PURGED)
 * </para></listitem>
 * <listitem><para>
 * Object identified by given guid is not deleted (MGD_ERR_USER_DATA)
 * </para></listitem>
 * <listitem><para>
 * Either object's or repligard's record couldn't be updated (MGD_ERR_INTERNAL)
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05
 */ 
gboolean	
midgard_schema_object_factory_object_undelete (MidgardConnection *mgd, const gchar *guid)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (guid != NULL, FALSE);

	MidgardObjectClass *klass;
	gint rv;
	gboolean ret;

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	MidgardDBObject *repligard = NULL;
	GError *error = NULL;
	GValue gval = {0, };
	g_value_init (&gval, G_TYPE_STRING);
	g_value_set_string (&gval, guid);
	midgard_core_query_get_object (mgd, g_type_name (MIDGARD_TYPE_REPLIGARD), &repligard, &error, "guid", &gval, NULL);
	g_value_unset (&gval);
	
	if (error) {
		MIDGARD_ERRNO_SET_STRING (mgd, error->code, "%s", error->message);
		g_clear_error (&error);
		if (repligard)
			g_object_unref (G_OBJECT (repligard));
		return FALSE;
	}

	guint action = 0;
	gchar *classname;
	g_object_get (repligard,
			"action", &action,
			"type", &classname, NULL);
	g_object_unref (repligard);

	switch (action) {

		case MGD_OBJECT_ACTION_DELETE:
			klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME (classname);
			const gchar *tablename = midgard_core_class_get_table (MIDGARD_DBOBJECT_CLASS(klass));
			const gchar *deleted_field = midgard_core_object_get_deleted_field (MIDGARD_DBOBJECT_CLASS (klass));
			if (!deleted_field) {
				MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INVALID_PROPERTY, 
						"Object identified by %s guid doesn't provide 'metadata' or 'deleted' property", guid);
				return FALSE;
			}
			/* Update object's metadata */
	   		GString *sql = g_string_new ("UPDATE ");
			g_string_append_printf (sql, "%s SET metadata_deleted=0 WHERE guid = '%s'", tablename, guid);
  			rv = midgard_core_query_execute (mgd, sql->str, FALSE);
			g_string_free (sql, TRUE);
			/* Update repligard record */
			sql = g_string_new ("UPDATE ");
			g_string_append_printf (sql, "repligard SET object_action=%d WHERE guid = '%s'", MGD_OBJECT_ACTION_UPDATE, guid);
  			guint rep_rv = midgard_core_query_execute (mgd, sql->str, FALSE);
			g_string_free (sql, TRUE);

			if (rv == 0 || rep_rv == 0) { 
  				MIDGARD_ERRNO_SET (mgd, MGD_ERR_INTERNAL);
			}
			else {
				ret = TRUE;
			}
			break;

		case MGD_OBJECT_ACTION_PURGE:
			MIDGARD_ERRNO_SET (mgd, MGD_ERR_OBJECT_PURGED);
			break;

		case MGD_OBJECT_ACTION_NONE:
	   		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_USER_DATA, "Object identified by %s guid is not deleted", guid);
			break;
	}

	g_free (classname);

	return ret;
}

/**
 * midgard_schema_object_factory_create_object:
 * @mgd: #MidgardConnection instance
 * @classname: name of the class created object is instance of
 * @value: value which holds object's identifier
 *
 * Check midgard_object_new() for returned value and possible errors.
 *
 * Returns: (transfer full): new #MidgardObject derived instance or NULL
 * Since: 10.05.1
 */ 
MidgardObject*  
midgard_schema_object_factory_create_object(MidgardConnection *mgd, const gchar *classname, GValue *value)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (classname != NULL, NULL);

	return midgard_object_new (mgd, classname, value);
}

/* GOBJECT ROUTINES */

static void
_midgard_schema_object_factory_class_init (MidgardSchemaObjectFactoryClass *klass, gpointer class_data)
{
	/* Do nothing atm */
	return;	
}

GType midgard_schema_object_factory_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardSchemaObjectFactoryClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_schema_object_factory_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSchemaObjectFactory),
			0,              /* n_preallocs */
			NULL /* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSchemaObjectFactory", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}

