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
#include "midgard_test_object_tree.h"

static MidgardConnection *mgd_global = NULL;

_MGD_TEST_OBJECT_SETUP
_MGD_TEST_OBJECT_TEARDOWN

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);
	g_test_bug_base (MIDGARD_TRAC_TICKET_BASE_URL);

	midgard_init();

	/* FIXME, it should be fixed with API... */
	MidgardSchema *schema = g_object_new(MIDGARD_TYPE_SCHEMA, NULL);
	midgard_schema_init(schema, NULL);
	midgard_schema_read_dir(schema, NULL);

	guint n_types, i;
	const gchar *typename;
	gchar *testname = NULL;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);

	MidgardConfig *config = NULL;
	mgd_global = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	MidgardUser *user = midgard_user_quick_login (mgd_global, "admin", "password");
	g_assert(user != NULL);

	for(i = 0; i < n_types; i++) {

		typename = g_type_name(all_types[i]);	
		
		if(g_str_equal(typename, "midgard_attachment")
				|| g_str_equal(typename, "midgard_parameter"))
			continue;

		MgdObject *object = midgard_test_object_basic_new(mgd_global, typename, NULL);
		g_assert(object != NULL);

		testname = g_strconcat("/midgard_object/tree/", typename, "/basic", NULL);
		g_test_add(testname, MgdObjectTest, object, midgard_test_setup,  
				midgard_test_object_tree_basic, midgard_test_teardown_foo);
		g_free(testname);

		testname = g_strconcat("/midgard_object/tree/", typename, "/create", NULL);
		g_test_add(testname, MgdObjectTest, object, midgard_test_setup,  
				midgard_test_object_tree_create, midgard_test_teardown_foo);
		g_free(testname);	

		testname = g_strconcat("/midgard_object/tree/", typename, "/get_parent", NULL);
		g_test_add(testname, MgdObjectTest, object, midgard_test_setup,  
				midgard_test_object_tree_get_parent, midgard_test_teardown_foo);
		g_free(testname);	

		_MGD_TEST_UNREF_MGDOBJECT(object)
	}

	g_free(all_types);

	/* Finalize */
	_MGD_TEST_UNREF_GOBJECT(config)
	_MGD_TEST_UNREF_GOBJECT(user)
	_MGD_TEST_UNREF_SCHEMA
	_MGD_TEST_UNREF_MIDGARD

	return g_test_run();
}
