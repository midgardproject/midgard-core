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

#include "midgard_test.h"

static MidgardConnection *mgd_global = NULL;

static void 
midgard_test_workspace_context_setup (MidgardWorkspaceContextTest *mwct, gconstpointer data) 
{ 
	MidgardWorkspaceContext *mwc = NULL;
	if (data)
		mwc = MIDGARD_WORKSPACE_CONTEXT (data); 
	mwct->mgd = mgd_global; 
	mwct->mwc = mwc; 
}

static void 
midgard_test_workspace_context_teardown_foo (MidgardWorkspaceContextTest *mwct, gconstpointer data)
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

	MidgardWorkspaceContext *mwc = NULL;

	g_test_add("/midgard_workspace_context/create", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_create, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/update", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_update, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/purge", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_purge, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/exists", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_exists, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/get_by_path", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_get_by_path, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/get_path", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_get_path, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/list_workspace_names", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_list_workspace_names, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/get_workspace_by_name", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_get_workspace_by_name, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/list_children", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_list_children, midgard_test_workspace_context_teardown_foo);
	g_test_add("/midgard_workspace_context/has_workspace", MidgardWorkspaceContextTest, mwc, midgard_test_workspace_context_setup,  
			midgard_test_workspace_context_has_workspace, midgard_test_workspace_context_teardown_foo);

	/* Finalize */
	///_MGD_TEST_UNREF_GOBJECT(user)
	//_MGD_TEST_UNREF_SCHEMA
	//_MGD_TEST_UNREF_MIDGARD

	return g_test_run();
}
