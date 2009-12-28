/* 
 * Copyright (C) 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_test_replicator.h"

#define _MGD_TEST_REPLICATOR_SPOOL_DIR "midgard_test_replicator_spool"

static gchar *
_build_object_spool_file (GObject *object)
{
	g_assert (object != NULL);

	gchar *guid = NULL;
	g_object_get (object, "guid", &guid, NULL);

	g_assert (guid != NULL);
	g_assert_cmpstr (guid, !=, "");
	g_assert (midgard_is_guid(guid) != FALSE);

	gchar *dirpath = g_build_path (G_DIR_SEPARATOR_S, G_DIR_SEPARATOR_S,	
			"tmp", _MGD_TEST_REPLICATOR_SPOOL_DIR, G_OBJECT_TYPE_NAME (object), NULL);

	gint replicator_spool_dir_created = g_mkdir_with_parents (dirpath, 0770);
	g_assert_cmpint (replicator_spool_dir_created, >, -1);

	gchar *filepath = g_build_path (G_DIR_SEPARATOR_S, dirpath, guid, NULL);

	g_free (guid);
	g_free (dirpath);

	return filepath;
}

static gboolean 
_set_file_contents (gchar *filepath, const gchar *data)
{
	g_assert (filepath != NULL);
	g_assert (data != NULL);

	g_assert_cmpstr (filepath, !=, "");
	g_assert_cmpstr (data, !=, "");

	GError *err = NULL;

	gboolean set_file_contents = g_file_set_contents ((const gchar *)filepath, data, -1, &err);

	g_assert (set_file_contents != FALSE);
	g_assert (err == NULL);

	return set_file_contents;
}

static gboolean
_write_xml_to_file (GObject *object, const gchar *xml)
{
	g_assert (object != NULL);
	g_assert (xml != NULL);

	gchar *filepath = _build_object_spool_file (object);

	gboolean set_file_contents = _set_file_contents (filepath, xml);

	g_free(filepath);

	return set_file_contents;
}

static gchar *
_read_object_xml_file (GObject *object)
{
	g_assert (object != NULL);

	gchar *filepath = _build_object_spool_file (object);
	GError *err = NULL;
	gchar *contents;

	gboolean get_file_contents = g_file_get_contents ((const gchar *)filepath, &contents, NULL, &err);

	g_free (filepath);

	g_assert (get_file_contents != FALSE);
	g_assert (err == NULL);

	return contents;
}

/* This should be run only when all objects are created and none of them is deleted */
void midgard_test_replicator_update_object_links(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	
	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_GET_CLASS(object);
	g_assert(klass != NULL);
	MidgardReflectionProperty *mrp = midgard_reflection_property_new(klass);
	g_assert(mrp != NULL);
	
	guint n_prop = 0;
	guint i = 0;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_CLASS(klass), &n_prop);

	for (i = 0; i < n_prop; i++) {

		gboolean islink = midgard_reflection_property_is_link(mrp, pspecs[i]->name);
		if (!islink)
			continue;

		/* Get link class pointer and link's target property name */
		const MidgardDBObjectClass *pklass = midgard_reflection_property_get_link_class(mrp, pspecs[i]->name);
		g_assert(pklass != NULL);
		const gchar *target = midgard_reflection_property_get_link_target(mrp, pspecs[i]->name);
		g_assert(target != NULL);

		/* Initialize QB for link class, so we can fetch at least one object */
		MidgardQueryBuilder *builder = 
			midgard_query_builder_new(mgd, G_OBJECT_CLASS_NAME(G_OBJECT_CLASS(pklass)));
		midgard_query_builder_set_limit(builder, 1);

		guint n_objects;
		GObject **objects = midgard_query_builder_execute(builder, &n_objects);
		
		if (objects == NULL) {
				
			g_object_unref(builder);
			continue;
		}

		/* Check guids, to avoid circular updates */
		gchar *oguid = NULL;
		gchar *dboguid = NULL;
		g_object_get (object, "guid", &oguid, NULL);
		g_object_get (objects[0], "guid", &dboguid, NULL);

		if (!g_str_equal (oguid, dboguid)) {

			/* Check the type of target property */
			GParamSpec *pspec = g_object_class_find_property(G_OBJECT_CLASS(pklass), target);
			g_assert(pspec != NULL);

			/* MgdSchema parser must validate types compatibility, it's not our business at this moement */
			GValue val = {0, };
			g_value_init(&val, pspec->value_type);
			g_object_get_property(objects[0], target, &val);

			/* Set object's property so it points to something real */
			g_object_set_property(G_OBJECT(object), pspecs[i]->name, &val);

			midgard_object_update(object);
		
			g_value_unset(&val);
		}

		g_free (oguid);
		g_free (dboguid);
		g_object_unref(objects[0]);
		g_object_unref(builder);
		g_free(objects);
	}

	g_free(pspecs);
	g_object_unref(mrp);
}

static gchar *__serialize_object(GObject *object)
{
	gchar *xml = midgard_replicator_serialize(object);
	g_assert(xml != NULL);

	/* TODO, this is not implemented in core 
	gboolean xml_is_valid = midgard_replicator_xml_is_valid((const gchar *)xml);
	g_assert(xml_is_valid != FALSE); */
	
	return xml;
}

static const gchar *vprops[] = {
	"sitegroup",
	"id",
	"sid",
	"action",
	NULL
};

static const gchar *metadata_vprops[] = {
	"exported",
	"size",
	NULL
};

static gboolean __is_volatile(const gchar **strv, const gchar *prop)
{
	g_assert(strv != NULL);
	g_assert(prop != NULL);
	while (*strv != NULL) {
		
		if (g_str_equal(*strv++, prop)) {
			return TRUE;
		}
	}
	
	return FALSE;
}

static gchar *__class_property(GObject *object, const gchar *property, GValue *val)
{
	GString *str = g_string_new("");

	g_string_append_printf(str, "%s.%s (VALUE: ", 
			G_OBJECT_TYPE_NAME(object), property);

	switch (G_VALUE_TYPE(val)) {

		case G_TYPE_STRING:
			g_string_append_printf(str, "%s", g_value_get_string(val));
			break;

		case G_TYPE_UINT:
			g_string_append_printf(str, "%d", g_value_get_uint(val));
			break;

		case G_TYPE_INT:
			g_string_append_printf(str, "%d", g_value_get_int(val));
			break;

		case G_TYPE_FLOAT:
			g_string_append_printf(str, "%f", g_value_get_float(val));
			break;

		case G_TYPE_BOOLEAN:
			g_string_append_printf(str, "%d", g_value_get_boolean(val));
			break;
	}

	g_string_append(str, " )");

	return g_string_free(str, FALSE);
}

static gboolean __is_unserialized_object_valid(GObject *orig, GObject *copy, const gchar **volatiles)
{
	g_assert(orig != NULL);
	g_assert(copy != NULL);

	guint n_prop = 0;
	guint i = 0;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(orig), &n_prop);
	g_assert(pspecs != NULL);
	GValue orig_val = {0, };
	GValue copy_val = {0, };

	/* metadata */
	GObject *morig = NULL;
	GObject *mcopy = NULL;

	for (i = 0; i < n_prop; i++) {

		/* We can not test properties like sitegroup or id.
		 * Thos are not replication safe, so we ignore them. */
		if (__is_volatile(volatiles, pspecs[i]->name))
			continue;

		g_value_init(&orig_val, pspecs[i]->value_type);
		g_value_init(&copy_val, pspecs[i]->value_type);

		g_object_get_property(orig, pspecs[i]->name, &orig_val); 
		g_object_get_property(copy, pspecs[i]->name, &copy_val);

		switch (pspecs[i]->value_type) {
			
			case G_TYPE_STRING:
		
				if (g_value_get_string(&orig_val) == NULL)
					break;

				g_assert_cmpstr(__class_property(orig, pspecs[i]->name, &orig_val),
						==, __class_property(copy, pspecs[i]->name, &copy_val));



				if (g_value_get_string(&orig_val) == NULL
					|| g_value_get_string(&copy_val) == NULL) {
				


				}

				if (!g_str_equal(g_value_get_string(&orig_val), 
							g_value_get_string(&copy_val))) {
					g_error("%s.%s", G_OBJECT_TYPE_NAME(orig), pspecs[i]->name);
					g_assert_cmpstr(__class_property(orig, pspecs[i]->name, &orig_val), 
							==, __class_property(copy, pspecs[i]->name, &copy_val));
					return FALSE;
				}

				break;

			case G_TYPE_UINT:

				if(!g_value_get_uint(&orig_val) != g_value_get_uint(&copy_val)) {
						g_error("%s.%s", G_OBJECT_TYPE_NAME(orig), pspecs[i]->name);
			
					g_assert_cmpstr(__class_property(orig, pspecs[i]->name, &orig_val), 
							==, __class_property(copy, pspecs[i]->name, &copy_val));
					return FALSE;
				}

				break;

			case G_TYPE_INT:

				g_assert_cmpstr(__class_property(orig, pspecs[i]->name, &orig_val),
						==, __class_property(copy, pspecs[i]->name, &copy_val));

				if(!g_value_get_int(&orig_val) != g_value_get_int(&copy_val)) {	
			
					g_assert_cmpstr(__class_property(orig, pspecs[i]->name, &orig_val), 
							==, __class_property(copy, pspecs[i]->name, &copy_val));
					return FALSE;
				}

				break;

			case G_TYPE_BOOLEAN:

				if(!g_value_get_boolean(&orig_val) != g_value_get_boolean(&copy_val)) {
								g_error("%s.%s", G_OBJECT_TYPE_NAME(orig), pspecs[i]->name);
		
					g_assert_cmpstr(__class_property(orig, pspecs[i]->name, &orig_val), 
							==, __class_property(copy, pspecs[i]->name, &copy_val));
					return FALSE;
				}

				break;

			case G_TYPE_FLOAT:

				if(!g_value_get_float(&orig_val) != g_value_get_float(&copy_val)) {
									g_error("%s.%s", G_OBJECT_TYPE_NAME(orig), pspecs[i]->name);
	
					g_assert_cmpstr(__class_property(orig, pspecs[i]->name, &orig_val), 
							==, __class_property(copy, pspecs[i]->name, &copy_val));
					return FALSE;
				}

				break;

			case G_TYPE_OBJECT:

				//morig = g_value_get_object(&orig_val);
				//mcopy = g_value_get_object(&copy_val); 
				//if (!__is_unserialized_object_valid(morig, mcopy, metadata_vprops))
				//	return FALSE;
				break;

			default:
				g_error("%s.%s", G_OBJECT_TYPE_NAME(orig), pspecs[i]->name);
				g_assert_cmpstr("Unhandled property type", ==, G_VALUE_TYPE_NAME(&orig_val));
				return FALSE;
				break;
		}

		g_value_unset(&orig_val);
		g_value_unset(&copy_val);
	}

	return TRUE;
}

void 
midgard_test_replicator_serialize (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	
	gchar *serialized_object_xml = midgard_replicator_serialize (G_OBJECT(object));
	
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (serialized_object_xml != NULL);
	g_assert_cmpstr (serialized_object_xml, !=, "");

	gboolean serialized_object_write = _write_xml_to_file (G_OBJECT (object), serialized_object_xml);
	g_assert (serialized_object_write != FALSE);
}

void 
midgard_test_replicator_unserialize (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);

	gchar *object_xml_content = _read_object_xml_file (G_OBJECT(object));
	
	g_assert(object_xml_content != NULL);
	g_assert_cmpstr (object_xml_content, !=, "");
	
	GObject **objects = midgard_replicator_unserialize(mgd, (const gchar *)object_xml_content, FALSE);

	g_free(object_xml_content);

	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert(objects != NULL);
	g_assert(objects[0] != NULL);

	GObject *orig = G_OBJECT(object);
	GObject *copy = objects[0];
	guint n_prop = 0;
	guint i = 0;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(orig), &n_prop);
	g_assert(pspecs != NULL);
	GValue orig_val = {0, };
	GValue copy_val = {0, };

	/* metadata */
	GObject *morig = NULL;
	GObject *mcopy = NULL;

	for (i = 0; i < n_prop; i++) {

		/* We can not test properties like sitegroup or id.
		 * Thos are not replication safe, so we ignore them. */
		if (__is_volatile(vprops, pspecs[i]->name))
			continue;

		g_value_init(&orig_val, pspecs[i]->value_type);
		g_value_init(&copy_val, pspecs[i]->value_type);

		g_object_get_property(orig, pspecs[i]->name, &orig_val); 
		g_object_get_property(copy, pspecs[i]->name, &copy_val);

		/* FIXME: Workaround for string default value being not initialized in constructor */	
		if (G_VALUE_HOLDS_STRING(&orig_val) && g_value_get_string(&orig_val) == NULL) {
		
			g_value_unset(&orig_val);
			g_value_unset(&copy_val);
			continue;
		}
	

		g_assert_cmpstr(__class_property(orig, pspecs[i]->name, &orig_val),
				==, __class_property(copy, pspecs[i]->name, &copy_val));

		g_value_unset(&orig_val);
		g_value_unset(&copy_val);
	}

	g_free(pspecs);

	if (!midgard_object_class_has_metadata (MIDGARD_OBJECT_GET_CLASS (object))) {
	
		i = 0;
		while (objects[i] != NULL) {
	
			g_object_unref(objects[i]);
			i++;
		}
		
		g_free(objects);

		return;
	}

	g_object_get (orig, "metadata", &morig, NULL);
	g_assert (morig != NULL);
	g_object_get (copy, "metadata", &mcopy, NULL);
	g_assert (mcopy != NULL);

	pspecs = g_object_class_list_properties (G_OBJECT_GET_CLASS (morig), &n_prop);
	g_assert (pspecs != NULL);	

	for (i = 0; i < n_prop; i++) {

		/* We can not test properties like sitegroup or id.
		 * Thos are not replication safe, so we ignore them. */
		if (__is_volatile(vprops, pspecs[i]->name))
			continue;

		g_value_init(&orig_val, pspecs[i]->value_type);
		g_value_init(&copy_val, pspecs[i]->value_type);

		g_object_get_property(morig, pspecs[i]->name, &orig_val); 
		g_object_get_property(mcopy, pspecs[i]->name, &copy_val);

		if (G_VALUE_HOLDS_STRING(&orig_val) && g_value_get_string(&orig_val) == NULL) {
		
			g_value_unset(&orig_val);
			g_value_unset(&copy_val);
			continue;
		}
	
		g_assert_cmpstr(__class_property(morig, pspecs[i]->name, &orig_val),
				==, __class_property(mcopy, pspecs[i]->name, &copy_val));
		
		g_value_unset(&orig_val);
		g_value_unset(&copy_val);
	}

	g_free(pspecs);

	i = 0;
	while (objects[i] != NULL) {

		g_object_unref(objects[i]);
		i++;
	}

	g_free(objects);
}

void 
midgard_test_replicator_object_delete (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);

	return;
}

void midgard_test_replicator_export_created (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);

	if (!midgard_object_class_has_metadata (MIDGARD_OBJECT_GET_CLASS (object)))
		return;

	/* Initialize GValue of MIDGARD_TYPE_TIMESTAMP */
	GValue tval = {0, };
	g_value_init (&tval, MIDGARD_TYPE_TIMESTAMP);
	g_object_get_property (G_OBJECT (object->metadata), "exported", &tval);

	/* And convert it to string */
	GValue strval = {0, };
	g_value_init (&strval, G_TYPE_STRING);
	g_value_transform (&tval, &strval);

	g_assert_cmpstr ("0001-01-01 00:00:00+0000", ==, g_value_get_string (&strval));

	g_value_unset (&tval);
	g_value_unset (&strval);

	gboolean object_is_exported = midgard_replicator_export (MIDGARD_DBOBJECT(object));
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (object_is_exported != FALSE);

	g_value_init (&tval, MIDGARD_TYPE_TIMESTAMP);
	g_object_get_property (G_OBJECT (object->metadata), "exported", &tval);

	g_value_init (&strval, G_TYPE_STRING);
	g_value_transform (&tval, &strval);

	g_assert_cmpstr ("0001-01-01 00:00:00+0000", !=, g_value_get_string (&strval));

	g_value_unset (&tval);
	g_value_unset (&strval);
}

void midgard_test_replicator_export_updated (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);

	if (!midgard_object_class_has_metadata (MIDGARD_OBJECT_GET_CLASS (object)))
		return;

	/* Initialize GValue of MIDGARD_TYPE_TIMESTAMP */
	GValue tval = {0, };
	g_value_init (&tval, MIDGARD_TYPE_TIMESTAMP);
	g_object_get_property (G_OBJECT (object->metadata), "exported", &tval);

	/* And convert it to string */
	GValue strval = {0, };
	g_value_init (&strval, G_TYPE_STRING);
	g_value_transform (&tval, &strval);

	g_assert_cmpstr ("0001-01-01 00:00:00+0000", !=, g_value_get_string (&strval));

	g_value_unset (&tval);
	g_value_unset (&strval);

	gboolean object_is_exported = midgard_replicator_export (MIDGARD_DBOBJECT(object));
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (object_is_exported != FALSE);

	g_value_init (&tval, MIDGARD_TYPE_TIMESTAMP);
	g_object_get_property (G_OBJECT (object->metadata), "exported", &tval);

	g_value_init (&strval, G_TYPE_STRING);
	g_value_transform (&tval, &strval);

	g_assert_cmpstr ("0001-01-01 00:00:00+0000", !=, g_value_get_string (&strval));

	g_value_unset (&tval);
	g_value_unset (&strval);
}

void 
midgard_test_replicator_prepare_dirs (void)
{
	gint replicator_archive_spool_dir_dependent = g_mkdir_with_parents (MIDGARD_TEST_REPLICATOR_DEPENDENT_DIR, 0770);
	g_assert_cmpint (replicator_archive_spool_dir_dependent, >, -1);
		
	gint replicator_archive_spool_dir_independent = g_mkdir_with_parents (MIDGARD_TEST_REPLICATOR_INDEPENDENT_DIR, 0770);
	g_assert_cmpint (replicator_archive_spool_dir_independent, >, -1);

	return;
}

static gchar *
_create_xml_path (const gchar *dir, MidgardObject *object)
{
	g_assert (object != NULL);

	gchar *guid = NULL;
	g_object_get (object, "guid", &guid, NULL);

	g_assert (guid != NULL);
	g_assert_cmpstr (guid, !=, "");
	g_assert (midgard_is_guid(guid) != FALSE);

	gchar *filepath = g_build_path (G_DIR_SEPARATOR_S, dir, guid, NULL);

	g_free (guid);

	return filepath;
}

void 
midgard_test_replicator_export_archive (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT (mot);

	gchar *filepath = NULL;
	MidgardObject *parent_object = midgard_object_get_parent (object);
	if (parent_object) {

		filepath = _create_xml_path (MIDGARD_TEST_REPLICATOR_DEPENDENT_DIR, object);
		g_object_unref (parent_object);
	
	} else {

		filepath = _create_xml_path (MIDGARD_TEST_REPLICATOR_INDEPENDENT_DIR, object);
	}

	gchar *object_archive_xml = midgard_replicator_serialize (G_OBJECT(object));
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (object_archive_xml != NULL);
	g_assert_cmpstr (object_archive_xml, !=, "");

	gboolean object_archive_stored = _set_file_contents (filepath, object_archive_xml);
	g_assert (object_archive_stored != FALSE);

	g_free (filepath);
}

void midgard_test_replicator_export_purged(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	/* TODO */
}

void midgard_test_replicator_serialize_blob(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	/* TODO */
}

void midgard_test_replicator_export_blob(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	/* TODO */
}

void midgard_test_replicator_export_by_guid(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	/* TODO */
}

void midgard_test_replicator_export_media(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	/* TODO */
}

void midgard_test_replicator_import_object_already_imported(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);

	if (!midgard_object_class_has_metadata (MIDGARD_OBJECT_GET_CLASS (object)))
		return;

	gboolean midgard_object_imported = midgard_replicator_import_object (MIDGARD_DBOBJECT (object), FALSE);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OBJECT_IMPORTED);
	g_assert (midgard_object_imported == FALSE);
}

void midgard_test_replicator_import_from_xml(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	/* TODO */
}
