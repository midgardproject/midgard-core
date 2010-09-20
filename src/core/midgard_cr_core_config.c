/* MidgardCore Config routines
 *    
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "midgard_cr_core_config.h"

gchar *
midgard_cr_core_config_build_path (const gchar **dirs, const gchar *filename, gboolean user)
{
	gchar *filepath = NULL;
	gchar *path = NULL;
	guint i = 0, j = 0, k = 0;
	gchar **paths = NULL;

	if(user) {
		
		while(dirs && dirs[i]) {	
			i++;
		}
		
		paths = g_new(gchar *, i+2);
		paths[0] = g_strconcat(".", MIDGARD_CORE_PACKAGE_NAME, NULL);
		//paths[1] = "conf.d";

		k = 1;
		while(dirs && dirs[j]) {
			paths[k] = g_strdup((gchar*)dirs[j]);
			j++;
			k++;
		}

		/* Add terminating NULL */
		paths[k] = NULL;

		/* Check if every directory in path exists, if not create */
		i = 0;
		gchar *fpath = g_strdup(g_get_home_dir());
		
		while(paths[i]) {
			
			path = g_build_path(G_DIR_SEPARATOR_S,
					fpath, paths[i], NULL);
			
			if(!g_file_test((const gchar *)path,
						G_FILE_TEST_EXISTS)) {
				g_mkdir(path, 0700);
			}
			
			if(!g_file_test((const gchar *)path,
						G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
				
				g_warning("%s is not a directory!", path);
				g_free(path);
				g_strfreev(paths);
				g_free(fpath);
				return NULL;
			}
			
			g_free(fpath);
			fpath = g_strdup(path);
			g_free(path);
			path = NULL;
			i++;
		}
		
		if(filename) {
			
			filepath = g_build_path(G_DIR_SEPARATOR_S,
					fpath, filename, NULL);
			g_free(fpath);
		
		} else {

			filepath = fpath;

		}

	} else {
		
		/* TODO */
	}
	
	g_strfreev(paths);

	return filepath;
}

void midgard_cr_core_config_set_dbtype_id (MidgardCRConfig *self, const gchar *tmpstr)
{
	const gchar *_dbtype = tmpstr;

	if (g_str_equal(tmpstr, "MySQL")) {
		self->dbtype_id = MIDGARD_CORE_DB_TYPE_MYSQL;
	} else if (g_str_equal(tmpstr, "PostgreSQL")) {
		self->dbtype_id = MIDGARD_CORE_DB_TYPE_POSTGRES;
	} else if (g_str_equal(tmpstr, "FreeTDS")) {
		self->dbtype_id = MIDGARD_CORE_DB_TYPE_FREETDS;
	} else if (g_str_equal(tmpstr, "SQLite")) {
		self->dbtype_id = MIDGARD_CORE_DB_TYPE_SQLITE;
	} else if (g_str_equal(tmpstr, "ODBC")) {
		self->dbtype_id = MIDGARD_CORE_DB_TYPE_ODBC;
	} else if (g_str_equal(tmpstr, "Oracle")) {
		self->dbtype_id = MIDGARD_CORE_DB_TYPE_ORACLE;
	} else if (tmpstr == NULL) {
		self->dbtype_id = MIDGARD_CORE_DB_TYPE_MYSQL;
	} else {
		g_warning ("'%s' database type is invalid. Setting default MySQL one", tmpstr);
		self->dbtype_id = MIDGARD_CORE_DB_TYPE_MYSQL;
		_dbtype = "MySQL";
	}
}

void _cr_config_create_log_dir(const gchar *path)
{
	if (path == NULL || *path == '\0')
		return;
	
	if (g_file_test(path, G_FILE_TEST_EXISTS))
		return;
	
	gchar *dir_path = g_path_get_dirname(path);
	
	gint rv = g_mkdir_with_parents((const gchar *)dir_path, 0711);
	
	if (rv == -1) {
		g_warning ("Failed to create '%s' directory", dir_path);
		g_free (dir_path);
		return ;
	}
}

static void __set_config_from_keyfile(MidgardCRConfig *self, GKeyFile *keyfile, const gchar *filename)
{
	g_assert (self != NULL);
	g_assert (keyfile != NULL);

	gchar  *tmpstr;
	gboolean tmpbool;

	/* Get database type */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Type", NULL);
	midgard_cr_config_set_dbtype (self, tmpstr);
	g_free (tmpstr);
		
	/* Get host name or IP */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Host", NULL);
	midgard_cr_config_set_host (self, tmpstr);
	g_free (tmpstr);

	/* Get database port */
	guint port = g_key_file_get_integer (keyfile, "MidgardDatabase", "Port", NULL);
	if (port > 0)
		midgard_cr_config_set_dbport (self, port);
	else if (port < 0) 
		g_warning ("Invalid, negative value for database port");

	/* Get database name */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Name", NULL);
	midgard_cr_config_set_dbname (self, tmpstr);
	g_free (tmpstr);

	/* Get database's username */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Username", NULL);
	midgard_cr_config_set_dbuser (self, tmpstr);
	g_free (tmpstr);

	/* Get password for database user */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Password", NULL);
	midgard_cr_config_set_dbpass (self, tmpstr);	
	g_free (tmpstr);

	/* Get directory for SQLite database  */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "DatabaseDir", NULL);
	midgard_cr_config_set_dbdir (self, tmpstr);
	g_free (tmpstr);

	/* Get log filename */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Logfile", NULL);
	midgard_cr_config_set_logfilename (self, tmpstr);
	g_free (tmpstr);

	/* Get log level */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Loglevel", NULL);
	midgard_cr_config_set_loglevel (self, tmpstr);
	g_free (tmpstr);

	/* Get database creation mode */
	tmpbool = g_key_file_get_boolean(keyfile, "MidgardDatabase", "TableCreate", NULL);
	midgard_cr_config_set_tablecreate (self, tmpbool);

	/* Get database update mode */
	tmpbool = g_key_file_get_boolean(keyfile, "MidgardDatabase", "TableUpdate", NULL);
	midgard_cr_config_set_tableupdate (self, tmpbool);

	/* Get SG admin username */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "MidgardUsername", NULL);
	midgard_cr_config_set_midgardusername (self, tmpstr);
	g_free (tmpstr);

	/* Get SG admin password */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "MidgardPassword", NULL);
	midgard_cr_config_set_midgardpassword (self, tmpstr);
	g_free (tmpstr);

	/* Get test mode */
	tmpbool = g_key_file_get_boolean(keyfile, "MidgardDatabase", "TestUnit", NULL);
	midgard_cr_config_set_testunit (self, tmpbool);

	/* DIRECTORIES */

	/* BlobDir */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDir", "BlobDir", NULL);
	midgard_cr_config_set_blobdir (self, tmpstr);
	g_free (tmpstr);

	/* ShareDir  */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDir", "ShareDir", NULL);
	midgard_cr_config_set_sharedir (self , tmpstr);
	g_free (tmpstr);

	/* VarDir */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDir", "VarDir", NULL);
	midgard_cr_config_set_vardir (self, tmpstr);
	g_free (tmpstr);

	/* CacheDir */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDir", "ShareDir", NULL);
	midgard_cr_config_set_cachedir (self, tmpstr);
	g_free (tmpstr);

	return;
}

/**
 * midgard_cr_core_config_read_file:
 * @config: #MidgardCRConfig object instance
 * @filename: name of the file to read 
 * @user: boolean switch for system or user's config files
 * @error: pointer to store error
 * 
 * This method reads configuration file from the given name and sets MidgardCRConfig object's properties.
 * Such initialized MidgardCRConfig instance may be reused among midgard-core and midgard-php extension 
 * for example, without any need to re-read configuration file and without  any need to re-initalize 
 * #MidgardCRConfig object instance. 
 *
 * Set %TRUE as @user boolean value to read files from user's home directory.
 *
 * Returns: %TRUE when file has been read , %FALSE otherwise.
 * Since: 9.3
 */ 
gboolean 
midgard_cr_core_config_read_file (MidgardCRConfig *config, const gchar *filename, gboolean user, GError **error)
{
	gchar *fname = NULL;	
	GKeyFile *keyfile;	

	g_assert (config != NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	if (user) {
		gchar *_umcd = g_strconcat(".", MIDGARD_CORE_PACKAGE_NAME, NULL);
		fname = g_build_path (G_DIR_SEPARATOR_S, 
				g_get_home_dir(), _umcd, "conf.d", filename, NULL);
		g_free (_umcd);
	} else {
		gchar *confdir = midgard_cr_core_config_get_default_confdir ();
		fname = g_build_path (G_DIR_SEPARATOR_S, confdir, "/", filename, NULL);
		g_free (confdir);
	}

	GError *kf_error = NULL;
	keyfile = g_key_file_new();

	if (!g_key_file_load_from_file(keyfile, fname, G_KEY_FILE_NONE, &kf_error)) {

		g_free (fname);
		g_propagate_error (error, kf_error);
		return FALSE;
	}

	// checking consistency of config-file
	// 1) upgrade-advice
	if (!g_key_file_has_group(keyfile, "MidgardDatabase") && g_key_file_has_group(keyfile, "Database")) {
		g_warning ("Config-file '%s' doesn't have [MidgardDatabase] section, but has old [Database] section. You should rename it.", fname);
	}
	// 2) check for unsupported sections
	{
		gsize sections_len, i;
		gchar **sections = g_key_file_get_groups(keyfile, &sections_len);

		for (i = 0; i < sections_len; i++) {
			if (!g_str_equal(sections[i], "MidgardDatabase") && !g_str_equal(sections[i], "MidgardDir")) {
				g_warning ("Config-file '%s' has unsupported section [%s]. It won't be parsed.", fname, sections[i]);
			}
		}

		g_strfreev (sections);
	}

	g_free (fname);

	__set_config_from_keyfile (config, keyfile, filename);

	return TRUE;
}

/**
 * midgard_config_read_file_at_path:
 * @self: #MidgardCRConfig instance
 * @filepath: a path to read file from 
 * @error: a pointer to hold error  
 * 
 * Returns: %TRUE if file has been read, %FALSE otherwise
 */
gboolean midgard_cr_core_config_read_file_at_path(MidgardCRConfig *self, const gchar *filepath, GError **error)
{
	g_assert (self != NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GKeyFile *keyfile = NULL;

	GError *kf_error = NULL;
	keyfile = g_key_file_new();

	if (!g_key_file_load_from_file(keyfile, filepath, G_KEY_FILE_NONE, &kf_error)) {
		g_propagate_error (error, kf_error);
		return FALSE;
	}

	__set_config_from_keyfile(self, keyfile, filepath);

	return TRUE;
}

/**
 * midgard_cr_core_config_read_data:
 * @self: #MidgardCRConfig instance
 * @data: a NULL-terminated buffer containing the configuration
 * @error: a pointer to hold error  
 * 
 * Returns: %TRUE if data has been read, %FALSE otherwise
 */
gboolean 
midgard_cr_core_config_read_data (MidgardCRConfig *self, const gchar *data, GError **error)
{
	g_assert (self != NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GKeyFile *keyfile = NULL;
	GError *kf_error = NULL;
	
	keyfile = g_key_file_new();
	if (!g_key_file_load_from_data (
				keyfile, data, strlen(data), G_KEY_FILE_NONE, &kf_error)) {
	
		g_propagate_error (error, kf_error);	
		return FALSE;
	}

	__set_config_from_keyfile(self, keyfile, NULL);

	return TRUE;
}

/**
 * midgard_cr_core_config_list_files:
 * @user: boolean switch for system or user's config files
 *
 * List all available configuration files.
 * If @user value is set to %TRUE, all available files from ~/.midgard/conf.d will be listed. 
 * Only system files ( usually from /etc/midgard/conf.d ) will be listed if @user value is set to %FALSE.
 *
 * Returned array should be freed when no longer needed.
 *
 * Returns: newly allocated and %NULL terminated array of file names.
 *
 */
gchar **
midgard_cr_core_config_list_files (gboolean user, guint *n_files, GError **err)
{
	gchar *config_dir;
	*n_files = 0;

	if (user) {
		
		gchar *_umcd = g_strconcat(".", MIDGARD_CORE_PACKAGE_NAME, NULL);
		config_dir = g_build_path (G_DIR_SEPARATOR_S,
				g_get_home_dir(), _umcd, "conf.d", NULL);
		g_free (_umcd);
		if (!g_file_test (config_dir, G_FILE_TEST_IS_DIR)){
			g_error("Config directory '%s' doesn't exist",
					config_dir);
			g_free (config_dir);
			return NULL;
		} 

	} else {
		
		config_dir = midgard_cr_core_config_get_default_confdir ();
	}

	GError *error = NULL;
	GSList *list = NULL;
	gchar **filenames = NULL;
	guint i = 0, j = 0;
	GDir *dir = g_dir_open(config_dir, 0, &error);
	if (dir != NULL) {
		const gchar *file = g_dir_read_name(dir);
	
		while (file != NULL) {
			
			if (!g_str_has_prefix(file, ".")
					&& (!g_str_has_prefix(file, "#"))
					&& (!g_str_has_suffix(file, "~"))
					&& (!g_str_has_suffix(file, "example"))) {
					
				list = g_slist_append(list, (gpointer)g_strdup(file));
				i++;
			}
			file = g_dir_read_name(dir);
		}
		
		g_dir_close(dir);
		
		filenames = g_new(gchar *, i+1);
		for( ; list; list = list->next) {
			filenames[j] = (gchar *)list->data;
			j++;
		}
		filenames[i] = NULL;		
	}
	
	g_free (config_dir);

	*n_files = j;
	return filenames;
}

/**
 * midgard_cr_core_config_save_at_path
 */
gboolean
midgard_cr_core_config_save_at_path (MidgardCRConfig *self, const gchar *path, GError **error)
{
	guint n_props, i;
	GValue pval = {0,};
	const gchar *nick = NULL;
	gchar *tmpstr;
	GParamSpec **props = g_object_class_list_properties (G_OBJECT_GET_CLASS (G_OBJECT (self)), &n_props);

	// It should not happen 
	if (!props)
		g_error("Midgard Config class has no members registered");

	if (self->keyfile == NULL)
		self->keyfile = g_key_file_new ();

	for (i = 0; i < n_props; i++) {	
		nick = g_param_spec_get_nick (props[i]);
		g_value_init (&pval,props[i]->value_type); 
		g_object_get_property (G_OBJECT (self), (gchar*)props[i]->name, &pval);

		const gchar *keygroup = "MidgardDatabase";
		if (g_str_equal (props[i]->name, "sharedir")
				|| g_str_equal(props[i]->name, "vardir")
				|| g_str_equal(props[i]->name, "cachedir")
				|| g_str_equal(props[i]->name, "blobdir")) 
		{	
			keygroup = "MidgardDir";
		}

		switch (props[i]->value_type) {
	
			case G_TYPE_STRING:
				tmpstr = (gchar *)g_value_get_string (&pval);
				if (!tmpstr)
					tmpstr = "";
				g_key_file_set_string (self->keyfile, keygroup, nick, tmpstr);
				break;

			case G_TYPE_BOOLEAN:
				g_key_file_set_boolean (self->keyfile, keygroup, nick, g_value_get_boolean (&pval));
				break;

			case G_TYPE_UINT:
				g_key_file_set_integer (self->keyfile, keygroup, nick, g_value_get_uint (&pval));
				break;

		}
		g_key_file_set_comment (self->keyfile, keygroup, nick,
				g_param_spec_get_blurb (props[i]), NULL);
		g_value_unset (&pval);
	}

	g_free (props);

	gsize length;
	GError *kf_error = NULL;
	
	gchar *content = g_key_file_to_data (self->keyfile, &length, &kf_error);

	if (length < 1) {
		if (kf_error)
			g_propagate_error (error, kf_error);	
		return FALSE;
	}

	if (kf_error)
		g_clear_error (&kf_error);

	gboolean saved = g_file_set_contents (path, content, length, &kf_error);
	g_free (content);

	if (!saved) {
		if (kf_error)
			g_propagate_error (error, kf_error);
		return FALSE;
	}

	return TRUE;
}

/**
 * midgard_config_save_file:
 * @self: #MidgardCRConfig instance
 * @name: configuration filename
 * @user: system or home directory switch
 * @error: pointer to store GError
 *
 * Saves configuration file for the given #MidgardCRConfig.
 *
 * This method saves configuration file with the given name.
 * If third user parameter is set to %TRUE, then configuration file will 
 * be saved in ~/.midgard2/conf.d directory.
 *
 * User's conf.d directory will be created if doesn't exist.
 * 
 * Returns: %TRUE on success or %FALSE  ( with propper warning message ) if system wide 
 * directory doesn't exist or file can not be saved.
 */
gboolean 
midgard_cr_core_config_save_file (MidgardCRConfig *self, const gchar *name, gboolean user, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	gchar *cnfpath = NULL;
	guint namel = strlen (name);
	gchar *_umcd = NULL;
	gchar *path;

	if (namel < 1) {
		g_warning ("Can not save configuration file without a name");
		return FALSE;
	}

	if (self->keyfile == NULL)
		self->keyfile = g_key_file_new ();

	// Check configuration directory.
	// If user's conf.d dir doesn't exist, create it
	if (user) {
		gchar *_cnf_path = midgard_cr_core_config_build_path (NULL, NULL, TRUE);
		g_free (_cnf_path);
		_umcd = g_strconcat (".", MIDGARD_CORE_PACKAGE_NAME, NULL);
		path = g_build_path (G_DIR_SEPARATOR_S, g_get_home_dir (), _umcd, NULL);
		g_free (_umcd);

		// Check if .midgard directory exists 
		if (!g_file_test ((const gchar *)path, G_FILE_TEST_EXISTS)) {
			g_mkdir(path, 0);
			g_chmod(path, 0700);
		}

		if (!g_file_test ((const gchar *)path, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
			g_warning ("%s is not a directory!", path);
			g_free (path);
			return FALSE;
		}
		g_free (path);
		_umcd = g_strconcat (".", MIDGARD_CORE_PACKAGE_NAME, NULL);
		path = g_build_path (G_DIR_SEPARATOR_S, g_get_home_dir(), _umcd, "conf.d", NULL);
		g_free (_umcd);
		
		// Check if .midgard/conf.d directory exists 
		if (!g_file_test ((const gchar *)path, G_FILE_TEST_EXISTS)) {
			g_mkdir (path, 0);
			g_chmod (path, 0700);
		}

		if (!g_file_test ((const gchar *)path, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
			g_warning ("%s is not a directory!", path);
			g_free (path);
			return FALSE;
		}
		g_free (path);

		_umcd = g_strconcat (".", MIDGARD_CORE_PACKAGE_NAME, NULL);
		cnfpath = g_build_path (G_DIR_SEPARATOR_S, g_get_home_dir (), _umcd, "conf.d", name, NULL);
		g_free (_umcd); 
	} else {
		gchar *confdir = midgard_cr_core_config_get_default_confdir ();	
		if (!g_file_test ((const gchar *)confdir, G_FILE_TEST_EXISTS)
				|| (!g_file_test ((const gchar *)confdir,
						G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))) {
			g_warning ("%s doesn't exist or is not a directory!", confdir);
			g_free (confdir);
			return FALSE;
		}
		cnfpath = g_build_path (G_DIR_SEPARATOR_S, confdir, name, NULL);
		g_free (confdir);
	}

	GError *err = NULL;
	gboolean rv = TRUE;

	rv = midgard_cr_core_config_save_at_path (self, cnfpath, &err);	
	g_free (cnfpath);
	if (!rv) 
		g_propagate_error (error, err);
		
	return rv;
}

/**
 * midgard_cr_core_config_save_configuration_at_path:
 */
gboolean 
midgard_cr_core_config_save_file_at_path (MidgardCRConfig *self, const gchar *path, GError **error)
{
	GError *err = NULL;
	gboolean rv = TRUE;

	rv = midgard_cr_core_config_save_at_path (self, path, &err);
	if (!rv) {
		g_propagate_error (error, err);
		return rv;
	}

	return rv;
}

gboolean _cr_config_create_base_blobdir(const gchar *path)
{
	if (path == NULL)
		return FALSE;
	
	if (g_str_equal(path, ""))
		return FALSE;
	
	if (g_file_test(path, G_FILE_TEST_EXISTS))
		return TRUE;
	
	gchar *dir_path = g_path_get_dirname(path);
	
	gint rv = g_mkdir_with_parents((const gchar *)dir_path, 0711);
	
	if (dir_path)
		g_free (dir_path);
	
	if (rv == -1)
		return FALSE;
	
	return TRUE;
}

/* recursively prepare blobdir */
gboolean _cr_config_recurse_prepare_blobdir(const gchar *parentdir, int max_depth, int depth)
{
	int h;
	
	if (depth >= max_depth) return TRUE;
	
	for (h = 0; h < 0x10; h++)
	{
		gchar *dir_path = g_strdup_printf("%s/%X", parentdir, h);
		if (dir_path == NULL) return FALSE;
		
		/* create dir */
		gint rv = g_mkdir_with_parents((const gchar *)dir_path, 0711);
		if (rv == -1)
		{
			g_free (dir_path);
			return FALSE;
		}
		
		/* create subdirs */
		gboolean err = _cr_config_recurse_prepare_blobdir(dir_path, max_depth, depth + 1);
		if (dir_path) g_free (dir_path);
		if (err == FALSE)
			return FALSE;
	}
	
	return TRUE;
}

/**
 * midgard_config_create_blobdir:
 * @self: #MidgardCRConfig instance
 *
 * Creates directories for blobs
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 */
gboolean midgard_cr_core_config_create_blobdir(MidgardCRConfig *self)
{
	/*
	g_assert (self != NULL);

	gboolean err = FALSE;	
	
	if (self->priv->_blobdir == NULL || *self->priv->_blobdir == '\0')
	{
		g_warning ("Invalid blobdir (NULL)");
		return FALSE;
	}

	//Base configured blobdir might not exist, create all remaining subdirectories within path 
	err = _cr_config_create_base_blobdir(self->priv->_blobdir);
	if (err == FALSE)
	{
		g_error("Failed to create configured directory %s", self->priv->_blobdir);
	}
	
	// now create directories for blobs recursively 
	err = _cr_config_recurse_prepare_blobdir(self->priv->_blobdir,
	                                2, // set this to depth of the directories 
	                                0); // let this be zero always  );
	if (err == FALSE)
	{
		g_error("Failed to prepare blobdir");
		return FALSE;
	}

	return TRUE;
	*/
}

gchar *
midgard_cr_core_config_get_default_confdir (void)
{
	gchar *confdir = NULL;
	confdir = g_strdup (getenv ("MIDGARD_ENV_GLOBAL_CONFDIR"));

	if (!confdir || (confdir && g_str_equal (confdir, ""))) {
		g_free (confdir);

		if (g_str_equal (MIDGARD_CORE_LIB_PREFIX, "/usr")) {
			// Using the standard prefix "/usr", so, choosing "/etc" 
			confdir = g_build_path (G_DIR_SEPARATOR_S, " ", "etc", MIDGARD_CORE_PACKAGE_NAME, "conf.d", NULL);
		} else {
			//Using some other prefix, so choosing "prefix/etc" 
			confdir = g_build_path (G_DIR_SEPARATOR_S, MIDGARD_CORE_LIB_PREFIX, "etc", MIDGARD_CORE_PACKAGE_NAME, "conf.d", NULL);
		}
	}

	return g_strchug (confdir);
}
