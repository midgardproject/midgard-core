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

#include "midgard_test_config.h"

MidgardConfig *
midgard_test_config_new_user_config (const gchar *name)
{
	MidgardConfig *config = midgard_config_new();
	gboolean saved = FALSE;
	gboolean read = FALSE;

	g_object_set (config, "database", CONFIG_DB_NAME, NULL);
	g_object_set (config, "dbtype", "MySQL", NULL);
	g_object_set (config, "dbuser", CONFIG_DB_NAME, NULL);
	g_object_set (config, "dbpass", CONFIG_DB_NAME, NULL);
	//g_object_set(config, "dbtype", "SQLite", NULL);
	saved = midgard_config_save_file (config, name, TRUE, NULL);
	g_assert (saved == TRUE);
	g_object_unref (config);

	config = midgard_config_new();
	read = midgard_config_read_file (config, name, TRUE, NULL);
	g_assert (read == TRUE);

	return config;
}

void 
midgard_test_config_init (void)
{
	MidgardConfig *config = midgard_test_config_new_user_config (CONFIG_CONFIG_NAME);
	g_object_unref (config);
}

void 
midgard_test_config_check_properties (void)
{
	MidgardConfig *config = midgard_config_new();

	gchar *blobdir;
	gchar *cachedir;
	gchar *database;
	gchar *dbpass;
	gchar *dbtype;
	gchar *dbuser;
	gchar *host;
	gchar *logfilename;
	gchar *loglevel;
	gchar *midgardpassword;
	gchar *midgardusername;
	gchar *pamfile;
	gchar *sharedir;
	gboolean tablecreate;
	gboolean tableupdate;
	gboolean testunit;
	gchar *vardir;

	g_object_get (config,
			"blobdir", &blobdir,
			"cachedir", &cachedir,
			"database", &database,
			"dbpass", &dbpass,
			"dbtype", &dbtype,
			"dbuser", &dbuser,
			"host", &host,
			"logfilename", &logfilename,
			"loglevel", &loglevel,
			"midgardpassword", &midgardpassword,
			"midgardusername", &midgardusername,
			"pamfile", &pamfile,
			"sharedir", &sharedir,
			"tablecreate", &tablecreate,
			"tableupdate", &tableupdate,
			"testunit", &testunit,
			"vardir", &vardir,
			NULL);

	/* Check default properties */
	/* TODO, should we remove this checks or make them smarter ?*/
	/*g_assert_cmpstr (blobdir, ==, "");
	g_assert_cmpstr (sharedir, ==, "");
	g_assert_cmpstr (vardir, ==, "");
	g_assert_cmpstr (cachedir, ==, ""); */
	g_assert_cmpstr (database, !=, "");
	g_assert_cmpstr (database, !=, NULL);
	g_assert_cmpstr (database, ==, "midgard");

	g_assert_cmpstr (dbpass, !=, "");
	g_assert_cmpstr (dbpass, !=, NULL);
	g_assert_cmpstr (dbpass, ==, "midgard");

	g_assert_cmpstr (dbtype, !=, "");
	g_assert_cmpstr (dbtype, !=, NULL);
	g_assert_cmpstr (dbtype, ==, "MySQL");

	g_assert_cmpstr (dbuser, !=, "");
	g_assert_cmpstr (dbuser, !=, NULL);
	g_assert_cmpstr (dbuser, ==, "midgard");
	
	g_assert_cmpstr (host, !=, "");
	g_assert_cmpstr (host, !=, NULL);
	g_assert_cmpstr (host, ==, "localhost");
	
	g_assert_cmpstr (logfilename, !=, NULL);
	g_assert_cmpstr (logfilename, ==, "");
		
	g_assert_cmpstr (loglevel, !=, "");
	g_assert_cmpstr (loglevel, !=, NULL);
	g_assert_cmpstr (loglevel, ==, "warn");
	
	g_assert_cmpstr (midgardpassword, !=, "");
	g_assert_cmpstr (midgardpassword, !=, NULL);
	g_assert_cmpstr (midgardpassword, ==, "password");
	
	g_assert_cmpstr (midgardusername, !=, "");
	g_assert_cmpstr (midgardusername, !=, NULL);
	g_assert_cmpstr (midgardusername, ==, "admin");
	
	g_assert_cmpstr (pamfile, !=, NULL);
	g_assert_cmpstr (pamfile, ==, "");

	g_assert_cmpuint (tablecreate, ==, FALSE);
	g_assert_cmpuint (tableupdate, ==, FALSE);
	g_assert_cmpuint (testunit, ==, FALSE);

	g_free (blobdir);
	g_free (cachedir);
	g_free (database);
	g_free (dbpass);
	g_free (dbtype);
	g_free (dbuser);
	g_free (host);
	g_free (logfilename);
	g_free (loglevel);
	g_free (midgardpassword);
	g_free (midgardusername);
	g_free (pamfile);
	g_free (sharedir);
	g_free (vardir);

	g_object_unref (config);
}

void 
midgard_test_config_save_file (void)
{
	MidgardConfig *config = midgard_config_new();

	GError *error = NULL;
	gboolean config_saved = midgard_config_save_file (config, "midgard_test_one", TRUE, &error);
	g_assert (config_saved == TRUE);
	g_assert (error == NULL);

	/* Ugly, but try to save in system wide directory */
	if (!g_str_equal (g_get_user_name(), "root")) {
		
		gboolean system_config_saved = midgard_config_save_file (config, "midgard_test_one", FALSE, &error);
		g_assert (system_config_saved == FALSE);
		g_assert (error != NULL);
		g_clear_error (&error);
	}

	gboolean config_two_saved = midgard_config_save_file (config, "midgard_test_two", TRUE, &error);
	g_assert (config_two_saved == TRUE);
	g_assert (error == NULL);

	g_object_unref (config);
}

void 
midgard_test_config_read_file (void)
{
	MidgardConfig *config = midgard_config_new();

	GError *error = NULL;
	gboolean config_read = midgard_config_read_file (config, "midgard_test_one", TRUE, &error);
	g_assert (config_read == TRUE);
	g_assert (error == NULL);

	gboolean config_two_read = midgard_config_read_file (config, "midgard_test_two", TRUE, &error);
	g_assert (config_two_read == TRUE);
	g_assert (error == NULL);

	gboolean config_not_exists_read = midgard_config_read_file (config, "some_file_which_doesnt_exist", TRUE, &error);
	g_assert (config_not_exists_read != TRUE);
	g_assert (error != NULL);
	g_clear_error (&error);

	g_object_unref (config);
}

void 
midgard_test_config_read_file_at_path (void)
{
	MidgardConfig *config = midgard_config_new();

	GError *error = NULL;
	gboolean config_not_exists_read = midgard_config_read_file_at_path (config, "/the/wrong/path/file", &error);
	g_assert (config_not_exists_read != TRUE);
	g_assert (error != NULL);
	g_clear_error (&error);

	g_object_unref (config);
}

void 
midgard_test_config_list_files (void)
{
	gchar **cfg_list = midgard_config_list_files (TRUE);
	g_assert (cfg_list != NULL);

	guint i = 0;
	guint valid_config_files = 0;

	/* We expect at least two files created in read_file test */
	while (cfg_list[i] != NULL) {

		if (g_str_equal (cfg_list[i], "midgard_test_one")
				|| g_str_equal (cfg_list[i], "midgard_test_two")) 
			valid_config_files ++;

		i++;
	}

	g_assert_cmpuint (valid_config_files, >, 1);

	g_strfreev (cfg_list);
}
