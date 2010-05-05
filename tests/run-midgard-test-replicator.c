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
#include <glib/gprintf.h>

static MidgardConnection *mgd_global = NULL;

_MGD_TEST_OBJECT_SETUP
_MGD_TEST_OBJECT_TEARDOWN

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);
	
	midgard_init();

	g_log_set_always_fatal(G_LOG_LEVEL_WARNING);
	g_log_set_fatal_mask("GLib-GObject", G_LOG_LEVEL_CRITICAL);

	/* FIXME, it should be fixed with API... */
	MidgardSchema *schema = g_object_new(MIDGARD_TYPE_SCHEMA, NULL);
	midgard_schema_init(schema, NULL);
	midgard_schema_read_dir(schema, NULL);

	guint n_types, i;
	const gchar *typename;
	gchar *testname;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);

	MidgardConfig *config = NULL;
	guint loghandler =
		g_log_set_handler("midgard-core", G_LOG_LEVEL_MASK, midgard_error_default_log, NULL);
	mgd_global = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);	
	MidgardUser *user = midgard_user_quick_login (mgd_global, "admin", "password");
	g_assert(user != NULL);

	midgard_connection_enable_replication (mgd_global, TRUE);
	midgard_connection_enable_dbus (mgd_global, TRUE);
	midgard_connection_enable_quota (mgd_global, TRUE);

	midgard_test_replicator_prepare_dirs();

	/* Lock root and its membershipp */
	midgard_test_lock_root_objects(mgd_global, user);

	GObject **objects = g_new(GObject *, n_types+1);
	MidgardObject *object = NULL;

	for (i = 0; i < n_types; i++) {

		typename = g_type_name(all_types[i]);	
		
		if(g_str_equal(typename, "midgard_attachment")
				|| g_str_equal(typename, "midgard_parameter"))
			continue;

		object = midgard_test_object_basic_new(mgd_global, typename, NULL);
		g_assert(object != NULL);

		testname = g_strconcat("/midgard_object/", typename, "/create", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_create, midgard_test_teardown_foo);
		g_free(testname);

		objects[i] = G_OBJECT(object);
	}

	objects[i] = NULL;
	
	i = 0;
	while (objects[i] != NULL) {

		typename = g_type_name(all_types[i]);
		
		if(g_str_equal(typename, "midgard_attachment")
				|| g_str_equal(typename, "midgard_parameter")) {
			i++;
			continue;
		}

		object = MIDGARD_OBJECT(objects[i]);

		testname = g_strconcat("/midgard_replicator/", typename, "/update_object_links", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_update_object_links, midgard_test_teardown_foo);
		g_free(testname);

		i++;
	}

	i = 0;
	while (objects[i] != NULL) {

		typename = g_type_name(all_types[i]);
		
		if(g_str_equal(typename, "midgard_attachment")
				|| g_str_equal(typename, "midgard_parameter")) {
			i++;
			continue;
		}

		object = MIDGARD_OBJECT(objects[i]);

		testname = g_strconcat("/midgard_replicator/", typename, "/serialize_created", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_serialize, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_replicator/", typename, "/unserialize_created", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_unserialize, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_replicator/", typename, "/export_created", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_export_created, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_replicator/", typename, "/import_already_imported", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_import_object_already_imported, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_replicator/", typename, "/update", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_update, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_replicator/", typename, "/serialize_updated", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_serialize, midgard_test_teardown_foo);
		g_free(testname); 

		testname = g_strconcat("/midgard_replicator/", typename, "/unserialize_updated", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_unserialize, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_replicator/", typename, "/export_updated", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_export_updated, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_replicator/", typename, "/export_archive", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_export_archive, midgard_test_teardown_foo);
		g_free(testname);

		/*	
		testname = g_strconcat("/midgard_replicator/", typename, "/delete", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_object_basic_delete, midgard_test_teardown_foo);
		g_free(testname);
		
		testname = g_strconcat("/midgard_replicator/", typename, "/serialize_deleted", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_serialize, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_replicator/", typename, "/unserialize_deleted", NULL);
		g_test_add(testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_replicator_serialize, midgard_test_teardown_foo);
		g_free(testname);
		*/

		/*
		testname = g_strconcat("/midgard_object_class/", typename, "/purge", NULL);
		g_test_add(testname, MidgardObjectTest, undelete_object, midgard_test_setup,  
				midgard_test_object_basic_purge, midgard_test_teardown_foo);
		g_free(testname); */ 

		//_MGD_TEST_UNREF_MGDOBJECT(undelete_object) 

		i++;
	}

	//g_free(objects);
	g_free(all_types);

	/* Finalize */
	_MGD_TEST_UNREF_GOBJECT(user)
	_MGD_TEST_UNREF_SCHEMA
	_MGD_TEST_UNREF_MIDGARD

	return g_test_run();
}
