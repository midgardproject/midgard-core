/* MidgardCore SQLStorageContentManager routines
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

#include "midgard_cr_core_sql_storage_content_manager.h"
#include "midgard_cr_core_schema_object.h"
#include <sql-parser/gda-sql-parser.h>
#include "src/core/midgard_cr_core_storage_sql.h"
#include "midgard_cr_core_sqlstoragemanager.h"

#define __ADD_COLS_AND_VALUES(__model, __pspec) \
	const gchar *col_name = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (__model)); \
		if (add_coma) { \
			g_string_append (colnames, ", "); \
			g_string_append (values, ", "); \
		} \
	       	g_string_append (colnames, col_name); \
		const gchar *type_name = g_type_name (__pspec->value_type); \
		if (__pspec->value_type == MIDGARD_CR_TYPE_TIMESTAMP) \
			type_name = "string"; \
		g_string_append_printf (values, "##%s::%s", col_name, type_name); \
		add_coma = TRUE;

/* ************************* */
/* INSERT SQL QUERY ROUTINES */
/* ************************* */

static void
__initialize_statement_insert (MidgardCRRepositoryObjectClass *klass, MgdSchemaTypeAttr *type_attr, MidgardCRSQLTableModel *table_model, GError **error)
{
	GString *sql = g_string_new ("INSERT INTO ");
	guint n_props;
	guint i;
	const gchar *table = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (table_model));
	g_string_append (sql, table);

	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (klass), &n_props);
	g_return_if_fail (pspecs != NULL);

	GString *colnames = g_string_new ("");
	GString *values = g_string_new ("");
	const gchar *pk = "id"; /* FIXME, get primary from model */
	gboolean add_coma = FALSE;
	const gchar *property_name;
	
	for (i = 0; i < n_props; i++) {
		property_name = pspecs[i]->name;	
		MidgardCRSQLColumnModel *col_model = 
			MIDGARD_CR_SQL_COLUMN_MODEL (midgard_cr_model_get_model_by_name (MIDGARD_CR_MODEL (table_model), property_name));

		/* Ignore primary key and property without column model */
		if (pk && g_str_equal (property_name, pk) 
				|| col_model == NULL)
			continue;

		/* Handle referenced object case */
		if (G_TYPE_FUNDAMENTAL (pspecs[i]->value_type) == G_TYPE_OBJECT) {
			guint n_sub_models;
			guint k;
			/* Get class pointer, initialize it, it's been not referenced before */
			GObjectClass *pklass = g_type_class_peek (pspecs[i]->value_type);
			if (pklass == NULL)
				pklass = g_type_class_ref (pspecs[i]->value_type);
			/* Get all models associated with property and add those to statement */
			MidgardCRModel **sub_models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (col_model), &n_sub_models);
			if (sub_models && (pklass != NULL)) {
				for (k = 0; k < n_sub_models; k++) {
					GParamSpec *pspec = 
						g_object_class_find_property (pklass, midgard_cr_model_get_name (MIDGARD_CR_MODEL (sub_models[k])));
					__ADD_COLS_AND_VALUES (sub_models[k], pspec);
				}
				continue;
			}
			g_warning ("Invalid property %s of object type without proper model", property_name);
		}
      
	       	__ADD_COLS_AND_VALUES (col_model, pspecs[i]);	
	}

	/* FIXME, add metadata and references objects statement */
      	/* __statement_insert_add_metadata_fields (klass, colnames, values); */

	g_string_append_printf (sql, " (%s) VALUES (%s)", colnames->str, values->str);
	GdaSqlParser *parser = gda_sql_parser_new ();
	GdaStatement *stmt;
	GError *err = NULL;
	stmt = gda_sql_parser_parse_string (parser, sql->str, NULL, &err);
	g_string_free (sql, TRUE);
	g_string_free (colnames, TRUE);
	g_string_free (values, TRUE);

	if (!stmt) {
		g_propagate_error (error, err);
	       	return;
	}

	GdaSet *params;
	if (!gda_statement_get_parameters (stmt, &params, &err)) {
		g_propagate_error (error, err);
		return;
	}

	type_attr->prepared_sql_statement_insert = stmt;
	type_attr->prepared_sql_statement_insert_params = params;	

	return;
}

void 
__set_query_insert_parameters (MidgardCRStorageModel *table_model, GObjectClass *klass, GObject *object, GdaSet *set)
{
	guint n_models;
	guint i;
	MidgardCRModel **models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (table_model), &n_models);
	if (!models)
		return;

	const gchar *property_name;
	for (i = 0; i < n_models; i++) {
		guint n_sub_models;
		guint j;
		property_name = midgard_cr_model_get_name (MIDGARD_CR_MODEL (models[i]));
		GParamSpec *pspec = g_object_class_find_property (klass, property_name);
		MidgardCRModel **sub_models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (models[i]), &n_sub_models);
		if (sub_models) {
			/* FIXME, handle error case */
			GObjectClass *pklass = g_type_class_peek (pspec->value_type);
			GObject *pobject = NULL;
			g_object_get (G_OBJECT (object), property_name, &pobject, NULL);
			__set_query_insert_parameters (MIDGARD_CR_STORAGE_MODEL (models[i]), G_OBJECT_CLASS (pklass), pobject, set);
		} else {
			/* If this is primary key, exclude it from query */
			if (midgard_cr_storage_model_property_get_primary (MIDGARD_CR_STORAGE_MODEL_PROPERTY (models[i])))
				continue;	

			const gchar *property_name = midgard_cr_model_get_name (MIDGARD_CR_MODEL (models[i]));
			const gchar *column_name = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (models[i]));
			GValue val = {0, };
			g_value_init (&val, pspec->value_type);
			if (object) { 
				g_object_get_property (G_OBJECT (object), property_name, &val);
			} else {
				/*FIXME, get default values from model */
				if (pspec->value_type == G_TYPE_STRING)
					g_value_set_string (&val, "");
				if (pspec->value_type == G_TYPE_INT)
					g_value_set_int (&val, 0);
			}

			/* FIXME, get default value */
			if (G_VALUE_HOLDS_STRING (&val) && (g_value_get_string (&val) == NULL))
				g_value_set_string (&val, "");
			
			GdaHolder *holder = gda_set_get_holder (set, column_name);
			if (!holder) {
				g_critical ("Can not find value holder for %s parameter", column_name);
				continue;
			}
			GError *err = NULL;
			gda_holder_set_value (holder, (const GValue*) &val, &err);
			if (err) {
				g_warning ("Failed to set %s parameter value. %s", column_name, err->message ? err->message : "Unknown reason");
				g_clear_error (&err);
			}

			g_value_unset (&val);
		}
	}
}

void 
midgard_cr_core_sql_storage_content_manager_storable_insert (
		MidgardCRRepositoryObject *storable, MidgardCRSQLStorageManager *manager,  MidgardCRObjectModel *object_model, 
		MidgardCRSQLTableModel *table_model, GError **error) 
{
	/* Allow NULL object_model.
	 * We need to follow table_model which describes table we want insert record into */
	g_return_if_fail (storable != NULL);
	g_return_if_fail (MIDGARD_CR_IS_STORABLE (storable));
	g_return_if_fail (manager != NULL);
	g_return_if_fail (table_model != NULL);
	g_return_if_fail (error == NULL || *error == NULL);

	GError *err = NULL;

	/* If this is StorageObject, create SQL query on demand (until something faster is implemented) */
	if (MIDGARD_CR_IS_STORAGE_OBJECT (storable)) {
		gchar *query = midgard_cr_core_storage_sql_create_query_insert (G_OBJECT (storable), object_model, MIDGARD_CR_STORAGE_MODEL (table_model));
		if (!query) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to generate SQL INSERT query for given %s instance", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
			return;
		}
		gint rv = midgard_cr_core_sql_storage_manager_query_execute (manager, query, &err);
		g_free (query);
		if (err) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to execute SQL INSERT query for given %s instance. %s ", 
					G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
			return;
		}
	}

	/* RepositoryObject */
	if (!MIDGARD_CR_IS_REPOSITORY_OBJECT (storable)) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_OBJECT_INVALID,
				"Expected RepositoryObject. %s given to create SQL INSERT query", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
		return;
	}

	MidgardCRRepositoryObjectClass *rklass = MIDGARD_CR_REPOSITORY_OBJECT_GET_CLASS (storable);
	MgdSchemaTypeAttr *type_attr = g_type_class_get_private ((GTypeClass*)rklass, G_OBJECT_CLASS_TYPE (rklass));
	if (!type_attr) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"Missed private data of %s class ", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
		return;
	}

	/* Check if prepared INSERT statement exists*/
	if (type_attr->prepared_sql_statement_insert == NULL) {
		/* Generate prepared INSERT statement */
		__initialize_statement_insert (rklass, type_attr, table_model, &err);
		if (err) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to initialize prepared SQL statement INSERT for given %s class. %s ", 
					G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
			return;
		}
	}
	/* Set statement parameters */
	GdaSet *set = type_attr->prepared_sql_statement_insert_params;
	__set_query_insert_parameters (MIDGARD_CR_STORAGE_MODEL (table_model), G_OBJECT_CLASS (rklass), G_OBJECT (storable), set);

	/* Execute INSERT query */
	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	GdaStatement *stmt = type_attr->prepared_sql_statement_insert;

	__STORAGE_MANAGER_UPDATE_PROFILER_WITH_STMT (manager, stmt, set);

	gint inserted = gda_connection_statement_execute_non_select (cnc, stmt, set, NULL, &err);
	if (inserted == -1) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"Failed to execute SQL statement INSERT for given '%s' object. %s ", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
	}	

	return;
}

/* ************************* */
/* UPDATE SQL QUERY ROUTINES */
/* ************************* */

#define __ADD_COLS_AND_VALUES_TO_UPDATE(__model, __pspec) \
	const gchar *col_name = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (__model)); \
	const gchar *type_name = g_type_name (__pspec->value_type); \
	if (__pspec->value_type == MIDGARD_CR_TYPE_TIMESTAMP) \
		type_name = "string"; \
	g_string_append_printf (query, "%s%s=##%s::%s", add_coma ? ", " : "", col_name, col_name, type_name); \
	add_coma = TRUE;

static void
__initialize_statement_update (MidgardCRRepositoryObjectClass *klass, MgdSchemaTypeAttr *type_attr, MidgardCRSQLTableModel *table_model, GError **error)
{
	GString *query = g_string_new ("");
	guint n_props;
	guint i;
	const gchar *table = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (table_model));
	g_string_append_printf (query, "UPDATE %s SET ", table);

	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (klass), &n_props);
	g_return_if_fail (pspecs != NULL);

	const gchar *pk = "id"; /* FIXME, get primary from model */
	gboolean add_coma = FALSE;
	const gchar *property_name;
	
	for (i = 0; i < n_props; i++) {
		property_name = pspecs[i]->name;	
		MidgardCRSQLColumnModel *col_model = 
			MIDGARD_CR_SQL_COLUMN_MODEL (midgard_cr_model_get_model_by_name (MIDGARD_CR_MODEL (table_model), property_name));

		/* Ignore primary key and property without column model */
		if (pk && g_str_equal (property_name, pk) 
				|| col_model == NULL)
			continue;

		/* Handle referenced object case */
		if (G_TYPE_FUNDAMENTAL (pspecs[i]->value_type) == G_TYPE_OBJECT) {
			guint n_sub_models;
			guint k;
			/* Get class pointer, initialize it, it's been not referenced before */
			GObjectClass *pklass = g_type_class_peek (pspecs[i]->value_type);
			if (pklass == NULL)
				pklass = g_type_class_ref (pspecs[i]->value_type);
			/* Get all models associated with property and add those to statement */
			MidgardCRModel **sub_models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (col_model), &n_sub_models);
			if (sub_models && (pklass != NULL)) {
				for (k = 0; k < n_sub_models; k++) {
					GParamSpec *pspec = 
						g_object_class_find_property (pklass, midgard_cr_model_get_name (MIDGARD_CR_MODEL (sub_models[k])));
					__ADD_COLS_AND_VALUES_TO_UPDATE (sub_models[k], pspec);
				}
				continue;
			}
			g_warning ("Invalid property %s of object type without proper model", property_name);
		}
      
	       	__ADD_COLS_AND_VALUES_TO_UPDATE (col_model, pspecs[i]);	
	}

	/* FIXME, add metadata and references objects statement */
      	/* __statement_insert_add_metadata_fields (klass, colnames, values); */

	g_string_append (query, " WHERE mgd_guid=##mgd_guid::string");
	GdaSqlParser *parser = gda_sql_parser_new ();
	GdaStatement *stmt;
	GError *err = NULL;
	stmt = gda_sql_parser_parse_string (parser, query->str, NULL, &err);
	g_string_free (query, TRUE);

	if (!stmt) {
		g_propagate_error (error, err);
	       	return;
	}

	GdaSet *params;
	if (!gda_statement_get_parameters (stmt, &params, &err)) {
		g_propagate_error (error, err);
		return;
	}

	type_attr->prepared_sql_statement_update = stmt;
	type_attr->prepared_sql_statement_update_params = params;	

	return;
}

void 
midgard_cr_core_sql_storage_content_manager_storable_update (
		MidgardCRRepositoryObject *storable, MidgardCRSQLStorageManager *manager,  MidgardCRObjectModel *object_model, 
		MidgardCRSQLTableModel *table_model, GError **error) 
{
	/* Allow NULL object_model.
	 * We need to follow table_model which describes table we want insert record into */
	g_return_if_fail (storable != NULL);
	g_return_if_fail (MIDGARD_CR_IS_STORABLE (storable));
	g_return_if_fail (manager != NULL);
	g_return_if_fail (table_model != NULL);
	g_return_if_fail (error == NULL || *error == NULL);

	GError *err = NULL;

	/* If this is StorageObject, create SQL query on demand (until something faster is implemented) */
	if (MIDGARD_CR_IS_STORAGE_OBJECT (storable)) {
		gchar *query = midgard_cr_core_storage_sql_create_query_update (G_OBJECT (storable), object_model, MIDGARD_CR_STORAGE_MODEL (table_model));
		if (!query) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to generate SQL UPDATE query for given %s instance", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
			return;
		}
		gint rv = midgard_cr_core_sql_storage_manager_query_execute (manager, query, &err);
		g_free (query);
		if (err) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to execute SQL UPDATE query for given %s instance. %s ", 
					G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
			return;
		}
	}

	/* RepositoryObject */
	if (!MIDGARD_CR_IS_REPOSITORY_OBJECT (storable)) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_OBJECT_INVALID,
				"Expected RepositoryObject. %s given to create SQL UPDATE query", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
		return;
	}

	MidgardCRRepositoryObjectClass *rklass = MIDGARD_CR_REPOSITORY_OBJECT_GET_CLASS (storable);
	MgdSchemaTypeAttr *type_attr = g_type_class_get_private ((GTypeClass*)rklass, G_OBJECT_CLASS_TYPE (rklass));
	if (!type_attr) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"Missed private data of %s class ", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
		return;
	}

	/* Check if prepared UPDATE statement exists*/
	if (type_attr->prepared_sql_statement_update == NULL) {
		/* Generate prepared UPDATE statement */
		__initialize_statement_update (rklass, type_attr, table_model, &err);
		if (err) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to initialize prepared SQL statement UPDATE for given %s class. %s ", 
					G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
			return;
		}
	}
	/* Set statement parameters */
	GdaSet *set = type_attr->prepared_sql_statement_update_params;
	__set_query_insert_parameters (MIDGARD_CR_STORAGE_MODEL (table_model), G_OBJECT_CLASS (rklass), G_OBJECT (storable), set);

	/* Execute UPDATE query */
	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	GdaStatement *stmt = type_attr->prepared_sql_statement_update;

	__STORAGE_MANAGER_UPDATE_PROFILER_WITH_STMT (manager, stmt, set);

	gint updated = gda_connection_statement_execute_non_select (cnc, stmt, set, NULL, &err);
	
	if (updated == 0) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_OBJECT_NOT_EXISTS,
				"Failed to execute SQL statement UPDATE for given '%s' object. No record identified by '%s'.", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), MIDGARD_CR_REPOSITORY_OBJECT (storable)->_guid);
		g_clear_error (&err);
	}	

	if (updated == -1) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"Failed to execute SQL statement UPDATE for given '%s' object. %s ", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
	}	

	if (updated == -2) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"SQL provider didn't return information about affected rows for '%s' object identified by '%s' ", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), MIDGARD_CR_REPOSITORY_OBJECT (storable)->_guid);
		g_clear_error (&err);
	}	

	return;
}

/* ************************* */
/* DELETE SQL QUERY ROUTINES */
/* ************************* */

static void
__initialize_statement_delete (MidgardCRRepositoryObjectClass *klass, MgdSchemaTypeAttr *type_attr, MidgardCRSQLTableModel *table_model, GError **error)
{
	GString *query = g_string_new ("");
	guint n_props;
	guint i;
	const gchar *table = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (table_model));
	g_string_append_printf (query, "DELETE FROM %s ", table);

	g_string_append (query, " WHERE mgd_guid=##mgd_guid::string");
	GdaSqlParser *parser = gda_sql_parser_new ();
	GdaStatement *stmt;
	GError *err = NULL;
	stmt = gda_sql_parser_parse_string (parser, query->str, NULL, &err);
	g_string_free (query, TRUE);

	if (!stmt) {
		g_propagate_error (error, err);
	       	return;
	}

	GdaSet *params;
	if (!gda_statement_get_parameters (stmt, &params, &err)) {
		g_propagate_error (error, err);
		return;
	}

	type_attr->prepared_sql_statement_delete = stmt;
	type_attr->prepared_sql_statement_delete_params = params;	

	return;
}

void 
midgard_cr_core_sql_storage_content_manager_storable_purge (
		MidgardCRRepositoryObject *storable, MidgardCRSQLStorageManager *manager,  MidgardCRObjectModel *object_model, 
		MidgardCRSQLTableModel *table_model, GError **error) 
{
	/* Allow NULL object_model.
	 * We need to follow table_model which describes table we want insert record into */
	g_return_if_fail (storable != NULL);
	g_return_if_fail (MIDGARD_CR_IS_STORABLE (storable));
	g_return_if_fail (manager != NULL);
	g_return_if_fail (table_model != NULL);
	g_return_if_fail (error == NULL || *error == NULL);

	GError *err = NULL;

	/* If this is StorageObject, create SQL query on demand (until something faster is implemented) */
	if (MIDGARD_CR_IS_STORAGE_OBJECT (storable)) {
		/* FIXME
		gchar *query = midgard_cr_core_storage_sql_create_query_delete (G_OBJECT (storable), object_model, MIDGARD_CR_STORAGE_MODEL (table_model));
		if (!query) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to generate SQL UPDATE query for given %s instance", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
			return;
		}
		gint rv = midgard_cr_core_sql_storage_manager_query_execute (manager, query, &err);
		g_free (query);
		if (err) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to execute SQL UPDATE query for given %s instance. %s ", 
					G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
			return;
		} */
	}

	/* RepositoryObject */
	if (!MIDGARD_CR_IS_REPOSITORY_OBJECT (storable)) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_OBJECT_INVALID,
				"Expected RepositoryObject. %s given to create SQL DELETE query", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
		return;
	}

	MidgardCRRepositoryObjectClass *rklass = MIDGARD_CR_REPOSITORY_OBJECT_GET_CLASS (storable);
	MgdSchemaTypeAttr *type_attr = g_type_class_get_private ((GTypeClass*)rklass, G_OBJECT_CLASS_TYPE (rklass));
	if (!type_attr) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"Missed private data of %s class ", G_OBJECT_TYPE_NAME (G_OBJECT (storable)));
		return;
	}

	/* Check if prepared DELETE statement exists*/
	if (type_attr->prepared_sql_statement_delete == NULL) {
		/* Generate prepared DELETE statement */
		__initialize_statement_delete (rklass, type_attr, table_model, &err);
		if (err) {
			*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
					"Failed to initialize prepared SQL statement DELETE for given %s class. %s ", 
					G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
			return;
		}
	}
	/* Get guid parameter and set its value */
	GdaSet *set = type_attr->prepared_sql_statement_delete_params;
	GdaHolder *holder = gda_set_get_holder (set, "mgd_guid"); /* FIXME, do not hardcode guid column name */
	GValue val = {0, };
	g_value_init (&val, G_TYPE_STRING);
	g_object_get_property (G_OBJECT (storable), "guid", &val);
        gda_holder_set_value (holder, (const GValue*) &val, &err);
	if (err) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"Failed to set GUID's parameter value for SQL statement DELETE, for given %s class. %s ", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
		return;
	}

	/* Execute DELETE query */
	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	GdaStatement *stmt = type_attr->prepared_sql_statement_delete;

	__STORAGE_MANAGER_UPDATE_PROFILER_WITH_STMT (manager, stmt, set);

	gint updated = gda_connection_statement_execute_non_select (cnc, stmt, set, NULL, &err);
	
	if (updated == 0) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_OBJECT_NOT_EXISTS,
				"Failed to execute SQL statement DELETE for given '%s' object. No record identified by '%s'.", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), MIDGARD_CR_REPOSITORY_OBJECT (storable)->_guid);
		g_clear_error (&err);
	}	

	if (updated == -1) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"Failed to execute SQL statement DELETE for given '%s' object. %s ", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
	}	

	if (updated == -2) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"SQL provider didn't return information about deleted rows for '%s' object identified by '%s' ", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), MIDGARD_CR_REPOSITORY_OBJECT (storable)->_guid);
		g_clear_error (&err);
	}	

	return;
}
