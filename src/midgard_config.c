/* Midgard config files routine functions 
 *    
 * Copyright (C) 2005, 2006, 2007 Piotr Pokora <piotrek.pokora@gmail.com>
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
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <glib-object.h>
#include <glib/gstdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "midgard_object.h"
#include "midgard_config.h"
#include "midgard_datatypes.h"
#include "midgard_core_object.h"
#include "midgard_core_query.h"
#include "midgard_defs.h"
#include "midgard_core_config.h"
#include "midgard_connection.h"
#include "midgard_error.h"
#include "midgard_core_object_class.h"

static gchar *__get_default_confdir(void);

/* Properties */
enum {
	MIDGARD_CONFIG_DBTYPE = 1,
	MIDGARD_CONFIG_DBNAME,
	MIDGARD_CONFIG_DBPORT,
	MIDGARD_CONFIG_DBUSER, 
	MIDGARD_CONFIG_DBPASS,
       	MIDGARD_CONFIG_DBDIR,	
	MIDGARD_CONFIG_HOST,
	MIDGARD_CONFIG_LOGFILENAME,
	MIDGARD_CONFIG_LOGLEVEL,
	MIDGARD_CONFIG_TABLECREATE,
	MIDGARD_CONFIG_TABLEUPDATE,
	MIDGARD_CONFIG_LOGFILE,
	MIDGARD_CONFIG_TESTUNIT,
	MIDGARD_CONFIG_MGDUSERNAME,
	MIDGARD_CONFIG_MGDPASSWORD,
	MIDGARD_CONFIG_AUTHTYPE,
	MIDGARD_CONFIG_PAMFILE,
	MIDGARD_CONFIG_BLOBDIR,
	MIDGARD_CONFIG_SHAREDIR,
	MIDGARD_CONFIG_VARDIR,
	MIDGARD_CONFIG_CACHEDIR
};

static MidgardConfigPrivate *midgard_config_private_new(void)
{
	MidgardConfigPrivate *config_private = g_new(MidgardConfigPrivate, 1);

        config_private->keyfile = NULL;
	config_private->configname = NULL;
	config_private->g_file = NULL;
	config_private->output_stream = NULL;

	return config_private;
}

/**
 * midgard_config_new:
 *
 * Initializes new instance of MidgardConfig object type. 
 * NULL is returned when object can not be initialized.
 *
 * Returns: pointer to @MidgardConfig object or %NULL on failure.
 */
MidgardConfig *midgard_config_new(void)
{
	MidgardConfig *self = 
		g_object_new(MIDGARD_TYPE_CONFIG, NULL);

	return self;
}

void __set_dbtype(MidgardConfig *self, const gchar *tmpstr)
{
	const gchar *_dbtype = tmpstr;

	if(g_str_equal(tmpstr, "MySQL")) {
		self->priv->dbtype = MIDGARD_DB_TYPE_MYSQL;
	} else if(g_str_equal(tmpstr, "PostgreSQL")) {
		self->priv->dbtype = MIDGARD_DB_TYPE_POSTGRES;
	} else if(g_str_equal(tmpstr, "FreeTDS")) {
		self->priv->dbtype = MIDGARD_DB_TYPE_FREETDS;
	} else if(g_str_equal(tmpstr, "SQLite")) {
		self->priv->dbtype = MIDGARD_DB_TYPE_SQLITE;
	} else if(g_str_equal(tmpstr, "ODBC")) {
		self->priv->dbtype = MIDGARD_DB_TYPE_ODBC;
	} else if(g_str_equal(tmpstr, "Oracle")) {
		self->priv->dbtype = MIDGARD_DB_TYPE_ORACLE;
	} else if(tmpstr == NULL) {
		self->priv->dbtype = MIDGARD_DB_TYPE_MYSQL;
	} else {
		g_warning ("'%s' database type is invalid. Setting default MySQL one", tmpstr);
		self->priv->dbtype = MIDGARD_DB_TYPE_MYSQL;
		_dbtype = "MySQL";
	}

	g_free (self->dbtype);
	self->dbtype = g_strdup(_dbtype);
}

void __create_log_dir(const gchar *path)
{
	if(path == NULL || *path == '\0')
		return;
	
	if(g_file_test(path, G_FILE_TEST_EXISTS))
		return;
	
	gchar *dir_path = g_path_get_dirname(path);
	
	gint rv = g_mkdir_with_parents((const gchar *)dir_path, 0711);
	
	if(rv == -1) {
		g_warning ("Failed to create '%s' directory", dir_path);
		g_free (dir_path);
		return ;
	}
}

static void __set_config_from_keyfile(MidgardConfig *self, GKeyFile *keyfile, const gchar *filename)
{
	g_assert (self != NULL);
	g_assert (keyfile != NULL);

	gchar  *tmpstr;
	gboolean tmpbool;

	/* Get database type */
	self->priv->dbtype = MIDGARD_DB_TYPE_MYSQL;
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Type", NULL);
	if(tmpstr == NULL || *tmpstr == '\0') {
		g_free (tmpstr);
		tmpstr = g_strdup("MySQL");
	}

	__set_dbtype(self, tmpstr);
	g_free (tmpstr);
		
	/* Get host name or IP */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Host", NULL);
	if(tmpstr == NULL)
		tmpstr = g_strdup("localhost");
	if(self->host)
		g_free (self->host);
	self->host = g_strdup(tmpstr);
	g_free (tmpstr);

	/* Get database port */
	guint port = g_key_file_get_integer (keyfile, "MidgardDatabase", "Port", NULL);
	if (port > 0)
		self->dbport = port;
	else if (port < 0) 
		g_warning ("Invalid, negative value for database port");

	/* Get database name */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Name", NULL);
	if(tmpstr != NULL && *tmpstr != '\0') {
		g_free (self->database);
		self->database = g_strdup(tmpstr);
	}
	g_free (tmpstr);

	/* Get database's username */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Username", NULL);
	if(tmpstr != NULL && *tmpstr != '\0') {
		g_free (self->dbuser);
		self->dbuser = g_strdup(tmpstr);
	}
	g_free (tmpstr);

	/* Get password for database user */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Password", NULL);
	if(tmpstr != NULL && *tmpstr != '\0') {
		g_free (self->dbpass);
		self->dbpass = g_strdup(tmpstr);
	}
	g_free (tmpstr);

	/* Get directory for SQLite database  */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "DatabaseDir", NULL);
	if(tmpstr != NULL && *tmpstr != '\0') {
		g_free (self->dbdir);
		self->dbdir = g_strdup(tmpstr);
	}
	g_free (tmpstr);

	/* Get log filename */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Logfile", NULL);
	if(tmpstr != NULL && !g_str_equal(tmpstr, "")) {

		__create_log_dir((const gchar *) tmpstr);

		self->logfilename = g_strdup(tmpstr);
		GError *err = NULL;
		GFile *g_file = g_file_new_for_path ((const gchar *)self->logfilename);
		GFileOutputStream *output_stream = 
			g_file_append_to (g_file, G_FILE_CREATE_NONE, NULL, &err);
		g_free (tmpstr);
		tmpstr = NULL;

		if(!output_stream){
			g_warning ("Can not open '%s' logfile. %s", 
					self->logfilename,
					err->message);
		} else {
			self->priv->g_file = g_file;
			self->priv->output_stream = output_stream;
		}
		if (err)
			g_clear_error(&err);
	}

	if(tmpstr)
		g_free (tmpstr);

	/* Get log level */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "Loglevel", NULL);
	if(tmpstr == NULL)
		tmpstr = g_strdup("warn");
	if(self->loglevel)
		g_free (self->loglevel);
	self->loglevel = g_strdup(tmpstr);
	g_free (tmpstr);
	tmpstr = NULL;

	/* Get database creation mode */
	tmpbool = g_key_file_get_boolean(keyfile, "MidgardDatabase", "TableCreate", NULL);
	self->tablecreate = tmpbool;


	/* Get database update mode */
	tmpbool = g_key_file_get_boolean(keyfile, "MidgardDatabase", "TableUpdate", NULL);
	self->tableupdate = tmpbool;

	/* Get SG admin username */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "MidgardUsername", NULL);
	if(tmpstr != NULL && *tmpstr != '\0') {
		if(self->mgdusername)
			g_free (self->mgdusername);
		self->mgdusername = g_strdup(tmpstr);		
		g_free (tmpstr);
	}

	/* Get SG admin password */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "MidgardPassword", NULL);
	if(tmpstr != NULL && *tmpstr != '\0') {
		if(self->mgdpassword)
			g_free (self->mgdpassword);
		self->mgdpassword = g_strdup(tmpstr);
		g_free (tmpstr);
	}

	/* Get test mode */
	tmpbool = g_key_file_get_boolean(keyfile, "MidgardDatabase", "TestUnit", NULL);
	self->testunit = tmpbool;

	/* Get auth type */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "AuthType", NULL);
	midgard_config_set_authtype(self, (const gchar *)tmpstr);
	g_free (tmpstr);

	/* Get Pam file */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDatabase", "PamFile", NULL);
	if(tmpstr == NULL)
		tmpstr = g_strdup("midgard");
	g_free (self->pamfile);
	self->pamfile = g_strdup(tmpstr);
	g_free (tmpstr);

	/* Disable threads */
	tmpbool = g_key_file_get_boolean(keyfile, "MidgardDatabase", "GdaThreads", NULL);
	self->gdathreads = tmpbool;

	/* DIRECTORIES */

	/* BlobDir */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDir", "BlobDir", NULL);
	if(tmpstr != NULL && *tmpstr != '\0') {

		g_free (self->blobdir);
		self->blobdir = g_strdup(tmpstr);
		g_free (tmpstr);
	
	} else {

		gchar *db_blobdir = g_build_path(G_DIR_SEPARATOR_S, 
				self->blobdir, self->database, NULL);
		g_free (self->blobdir);
		self->blobdir = db_blobdir;
	}

	/* ShareDir */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDir", "ShareDir", NULL);
	if(tmpstr != NULL && *tmpstr != '\0') {

		g_free (self->sharedir);
		self->sharedir = tmpstr;
	}

	/* VarDir */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDir", "VarDir", NULL);
	if(tmpstr != NULL || (tmpstr && *tmpstr != '\0')) {

		g_free (self->vardir);
		self->vardir = tmpstr;
	}

	/* CacheDir */
	tmpstr = g_key_file_get_string (keyfile, "MidgardDir", "CacheDir", NULL);
	if(tmpstr != NULL || (tmpstr && *tmpstr == '\0')) {

		g_free (self->cachedir);
		self->cachedir = tmpstr;
	} 

	/* SchemaDir */
	g_free (self->schemadir);
	self->schemadir = g_build_path(G_DIR_SEPARATOR_S, self->sharedir, MIDGARD_CONFIG_RW_SCHEMA, NULL);

	/* ViewsDir */
	g_free (self->viewsdir);
	self->viewsdir = g_build_path(G_DIR_SEPARATOR_S, self->sharedir, MIDGARD_CONFIG_RW_VIEW, NULL);

	/* We will free it when config is unref */
	self->priv->keyfile = keyfile;

	if (filename)
		self->priv->configname = g_strdup(filename);

	return;
}

/**
 * midgard_config_read_file:
 * @self: #MidgardConfig object instance
 * @filename: name of the file to read 
 * @user: boolean switch for system or user's config files
 * @error: pointer to store error
 * 
 * This method reads configuration file from the given name and sets MidgardConfig object's properties.
 * Such initialized MidgardConfig instance may be reused among midgard-core and midgard-php extension 
 * for example, without any need to re-read configuration file and without  any need to re-initalize 
 * #MidgardConfig object instance. 
 *
 * Set %TRUE as @user boolean value to read files from user's home directory.
 *
 * Returns: %TRUE when file has been read , %FALSE otherwise.
 * Since: 9.3
 */ 
gboolean midgard_config_read_file(MidgardConfig *self, const gchar *filename, gboolean user, GError **error)
{
	gchar *fname = NULL;	
	GKeyFile *keyfile;	

	g_assert (self != NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	if(filename == NULL || *filename == '\0') {
		
			g_set_error(error, MGD_GENERIC_ERROR, 
					MGD_ERR_INVALID_NAME, 
					"Configuration name can not be empty");
			
			return FALSE;
	}
	
	if(user) {
		gchar *_umcd = g_strconcat(".", MIDGARD_PACKAGE_NAME, NULL);
		fname = g_build_path (G_DIR_SEPARATOR_S, 
				g_get_home_dir(), _umcd, "conf.d", filename, NULL);
		g_free (_umcd);
	} else {
		fname = g_build_path (G_DIR_SEPARATOR_S,
				self->confdir, "/", filename, NULL);
	}

	GError *kf_error = NULL;
	keyfile = g_key_file_new();

	if (!g_key_file_load_from_file(keyfile, fname, G_KEY_FILE_NONE, &kf_error)) {

		if (kf_error && kf_error->message) {

			g_propagate_error (error, kf_error);
		
		} else { 

			g_set_error (error, MGD_GENERIC_ERROR,
					MGD_ERR_INTERNAL,
					"Can not open %s. %s" , fname, kf_error && kf_error->message ? kf_error->message : "Unknown reason.");

			g_clear_error(&kf_error);
		}

		g_free (fname);
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

	__set_config_from_keyfile(self, keyfile, filename);

	return TRUE;
}

/**
 * midgard_config_read_file_at_path:
 * @self: #MidgardConfig instance
 * @filepath: a path to read file from 
 * @error: a pointer to hold error  
 * 
 * Returns: %TRUE if file has been read, %FALSE otherwise
 */
gboolean midgard_config_read_file_at_path(MidgardConfig *self, const gchar *filepath, GError **error)
{
	g_assert (self != NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GKeyFile *keyfile = NULL;

	if (filepath == NULL || *filepath == '\0') {
		
			g_set_error (error, MGD_GENERIC_ERROR, MGD_ERR_INVALID_NAME, "Configuration file path can not be empty");
	
		return FALSE;
	}

	GError *kf_error = NULL;
	keyfile = g_key_file_new();

	if (!g_key_file_load_from_file(keyfile, filepath, G_KEY_FILE_NONE, &kf_error)) {

		g_set_error (error, MGD_GENERIC_ERROR, MGD_ERR_INTERNAL, "Can not open %s. %s" , filepath, kf_error->message);

		g_clear_error(&kf_error);
		return FALSE;
	}

	__set_config_from_keyfile(self, keyfile, filepath);

	return TRUE;
}

/**
 * midgard_config_read_data:
 * @self: #MidgardConfig instance
 * @data: a NULL-terminated buffer containing the configuration
 * @error: a pointer to hold error  
 * 
 * Returns: %TRUE if data has been read, %FALSE otherwise
 */
gboolean midgard_config_read_data(MidgardConfig *self, const gchar *data, GError **error)
{
	g_assert (self != NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GKeyFile *keyfile = NULL;
	GError *kf_error = NULL;
	
	keyfile = g_key_file_new();
	if (!g_key_file_load_from_data (
				keyfile, data, strlen(data), G_KEY_FILE_NONE, &kf_error)) {
		
		g_set_error (error, MGD_GENERIC_ERROR,
				MGD_ERR_INTERNAL,
				"Can not read the configuration data");
		g_clear_error(&kf_error);
		return FALSE;
	}

	__set_config_from_keyfile(self, keyfile, NULL);

	return TRUE;
}

/**
 * midgard_config_list_files:
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
gchar **midgard_config_list_files(gboolean user)
{
	gchar *config_dir;

	if(user) {
		
		gchar *_umcd = g_strconcat(".", MIDGARD_PACKAGE_NAME, NULL);
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
		
		config_dir = __get_default_confdir();
	}

	GError *error = NULL;
	GSList *list = NULL;
	gchar **filenames = NULL;
	guint i = 0, j = 0;
	GDir *dir = g_dir_open(config_dir, 0, &error);
	if (dir != NULL) {
		const gchar *file = g_dir_read_name(dir);
	
		while (file != NULL) {
			
			if(!g_str_has_prefix(file, ".")
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

	return filenames;
}

/**
 * midgard_config_save_file:
 * @self: #MidgardConfig instance
 * @name: configuration filename
 * @user: system or home directory switch
 * @error: pointer to store GError
 *
 * Saves configuration file for the given #MidgardConfig.
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
gboolean midgard_config_save_file(MidgardConfig *self,
		const gchar *name, gboolean user, GError **error)
{
	g_return_val_if_fail(self != NULL, FALSE);
	g_return_val_if_fail(name != NULL, FALSE);

	gchar *cnfpath = NULL;
	guint namel = strlen(name);
	gchar *_umcd = NULL;

	if(namel < 1) {
		g_warning ("Can not save configuration file without a name");
		return FALSE;
	}

	if(self->priv->keyfile == NULL)
		self->priv->keyfile = g_key_file_new();

	/* Check configuration directory.
	 * If user's conf.d dir doesn't exist, create it */
	if(user) {

		gchar *_cnf_path = midgard_core_config_build_path(NULL, NULL, TRUE);
		g_free (_cnf_path);

		_umcd = g_strconcat(".", MIDGARD_PACKAGE_NAME, NULL);
		gchar *path = g_build_path(G_DIR_SEPARATOR_S,
				g_get_home_dir(), _umcd, NULL);
		g_free (_umcd);

		/* Check if .midgard directory exists */
		if(!g_file_test((const gchar *)path, 
					G_FILE_TEST_EXISTS)) {
			g_mkdir(path, 0);
			g_chmod(path, 0700);
		}

		if(!g_file_test((const gchar *)path,
					G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {

			g_warning ("%s is not a directory!", path);
			g_free (path);
			return FALSE;
		}
		g_free (path);

		_umcd = g_strconcat(".", MIDGARD_PACKAGE_NAME, NULL);
		path = g_build_path(G_DIR_SEPARATOR_S, 
				g_get_home_dir(), _umcd, "conf.d", NULL);
		g_free (_umcd);
		
		/* Check if .midgard/conf.d directory exists */
		if(!g_file_test((const gchar *)path, 
					G_FILE_TEST_EXISTS)) {
			g_mkdir(path, 0);
			g_chmod(path, 0700);
		}

		if(!g_file_test((const gchar *)path,
					G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {

			g_warning ("%s is not a directory!", path);
			g_free (path);
			return FALSE;
		}
		g_free (path);

		_umcd = g_strconcat(".", MIDGARD_PACKAGE_NAME, NULL);
		cnfpath = g_build_path (G_DIR_SEPARATOR_S,
				g_get_home_dir(), _umcd, "conf.d", name, NULL);
		g_free (_umcd);

	} else {
		
		if(!g_file_test((const gchar *)self->confdir,
					G_FILE_TEST_EXISTS)
				|| (!g_file_test((const gchar *)self->confdir,
						G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))) {
			g_warning ("%s doesn't exist or is not a directory!", self->confdir);
			return FALSE;
		}

		cnfpath = g_build_path (G_DIR_SEPARATOR_S, self->confdir, name, NULL);

	}

	guint n_props, i;
	GValue pval = {0,};
	const gchar *nick = NULL;
	gchar *tmpstr;
	GParamSpec **props = g_object_class_list_properties(
			G_OBJECT_GET_CLASS(G_OBJECT(self)), &n_props);

	/* It should not happen */
	if(!props)
		g_error("Midgard Config class has no members registered");

	for(i = 0; i < n_props; i++) {
		
		nick = g_param_spec_get_nick (props[i]);

		g_value_init(&pval,props[i]->value_type); 
		g_object_get_property(G_OBJECT(self), (gchar*)props[i]->name, &pval);

		const gchar *keygroup = "MidgardDatabase";

		if (g_str_equal(props[i]->name, "sharedir")
				|| g_str_equal(props[i]->name, "vardir")
				|| g_str_equal(props[i]->name, "cachedir")
				|| g_str_equal(props[i]->name, "blobdir")) 
		{	
			keygroup = "MidgardDir";
		}


		switch(props[i]->value_type){
	
			case G_TYPE_STRING:
				tmpstr = (gchar *)g_value_get_string (&pval);
				if(!tmpstr)
					tmpstr = "";
				g_key_file_set_string (self->priv->keyfile,
						keygroup,
						nick, tmpstr);
				break;

			case G_TYPE_BOOLEAN:
				g_key_file_set_boolean(self->priv->keyfile,
						keygroup, 
						nick, 
						g_value_get_boolean(&pval));
				break;

			case G_TYPE_UINT:
				g_key_file_set_integer (self->priv->keyfile,
						keygroup, 
						nick, 
						g_value_get_uint (&pval));
				break;

		}

		g_key_file_set_comment(self->priv->keyfile, keygroup, nick,
				g_param_spec_get_blurb(props[i]), NULL);
		g_value_unset(&pval);
	}

	g_free (props);

	gsize length;
	GError *kf_error = NULL;
	
	gchar *content = g_key_file_to_data (self->priv->keyfile, &length, &kf_error);

	if (length < 1) {
	
		if (kf_error)
			g_propagate_error (error, kf_error);	
	
		return FALSE;
	}

	if (kf_error)
		g_clear_error (&kf_error);

	gboolean saved = g_file_set_contents(cnfpath, content, length, &kf_error);

	g_free (content);

	if (!saved) {

		if (kf_error)
			g_propagate_error (error, kf_error);

		g_free (cnfpath);
		return FALSE;
	}

#ifdef G_OS_WIN32
	/* TODO , implement WIN32 */
#else 
	g_chmod(cnfpath, 0600);
#endif /* G_OS_WIN32 */

	g_free (cnfpath);

	return TRUE;
}

gboolean __create_base_blobdir(const gchar *path)
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
gboolean __recurse_prepare_blobdir(const gchar *parentdir, int max_depth, int depth)
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
		gboolean err = __recurse_prepare_blobdir(dir_path, max_depth, depth + 1);
		if (dir_path) g_free (dir_path);
		if (err == FALSE)
			return FALSE;
	}
	
	return TRUE;
}

/**
 * midgard_config_create_blobdir:
 * @self: #MidgardConfig instance
 *
 * Creates directories for blobs
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 */
gboolean midgard_config_create_blobdir(MidgardConfig *self)
{
	g_assert (self != NULL);

	gboolean err = FALSE;	
	
	if (self->blobdir == NULL || *self->blobdir == '\0')
	{
		g_warning ("Invalid blobdir (NULL)");
		return FALSE;
	}

	/* Base configured blobdir might not exist, create all remaining subdirectories within path */
	err = __create_base_blobdir(self->blobdir);
	if (err == FALSE)
	{
		g_error("Failed to create configured directory %s", self->blobdir);
	}
	
	/* now create directories for blobs recursively */
	err = __recurse_prepare_blobdir(self->blobdir,
	                                2, /* set this to depth of the directories */
	                                0 /* let this be zero always */ );
	if (err == FALSE)
	{
		g_error("Failed to prepare blobdir");
		return FALSE;
	}

	return TRUE;
}

static gchar *
__get_default_confdir (void)
{
	gchar *confdir = NULL;
	confdir = g_strdup (getenv("MIDGARD_ENV_GLOBAL_CONFDIR"));

	if (!confdir || (confdir && g_str_equal (confdir, ""))) {
		g_free (confdir);

		if (g_str_equal(MIDGARD_LIB_PREFIX, "/usr")) {
			/* Using the standard prefix "/usr", so, choosing "/etc" */
			confdir = g_build_path(G_DIR_SEPARATOR_S, " ", "etc", MIDGARD_PACKAGE_NAME, "conf.d", NULL);
		} else {
			/* Using some other prefix, so choosing "prefix/etc" */
			confdir = g_build_path(G_DIR_SEPARATOR_S, MIDGARD_LIB_PREFIX, "etc", MIDGARD_PACKAGE_NAME, "conf.d", NULL);
		}
	}

	return g_strchug(confdir);
}

static void __config_struct_new(MidgardConfig *self)
{
	self->dbtype = g_strdup("MySQL");
	self->mgdusername = g_strdup("admin");
	self->mgdpassword = g_strdup("password");
	self->host = g_strdup("localhost");
	self->database = g_strdup("midgard");
	self->dbport = 0;	
	self->dbuser = g_strdup("midgard");
	self->dbpass = g_strdup("midgard");
	self->dbdir = g_strdup ("");
	self->logfilename = g_strdup ("");;
	self->loglevel = g_strdup("warn");
	self->pamfile = g_strdup ("");

	self->tablecreate = FALSE;
	self->tableupdate = FALSE;
	self->testunit = FALSE;

	/* MidgardDir */
	/* Set directories using particular order:
	   1. Get environment variables
	   2. If above are not set, try to determine FSH defaults */
	self->blobdir = g_strdup(getenv("MIDGARD_ENV_GLOBAL_BLOBDIR"));
	self->sharedir = g_strdup(getenv("MIDGARD_ENV_GLOBAL_SHAREDIR"));
	self->vardir = g_strdup(getenv("MIDGARD_ENV_GLOBAL_VARDIR"));
	self->cachedir = g_strdup(getenv("MIDGARD_ENV_GLOBAL_CACHEDIR"));

	if (!self->sharedir 
			|| (self->sharedir && g_str_equal(self->sharedir, ""))) {
		g_free (self->sharedir);
		self->sharedir = g_build_path(G_DIR_SEPARATOR_S, 
				MIDGARD_LIB_PREFIX, "share", MIDGARD_PACKAGE_NAME, NULL);
	}

	if (g_str_equal(MIDGARD_LIB_PREFIX, "/usr")) {

		if (!self->blobdir 
				|| (self->blobdir && g_str_equal(self->blobdir, ""))) {
			g_free (self->blobdir);
			self->blobdir = g_build_path(G_DIR_SEPARATOR_S, 
					G_DIR_SEPARATOR_S, "var", "lib", MIDGARD_PACKAGE_NAME, "blobs", NULL);
		}

		if (!self->vardir 
				|| (self->vardir && g_str_equal(self->vardir, ""))) {
			g_free (self->vardir);
			self->vardir = g_build_path(G_DIR_SEPARATOR_S, 
					G_DIR_SEPARATOR_S, "var", "lib", MIDGARD_PACKAGE_NAME, NULL);
		}

		if (!self->cachedir 
				|| (self->cachedir && g_str_equal(self->cachedir, ""))) {
			g_free (self->cachedir);
			self->cachedir = g_build_path(G_DIR_SEPARATOR_S, 
					G_DIR_SEPARATOR_S, "var", "cache", MIDGARD_PACKAGE_NAME, NULL);
		}

	} else if (g_str_equal(MIDGARD_LIB_PREFIX, "/usr/local")) {

		if (!self->blobdir 
				|| (self->blobdir && g_str_equal(self->blobdir, ""))) {
			g_free (self->blobdir);
			self->blobdir = g_build_path(G_DIR_SEPARATOR_S, 
					G_DIR_SEPARATOR_S, "var", "local", "lib", MIDGARD_PACKAGE_NAME, "blobs", NULL);
		}

		if (!self->vardir 
				|| (self->vardir && g_str_equal(self->vardir, ""))) {
			g_free (self->vardir);
			self->vardir = g_build_path(G_DIR_SEPARATOR_S, 
					G_DIR_SEPARATOR_S, "var", "local", "lib", MIDGARD_PACKAGE_NAME, NULL);
		}

		if (!self->cachedir 
				|| (self->cachedir && g_str_equal(self->cachedir, ""))) {
			g_free (self->cachedir);
			self->cachedir = g_build_path(G_DIR_SEPARATOR_S, 
					G_DIR_SEPARATOR_S, "var", "local", "cache", MIDGARD_PACKAGE_NAME, NULL);
		}
	
	} else {

		if (!self->blobdir 
				|| (self->blobdir && g_str_equal(self->blobdir, ""))) {
			g_free (self->blobdir);
			self->blobdir = g_build_path(G_DIR_SEPARATOR_S, 
					MIDGARD_LIB_PREFIX, "var", "lib", MIDGARD_PACKAGE_NAME, "blobs", NULL);
		}

		if (!self->vardir 
				|| (self->vardir && g_str_equal(self->vardir, ""))) {
			g_free (self->vardir);
			self->vardir = g_build_path(G_DIR_SEPARATOR_S, 
					MIDGARD_LIB_PREFIX, "var", "lib", MIDGARD_PACKAGE_NAME, NULL);
		}

		if (!self->cachedir 
				|| (self->cachedir && g_str_equal(self->cachedir, ""))) {
			g_free (self->cachedir);
			self->cachedir = g_build_path(G_DIR_SEPARATOR_S, 
					MIDGARD_LIB_PREFIX, "var", "cache", MIDGARD_PACKAGE_NAME, NULL);
		}
	}

	self->confdir = __get_default_confdir();
}

static void
__config_copy (MidgardConfig *self, MidgardConfig *src)
{
	g_free (self->dbtype);
	self->dbtype = g_strdup(src->dbtype);
	g_free (self->mgdusername);
	self->mgdusername = g_strdup(src->mgdusername);
	g_free (self->mgdpassword);
	self->mgdpassword = g_strdup(src->mgdpassword);
	g_free (self->host);
	self->host = g_strdup(src->host);
	g_free (self->database);
	self->database = g_strdup(src->database);
	self->dbport = src->dbport;	
	g_free (self->dbuser);
	self->dbuser = g_strdup(src->dbuser);
	g_free (self->dbpass);
	self->dbpass = g_strdup(src->dbpass);
	g_free (self->dbdir);
	self->dbdir = g_strdup (src->dbdir);
	g_free (self->logfilename);
	self->logfilename = g_strdup (src->logfilename);;
	g_free (self->loglevel);
	self->loglevel = g_strdup(src->loglevel);
	g_free (self->pamfile);
	self->pamfile = g_strdup (src->pamfile);

	self->tablecreate = src->tablecreate;
	self->tableupdate = src->tableupdate;
	self->testunit = src->testunit;

	g_free (self->blobdir);
	self->blobdir = g_strdup(src->blobdir);
	g_free (self->sharedir);
	self->sharedir = g_strdup(src->sharedir);
	g_free (self->vardir);
	self->vardir = g_strdup(src->vardir);
	g_free (self->cachedir);
	self->cachedir = g_strdup(src->cachedir);
	g_free (self->confdir);
	self->confdir = g_strdup(src->confdir);
	self->gdathreads = src->gdathreads;

	return;
}

void 
__midgard_config_struct_free (MidgardConfig *self)
{
	g_free (self->mgdusername);
	self->mgdusername = NULL;

	g_free (self->mgdpassword);
	self->mgdpassword = NULL;
	
	g_free (self->host);
	self->host = NULL;
	
	g_free (self->database);	
	self->database = NULL;

	self->dbport = 0;

	g_free (self->dbuser);
	self->dbuser = NULL;

	g_free (self->dbpass);
	self->dbpass = NULL;

	g_free (self->dbdir);
	self->dbdir = NULL;

	g_free (self->logfilename);
	self->logfilename = NULL;

	g_free (self->schemadir);
	self->schemadir = NULL;
		
	g_free (self->loglevel);
	self->loglevel = NULL;
	
	g_free (self->pamfile);
	self->pamfile = NULL;
	
	g_free (self->confdir);
	self->confdir = NULL;

	/* MidgardDir */
	if (self->blobdir)
		g_free (self->blobdir);
	self->blobdir = NULL;

	if (self->sharedir)
		g_free (self->sharedir);
	self->sharedir = NULL;

	if (self->cachedir)
		g_free (self->cachedir);
	self->cachedir = NULL;

	if (self->vardir)
		g_free (self->vardir);
	self->vardir = NULL;

	if (self->viewsdir)
		g_free (self->viewsdir);
	self->viewsdir = NULL;

	if (self->dbtype)
		g_free (self->dbtype);
	self->dbtype = NULL;

	/* Free priv */
	if (self->priv->keyfile)
		g_key_file_free (self->priv->keyfile);
	self->priv->keyfile = NULL;

	g_free (self->priv->configname);
	self->priv->configname = NULL;

	if (self->priv->output_stream)
		g_object_unref (self->priv->output_stream);
	self->priv->output_stream = NULL;
	if (self->priv->g_file)
		g_object_unref  (self->priv->g_file);
	self->priv->g_file = NULL;

	g_free (self->priv);
	self->priv = NULL;

}

/* API helpers, getters */
const gchar *midgard_config_get_database_name(MidgardConfig *self)
{
	g_assert (self != NULL);	
	return self->database;
}

const gchar *midgard_config_get_database_type(MidgardConfig *self)
{
	g_assert (self != NULL);
	return self->dbtype;
}

const gchar *midgard_config_get_database_username(MidgardConfig *self)
{
	g_assert (self != NULL);
	return self->dbuser;
}

const gchar *midgard_config_get_database_password(MidgardConfig *self)
{
	g_assert (self != NULL);
	return self->dbpass;
}

const gchar *midgard_config_get_database_host(MidgardConfig *self)
{
	g_assert (self != NULL);	
	return self->host;
}

guint midgard_config_get_database_port(MidgardConfig *self)
{
	g_assert (self != NULL);
	return self->dbport;
}

const gchar *midgard_config_get_blobdir(MidgardConfig *self)
{
	g_assert (self != NULL);	
	return self->blobdir;
}

const gchar *midgard_config_get_logfile(MidgardConfig *self)
{
	g_assert (self != NULL);	
	return self->logfilename;
}

const gchar *midgard_config_get_loglevel(MidgardConfig *self)
{
	g_assert (self != NULL);	
	return self->loglevel;
}

gboolean midgard_config_get_table_create(MidgardConfig *self)
{
	g_assert (self != NULL);
	return self->tablecreate;
}

gboolean midgard_config_get_table_update(MidgardConfig *self)
{
	g_assert (self != NULL);	
	return self->tableupdate;
}

gboolean midgard_config_get_testunit(MidgardConfig *self)
{
	g_assert (self != NULL);	
	return self->testunit;
}

const gchar *midgard_config_get_midgard_username(MidgardConfig *self)
{
	g_assert (self != NULL);
	return  self->mgdusername;
}

const gchar *midgard_config_get_midgard_password(MidgardConfig *self)
{
	g_assert (self != NULL);	
	return self->mgdpassword;
}

const gchar *midgard_config_get_authtype(MidgardConfig *self)
{
	g_assert (self != NULL);	

	return (const gchar *) self->authtype;
}

const gchar *midgard_config_get_pamfile(MidgardConfig *self)
{
	g_assert (self != NULL);
	return self->pamfile;
}

/* setters */
void midgard_config_set_database_name(MidgardConfig *self, const gchar *name)
{
	g_assert (self != NULL && name != NULL);
	
	if(self->database)
		g_free (self->database);
	
	self->database = g_strdup(name);
}

void midgard_config_set_database_type(MidgardConfig *self, const gchar *type)
{
	g_assert (self != NULL && type != NULL);
	
	if(self->dbtype)
		g_free (self->dbtype);

	self->dbtype = NULL;

	__set_dbtype(self, type);
}

void midgard_config_set_database_username(MidgardConfig *self, const gchar *username)
{
	g_assert (self != NULL && username != NULL);

	if(self->dbuser)
		g_free (self->dbuser);

	self->dbuser = g_strdup(username);
}

void midgard_config_set_database_password(MidgardConfig *self, const gchar *password)
{
	g_assert (self != NULL && password != NULL);

	if(self->dbpass)
		g_free (self->dbpass);

	self->dbpass = g_strdup(password);
}

void midgard_config_set_database_host(MidgardConfig *self, const gchar *host)
{
	g_assert (self != NULL && host != NULL);

	if(self->host)
		g_free (self->host);

	self->host = g_strdup(host);
}

void midgard_config_set_database_port(MidgardConfig *self, guint port)
{
	g_assert (self != NULL);
	self->dbport = port;
}

void midgard_config_set_blobdir(MidgardConfig *self, const gchar *blobdir)
{
	g_assert (self != NULL && blobdir != NULL);

	if(self->blobdir)
		g_free (self->blobdir);

	self->blobdir = g_strdup(blobdir);
}

void midgard_config_set_logfile(MidgardConfig *self, const gchar *logfile)
{
	g_assert (self != NULL && logfile != NULL);

	if(self->logfilename)
		g_free (self->logfilename);

	self->logfilename = g_strdup(logfile);
}

void midgard_config_set_loglevel(MidgardConfig *self, const gchar *loglevel)
{
	g_assert (self != NULL && loglevel != NULL);

	if(self->loglevel)
		g_free (self->loglevel);

	self->loglevel = g_strdup(loglevel);
}

void midgard_config_set_table_create(MidgardConfig *self, gboolean toggle)
{
	g_assert (self != NULL);
	self->tablecreate = toggle;
}

void midgard_config_set_table_update(MidgardConfig *self, gboolean toggle)
{
	g_assert (self != NULL);
	self->tableupdate = toggle;
}

void midgard_config_set_testunit(MidgardConfig *self, gboolean toggle)
{
	g_assert (self != NULL);
	self->testunit = toggle;
}

void midgard_config_set_midgard_username(MidgardConfig *self, const gchar *username)
{
	g_assert (self != NULL && username != NULL);

	if(self->mgdusername)
		g_free (self->mgdusername);

	self->mgdusername = g_strdup(username);
}

void midgard_config_set_midgard_password(MidgardConfig *self, const gchar *password)
{
	g_assert (self != NULL && password != NULL);

	if(self->mgdpassword)
		g_free (self->mgdpassword);

	self->mgdpassword = g_strdup(password);
}

void midgard_config_set_authtype(MidgardConfig *self, const gchar *authtype)
{
	g_assert (self != NULL);

	if(authtype == NULL) 
		self->authtype = (gchar *) authtype;
}

void midgard_config_set_pamfile(MidgardConfig *self, const gchar *pamfile)
{
	g_assert (self != NULL && pamfile != NULL);

	if(self->pamfile)
		g_free (self->pamfile);

	self->pamfile = g_strdup(pamfile);
}

/**
 * midgard_config_copy:
 * @self: #MidgardConfig instance
 *
 * Returns: deep copy of given #MidgardConfig object
 * Since: 10.05
 */ 
MidgardConfig *
midgard_config_copy (MidgardConfig *self) 
{
	MidgardConfig *copy = midgard_config_new ();
	__config_copy (copy, self);

	if (self->priv->configname)
		copy->priv->configname = g_strdup (self->priv->configname);

	if (copy->logfilename && self->priv->output_stream) {
		copy->priv->g_file = g_file_new_for_path ((const gchar *)self->logfilename);
		copy->priv->output_stream = g_file_append_to (copy->priv->g_file, G_FILE_CREATE_NONE, NULL, NULL);
	}

	copy->priv->dbtype = self->priv->dbtype;

	return copy;
}

/* GOBJECT ROUTINES */
static void
_midgard_config_set_property (GObject *object, guint property_id,
		        const GValue *value, GParamSpec *pspec){
	
	MidgardConfig *self = (MidgardConfig *) object;

	switch (property_id) {

		case MIDGARD_CONFIG_DBTYPE:
			__set_dbtype(self, (gchar *)g_value_get_string (value));
			//self->dbtype = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_DBNAME:
			g_free (self->database);
			self->database = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_DBUSER: 
			g_free (self->dbuser);
			self->dbuser = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_DBPASS:
			g_free (self->dbpass);
			self->dbpass = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_HOST:
			g_free (self->host);
			self->host = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_DBPORT:
			self->dbport = g_value_get_uint (value);
			break;

		case MIDGARD_CONFIG_DBDIR:
			g_free (self->dbdir);
			self->dbdir = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_LOGFILENAME:
			g_free (self->logfilename);
			self->logfilename = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_LOGLEVEL:
			g_free (self->loglevel);
			self->loglevel = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_TABLECREATE:
			self->tablecreate = g_value_get_boolean(value);
			break;

		case MIDGARD_CONFIG_TABLEUPDATE:
			self->tableupdate = g_value_get_boolean(value);
			break;

		case MIDGARD_CONFIG_TESTUNIT:
			self->testunit = g_value_get_boolean(value);
			break;
		
		case MIDGARD_CONFIG_MGDUSERNAME:
			g_free (self->mgdusername);
			self->mgdusername = g_value_dup_string (value);
			break;
			
		case MIDGARD_CONFIG_MGDPASSWORD:
			g_free (self->mgdpassword);
			self->mgdpassword = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_AUTHTYPE:
			g_free (self->authtype);
			self->authtype = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_PAMFILE:
			g_free (self->pamfile);
			self->pamfile = g_value_dup_string (value);
			break;

		/* MidgardDir */
		case MIDGARD_CONFIG_BLOBDIR:
			g_free (self->blobdir);
			self->blobdir = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_SHAREDIR:
			g_free (self->sharedir);
			self->sharedir = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_VARDIR:
			g_free (self->vardir);
			self->vardir = g_value_dup_string (value);
			break;

		case MIDGARD_CONFIG_CACHEDIR:
			g_free (self->cachedir);
			self->cachedir = g_value_dup_string (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
			break;
	}	
}

static void
_midgard_config_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec){

	MidgardConfig *self = (MidgardConfig *) object;
	
	switch (property_id) {
		
		case MIDGARD_CONFIG_DBTYPE:
			g_value_set_string (value, self->dbtype);
			break;
	
		case MIDGARD_CONFIG_DBNAME: 
			g_value_set_string (value, self->database);
			break;

		case MIDGARD_CONFIG_DBUSER: 
			g_value_set_string (value, self->dbuser);
			break;

		case MIDGARD_CONFIG_DBPASS: 
			g_value_set_string (value, self->dbpass);
			break;
		
		case MIDGARD_CONFIG_DBDIR:
			g_value_set_string (value, self->dbdir);
			break;

		case MIDGARD_CONFIG_HOST:
			g_value_set_string (value, self->host);
			break;

		case MIDGARD_CONFIG_DBPORT:
			g_value_set_uint (value, self->dbport);
			break;

		case MIDGARD_CONFIG_LOGFILENAME:
			g_value_set_string (value, self->logfilename);
			break;

		case MIDGARD_CONFIG_LOGLEVEL:
			g_value_set_string (value, self->loglevel);
			break;

		case MIDGARD_CONFIG_TABLECREATE:
			g_value_set_boolean (value, self->tablecreate);
			break;
	
		case MIDGARD_CONFIG_TABLEUPDATE:
			g_value_set_boolean (value, self->tableupdate);
			break;

		case MIDGARD_CONFIG_TESTUNIT:
			g_value_set_boolean (value, self->testunit);
			break;
			
		case MIDGARD_CONFIG_MGDUSERNAME:
			g_value_set_string (value, self->mgdusername);
			break;
		
		case MIDGARD_CONFIG_MGDPASSWORD:
			g_value_set_string (value, self->mgdpassword);
			break;

		case MIDGARD_CONFIG_AUTHTYPE:
			g_value_set_string (value, self->authtype);
			break;

		case MIDGARD_CONFIG_PAMFILE:
			g_value_set_string (value, self->pamfile);
			break;
		
		/* MidgardDir */
		case MIDGARD_CONFIG_BLOBDIR:
			g_value_set_string (value, self->blobdir);
			break;

		case MIDGARD_CONFIG_SHAREDIR:
			g_value_set_string (value, self->sharedir);
			break;

		case MIDGARD_CONFIG_VARDIR:
			g_value_set_string (value, self->vardir);
			break;

		case MIDGARD_CONFIG_CACHEDIR:
			g_value_set_string (value, self->cachedir);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
			break;
	}
}

static void 
_midgard_config_finalize(GObject *object)
{
	g_assert (object != NULL);
	
	MidgardConfig *self = (MidgardConfig *) object;

	__midgard_config_struct_free (self);
}

static void 
_midgard_config_class_init (gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);	
	GParamSpec *pspec;
	
	gobject_class->set_property = _midgard_config_set_property;
	gobject_class->get_property = _midgard_config_get_property;
	gobject_class->finalize = _midgard_config_finalize;
	
	/* Register properties */
	pspec = g_param_spec_string ("dbtype",
			"Type",
			"Database type ( by default MySQL )",
			"MySQL",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_DBTYPE,
			pspec);
	
	pspec = g_param_spec_string ("host",
			"Host",
			"Database host ( 'localhost' by default )",
			"localhost",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_HOST,
			pspec);
	
	pspec = g_param_spec_uint ("port",
			"Port",
			"Database port ( 0 by default )",
			0, G_MAXUINT32, 0, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_DBPORT,
			pspec);	

	pspec = g_param_spec_string ("database",
			"Name",
			"Name of the database",
			"midgard",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_DBNAME,
			pspec);

	pspec = g_param_spec_string ("dbuser",
			"Username",
			"Username for user who is able to connect to database",
			"midgard",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_DBUSER,
			pspec);

	pspec = g_param_spec_string ("dbpass",
			"Password",
			"Password used by user who is able to connect to database",
			"midgard",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_DBPASS,
			pspec);
	
	pspec = g_param_spec_string ("dbdir",
			"DatabaseDir",
			"Directory for SQLite database file ('~/.midgard2/data' by default)",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_DBDIR,
			pspec);

	pspec = g_param_spec_string ("logfilename",
			"Logfile",
			"Location of the log file",
			"",
			G_PARAM_READWRITE);    
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_LOGFILENAME,
			pspec);

	pspec = g_param_spec_string ("loglevel",
			"Loglevel",
			"Log level",
			"warn",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_LOGLEVEL,
			pspec);

	pspec = g_param_spec_boolean("tablecreate",
			"TableCreate",
			"Database creation switch",
			FALSE, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_TABLECREATE,
			pspec);

	pspec = g_param_spec_boolean("tableupdate",
			"TableUpdate",
			"Database update switch",
			FALSE, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_TABLEUPDATE,
			pspec);

	pspec = g_param_spec_boolean("testunit",
			"TestUnit",
			"Database and objects testing switch",
			FALSE, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_TESTUNIT,
			pspec);
	
	pspec = g_param_spec_string ("midgardusername",
			"MidgardUsername",
			"Midgard user's login",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_MGDUSERNAME,
			pspec);
	
	pspec = g_param_spec_string ("midgardpassword",
			"MidgardPassword",
			"Midgard user's password",
			"",
			G_PARAM_READWRITE);   
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_MGDPASSWORD,
			pspec);

	pspec = g_param_spec_string ("pamfile",
			"PamFile",
			"Name of the file used with PAM authentication type",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_PAMFILE,
			pspec);
	
	pspec = g_param_spec_string ("authtype",
			"AuthType",
			"Authentication type used with connection.",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_AUTHTYPE,
			pspec);	

	/* MidgardDir */	
	pspec = g_param_spec_string ("blobdir",
			"BlobDir",
			"Location of the blobs directory.",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_BLOBDIR,
			pspec);

	pspec = g_param_spec_string ("sharedir",
			"ShareDir",
			"Directory for shared, architecture independent files.",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_SHAREDIR,
			pspec);
	
	pspec = g_param_spec_string ("vardir",
			"VarDir",
			"Application specific directories.",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_VARDIR,
			pspec);
	
	pspec = g_param_spec_string ("cachedir",
			"CacheDir",
			"Cached files",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_CONFIG_CACHEDIR,
			pspec);

}

static void _midgard_config_instance_init(
		GTypeInstance *instance, gpointer g_class) 
{
	MidgardConfig *self = (MidgardConfig *) instance;

	__config_struct_new(self);
	self->priv = midgard_config_private_new();
	__set_dbtype (self, "MySQL");
}

GType 
midgard_config_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardConfigClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_config_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardConfig),
			0,              /* n_preallocs */
			(GInstanceInitFunc) _midgard_config_instance_init/* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardConfig", &info, 0);
	}	
	return type;
}
