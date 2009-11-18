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

gboolean midgard_test_database_create(MidgardConnection *mgd)
{
	g_assert(mgd != NULL);

	guint n_types, i;
	const gchar *typename;
	MidgardObjectClass *klass = NULL;
	gboolean class_table_updated;
       	gboolean class_table_exists;
	gboolean class_table_created;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);

	/* Fail, if we have less than 10 classes */
	g_assert_cmpint(n_types, >, 10);

	/* midgard_connection_set_loglevel(mgd, "debug", NULL); */

	gboolean base_storage_created = midgard_storage_create_base_storage(mgd);
	
	g_assert(base_storage_created != FALSE);

	for(i = 0; i < n_types; i++) {
		
		typename = g_type_name(all_types[i]);
		klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(typename);

		class_table_created = midgard_storage_create_class_storage(mgd, MIDGARD_DBOBJECT_CLASS (klass));
		g_assert(class_table_created == TRUE);

		class_table_exists = midgard_config_class_table_exists(NULL, klass, mgd);
		g_assert(class_table_exists == TRUE);		
	}
	
	g_free(all_types);

	return TRUE;
}

gboolean midgard_test_database_update(MidgardConnection *mgd)
{
	g_assert(mgd != NULL);

	guint n_types, i;
	const gchar *typename;
	MidgardObjectClass *klass = NULL;
	gboolean updated, exists;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);

	/* Fail, if we have less than 10 classes */
	g_assert_cmpint(n_types, >, 10);

	gboolean created = midgard_storage_create_base_storage(mgd);
	g_assert(created == TRUE);

	for(i = 0; i < n_types; i++) {
		
		typename = g_type_name(all_types[i]);
		klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(typename);

		updated = midgard_storage_update_class_storage (mgd, MIDGARD_DBOBJECT_CLASS (klass));
		g_assert(created == TRUE);

		exists = midgard_storage_class_storage_exists (mgd, MIDGARD_DBOBJECT_CLASS (klass));
		g_assert(exists == TRUE);		
	}
	
	g_free(all_types);

	return TRUE;
}

void midgard_test_database_run_create(void)
{
	MidgardConfig *config = NULL;
	MidgardConnection *mgd =
		midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	gboolean created = midgard_test_database_create(mgd);
	g_assert(created == TRUE);

	g_object_unref(mgd);

	return;
}

void midgard_test_database_run_update(void)
{
	MidgardConfig *config = NULL;
	MidgardConnection *mgd =
		midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	gboolean created = midgard_test_database_update(mgd);
	g_assert(created == TRUE);

	g_object_unref(mgd);

	return;
}
