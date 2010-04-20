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

#include "midgard_test.h"

static MidgardConnection *mgd_global = NULL;

_MGD_TEST_OBJECT_SETUP
//_MGD_TEST_OBJECT_TEARDOWN

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);

	g_test_bug_base (MIDGARD_TRAC_TICKET_BASE_URL);

	g_test_add_func("/midgard_object/basic", midgard_test_object_basic_run);
	
	midgard_init();

	/* FIXME, it should be fixed with API... */
	MidgardSchema *schema = g_object_new(MIDGARD_TYPE_SCHEMA, NULL);
	midgard_schema_init(schema, NULL);
	midgard_schema_read_dir(schema, NULL);

	guint n_types, i;
	const gchar *typename;	
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);

	MidgardConfig *config = NULL;
	/* guint loghandler =
		g_log_set_handler("midgard-core", G_LOG_LEVEL_MASK, midgard_error_default_log, NULL); */
	mgd_global = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	/* midgard_connection_set_loglevel(mgd_global, "debug", NULL); */
	MidgardUser *user = midgard_user_quick_login (mgd_global, "admin", "password");
	g_assert(user != NULL);

	/* Lock root and its membershipp */
	midgard_test_lock_root_objects(mgd_global, user);

	for(i = 0; i < n_types; i++) {

		typename = g_type_name(all_types[i]);	
		
		if(g_str_equal(typename, "midgard_attachment")
				|| g_str_equal(typename, "midgard_parameter"))
			continue;

		MidgardObject *object = midgard_test_object_basic_new(mgd_global, typename, NULL);
		g_assert(object != NULL);

		gchar *testname = g_strconcat("/midgard_reflection_property/new/", typename, NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_property_reflector_new, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_reflection_property/get_midgard_type/", typename, NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_property_reflector_check_midgard_type, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_reflection_property/is_link/", typename, NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_property_reflector_check_is_link, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_reflection_property/is_linked/", typename, NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_property_reflector_check_is_linked, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_reflection_property/get_link_class/", typename, NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_property_reflector_check_link_class, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_reflection_property/get_link_name/", typename, NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_property_reflector_check_link_name, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_reflection_property/get_link_target/", typename, NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_property_reflector_check_link_target, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_reflection_property/description/", typename, NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_property_reflector_check_description, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_object/", typename, "/create", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_create, midgard_test_teardown_foo);
		g_free(testname);

		//testname = g_strconcat("/midgard_replicator/", typename, "/serialize", NULL);
		//g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
		//		midgard_test_replicator_serialize, midgard_test_teardown_foo);
		//g_free(testname);

		testname = g_strconcat("/midgard_object/", typename, "/get_by_id_created", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_get_by_id_created, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_object/", typename, "/get_by_guid_created", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_get_by_guid_created, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_object/", typename, "/constructor_id_created", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_constructor_id_created, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_object/", typename, "/constructor_guid_created", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_constructor_guid_created, midgard_test_teardown_foo);
		g_free(testname);
	
		testname = g_strconcat("/midgard_object/", typename, "/update", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_update, midgard_test_teardown_foo);
		g_free(testname);

		/*
		testname = g_strconcat("/midgard_replicator/", typename, "/serialize", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_serialize, midgard_test_teardown_foo);
		g_free(testname); */
	
		testname = g_strconcat("/midgard_object/", typename, "/get_by_id_updated", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_get_by_id_updated, midgard_test_teardown_foo);
		g_free(testname);
	
		testname = g_strconcat("/midgard_object/", typename, "/lock", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_lock, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_object/", typename, "/unlock", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_unlock, midgard_test_teardown_foo);
		g_free(testname); 
	
		testname = g_strconcat("/midgard_object/", typename, "/approve", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_approve, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_object/", typename, "/unapprove", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_unapprove, midgard_test_teardown_foo);
		g_free(testname); 

		testname = g_strconcat("/midgard_object/", typename, "/delete", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_delete, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_schema_object_factory/", typename, "/get_object_by_guid_deleted", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_schema_object_factory_get_object_by_guid_deleted, midgard_test_teardown_foo);
		g_free(testname);
	
		/*testname = g_strconcat("/midgard_replicator/", typename, "/serialize", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_serialize, midgard_test_teardown_foo);
		g_free(testname);*/

		// unref object 
		testname = g_strconcat("/midgard_object/", typename, "/get_by_id_deleted", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_get_by_id_deleted, midgard_test_unref_object);
		g_free(testname);

		MidgardObject *undelete_object = midgard_test_object_basic_new(mgd_global, typename, NULL);
		g_assert(undelete_object != NULL);
		
		testname = g_strconcat("/midgard_object_class/", typename, "/undelete", NULL);
		g_test_add(testname, MidgardObjectTest, undelete_object, midgard_test_setup,  
				midgard_test_object_class_undelete, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_object_class/", typename, "/purge", NULL);
		g_test_add(testname, MidgardObjectTest, undelete_object, midgard_test_setup,  
				midgard_test_object_basic_purge, midgard_test_teardown_foo);
		g_free(testname); 

		_MGD_TEST_UNREF_MGDOBJECT(undelete_object) 
	}

	g_free(all_types);

	/* Finalize */
	_MGD_TEST_UNREF_GOBJECT(user)
	_MGD_TEST_UNREF_SCHEMA
	_MGD_TEST_UNREF_MIDGARD

	return g_test_run();
}
