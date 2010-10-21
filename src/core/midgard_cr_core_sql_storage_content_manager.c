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
       
		const gchar *col_name = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (col_model));
		if (add_coma) {
			g_string_append (colnames, ", ");
			g_string_append (values, ", ");
		}
	       	g_string_append (colnames, col_name);
		const gchar *type_name = g_type_name (pspecs[i]->value_type);
		if (pspecs[i]->value_type == MIDGARD_CR_TYPE_TIMESTAMP)
			type_name = "string";
		g_string_append_printf (values, "##%s::%s", col_name, type_name);
		add_coma = TRUE;
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
__set_parameter_value (MidgardCRRepositoryObjectClass *klass, MidgardCRStorable *object, GdaHolder *holder)
{
	const gchar *pname = gda_holder_get_id (holder);
	GParamSpec *pspec = g_object_class_find_property (G_OBJECT_CLASS (klass), pname);
	GValue val = {0, };
	g_value_init (&val, pspec->value_type);
	g_object_get_property (G_OBJECT (object), pname, &val);
	gda_holder_set_value (holder, (const GValue*) &val, NULL);
	g_value_unset (&val);
}	

void 
midgard_cr_core_sql_storage_content_manager_storable_insert (
		MidgardCRStorable *storable, MidgardCRSQLStorageManager *manager,  MidgardCRObjectModel *object_model, 
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
	GSList *list;
	GdaSet *set = type_attr->prepared_sql_statement_insert_params;
	for (list = set->holders; list != NULL; list = list->next) {
		GdaHolder *holder = (GdaHolder *) list->data;
		g_print ("HAVE %s \n", gda_holder_get_id (holder));
		__set_parameter_value (rklass, storable, holder);
	}

	/* Execute INSERT query */
	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	GdaStatement *stmt = type_attr->prepared_sql_statement_insert;
        gchar *debug_sql = gda_connection_statement_to_sql (cnc, stmt, set, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
	g_debug ("Object create: %s", debug_sql);
	g_free (debug_sql);

	gint inserted = gda_connection_statement_execute_non_select (cnc, stmt, set, NULL, &err);
	if (inserted == -1) {
		*error = g_error_new (MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR, MIDGARD_CR_STORAGE_CONTENT_MANAGER_ERROR_INTERNAL,
				"Failed to initialize prepared SQL statement INSERT for given %s class. %s ", 
				G_OBJECT_TYPE_NAME (G_OBJECT (storable)), err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
	}	

	return;
}
