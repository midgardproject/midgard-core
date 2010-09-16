/* MidgardCore SQLStorageManager routines
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

#include "midgard_cr_core_sqlstoragemanager.h"
#include "midgard_cr_core_config.h"
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>

#define DEFAULT_DBNAME "midgard"
#define DEFAULT_DBUSER "midgard"
#define DEFAULT_DBPASS "midgard"
#define DEFAULT_DBNAME "midgard"
#define DEFAULT_DBTYPE "MYSQL"
#define DEFAULT_DBHOST "localhost"

static void 
cnc_add_part (GString *cnc, const gchar *name, const gchar *value, const gchar *def) 
{
	g_assert(cnc != NULL);
	g_assert(name != NULL);
	if (value == NULL)
		value = def;

	if (*value) {
		gchar *tmp;

		/* Add a separating semicolon if there already are
		 parameters before this one. */
		
		if (cnc->len > 0) {
			g_string_append_c(cnc, ';');
		}
		tmp = gda_rfc1738_encode (name);
		g_string_append(cnc, tmp);
		g_free (tmp);

		g_string_append_c(cnc, '=');
		
		tmp = gda_rfc1738_encode (value);
		g_string_append(cnc, tmp);
		g_free (tmp);
	}
}


gboolean 
midgard_cr_core_sql_storage_manager_open (MidgardCRSQLStorageManager *self, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
	
	MidgardCRConfig *config = midgard_cr_sql_storage_manager_get_config (self);
	g_return_val_if_fail (config != NULL, FALSE);

	if (!g_type_name (GDA_TYPE_CONNECTION))
		gda_init ();

	gchar *auth = NULL;
	const gchar *host = midgard_cr_config_get_host (config);
	const gchar *dbname = midgard_cr_config_get_dbname (config);
	const gchar *dbuser = midgard_cr_config_get_dbuser (config);
	const gchar *dbpass = midgard_cr_config_get_dbpass (config);
	const gchar *dbtype = midgard_cr_config_get_dbtype (config);
	gchar *tmpstr = NULL;
	guint port = midgard_cr_config_get_dbport (config);
	guint dbtype_id = config->dbtype_id;

	if (dbtype_id == MIDGARD_CORE_DB_TYPE_SQLITE) {

		gchar *path = NULL;
		const gchar *dbdir = midgard_cr_config_get_dbdir (config);
		if (!dbdir || *dbdir == '\0') {
			const gchar *sqlite_dir[] = {"data", NULL};
			path = midgard_cr_core_config_build_path (sqlite_dir, NULL, TRUE);
		} else {
			path = g_strdup (dbdir);
		}

		tmpstr = g_strconcat ("DB_DIR=", path, ";", "DB_NAME=", dbname, NULL);
		g_free (path);

	} else if (dbtype_id == MIDGARD_CORE_DB_TYPE_ORACLE) {

		GString *cnc = g_string_sized_new (100);
		cnc_add_part (cnc, "TNSNAME", dbname, DEFAULT_DBHOST);
		cnc_add_part (cnc, "HOST", host, DEFAULT_DBHOST);
		cnc_add_part (cnc, "DB_NAME", dbname, DEFAULT_DBTYPE);
		tmpstr = g_string_free (cnc, FALSE);
		cnc = g_string_sized_new (100);
		cnc_add_part (cnc, "USERNAME", dbuser, DEFAULT_DBUSER);
		cnc_add_part (cnc, "PASSWORD", dbpass, DEFAULT_DBPASS);
		auth = g_string_free (cnc, FALSE);

	} else { 
		
		GString *cnc = g_string_sized_new (100);
		cnc_add_part (cnc, "HOST", host, DEFAULT_DBHOST);

		if (port > 0) {

			GString *_strp = g_string_new ("");
			g_string_append_printf (_strp, "%d", port);
			cnc_add_part (cnc, "PORT", _strp->str, "");
			g_string_free (_strp, TRUE);
		}

		cnc_add_part (cnc, "DB_NAME", dbname, DEFAULT_DBTYPE);
		tmpstr = g_string_free (cnc, FALSE);
		GString *auth_str = g_string_sized_new (100);
		cnc_add_part (auth_str, "USERNAME", dbuser, DEFAULT_DBUSER);
		cnc_add_part (auth_str, "PASSWORD", dbpass, DEFAULT_DBPASS);
		auth = g_string_free (auth_str, FALSE);
	}

	GError *cnc_error = NULL;
	GdaConnection *connection = 
		gda_connection_open_from_string (dbtype, tmpstr, auth, GDA_CONNECTION_OPTIONS_NONE, &cnc_error);
	g_free(auth);	

	if (connection == NULL) { 
		g_propagate_error (error, cnc_error);
		g_free (tmpstr);
		return FALSE;	
	} 

	g_free (tmpstr);

	GdaSqlParser *parser = gda_connection_create_parser (connection);
	if (!parser)
		self->_parser = G_OBJECT (gda_sql_parser_new ());
	g_assert (self->_parser != NULL);

	self->_cnc = G_OBJECT (connection);

	return TRUE;
}

gboolean 
midgard_cr_core_sql_storage_manager_close (MidgardCRSQLStorageManager *storage_mgr, GError **error)
{
	return FALSE;
}
