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

#include "midgard_test_object_fetch.h"

/* DO NOT fail inside this function.
 * Main purpose is to get (or not) object intentionaly.
 * If it fails, return FALSE and fail with TRUE OR FALSE assertion 
 * depending on actual need */
gboolean midgard_test_object_fetch_by_id(MidgardObject *object, guint id)
{
	g_assert(object != NULL);

	MidgardConnection *mgd = (MidgardConnection *) midgard_object_get_connection(object);
	gboolean fetched = midgard_object_get_by_id(object, id);

	if(fetched == FALSE) {

		MIDGARD_TEST_ERROR_ASSERT(mgd, MGD_ERR_NOT_EXISTS);
		g_assert(fetched == FALSE);

		return FALSE;
	}

	MIDGARD_TEST_ERROR_OK(mgd);
	g_assert(fetched == TRUE);

	return TRUE;
}

void midgard_test_object_get_by_id_created(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *_object = mot->object;
	MidgardConnection *mgd = (MidgardConnection *)midgard_object_get_connection(_object);
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (_object);

	guint oid = 0;
	gchar *oguid = NULL;
	gchar *created = NULL;
	gchar *creator = NULL;
	MidgardMetadata *metadata;
	g_object_get(_object,
			"id", &oid,
			"guid", &oguid,
			"metadata", &metadata,
			NULL);
		
	g_assert_cmpuint(oid, >, 0);

	/* midgard_object_get_by_id */
	MidgardObject *object = midgard_test_object_basic_new(mgd, G_OBJECT_TYPE_NAME(_object), NULL);
	g_assert(object != NULL);
	gboolean fetched_by_id = midgard_test_object_fetch_by_id(object, oid);
	MIDGARD_TEST_ERROR_OK(mgd);
	g_assert(fetched_by_id == TRUE);

	if (midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass))) {

		GValue tval = {0, };
		g_value_init(&tval, MGD_TYPE_TIMESTAMP);
		g_object_get_property(G_OBJECT(metadata), "created", &tval);
		g_object_get(metadata, "creator", &creator, NULL);
		created = midgard_timestamp_get_string_from_value(&tval);

		midgard_test_metadata_check_created(object);
		midgard_test_metadata_check_datetime_properties(object, created, "created", "revised", NULL);
		midgard_test_metadata_check_person_references(object, creator, "creator", "revisor", NULL);

		g_value_unset(&tval);
		g_free(created);
		g_free(creator);
	}
	
	g_object_unref(object);
	g_free(oguid);
}

void midgard_test_object_get_by_id_updated(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *_object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (_object);

	guint oid = 0;
	gchar *oguid = NULL;
	gchar *revised = NULL;
	gchar *revisor = NULL;
	gchar *creator = NULL;
	gchar *created = NULL;
	MidgardMetadata *metadata;
	g_object_get(_object,
			"id", &oid,
			"guid", &oguid,
			"metadata", &metadata,
			NULL);
	
	g_assert_cmpuint(oid, >, 0);

	/* midgard_object_get_by_id */
	MidgardObject *object = midgard_test_object_basic_new(mgd, G_OBJECT_TYPE_NAME(_object), NULL);
	g_assert(object != NULL);
	gboolean fetched_by_id =
		midgard_test_object_fetch_by_id(object, oid);
	g_assert(fetched_by_id == TRUE);
	MIDGARD_TEST_ERROR_OK(mgd);

	if (midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass))) {

		GValue tval = {0, };
		g_value_init(&tval, MGD_TYPE_TIMESTAMP);
		g_object_get_property(G_OBJECT(metadata), "created", &tval);	
		created = midgard_timestamp_get_string_from_value(&tval);
		g_value_unset(&tval);

		g_value_init(&tval, MGD_TYPE_TIMESTAMP);
		g_object_get_property(G_OBJECT(metadata), "revised", &tval);
		revised = midgard_timestamp_get_string_from_value(&tval);
		g_value_unset (&tval);

		g_object_get(metadata,
				"revisor", &revisor, 
				"creator", &creator, 
				NULL);

		midgard_test_metadata_check_updated(object);
		midgard_test_metadata_check_datetime_properties(object, revised, "revised", NULL);
		midgard_test_metadata_check_datetime_properties(object, created, "created", NULL);
		midgard_test_metadata_check_person_references(object, revisor, "revisor", NULL);
		midgard_test_metadata_check_person_references(object, creator, "creator", NULL);

		g_free(revised);
		g_free(revisor);
		g_free(created);
		g_free(creator);
	}

	g_object_unref(object);	
	g_free(oguid);

}

void midgard_test_object_get_by_id_deleted(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *_object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (_object);

	if (!midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass)))
		return;

	guint oid = 0;
	g_object_get(_object,
			"id", &oid,	
			NULL);
	
	g_assert_cmpuint(oid, >, 0);

	/* midgard_object_get_by_id */
	MidgardObject *object = midgard_test_object_basic_new(mgd, G_OBJECT_TYPE_NAME(_object), NULL);
	g_assert(object != NULL);
	gboolean fetched_by_id =
		midgard_test_object_fetch_by_id(object, oid);
	
	g_assert(fetched_by_id == FALSE);
	MIDGARD_TEST_ERROR_ASSERT(mgd, MGD_ERR_NOT_EXISTS);
}

void midgard_test_object_constructor_id_created(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *_object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (_object);
	guint oid = 0;
	gchar *oguid = NULL;
	gchar *created = NULL;
	gchar *creator = NULL;
	MidgardMetadata *metadata;
	g_object_get(_object,
			"id", &oid,
			"guid", &oguid,
			"metadata", &metadata,
			NULL);

	g_assert_cmpuint(oid, >, 0);

	/* midgard_object_new, construct with id */
	GValue gval = {0, };
	g_value_init(&gval, G_TYPE_UINT);
	g_value_set_uint(&gval, oid);
	MidgardObject *object = midgard_test_object_basic_new(mgd, G_OBJECT_TYPE_NAME(_object), &gval);
	g_assert(object != NULL);
	MIDGARD_TEST_ERROR_OK(mgd);

	if (midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass))) {
	
		GValue tval = {0, };
		g_value_init(&tval, MGD_TYPE_TIMESTAMP);
		g_object_get_property(G_OBJECT(metadata), "created", &tval);
		g_object_get(metadata, "creator", &creator, NULL);
		created = midgard_timestamp_get_string_from_value(&tval);
	
		midgard_test_metadata_check_created(object);
		midgard_test_metadata_check_datetime_properties(object, created, "created", "revised", NULL);
		midgard_test_metadata_check_person_references(object, creator, "creator", "revisor", NULL);
	
		g_value_unset(&tval);
		g_free(created);
		g_free(creator);
	}

	g_object_unref(object);
	g_value_unset(&gval);
	g_free(oguid);
}

gboolean midgard_test_object_fetch_by_guid(MidgardObject *object, const gchar *guid)
{
	g_assert(object != NULL);

	MidgardConnection *mgd = MIDGARD_CONNECTION (midgard_object_get_connection(object));
	gboolean fetched = midgard_object_get_by_guid(object, guid);

	if(fetched == FALSE) {

		MIDGARD_TEST_ERROR_ASSERT(mgd, MGD_ERR_NOT_EXISTS);
		g_assert(fetched == FALSE);

		return FALSE;
	}

	MIDGARD_TEST_ERROR_OK(mgd);
	g_assert(fetched == TRUE);

	return TRUE;
}

void midgard_test_object_get_by_guid_created(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *_object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (_object);

	guint oid = 0;
	gchar *oguid = NULL;
	gchar *created = NULL;
	gchar *creator = NULL;
	MidgardMetadata *metadata;
	g_object_get(_object,
			"id", &oid,
			"guid", &oguid,
			"metadata", &metadata,
			NULL);
		
	MidgardObject *object = midgard_test_object_basic_new(mgd, G_OBJECT_TYPE_NAME(_object), NULL);
	g_assert(object != NULL);
	gboolean fetched_by_guid =
		midgard_test_object_fetch_by_guid(object, oguid);
	g_assert(fetched_by_guid == TRUE);
	MIDGARD_TEST_ERROR_OK(mgd);

	if (midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass))) {
	
		GValue tval = {0, };
		g_value_init(&tval, MGD_TYPE_TIMESTAMP);
		g_object_get_property (G_OBJECT(metadata), "created", &tval);
		g_object_get (metadata, "creator", &creator, NULL);
		created = midgard_timestamp_get_string_from_value (&tval);
		
		midgard_test_metadata_check_created(object);
		
		g_value_unset(&tval);
		g_free(created);
		g_free(creator);
	}

	g_object_unref(object);
	g_free(oguid);
}

void midgard_test_object_constructor_guid_created(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);

	MidgardObject *_object = mot->object;
	MidgardConnection *mgd = mot->mgd;
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS (_object);
	guint oid = 0;
	gchar *oguid = NULL;
	gchar *created = NULL;
	gchar *creator = NULL;
	MidgardMetadata *metadata;
	g_object_get(_object,
			"id", &oid,
			"guid", &oguid,
			"metadata", &metadata, 
			NULL);

	g_assert_cmpuint(oid, >, 0);

	GValue gval = {0, };
	g_value_init(&gval, G_TYPE_STRING);
	g_value_set_string(&gval, oguid);
	MidgardObject *object = midgard_test_object_basic_new(mgd, G_OBJECT_TYPE_NAME(_object), &gval);
	g_assert(object != NULL);

	if (midgard_reflector_object_has_metadata_class (G_OBJECT_CLASS_NAME (klass))) {
		
		GValue tval = {0, };
		g_value_init(&tval, MGD_TYPE_TIMESTAMP);
		g_object_get_property(G_OBJECT(metadata), "created", &tval);
		g_object_get(metadata, "creator", &creator, NULL);
		created = midgard_timestamp_get_string_from_value(&tval);
	
		midgard_test_metadata_check_created(object);
		midgard_test_metadata_check_datetime_properties(object, created, "created", "revised", NULL);
		midgard_test_metadata_check_person_references(object, creator, "creator", "revisor", NULL);
	
		g_value_unset(&tval);
		g_free(created);
		g_free(creator);
	}

	g_object_unref(object);
	g_value_unset(&gval);
	g_free(oguid);
}

void midgard_test_object_fetch_run(void)
{
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = 
		midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	g_assert(mgd != NULL);

	MidgardObject *object = 
		midgard_test_object_basic_new(mgd, "midgard_article", NULL);
	
	gboolean fetched_by_id = 
		midgard_test_object_fetch_by_id(object, 0);
	g_assert(fetched_by_id == FALSE);

	gboolean fetched_by_guid = 
		midgard_test_object_fetch_by_guid(object, "1dd6d5e36ae2dfc6d5e110notexists2a793640c240c2");
	g_assert(fetched_by_guid == FALSE);
}
