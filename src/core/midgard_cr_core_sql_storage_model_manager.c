/* MidgardCore SQLModelManager routines
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

#include "midgard_cr_core_sql_storage_model_manager.h"
#include "midgard_cr_core_storage_sql.h"
#include "midgardcr.h"

void 
midgard_cr_core_sql_storage_model_manager_prepare_create (MidgardCRSQLModelManager *manager, MidgardCRModel *model, GError **error)
{
	/* 
	g_return_if_fail (manager != NULL);
	g_return_if_fail (error == NULL || *error == NULL);

	gchar *columns = midgard_cr_core_storage_sql_create_query_insert_columns (
			G_OBJECT (model), manager->_object_model, manager->sql_table_model);
	gchar *values = midgard_cr_core_storage_sql_create_query_insert_values (
			G_OBJECT (model), manager->_object_model, manager->sql_table_model);
	GString *mquery = g_string_new ("INSERT INTO ");
	g_string_append_printf (mquery, "%s (%s) VALUES (%s) \n", 
			midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (manager->sql_table_model)),
			columns, values);
	g_free (columns);
	g_free (values);

	manager->_query_slist = g_slist_append (manager->_query_slist, g_string_free (mquery, FALSE));
	guint n_props;
	guint j;
	MidgardCRModel **property_models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (model), &n_props);
	for (j = 0; j < n_props; j++) {
		columns = midgard_cr_core_storage_sql_create_query_insert_columns (
				G_OBJECT (property_models[j]), manager->_class_property_model, manager->sql_storage_column_model);
		values = midgard_cr_core_storage_sql_create_query_insert_values (
				G_OBJECT (property_models[j]), manager->_class_property_model, manager->sql_storage_column_model);
		mquery = g_string_new ("INSERT INTO ");
		g_string_append_printf (mquery, "%s (%s) VALUES (%s) \n", 
				midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (manager->sql_storage_column_model)),
				columns, values);
		g_free (columns);
		g_free (values);
		manager->_query_slist = g_slist_append (manager->_query_slist, g_string_free (mquery, FALSE));
	}	
	return;
	*/
}

void 
midgard_cr_core_sql_table_model_manager_prepare_update (MidgardCRSQLModelManager *manager, GError **error)
{

}

void 
midgard_cr_core_sql_table_model_manager_prepare_save (MidgardCRSQLModelManager *manager, GError **error)
{

}

void 
midgard_cr_core_sql_table_model_manager_prepare_delete (MidgardCRSQLModelManager *manager, GError **error)
{

}

void 
midgard_cr_core_sql_table_model_manager_prepare_purge (MidgardCRSQLModelManager *manager, GError **error)
{

}

void 
midgard_cr_core_sql_table_model_manager_execute (MidgardCRSQLModelManager *manager, GError **error)
{
	/*
	g_return_if_fail (manager != NULL);
	g_return_if_fail (error == NULL || *error == NULL);

	GSList *list;
	GError *err = NULL;
	GdaConnection *cnc = GDA_CONNECTION (manager->_storage_manager->_cnc);
	GdaSqlParser *parser = (GdaSqlParser*) manager->_storage_manager->_parser;
	for (list = manager->_query_slist; list != NULL; list = list->next) {
		const gchar *query = (gchar *) list->data;
		midgard_core_storage_sql_query_execute (cnc, parser, query, &err);
		if (err)
			*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
					"%s", err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
		g_print ("QUERY %s \n", query);
	}
	list = NULL;
	for (list = manager->_query_slist; list != NULL; list = list->next) {
		g_free (list->data);
	}
	g_slist_free (manager->_query_slist);
	manager->_query_slist = NULL;
	
	err = NULL;
	midgard_cr_core_sql_storage_manager_load_models (manager->_storage_manager, &err);
	if (err)
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err->message ? err->message : "Unknown reason");
	g_clear_error (&err);
	*/
}
