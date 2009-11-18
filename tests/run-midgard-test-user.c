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

static void 
midgard_test_user_setup (MidgardUserTest *mut, gconstpointer data) 
{ 
	MidgardUser *user = MIDGARD_USER (data); 
	mut->mgd = mgd_global; 
	mut->user = user; 
}

static void 
midgard_test_user_teardown_foo (MidgardUserTest *mut, gconstpointer data)
{
	return;
}

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);	
	
	midgard_init();

	/* FIXME, it should be fixed with API... */
	MidgardSchema *schema = g_object_new(MIDGARD_TYPE_SCHEMA, NULL);
	midgard_schema_init(schema, NULL);
	midgard_schema_read_dir(schema, NULL);

	MidgardConfig *config = NULL;
	guint loghandler =
		g_log_set_handler("midgard-core", G_LOG_LEVEL_MASK, midgard_error_default_log, NULL);
	mgd_global = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	//midgard_connection_set_loglevel(mgd_global, "debug", NULL);

	MidgardUser *user = midgard_user_quick_login (mgd_global, "admin", "password");
	g_assert(user != NULL);

	g_test_add("/midgard_user/create", MidgardUserTest, user, midgard_test_user_setup,  
			midgard_test_user_create, midgard_test_user_teardown_foo);
	g_test_add("/midgard_user/update", MidgardUserTest, user, midgard_test_user_setup,  
			midgard_test_user_update, midgard_test_user_teardown_foo);
	g_test_add("/midgard_user/get", MidgardUserTest, user, midgard_test_user_setup,  
			midgard_test_user_get, midgard_test_user_teardown_foo);
	g_test_add("/midgard_user/query", MidgardUserTest, user, midgard_test_user_setup,  
			midgard_test_user_query, midgard_test_user_teardown_foo);
	g_test_add("/midgard_user/login", MidgardUserTest, user, midgard_test_user_setup,  
			midgard_test_user_login, midgard_test_user_teardown_foo);
	g_test_add("/midgard_user/logout", MidgardUserTest, user, midgard_test_user_setup,  
			midgard_test_user_logout, midgard_test_user_teardown_foo);

	/* Finalize */
	///_MGD_TEST_UNREF_GOBJECT(user)
	//_MGD_TEST_UNREF_SCHEMA
	//_MGD_TEST_UNREF_MIDGARD

	return g_test_run();
}
