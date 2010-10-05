/* MidgardCore SQLStorageModelManager routines
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

static gchar *
_prepare_model_create_query (MidgardCRModel *model)
{
	GString *query = g_string_new ("INSERT INTO ");
	const gchar *model_table;
	const gchar *model_columns;

	if (MIDGARD_CR_IS_SCHEMA_MODEL (model)) {
		model_table = TABLE_NAME_SCHEMA;
		model_columns = TABLE_SCHEMA_COLUMNS;
	} else if (MIDGARD_CR_IS_STORAGE_MODEL (model)) {
		model_table = TABLE_NAME_MAPPER;
		model_columns = TABLE_MAPPER_COLUMNS;
	}	

	g_string_append_printf (query, "%s (%s) VALUES (", model_table, model_columns);

	if (MIDGARD_CR_IS_SCHEMA_MODEL (model)) {
		/* Add class name */
		g_string_append_printf (query, "'%s'", midgard_cr_model_get_name (model));
		/* Add parent class name */
		const gchar *extends = "SchemaObject";
		MidgardCRModel *parent = midgard_cr_model_get_parent (model);
		if (parent)
			extends = midgard_cr_model_get_name (parent);
		g_string_append_printf (query, ", '%s'", extends);
	} else if (MIDGARD_CR_IS_STORAGE_MODEL (model)) {
		g_warning ("FIXME");	
	}	

	g_string_append (query, ")");
	return g_string_free (query, FALSE);
}

static gchar *
_prepare_model_property_create_query (MidgardCRModel *model)
{
	GString *query = g_string_new ("INSERT INTO ");
	const gchar *model_table;
	const gchar *model_columns;

	if (MIDGARD_CR_IS_SCHEMA_MODEL_PROPERTY (model)) {
		model_table = TABLE_NAME_SCHEMA_PROPERTIES;
		model_columns = TABLE_SCHEMA_PROPERTIES_COLUMNS;
	} else if (MIDGARD_CR_IS_STORAGE_MODEL (model)) {
		model_table = TABLE_NAME_MAPPER_PROPERTIES;
		model_columns = TABLE_MAPPER_PROPERTIES_COLUMNS;
	}	

	g_string_append_printf (query, "%s (%s) VALUES (", model_table, model_columns);

	if (MIDGARD_CR_IS_SCHEMA_MODEL_PROPERTY (model)) {
		/* class_name */
		MidgardCRModel *parent = midgard_cr_model_get_parent (model);
		g_string_append_printf (query, "'%s', ", midgard_cr_model_get_name (parent));
		/* propert_name */
		g_string_append_printf (query, "'%s', ", midgard_cr_model_get_name (model));
		/* g_type_name */
		g_string_append_printf (query, "'%s', ", 
				midgard_cr_model_property_get_valuetypename (MIDGARD_CR_MODEL_PROPERTY (model)));
		/* default_value */
		g_string_append_printf (query, "'%s', ", 
				midgard_cr_model_property_get_valuedefault (MIDGARD_CR_MODEL_PROPERTY (model)));
		/* nick */
		g_string_append (query, "'FIXME', ");
		/* description */
		g_string_append_printf (query, "'%s'", 
				midgard_cr_model_property_get_description (MIDGARD_CR_MODEL_PROPERTY (model)));
	} else if (MIDGARD_CR_IS_STORAGE_MODEL_PROPERTY (model)) {
		g_warning ("FIXME");	
	}	

	g_string_append (query, ")");
	return g_string_free (query, FALSE);
}

void 
midgard_cr_core_sql_storage_model_manager_prepare_create (MidgardCRSQLStorageModelManager *manager, GError **error)
{
	g_return_if_fail (manager != NULL);
	g_return_if_fail (error == NULL || *error == NULL);

	guint n_models;
	guint i;
	MidgardCRModel **models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (manager), &n_models);
	gchar *query;
	for (i = 0; i < n_models; i++) {
	
		gchar *columns = midgard_cr_core_storage_sql_create_query_insert_columns (
				G_OBJECT (models[i]), manager->_schema_model, manager->sql_storage_model);
		gchar *values = midgard_cr_core_storage_sql_create_query_insert_values (
				G_OBJECT (models[i]), manager->_schema_model, manager->sql_storage_model);
		GString *mquery = g_string_new ("INSERT INTO ");
		g_string_append_printf (mquery, "%s (%s) VALUES (%s) \n", 
				midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (manager->sql_storage_model)),
				columns, values);
		g_free (columns);
		g_free (values);
	
		manager->_query_slist = g_slist_append (manager->_query_slist, g_string_free (mquery, FALSE));
		guint n_props;
		guint j;
		MidgardCRModel **property_models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (models[i]), &n_props);
		for (j = 0; j < n_props; j++) {
			columns = midgard_cr_core_storage_sql_create_query_insert_columns (
					G_OBJECT (property_models[i]), manager->_class_property_model, manager->sql_storage_column_model);
			values = midgard_cr_core_storage_sql_create_query_insert_values (
				G_OBJECT (property_models[i]), manager->_class_property_model, manager->sql_storage_column_model);
			mquery = g_string_new ("INSERT INTO ");
			g_string_append_printf (mquery, "%s (%s) VALUES (%s) \n", 
				midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (manager->sql_storage_column_model)),
				columns, values);
			g_free (columns);
			g_free (values);
			manager->_query_slist = g_slist_append (manager->_query_slist, g_string_free (mquery, FALSE));
		}	
	}
	return;
}

void 
midgard_cr_core_sql_storage_model_manager_prepare_update (MidgardCRSQLStorageModelManager *manager, GError **error)
{

}

void 
midgard_cr_core_sql_storage_model_manager_prepare_save (MidgardCRSQLStorageModelManager *manager, GError **error)
{

}

void 
midgard_cr_core_sql_storage_model_manager_prepare_delete (MidgardCRSQLStorageModelManager *manager, GError **error)
{

}

void 
midgard_cr_core_sql_storage_model_manager_prepare_purge (MidgardCRSQLStorageModelManager *manager, GError **error)
{

}

void 
midgard_cr_core_sql_storage_model_manager_execute (MidgardCRSQLStorageModelManager *manager, GError **error)
{
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
		/* TODO, add queries to Profiler */
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
}
