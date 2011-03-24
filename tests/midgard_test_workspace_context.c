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

void 
midgard_test_workspace_context_create (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	GError *error = NULL;
	MidgardWorkspaceContext *workspace_context = midgard_workspace_context_new ();
	g_assert (workspace_context != NULL);
	g_assert (MIDGARD_IS_WORKSPACE_CONTEXT (workspace_context));
	
	gboolean workspace_context_created = midgard_workspace_manager_create (manager, MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_CONTEXT_PATH, &error);
	g_assert (workspace_context_created == TRUE);
	g_assert (error == NULL);

	g_object_unref (workspace_context);

	/* FAIL */
	MidgardWorkspaceContext *dummy_context = midgard_workspace_context_new ();
	workspace_context_created = midgard_workspace_manager_create (manager, MIDGARD_WORKSPACE_STORAGE (dummy_context), "", &error);
	g_assert (workspace_context_created == FALSE);
	g_assert (error != NULL);
	g_assert (error->code == MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_PATH);

	g_object_unref (dummy_context);
	g_clear_error (&error);
}

void 
midgard_test_workspace_context_update (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	MidgardWorkspaceContext *context = midgard_workspace_context_new ();
	g_assert (context != NULL);
	GError *error = NULL;
	gboolean get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (context), MGD_TEST_WORKSPACE_CONTEXT_PATH, &error);
	g_assert (get_by_path == TRUE);
	g_assert (error == NULL);

	/* COre doesn't update context, FAIL */
	gboolean update = midgard_workspace_manager_update (manager, MIDGARD_WORKSPACE_STORAGE (context), &error);
	g_assert (update == FALSE);
	g_assert (error != NULL);
	g_assert (error->code == MIDGARD_WORKSPACE_STORAGE_ERROR_CONTEXT_VIOLATION);

	g_object_unref (context);

}

void 
midgard_test_workspace_context_purge (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	g_print ("Purge context: %s \n", MISS_IMPL);
}

void 
midgard_test_workspace_context_exists (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	gboolean workspace_context_exists = midgard_workspace_manager_path_exists (manager, MGD_TEST_WORKSPACE_CONTEXT_PATH);
	g_assert (workspace_context_exists == TRUE);

	/* FAIL */
	workspace_context_exists = midgard_workspace_manager_path_exists (manager, "/NOT/EXISTS");
	g_assert (workspace_context_exists == FALSE);

	workspace_context_exists = midgard_workspace_manager_path_exists (manager, "//NOT/EXISTS");
	g_assert (workspace_context_exists == FALSE);

	workspace_context_exists = midgard_workspace_manager_path_exists (manager, "");
	g_assert (workspace_context_exists == FALSE);

	workspace_context_exists = midgard_workspace_manager_path_exists (manager, "/");
	g_assert (workspace_context_exists == FALSE);
}

void 
midgard_test_workspace_context_get_by_path (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	GError *error = NULL;
	MidgardWorkspaceContext *context = midgard_workspace_context_new ();
	g_assert (context != NULL);
	gboolean get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (context), MGD_TEST_WORKSPACE_CONTEXT_PATH, &error);	
	g_assert (get_by_path == TRUE);
	g_assert (error == NULL);

	g_object_unref (context);

	/* FAIL */
	context = midgard_workspace_context_new ();
	g_assert (context != NULL);
	get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (context), "", &error);	
	g_assert (get_by_path == FALSE);
	g_assert (error != NULL);
	g_assert (error->code == MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_PATH);
	
	g_clear_error (&error);
	g_object_unref (context);

	context = midgard_workspace_context_new ();
	g_assert (context != NULL);
	get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (context), "/NOT/EXIST", &error);	
	g_assert (get_by_path == FALSE);
	g_assert (error != NULL);
	g_assert (error->code == MIDGARD_WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS);
	
	g_clear_error (&error);
	g_object_unref (context);
}

void 
midgard_test_workspace_context_get_path (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	GError *error = NULL;
	MidgardWorkspaceContext *workspace_context = midgard_workspace_context_new ();
	g_assert (workspace_context != NULL);

	/* FAIL */
	const gchar *workspace_context_path = midgard_workspace_storage_get_path (MIDGARD_WORKSPACE_STORAGE (workspace_context));
	g_assert_cmpstr (workspace_context_path, ==, NULL);

	/* SUCCESS */
	gboolean get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_CONTEXT_PATH, &error);
	g_assert (get_by_path != FALSE);
	g_assert (error == NULL);

	workspace_context_path = midgard_workspace_storage_get_path (MIDGARD_WORKSPACE_STORAGE (workspace_context));
	g_assert (workspace_context_path != NULL);
	g_assert_cmpstr (workspace_context_path,  !=,  "");
	g_assert_cmpstr (workspace_context_path, ==, MGD_TEST_WORKSPACE_CONTEXT_PATH);

	g_object_unref (workspace_context);
}

void 
midgard_test_workspace_context_list_workspace_names (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	GError *error = NULL;
	MidgardWorkspaceContext *workspace_context = midgard_workspace_context_new ();
	g_assert (workspace_context != NULL);
	gboolean get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_CONTEXT_PATH, &error);
	g_assert (get_by_path != FALSE);
	g_assert (error == NULL);

	guint n_names;
	gchar **workspace_names = midgard_workspace_storage_list_workspace_names (MIDGARD_WORKSPACE_STORAGE (workspace_context), &n_names);
	g_assert (workspace_names != NULL);
	g_assert_cmpint (n_names, !=, 0);

	g_assert_cmpstr (workspace_names[0], ==, MGD_TEST_WORKSPACE_NAME_STABLE);
	g_assert_cmpstr (workspace_names[1], ==, MGD_TEST_WORKSPACE_NAME_TESTING);
	g_assert_cmpstr (workspace_names[2], ==, MGD_TEST_WORKSPACE_NAME_PRIVATE);

	g_object_unref (workspace_context);
	g_free (workspace_names);

	/* FAIL */
	workspace_context = midgard_workspace_context_new ();
	g_assert (workspace_context != NULL);

	workspace_names = midgard_workspace_storage_list_workspace_names (MIDGARD_WORKSPACE_STORAGE (workspace_context), &n_names);
	g_assert (workspace_names == NULL);
	g_assert (n_names == 0);
	
	/* Check if NULL value instead of storage pointer is safe */
	workspace_names = midgard_workspace_storage_list_workspace_names (MIDGARD_WORKSPACE_STORAGE (workspace_context), NULL);
	g_assert (workspace_names == NULL);

	g_object_unref (workspace_context);
}

void 
midgard_test_workspace_context_get_workspace_by_name (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	GError *error = NULL;
	MidgardWorkspaceContext *workspace_context = midgard_workspace_context_new ();
	g_assert (workspace_context != NULL);
	gboolean get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_CONTEXT_PATH, &error);
	g_assert (get_by_path != FALSE);
	g_assert (error == NULL);

	MidgardWorkspaceStorage *workspace = midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_NAME_STABLE);
	g_assert (workspace != NULL);
	g_object_unref (workspace);

	workspace = midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_NAME_TESTING);
	g_assert (workspace != NULL);
	g_object_unref (workspace);

	workspace = midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_NAME_PRIVATE);
	g_assert (workspace != NULL);
	g_object_unref (workspace);

	/* FAIL */
	workspace = midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (workspace_context), "");
	g_assert (workspace == NULL);

	g_object_unref (workspace_context);
}

void 
midgard_test_workspace_context_list_children (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	GError *error = NULL;
	MidgardWorkspaceContext *workspace_context = midgard_workspace_context_new ();
	g_assert (workspace_context != NULL);
	gboolean get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_CONTEXT_PATH, &error);
	g_assert (get_by_path != FALSE);
	g_assert (error == NULL);

	guint n_objects;
	MidgardWorkspaceStorage **children = midgard_workspace_storage_list_children (MIDGARD_WORKSPACE_STORAGE (workspace_context), &n_objects);
	g_assert (children != NULL);
	g_assert_cmpint (n_objects, ==, 3); 

	/* Stable */
	gchar *name;
	g_object_get (children[0], "name", &name, NULL);
	g_assert_cmpstr (name, ==, MGD_TEST_WORKSPACE_NAME_STABLE);
	g_free (name);
	
	/* Testing */	
	g_object_get (children[1], "name", &name, NULL);
	g_assert_cmpstr (name, ==, MGD_TEST_WORKSPACE_NAME_TESTING);
	g_free (name);

	/* Private */
	g_object_get (children[2], "name", &name, NULL);
	g_assert_cmpstr (name, ==, MGD_TEST_WORKSPACE_NAME_PRIVATE);
	g_free (name);

	g_object_unref (children[0]);
	g_object_unref (children[1]);
	g_object_unref (children[2]);
	g_free (children);

	g_object_unref (workspace_context);

	/* FAIL */

	workspace_context = midgard_workspace_context_new ();
	g_assert (workspace_context != NULL);	
	children = midgard_workspace_storage_list_children (MIDGARD_WORKSPACE_STORAGE (workspace_context), &n_objects);
	g_assert (children == NULL);
	g_assert (n_objects == 0); 

	g_object_unref (workspace_context);
}

void 
midgard_test_workspace_context_has_workspace (MidgardWorkspaceContextTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	const MidgardWorkspaceManager *manager = midgard_connection_get_workspace_manager (mgd);
	g_assert (manager != NULL);

	GError *error = NULL;
	MidgardWorkspaceContext *workspace_context = midgard_workspace_context_new ();
	g_assert (workspace_context != NULL);
	gboolean get_by_path = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_CONTEXT_PATH, &error);
	g_assert (get_by_path != FALSE);
	g_assert (error == NULL);

	MidgardWorkspaceStorage *workspace = midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_NAME_STABLE);
	g_assert (workspace != NULL);
	g_assert (midgard_workspace_context_has_workspace (workspace_context, MIDGARD_WORKSPACE (workspace)) == TRUE);
	g_object_unref (workspace);

	workspace = midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_NAME_TESTING);
	g_assert (workspace != NULL);
	g_assert (midgard_workspace_context_has_workspace (workspace_context, MIDGARD_WORKSPACE (workspace)) == TRUE);
	g_object_unref (workspace);

	workspace = midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (workspace_context), MGD_TEST_WORKSPACE_NAME_PRIVATE);
	g_assert (workspace != NULL);
	g_assert (midgard_workspace_context_has_workspace (workspace_context, MIDGARD_WORKSPACE (workspace)) == TRUE);
	g_object_unref (workspace);

	/* FAIL */
	MidgardWorkspace *ws = midgard_workspace_new (); 
	g_assert (ws != NULL);
	g_assert (midgard_workspace_context_has_workspace (workspace_context, ws) == FALSE);
	g_object_unref (ws);

	g_object_unref (workspace_context);
}
