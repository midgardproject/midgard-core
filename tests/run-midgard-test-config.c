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
static const gchar *object_guid = NULL;

/* Initialize MidgardConnection object and create tables for configured database.
 * Do not authenticate user here as it will fail. */

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);

	/* Midgardconfig */
	g_test_add_func("/midgard_config", midgard_test_config_init);
	g_test_add_func("/midgard_config/check_properties", midgard_test_config_check_properties);
	g_test_add_func("/midgard_config/save_file", midgard_test_config_save_file);
	g_test_add_func("/midgard_config/read_file", midgard_test_config_read_file);
	g_test_add_func("/midgard_config/read_file_at_path", midgard_test_config_read_file_at_path);
	g_test_add_func("/midgard_config/list_files", midgard_test_config_list_files);

	g_test_add_func("/midgard_connection", midgard_test_connection_run);

	g_test_add_func("/midgard_database/create", midgard_test_database_run_create);
	g_test_add_func("/midgard_database/update", midgard_test_database_run_update);

	g_test_add_func("/midgard_object/basic", midgard_test_object_basic_run);
	
	midgard_init();

	/* FIXME, it should be fixed with API... */
	MidgardSchema *schema = g_object_new(MIDGARD_TYPE_SCHEMA, NULL);
	midgard_schema_init(schema, NULL);
	midgard_schema_read_dir(schema, NULL);

	MidgardConfig *config = NULL;
	mgd_global = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	g_assert(mgd_global != NULL);

	return g_test_run();

	g_object_unref (schema);
	g_object_unref (mgd_global);

	midgard_close();
}
