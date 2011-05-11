/* 
 * Copyright (C) 2007, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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
 *   */

#include "midgard_defs.h"
#include "midgard_core_config.h"

gchar *midgard_core_config_build_path(
		const gchar **dirs, const gchar *filename, gboolean user)
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
		paths[0] = g_strconcat(".", MIDGARD_PACKAGE_NAME, NULL);
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

gchar*
midgard_core_config_build_schemadir_path (const gchar *dirname)
{
	gchar *lschema_dir = NULL;

	if (dirname != NULL && *dirname == '\0') {
		
		g_warning("Empty directory given to read schema files from. Use explicit NULL for system defaults.");
		return NULL;
	}

	if (dirname != NULL)
		lschema_dir = g_build_path(G_DIR_SEPARATOR_S, dirname, MIDGARD_CONFIG_RW_SCHEMA, NULL);

	if (lschema_dir == NULL) {
		
		const gchar *env_sharedir = g_getenv("MIDGARD_ENV_GLOBAL_SHAREDIR");

		if (env_sharedir != NULL && *env_sharedir != '\0') {

			lschema_dir = g_build_path(G_DIR_SEPARATOR_S, env_sharedir, MIDGARD_CONFIG_RW_SCHEMA, NULL);
 
		} else {

			lschema_dir = g_build_path(G_DIR_SEPARATOR_S,
					MIDGARD_LIB_PREFIX, "share", MIDGARD_PACKAGE_NAME, MIDGARD_CONFIG_RW_SCHEMA, NULL);
  		}
	}

	if (!g_file_test (lschema_dir, G_FILE_TEST_IS_DIR)) {

		g_warning("%s doesn't seem to be schema directory", lschema_dir);
		
		if (lschema_dir)
			g_free(lschema_dir);

		return NULL;
	}

	return lschema_dir;
}

gchar*
midgard_core_config_build_viewdir_path (const gchar *dirname)
{
	gchar *lview_dir = NULL;

	if (dirname != NULL && *dirname == '\0') {
		
		g_warning("Empty directory given to read view files from. Use explicit NULL for system defaults.");
		return NULL;
	}

	if (dirname != NULL)
		lview_dir = g_build_path(G_DIR_SEPARATOR_S, dirname, MIDGARD_CONFIG_RW_VIEWS, NULL);

	if (lview_dir == NULL) {
		
		const gchar *env_sharedir = g_getenv("MIDGARD_ENV_GLOBAL_SHAREDIR");

		if (env_sharedir != NULL && *env_sharedir != '\0') {

			lview_dir = g_build_path(G_DIR_SEPARATOR_S, env_sharedir, MIDGARD_CONFIG_RW_VIEWS, NULL);
 
		} else {

			lview_dir = g_build_path(G_DIR_SEPARATOR_S,
					MIDGARD_LIB_PREFIX, "share", MIDGARD_PACKAGE_NAME, MIDGARD_CONFIG_RW_VIEWS, NULL);
  		}
	}

	if (!g_file_test (lview_dir, G_FILE_TEST_IS_DIR)) {
		
		if (lview_dir)
			g_free(lview_dir);

		return NULL;
	}

	return lview_dir;
}
