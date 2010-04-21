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

#include "midgard_test_object_basic.h"
#include "midgard_test_query_builder.h"
#include <midgard/uuid.h>

MidgardObject *midgard_test_object_basic_new(MidgardConnection *mgd, const gchar *name, GValue *value)
{
	MidgardObject *object = midgard_object_new(mgd, name, value);
	g_assert(object != NULL);
	MIDGARD_TEST_ERROR_OK(mgd);

	guint id = 0;
	gchar *guid = NULL;

	g_object_get(object, 
			"id", &id,
			"guid", &guid, NULL);

	if(value == NULL) {
	
		g_assert_cmpint(id, ==, 0);
		g_assert_cmpstr(guid, ==, NULL);
		g_assert_cmpint(id, !=, 1);
		g_assert_cmpstr(guid, !=, "");
	
	} else {

		g_assert_cmpint(id, !=, 0);
		g_assert_cmpstr(guid, !=, NULL);
		g_assert_cmpint(id, !=, 0);
		g_assert_cmpstr(guid, !=, "");
		g_assert(midgard_is_guid(guid) != FALSE);
	}

	g_free(guid);

	midgard_test_object_basic_check_properties(object);
        
	if(value == NULL)
		midgard_test_metadata_check_new(object);

	return object;
}

MidgardObject *midgard_test_object_basic_new_by_guid(MidgardConnection *mgd, const gchar *name, const gchar *guid)
{
	g_assert(mgd != NULL);
	g_assert(name != NULL);
	g_assert(guid != NULL);

	GValue val = {0, };
	g_value_init(&val, G_TYPE_STRING);
	g_value_set_string(&val, guid);

	MidgardObject *object = midgard_test_object_basic_new(mgd, name, &val);

	g_value_unset(&val);

	return object;
}

void midgard_test_object_basic_create(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *object = mot->object;	
	MidgardConnection *mgd = mot->mgd;

	GParamSpec *pspec = 
		g_object_class_find_property(G_OBJECT_GET_CLASS(G_OBJECT(object)), "name");
	
	/* Do not check duplicate here, we do this in tree tests */
	if(pspec != NULL) {

		GString *nstr = g_string_new("");
		g_string_append_printf(nstr, "%s%d", MGD_TEST_OBJECT_NAME, g_random_int());
		g_object_set(object, "name", nstr->str, NULL);
		g_string_free(nstr, TRUE);
	}

	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS(object);
	const gchar *parent_property = midgard_reflector_object_get_property_parent (G_OBJECT_CLASS_NAME (klass));
	gboolean object_created = FALSE;

	if (parent_property) {

		/* handle the case when parent property is invalid */
		gboolean object_created = midgard_object_create(object);

		MIDGARD_TEST_ERROR_ASSERT(mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
		g_assert(object_created != TRUE);

		/* Set parent property */
		GParamSpec *pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), parent_property);
		if (pspec->value_type == G_TYPE_STRING)
			g_object_set(object, parent_property, midgard_uuid_new(), NULL);
		else 
			g_object_set(object, parent_property, 1, NULL);
	}

	object_created = midgard_object_create(object);

	MIDGARD_TEST_ERROR_OK(mgd);
	g_assert(object_created != FALSE);

	gchar *guid = NULL;
	g_object_get(object, "guid", &guid, NULL);
	g_assert(guid != NULL);
	g_assert_cmpstr(guid, !=, "");
	g_assert(midgard_is_guid(guid) != FALSE);
	g_free(guid);

	/* Check known metadata properties */
	midgard_test_metadata_check_create(object);

	MidgardUser *user = midgard_connection_get_user(mgd);
	g_assert(user != NULL);

	MidgardObject *person = midgard_user_get_person(user);
	g_assert(person != NULL);

	gchar *pguid = NULL;
	g_object_get(person, "guid", &pguid, NULL);
	g_assert_cmpstr(pguid, !=, "");
	g_assert_cmpstr(pguid, !=, NULL);

	/* Check known properties' values */
	midgard_test_metadata_check_person_references(object, 
			pguid, "creator", "revisor", NULL);

	g_free(pguid);

	return;
}

void midgard_test_object_basic_update(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	_MGD_TEST_MOT (mot);

	gboolean object_updated = midgard_object_update (object);

	g_assert (object_updated == TRUE);
	MIDGARD_TEST_ERROR_OK (mgd);
	
	/* Check known metadata properties */
	midgard_test_metadata_check_update(object);

	MidgardUser *user = midgard_connection_get_user(mgd);
	g_assert(user != NULL);

	MidgardObject *person = midgard_user_get_person(user);
	g_assert(person != NULL);

	gchar *pguid = NULL;
	g_object_get(person, "guid", &pguid, NULL);
	g_assert_cmpstr(pguid, !=, "");
	g_assert_cmpstr(pguid, !=, NULL);

	/* Check known properties' values */
	midgard_test_metadata_check_person_references(object, 
			pguid, "creator", "revisor", NULL);

	g_free(pguid);

	return;
}

void midgard_test_object_basic_delete(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *object = mot->object;	
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);

	if (!midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass)))
		return;

	gboolean object_deleted = midgard_object_delete(object);

	MIDGARD_TEST_ERROR_OK(mgd);
	g_assert(object_deleted == TRUE);
	
	/* Check known metadata properties */
	midgard_test_metadata_check_delete(object);

	MidgardUser *user = midgard_connection_get_user(mgd);
	g_assert(user != NULL);

	MidgardObject *person = midgard_user_get_person(user);
	g_assert(person != NULL);

	gchar *pguid = NULL;
	g_object_get(person, "guid", &pguid, NULL);
	g_assert_cmpstr(pguid, !=, "");
	g_assert_cmpstr(pguid, !=, NULL);

	/* Check known properties' values */
	/* FIXME: midgard_test_metadata_check_person_references(object, 
			pguid, "creator", "revisor", NULL);
			*/

	g_free(pguid);

	return;
}

void midgard_test_object_basic_purge(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	g_assert(mot->object != NULL);
	MidgardConnection *mgd = mot->mgd;
	const gchar *class_name = G_OBJECT_TYPE_NAME(mot->object);
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (mot->object);

	if (!midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass)))
		return;

	GObject **objects = midgard_test_query_builder_list_all_unlocked(mgd, class_name);
	g_assert(objects != NULL);

	guint i = 0;
	while(objects[i] != NULL) {
		
		MidgardObject *object = MIDGARD_OBJECT(objects[i]);
		gchar *guid;
		g_object_get(object, "guid", &guid, NULL);

		gboolean purged = midgard_object_purge(object);
		g_assert(purged == TRUE);

		MidgardObject *purged_object = midgard_object_new(mgd, G_OBJECT_TYPE_NAME(mot->object), NULL);
		gboolean get_purged = midgard_object_get_by_guid(purged_object, guid);
		g_free(guid);

		g_assert(get_purged == FALSE);
		MIDGARD_TEST_ERROR_ASSERT(mgd, MGD_ERR_NOT_EXISTS);

		g_object_unref(object);
		g_object_unref(purged_object);

		i++;
	}

	g_free(objects);
}

void midgard_test_object_basic_check_properties(MidgardObject *object)
{
	g_assert(object != NULL);

	guint number_of_properties = 0;
	GParamSpec **props = 
		g_object_class_list_properties(G_OBJECT_GET_CLASS(G_OBJECT(object)), &number_of_properties);

	g_assert_cmpint(number_of_properties, >, 4);

	guint i;
	GValue value = {0, };

	for(i = 0; i < number_of_properties; i++)
	{
		g_value_init(&value, props[i]->value_type);

		switch(G_TYPE_FUNDAMENTAL(props[i]->value_type))
		{
			case G_TYPE_STRING:
				g_assert_cmpstr(g_value_get_string(&value), ==, NULL);
				break;

			case G_TYPE_INT:
				g_assert_cmpint(g_value_get_int(&value), ==, 0);
				break;

			case G_TYPE_UINT:
				g_assert_cmpuint(g_value_get_uint(&value), ==, 0);
				break;

			case G_TYPE_FLOAT:
				g_assert_cmpfloat(g_value_get_float(&value), ==, 0);
				break;

			case G_TYPE_BOOLEAN:
				g_assert(g_value_get_boolean(&value) == FALSE);
				break;
		}

		g_value_unset(&value);
	}

	g_free(props);

	return;
}

void midgard_test_object_basic_run(void)
{
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = 
		midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	guint n_types, i;
	const gchar *typename;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);
	
	/* Fail, if we have less than 10 classes */
	g_assert_cmpint(n_types, >, 10);

	for(i = 0; i < n_types; i++) {
		
		typename = g_type_name(all_types[i]);
		MidgardObject *object = 
			midgard_test_object_basic_new(mgd, typename, NULL);

		g_assert(object != NULL);
	}

	g_free(all_types);

	return;
}

void midgard_test_object_basic_create_run(void)
{
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = 
		midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	guint n_types, i;
	const gchar *typename;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);
	
	/* Fail, if we have less than 10 classes */
	g_assert_cmpint(n_types, >, 10);

	MidgardUser *user = midgard_user_quick_login (mgd, "admin", "password");
	g_assert(user != NULL);

	for(i = 0; i < n_types; i++) {

		typename = g_type_name(all_types[i]);
		/* We can not test midgard_parameter and midgard_attachment here */
		/* Test will fail as parentguid is not defined */
		if(g_str_equal(typename, "midgard_attachment")
				|| g_str_equal(typename, "midgard_parameter"))
			continue;

		MidgardObject *object = midgard_test_object_basic_new(mgd, typename, NULL);
		g_assert(object != NULL);
		//midgard_test_object_basic_create(object);	
		//g_test_add_data_func("/midgard_object/basic/create/new", object, midgard_test_object_basic_create);
		
	}	
	g_object_unref(user);
	g_object_unref(mgd);

	g_free(all_types);

	return;
}

void midgard_test_object_basic_update_run(void)
{
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = 
		midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	guint n_types, i;
	const gchar *typename;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);
	
	/* Fail, if we have less than 10 classes */
	g_assert_cmpint(n_types, >, 10);

	for(i = 0; i < n_types; i++) {
		
		typename = g_type_name(all_types[i]);
		/*gboolean object_updated = 
			midgard_test_object_basic_update(mgd, typename, NULL);

		g_assert(object_updated != FALSE); */
	}

	g_object_unref(mgd);
	g_free(all_types);

	return;
}

void midgard_test_object_basic_lock(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);
	MidgardMetadata *metadata;

	if (!midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass)))
		return;

	g_object_get (object, "metadata", &metadata, NULL);

	gboolean object_islocked = midgard_object_is_locked(object);
	g_assert(object_islocked != TRUE);

	gboolean locked_value = midgard_object_lock(object);
	g_assert(locked_value != FALSE);

	gboolean locked_property;
	g_object_get(metadata, "islocked", &locked_property, NULL);
	g_assert(locked_property != FALSE);

	object_islocked = midgard_object_is_locked(object);
	g_assert(object_islocked != FALSE);

	gchar *pguid = midgard_test_get_current_person_guid(mgd);
	midgard_test_metadata_check_person_references(object,
			pguid, "locker", "revisor", NULL);

	gchar *revised = NULL;
	GValue tval = {0, };
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	revised = midgard_timestamp_get_string_from_value(&tval);
	midgard_test_metadata_check_datetime_properties(object,                      
			revised, "locked", "revised", NULL);

	/* get fresh object */
	gchar *oguid = NULL;
	g_object_get(object, "guid", &oguid, NULL);
	MidgardObject *locked_object = 
		midgard_test_object_basic_new_by_guid(mgd, G_OBJECT_TYPE_NAME(object), oguid);
	g_assert(locked_object != NULL);
	
	midgard_test_metadata_check_person_references(locked_object,
			pguid, "locker", "revisor", NULL);

	midgard_test_metadata_check_datetime_properties(locked_object,                      
			revised, "locked", "revised", NULL);

	object_islocked = midgard_object_is_locked(locked_object);
	g_assert(object_islocked != FALSE);

	g_object_unref(locked_object);
	g_value_unset(&tval);
	g_free(revised);
	g_free(oguid);
	g_free(pguid);
}

void midgard_test_object_basic_unlock(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);

	if (!midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass)))
		return;

	MidgardMetadata *metadata;
	g_object_get (object, "metadata", &metadata, NULL);

	gboolean object_islocked = midgard_object_is_locked(object);
	g_assert(object_islocked != FALSE);

	gboolean unlocked_object = midgard_object_unlock(object);
	g_assert(unlocked_object != FALSE);

	gboolean locked_property;
	g_object_get(metadata, "islocked", &locked_property, NULL);
	g_assert(locked_property != TRUE);

	object_islocked = midgard_object_is_locked(object);
	g_assert(object_islocked != TRUE);

	gchar *pguid = midgard_test_get_current_person_guid(mgd);
	midgard_test_metadata_check_person_references(object,
			pguid, "locker", "revisor", NULL);

	gchar *revised = NULL;
	GValue tval = {0, };
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	revised = midgard_timestamp_get_string_from_value(&tval);
	midgard_test_metadata_check_datetime_properties(object,                      
			revised, "locked", "revised", NULL);

	/* get fresh object */
	gchar *oguid = NULL;
	g_object_get(object, "guid", &oguid, NULL);
	MidgardObject *locked_object = 
		midgard_test_object_basic_new_by_guid(mgd, G_OBJECT_TYPE_NAME(object), oguid);
	g_assert(locked_object != NULL);
	
	midgard_test_metadata_check_person_references(locked_object,
			pguid, "locker", "revisor", NULL);

	midgard_test_metadata_check_datetime_properties(locked_object,                      
			revised, "locked", "revised", NULL);

	object_islocked = midgard_object_is_locked(locked_object);
	g_assert(object_islocked != TRUE);

	g_object_unref(locked_object);
	g_value_unset(&tval);
	g_free(revised);
	g_free(oguid);
	g_free(pguid);
}

void midgard_test_object_basic_approve(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);

	if (!midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass)))
		return;

	MidgardMetadata *metadata;
	g_object_get (object, "metadata", &metadata, NULL);

	gboolean object_isapproved = midgard_object_is_approved(object);
	g_assert(object_isapproved != TRUE);

	gboolean approve_value = midgard_object_approve(object);
	g_assert(approve_value != FALSE);

	gboolean approved_property;
	g_object_get(metadata, "isapproved", &approved_property, NULL);
	g_assert(approved_property != FALSE);

	object_isapproved = midgard_object_is_approved(object);
	g_assert(object_isapproved != FALSE);

	gchar *pguid = midgard_test_get_current_person_guid(mgd);
	midgard_test_metadata_check_person_references(object,
			pguid, "approver", "revisor", NULL);

	gchar *revised = NULL;
	GValue tval = {0, };
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	revised = midgard_timestamp_get_string_from_value(&tval);
	midgard_test_metadata_check_datetime_properties(object,                      
			revised, "approved", "revised", NULL);

	/* get fresh object */
	gchar *oguid = NULL;
	g_object_get(object, "guid", &oguid, NULL);
	MidgardObject *approved_object = 
		midgard_test_object_basic_new_by_guid(mgd, G_OBJECT_TYPE_NAME(object), oguid);
	g_assert(approved_object != NULL);
	
	midgard_test_metadata_check_person_references(approved_object,
			pguid, "approver", "revisor", NULL);

	midgard_test_metadata_check_datetime_properties(approved_object,                      
			revised, "approved", "revised", NULL);

	object_isapproved = midgard_object_is_approved(approved_object);
	g_assert(object_isapproved != FALSE);

	g_value_unset(&tval);
	g_object_unref(approved_object);
	g_free(revised);
	g_free(oguid);
	g_free(pguid);

}

void midgard_test_object_basic_unapprove(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (object);

	if (!midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass)))
		return;

	MidgardMetadata *metadata;
	g_object_get (object, "metadata", &metadata, NULL);

	gboolean object_isapproved = midgard_object_is_approved(object);
	g_assert(object_isapproved != FALSE);

	gboolean unapprove_value = midgard_object_unapprove(object);
	g_assert(unapprove_value != FALSE);

	gboolean approved_property;
	g_object_get(metadata, "isapproved", &approved_property, NULL);
	g_assert(approved_property != TRUE);

	object_isapproved = midgard_object_is_approved(object);
	g_assert(object_isapproved != TRUE);

	gchar *pguid = midgard_test_get_current_person_guid(mgd);
	midgard_test_metadata_check_person_references(object,
			pguid, "approver", "revisor", NULL);

	gchar *revised = NULL;
	GValue tval = {0, };
	g_value_init(&tval, MGD_TYPE_TIMESTAMP);
	g_object_get_property(G_OBJECT(metadata), "revised", &tval);
	revised = midgard_timestamp_get_string_from_value(&tval);
	midgard_test_metadata_check_datetime_properties(object,                      
			revised, "approved", "revised", NULL);

	/* get fresh object */
	gchar *oguid = NULL;
	g_object_get(object, "guid", &oguid, NULL);
	MidgardObject *unapproved_object = 
		midgard_test_object_basic_new_by_guid(mgd, G_OBJECT_TYPE_NAME(object), oguid);
	g_assert(unapproved_object != NULL);
	
	midgard_test_metadata_check_person_references(unapproved_object,
			pguid, "approver", "revisor", NULL);

	midgard_test_metadata_check_datetime_properties(unapproved_object,                      
			revised, "approved", "revised", NULL);

	object_isapproved = midgard_object_is_approved(unapproved_object);
	g_assert(object_isapproved != TRUE);

	g_value_unset(&tval);
	g_object_unref(unapproved_object);
	g_free(revised);
	g_free(oguid);
	g_free(pguid);
}
