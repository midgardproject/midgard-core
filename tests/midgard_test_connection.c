/* 
 * Copyright (C) 2008, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

MidgardConnection *midgard_test_connection_open_user_config(const gchar *name, MidgardConfig **config)
{
	g_assert(*config == NULL);
	MidgardConnection *mgd = midgard_connection_new();

	gboolean opened = FALSE;
	GError *err = NULL;

	*config = midgard_test_config_new_user_config(name);
	g_assert(*config != NULL);

	opened  = midgard_connection_open_config(mgd, *config);
	
	/* Fail, we didn't make connection */
	g_assert(opened == TRUE);

	/* Fail if we connected and error is set */
	g_assert(err == NULL);

	/* Fail if midgard error is not OK */
	MIDGARD_TEST_ERROR_OK(mgd);

	return mgd;
}

void midgard_test_connection_run(void)
{
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	g_object_unref(mgd);
	g_object_unref(config);
}

void    
midgard_test_connection_open (void)
{
	MidgardConnection *mgd = midgard_connection_new ();

	/* It tries to open config from system wide dir, so check false case */
	GError *error = NULL;
	gboolean false_connection_opened = midgard_connection_open (mgd, "the_wrong_name", &error);
	g_assert (false_connection_opened == FALSE);
	g_assert (error != NULL);

	g_clear_error (&error);
	g_object_unref (mgd);
}

void    
midgard_test_connection_open_config (void)
{
	MidgardConnection *mgd = midgard_connection_new ();

	/* We should have midgard_test database created, check against it */
	MidgardConfig *config = midgard_config_new();
	GError *error = NULL;
	gboolean config_read = midgard_config_read_file (config, "midgard_test", TRUE, &error);
	g_assert (config_read == TRUE);
	g_assert (error == NULL);

	gboolean config_connection_opened = midgard_connection_open_config (mgd, config);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (config_connection_opened == TRUE);

	/* Read_file fails and we stay with old config data, thus create new config. */
	MidgardConfig *fail_config = midgard_config_new();
	gboolean config_read_wrong = midgard_config_read_file (fail_config, "no_such_config", TRUE, &error);
	g_assert (config_read_wrong == FALSE);
	g_assert (error != NULL);
	g_clear_error (&error);

	g_object_unref (fail_config);

	/* Check #1464 */
	g_test_bug ("#1464");
	gboolean config_opened_again = midgard_connection_open_config(mgd, config);
	g_assert(config_opened_again == TRUE);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);

	fail_config = midgard_config_new();
	gboolean config_opened_again_fail = midgard_connection_open_config(mgd, fail_config);
	g_assert(config_opened_again_fail == FALSE);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INTERNAL);
	/* #1464 END */

	g_object_unref (fail_config);
	g_object_unref (config);
	g_object_unref (mgd);
}

void
midgard_test_connection_close (void)
{
	MidgardConnection *mgd = midgard_connection_new ();

	/* We should have midgard_test database created, check against it */
	MidgardConfig *config = midgard_config_new();
	GError *error = NULL;
	gboolean config_read = midgard_config_read_file (config, "midgard_test", TRUE, &error);
	g_assert (config_read == TRUE);
	g_assert (error == NULL);

	gboolean config_connection_opened = midgard_connection_open_config (mgd, config);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (config_connection_opened == TRUE);

	midgard_connection_close (mgd);

	gboolean config_connection_reopened = midgard_connection_open_config (mgd, config);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (config_connection_reopened == TRUE);
}

void    
midgard_test_connection_set_loglevel (void)
{
	MidgardConnection *mgd = midgard_connection_new ();

	/* Test every case found in docs */
	gboolean error_loglevel = midgard_connection_set_loglevel (mgd, "error", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (error_loglevel == TRUE);

	gboolean warn_loglevel = midgard_connection_set_loglevel (mgd, "warn", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (warn_loglevel == TRUE);

	gboolean warning_loglevel = midgard_connection_set_loglevel (mgd, "warning", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (warning_loglevel == TRUE);

	gboolean info_loglevel = midgard_connection_set_loglevel (mgd, "info", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (info_loglevel == TRUE);

	gboolean message_loglevel = midgard_connection_set_loglevel (mgd, "message", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (message_loglevel == TRUE);

	gboolean debug_loglevel = midgard_connection_set_loglevel (mgd, "debug", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (debug_loglevel == TRUE);

	/* Revert back to warn so we do not taint test output */
	warn_loglevel = midgard_connection_set_loglevel (mgd, "warn", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (warn_loglevel == TRUE);

	/* Check empty level */
	gboolean empty_loglevel = midgard_connection_set_loglevel (mgd, "", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (empty_loglevel == FALSE);
	
	/* Check explicit invalid */
	gboolean invalid_loglevel = midgard_connection_set_loglevel (mgd, "invalid", NULL);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (invalid_loglevel == FALSE);

	g_object_unref (mgd);
}

void    
midgard_test_connection_get_error (void)
{
	MidgardConnection *mgd = midgard_connection_new ();

	/* MGD_ERR_OK */
	MIDGARD_TEST_ERROR_OK (mgd);
	gint ok_errcode = midgard_connection_get_error (mgd);
	g_assert_cmpint (ok_errcode, !=, MGD_ERR_INTERNAL);
	g_assert_cmpint (ok_errcode, !=, MGD_ERR_ACCESS_DENIED);
	g_assert_cmpint (ok_errcode, ==, MGD_ERR_OK);

	/* MGD_ERR_NOT_CONNECTED */
	midgard_connection_open (mgd, "total_crap_file", NULL);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_NOT_CONNECTED);
	gint not_connected_error = midgard_connection_get_error (mgd);
	g_assert_cmpint (not_connected_error, !=, MGD_ERR_OK);
	g_assert_cmpint (not_connected_error, !=, MGD_ERR_ACCESS_DENIED);
	g_assert_cmpint (not_connected_error, == , MGD_ERR_NOT_CONNECTED);

	/* Random error codes */
	midgard_connection_set_error (mgd, MGD_ERR_NO_METADATA);
	gint no_metadata_error = midgard_connection_get_error (mgd);
	g_assert_cmpint (no_metadata_error, !=, MGD_ERR_OK);
	g_assert_cmpint (no_metadata_error, ==, MGD_ERR_NO_METADATA);

	midgard_connection_set_error (mgd, MGD_ERR_NOT_OBJECT);
	gint not_object_error = midgard_connection_get_error (mgd);
	g_assert_cmpint (not_object_error, !=, MGD_ERR_OK);
	g_assert_cmpint (not_object_error, ==, MGD_ERR_NOT_OBJECT);

	midgard_connection_set_error (mgd, MGD_ERR_MISSED_DEPENDENCE);
	gint missed_dependence_error = midgard_connection_get_error (mgd);
	g_assert_cmpint (missed_dependence_error, !=, MGD_ERR_OK);
	g_assert_cmpint (missed_dependence_error, ==, MGD_ERR_MISSED_DEPENDENCE);

	g_object_unref (mgd);
}

void    
midgard_test_connection_get_error_string (void)
{
	MidgardConnection *mgd = midgard_connection_new ();

	/* MGD_ERR_OK */
	MIDGARD_TEST_ERROR_OK (mgd);
	const gchar *ok_error = midgard_connection_get_error_string (mgd);
	g_assert_cmpstr (ok_error, !=, "Access Denied");
	g_assert_cmpstr (ok_error, !=, "Not Midgard Object");
	g_assert_cmpstr (ok_error, !=, "Siała baba mak");
	g_assert_cmpstr (ok_error, ==, "MGD_ERR_OK");

	/* Random error codes */
	midgard_connection_set_error (mgd, MGD_ERR_OBJECT_IS_LOCKED);
	const gchar *object_is_locked_error = midgard_connection_get_error_string (mgd);
	g_assert_cmpstr (object_is_locked_error, !=, "");
	g_assert_cmpstr (object_is_locked_error, !=, "Three questions");
	g_assert_cmpstr (object_is_locked_error, ==, "Object is locked");

	midgard_connection_set_error (mgd, MGD_ERR_OBJECT_DELETED);
	const gchar *object_deleted_error = midgard_connection_get_error_string (mgd);
	g_assert_cmpstr (object_deleted_error, !=, "");
	g_assert_cmpstr (object_deleted_error, !=, "Three questions");
	g_assert_cmpstr (object_deleted_error, ==, "Object deleted.");

	midgard_connection_set_error (mgd, MGD_ERR_INTERNAL);
	const gchar *internal_error = midgard_connection_get_error_string (mgd);
	g_assert_cmpstr (internal_error, !=, "");
	g_assert_cmpstr (internal_error, !=, "What is your favorite colour?");
	g_assert_cmpstr (internal_error, ==, "Critical internal error.");

	g_object_unref (mgd);
}

void    
midgard_test_connection_get_user (void)
{
	/* Get valid test connection */
	MidgardConfig *config = NULL;
        MidgardConnection *mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	MidgardUser *null_user = midgard_connection_get_user(mgd);
	g_assert (null_user == NULL);

	MidgardUser *valid_user = midgard_user_quick_login (mgd, "admin", "password");
	g_assert (valid_user != NULL);
	g_assert (!MIDGARD_IS_OBJECT (valid_user));
	g_assert (MIDGARD_IS_USER (valid_user));

	/* connection added refcount to user, so user is still valid */
	g_object_unref (valid_user);

	MidgardUser *still_valid_user = midgard_connection_get_user(mgd);	
	g_assert (still_valid_user != NULL);

	/* connection must return exactly the same pointer to user object */
	g_assert (valid_user == still_valid_user);

	/* Now we will remove all refcounts to user */
	gboolean user_logged_out = midgard_user_log_out (still_valid_user);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (user_logged_out == TRUE);

	/* user object should be destroyed at this point */
	MidgardUser *invalid_user = midgard_connection_get_user(mgd);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (invalid_user == NULL);

	g_assert (valid_user != invalid_user);

	g_object_unref (mgd);
}

void    
midgard_test_connection_copy (void)
{
	/* Get valid test connection */
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	MidgardConnection *copy = midgard_connection_copy(mgd);

	g_assert (midgard_connection_get_user (mgd) == midgard_connection_get_user (copy));
	g_assert_cmpuint (midgard_connection_get_loglevel (mgd), ==, midgard_connection_get_loglevel (copy));	
	g_assert_cmpuint (midgard_connection_get_error (mgd), ==, midgard_connection_get_error (copy));
	g_assert_cmpstr (midgard_connection_get_error_string (mgd), ==, midgard_connection_get_error_string (copy));

	guint n_auth_types;
	gchar **auths = midgard_connection_list_auth_types (mgd, &n_auth_types);
	g_assert (auths != NULL);
	g_assert_cmpuint (n_auth_types, >, 1);

	guint copy_n_auth_types;
	gchar **copy_auths = midgard_connection_list_auth_types (copy, &copy_n_auth_types);
	g_assert (copy_auths != NULL);
	g_assert_cmpuint (copy_n_auth_types, >, 1);

	g_strfreev (auths);
	g_object_unref (copy);
	g_object_unref (mgd);
}

void    
midgard_test_connection_enable_workspace (void)
{
	/* Get valid test connection */
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	midgard_connection_enable_workspace (mgd, TRUE);
	gboolean workspace_enabled = FALSE;
	workspace_enabled = midgard_connection_is_enabled_workspace (mgd);
	g_assert (workspace_enabled == TRUE);

	midgard_connection_enable_workspace (mgd, FALSE);
	workspace_enabled = midgard_connection_is_enabled_workspace (mgd);
	g_assert (workspace_enabled == FALSE);

	g_object_unref (mgd);
}

void    
midgard_test_connection_set_workspace (void)
{
	/* Get valid test connection */
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	MidgardWorkspace *workspace = midgard_workspace_new (mgd, NULL);
	gboolean workspace_is_set = midgard_connection_set_workspace (mgd, MIDGARD_WORKSPACE_STORAGE (workspace));
	g_assert (workspace_is_set == TRUE);

	g_object_unref (mgd);
}

void    
midgard_test_connection_get_workspace (void)
{
	/* Get valid test connection */
	MidgardConfig *config = NULL;
	MidgardConnection *mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	MidgardWorkspace *null_workspace = (MidgardWorkspace *)midgard_connection_get_workspace (mgd);
	g_assert (null_workspace == NULL);

	MidgardWorkspace *workspace = midgard_workspace_new (mgd, NULL);
	gboolean workspace_is_set = midgard_connection_set_workspace (mgd, MIDGARD_WORKSPACE_STORAGE (workspace));
	g_assert (workspace_is_set == TRUE);

	MidgardWorkspace *connection_workspace = (MidgardWorkspace *)midgard_connection_get_workspace (mgd);
	g_assert (connection_workspace == workspace);

	g_object_unref (mgd);

}

static const gchar *__auth_changed_username = NULL;
static void __auth_changed_callback (MidgardConnection *mgd, gpointer ud)
{
	__auth_changed_username = "John_Callback";
}

void    
midgard_test_connection_signals_auth_changed (void)
{
	/* Get valid test connection */
	MidgardConfig *config = NULL;
        MidgardConnection *mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	g_signal_connect(G_OBJECT(mgd), "auth-changed", G_CALLBACK(__auth_changed_callback), NULL);

	MidgardUser *user = midgard_user_quick_login (mgd, "admin", "password");
	g_assert (user != NULL);

	g_assert_cmpstr (__auth_changed_username, !=, "");
	g_assert_cmpstr (__auth_changed_username, !=, NULL);
	g_assert_cmpstr (__auth_changed_username, ==, "John_Callback");

	g_object_unref (user);
	g_object_unref (mgd);
}

static const gchar *__error_name = NULL;
static void __error_callback (MidgardConnection *mgd, gpointer ud)
{
	__error_name = "Error_Callback";
}

void    
midgard_test_connection_signals_error (void)
{
	MidgardConnection *mgd = midgard_connection_new ();

	g_signal_connect(G_OBJECT(mgd), "error", G_CALLBACK(__error_callback), NULL);
	gboolean connection_open_fail = midgard_connection_open (mgd, "crap_file", NULL);

	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_NOT_CONNECTED);
	g_assert (connection_open_fail == FALSE);

	g_assert_cmpstr (__error_name, !=, NULL);
	g_assert_cmpstr (__error_name, !=, "");
	g_assert_cmpstr (__error_name, ==, "Error_Callback");

	g_object_unref (mgd);
}

#define CONN_OPEN_STR 	"Connection opened"
#define CONN_CLOSE_STR	"Connection closed"

static const gchar *__conn_open_name = NULL;
static void __conn_open_callback (MidgardConnection *mgd, gpointer ud)
{
	__conn_open_name = "Connection opened";
}

void
midgard_test_connection_signals_connected (void)
{
	MidgardConnection *mgd = midgard_connection_new ();
	g_signal_connect(G_OBJECT(mgd), "connected", G_CALLBACK(__conn_open_callback), NULL);

	MidgardConfig *config = midgard_config_new();
	GError *error = NULL;
	gboolean config_read = midgard_config_read_file (config, "midgard_test", TRUE, &error);
	g_assert (config_read == TRUE);
	g_assert (error == NULL);

	gboolean config_connection_opened = midgard_connection_open_config (mgd, config);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (config_connection_opened == TRUE);
	
	/* Check string modified by callback */
	g_assert_cmpstr (__conn_open_name, !=, NULL);
	g_assert_cmpstr (__conn_open_name, !=, "");
	g_assert_cmpstr (__conn_open_name, ==, CONN_OPEN_STR);
}

static const gchar *__conn_close_name = NULL;
static void __conn_close_callback (MidgardConnection *mgd, gpointer ud)
{
	__conn_close_name = "Connection closed";
}

void
midgard_test_connection_signals_disconnected (void)
{
	MidgardConnection *mgd = midgard_connection_new ();
	g_signal_connect(G_OBJECT(mgd), "disconnected", G_CALLBACK(__conn_close_callback), NULL);

	MidgardConfig *config = midgard_config_new();
	GError *error = NULL;
	gboolean config_read = midgard_config_read_file (config, "midgard_test", TRUE, &error);
	g_assert (config_read == TRUE);
	g_assert (error == NULL);

	gboolean config_connection_opened = midgard_connection_open_config (mgd, config);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (config_connection_opened == TRUE);

	midgard_connection_close (mgd);
	
	/* Check string modified by callback */
	g_assert_cmpstr (__conn_close_name, !=, NULL);
	g_assert_cmpstr (__conn_close_name, !=, "");
	g_assert_cmpstr (__conn_close_name, ==, CONN_CLOSE_STR);
}

void    
midgard_test_connection_signals_lost_provider (void)
{
	MidgardConnection *mgd = midgard_connection_new ();

	/* TODO */

	g_object_unref (mgd);
}
