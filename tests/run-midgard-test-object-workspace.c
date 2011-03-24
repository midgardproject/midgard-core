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

#include "midgard_test_object_workspace.h"

static MidgardConnection *mgd_global = NULL;

static void 
midgard_test_object_workspace_setup (MidgardObjectWorkspaceTest *mwt, gconstpointer data) 
{ 
	MidgardWorkspace *mw = NULL;
	if (data)
		mw = MIDGARD_WORKSPACE (data); 
	mwt->mgd = mgd_global; 
	mwt->mw = mw; 
}

static void 
midgard_test_object_workspace_teardown_foo (MidgardObjectWorkspaceTest *mwt, gconstpointer data)
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
	midgard_connection_set_loglevel(mgd_global, "debug", NULL);

	MidgardWorkspace *mw = NULL;

	g_test_add("/midgard_object/workspace/create", MidgardObjectWorkspaceTest, mw, midgard_test_object_workspace_setup,  
			midgard_test_object_workspace_create, midgard_test_object_workspace_teardown_foo);
	g_test_add("/midgard_object/workspace/select_created", MidgardObjectWorkspaceTest, mw, midgard_test_object_workspace_setup,  
			midgard_test_object_workspace_select_created, midgard_test_object_workspace_teardown_foo);
	g_test_add("/midgard_object/workspace_context/select_created", MidgardObjectWorkspaceTest, mw, midgard_test_object_workspace_setup,  
			midgard_test_object_workspace_context_select_created, midgard_test_object_workspace_teardown_foo);
	g_test_add("/midgard_object/workspace/update", MidgardObjectWorkspaceTest, mw, midgard_test_object_workspace_setup,  
			midgard_test_object_workspace_update, midgard_test_object_workspace_teardown_foo);
	g_test_add("/midgard_object/workspace_context/update", MidgardObjectWorkspaceTest, mw, midgard_test_object_workspace_setup,  
			midgard_test_object_workspace_context_update, midgard_test_object_workspace_teardown_foo);
	g_test_add("/midgard_object/workspace/select_updated", MidgardObjectWorkspaceTest, mw, midgard_test_object_workspace_setup,  
			midgard_test_object_workspace_select_updated, midgard_test_object_workspace_teardown_foo);
	g_test_add("/midgard_object/workspace/delete", MidgardObjectWorkspaceTest, mw, midgard_test_object_workspace_setup,  
			midgard_test_object_workspace_delete, midgard_test_object_workspace_teardown_foo);

	/* Finalize */
	///_MGD_TEST_UNREF_GOBJECT(user)
	//_MGD_TEST_UNREF_SCHEMA
	//_MGD_TEST_UNREF_MIDGARD

	return g_test_run();
}
