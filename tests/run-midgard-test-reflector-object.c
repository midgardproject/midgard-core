/* 
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_test_reflector_object.h"

static MidgardConnection *mgd_global = NULL;

static void 
midgard_test_reflector_object_setup (MidgardReflectorObjectTest *mwt, gconstpointer data) 
{ 
	mwt->mgd = mgd_global; 
}

static void 
midgard_test_reflector_object_teardown_foo (MidgardReflectorObjectTest *mwt, gconstpointer data)
{
	return;
}

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);	

	midgard_init();

	MidgardConfig *config = NULL;
	guint loghandler =
		g_log_set_handler("midgard-core", G_LOG_LEVEL_MASK, midgard_error_default_log, NULL);
	mgd_global = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	midgard_connection_set_loglevel(mgd_global, "debug", NULL);

	g_test_add("/midgard_reflector_object/is_mixin", 
			MidgardReflectorObjectTest, NULL, 
			midgard_test_reflector_object_setup,  
			midgard_test_reflector_object_is_mixin,
			midgard_test_reflector_object_teardown_foo);
	g_test_add("/midgard_reflector_object/is_interface", 
			MidgardReflectorObjectTest, NULL, 
			midgard_test_reflector_object_setup,  
			midgard_test_reflector_object_is_interface, 
			midgard_test_reflector_object_teardown_foo);
	g_test_add("/midgard_reflector_object/is_abstract", 
			MidgardReflectorObjectTest, NULL, 
			midgard_test_reflector_object_setup,  
			midgard_test_reflector_object_is_abstract, 
			midgard_test_reflector_object_teardown_foo);
	g_test_add("/midgard_reflector_object/list_defined_properties", 
			MidgardReflectorObjectTest, NULL, 
			midgard_test_reflector_object_setup,  
			midgard_test_reflector_object_list_defined_properties, 
			midgard_test_reflector_object_teardown_foo);

	/* Finalize */
	///_MGD_TEST_UNREF_GOBJECT(user)
	//_MGD_TEST_UNREF_SCHEMA
	//_MGD_TEST_UNREF_MIDGARD

	return g_test_run();
}
