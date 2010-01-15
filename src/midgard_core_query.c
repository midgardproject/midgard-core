/* 
 * Copyright (C) 2006, 2007 Piotr Pokora <piotrek.pokora@gmail.com>
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
#include "midgard_object.h"
#include "midgard_reflection_property.h"
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_core_config.h"
#include "midgard_core_object_class.h"
#include "midgard_object_class.h"
#include "midgard_error.h"
#include "midgard_core_connection.h"
#ifdef HAVE_LIBGDA_4
#include <sql-parser/gda-sql-parser.h>
#endif

/* Do not use _DB_DEFAULT_DATETIME.
 * Some databases (like MySQL) fails to create datetime column with datetime which included timezone. 
 */ 
#define _DB_DEFAULT_DATETIME "0001-01-01 00:00:00"

static void __check_property_index(MidgardDBObjectClass *klass, MidgardReflectionProperty *mrp,
		const gchar *property, MidgardDBColumn *mdc);

MidgardDBColumn *midgard_core_dbcolumn_new(void) 
{
	MidgardDBColumn *mdc = g_new(MidgardDBColumn, 1);
	
	mdc->table_name = NULL;
	mdc->column_name = NULL;
	mdc->column_desc = NULL;
	mdc->dbtype = NULL;
	mdc->gtype = 0;
	mdc->index = FALSE;
	mdc->unique = FALSE;
	mdc->gvalue = NULL;
	mdc->primary = FALSE;
	mdc->dvalue = NULL;
	mdc->autoinc = FALSE;

	return mdc;
}

gchar *midgard_core_query_where_guid(
		const gchar *table, const gchar *guid)
{
	g_assert(guid != NULL);

	GString *where = g_string_new("");
	g_string_append_printf(where, "%s.guid = '%s' ", table, guid);

	return g_string_free(where, FALSE);
}

gint midgard_core_query_execute(MidgardConnection *mgd,
		const gchar *query,
		gboolean ignore_error)
{
	g_assert(mgd);
	g_assert(query != NULL);

#ifdef HAVE_LIBGDA_4
	GdaStatement *stmt;
	GError *lerror = NULL;
	gint number = 0;

	/* create a parser if necessary */
	if (!mgd->priv->parser) {
		mgd->priv->parser = gda_connection_create_parser (mgd->priv->connection);
		if (!mgd->priv->parser)
			mgd->priv->parser = gda_sql_parser_new ();
	}

	/* create a new statement */
	stmt = gda_sql_parser_parse_string (mgd->priv->parser, query, NULL, &lerror);
	if (stmt) {
		number = gda_connection_statement_execute_non_select (mgd->priv->connection, stmt,
								      NULL, NULL, &lerror);
		g_object_unref (stmt);
	}
#else
	GdaCommand *command;
	gint number = 0;
	command = gda_command_new(query, 
			GDA_COMMAND_TYPE_SQL, 
			GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	number  = gda_connection_execute_non_select_command(
			mgd->priv->connection, command, NULL, NULL);	
	gda_command_free(command);
#endif

	g_debug("query = %s", query);

	if(!ignore_error){
		
#ifdef HAVE_LIBGDA_4
		if (!stmt || (number == -1)) {
#else
		if (number == 0) {
#endif
			/* FIXME, error needed  */
			//gchar *fixme = "fixme";
			//g_warning("Single query failed: %s \n %s",
			//		query,
			//		fixme);
#ifdef HAVE_LIBGDA_4
			g_warning ("Error executing '%s': %s\n", query,
				   lerror && lerror->message ? lerror->message : "No detail");
#endif
		}
	}
#ifdef HAVE_LIBGDA_4
	if (lerror)
		g_error_free (lerror);
#endif

	return (number);
}

GdaDataModel *midgard_core_query_get_model(MidgardConnection *mgd, const gchar *query)
{
	g_assert(mgd != NULL);
	GError *lerror = NULL;

#ifdef HAVE_LIBGDA_4
	GdaStatement *stmt;
	GdaDataModel *model = NULL;

	/* create a parser if necessary */
	if (!mgd->priv->parser) {
		mgd->priv->parser = gda_connection_create_parser (mgd->priv->connection);
		if (!mgd->priv->parser)
			mgd->priv->parser = gda_sql_parser_new ();
	}

	/* create a new statement */
	stmt = gda_sql_parser_parse_string (mgd->priv->parser, query, NULL, &lerror);
	if (stmt) {
		model = gda_connection_statement_execute_select (mgd->priv->connection, stmt,
								 NULL, &lerror);
		g_object_unref (stmt);
	}
#else
	GdaCommand *command;
	command = gda_command_new (query, 
			GDA_COMMAND_TYPE_SQL,
			GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	GdaDataModel *model = 
		gda_connection_execute_select_command(
				mgd->priv->connection,
				command, NULL, &lerror);
	gda_command_free(command);
#endif

	if (lerror) {
	
		g_warning("Model query failed: %s", lerror && lerror->message ? lerror->message : "No description");
		g_error_free(lerror);
	}

	g_debug("Model query = %s", query);

	/* This is query error */
	if(!model) 
		return NULL;
	
	guint rows = gda_data_model_get_n_rows(model);

	if(rows == 0) {
		g_object_unref(model);
		model = NULL;
	}

	return model;
}

GValue *midgard_core_query_get_field_value(
			MidgardConnection *mgd,
			const gchar *field,
			const gchar *table,
			const gchar *where)
{
	g_assert(mgd != NULL);
	g_assert(field != NULL);
	g_assert(table != NULL);
	g_assert(where != NULL);

	GString *sql = g_string_new("SELECT ");
	g_string_append_printf(sql, " %s FROM %s WHERE %s",
			field, table, where);

	gchar *query = g_string_free(sql, FALSE);
	GdaDataModel *model =
		midgard_core_query_get_model(mgd, query);
	g_free(query);

	if(!model)
	       return NULL;
	
	if (!GDA_IS_DATA_MODEL (model)) {
	
		g_warning ("Returned model is not GDA_DATA_MODEL");
		MIDGARD_ERRNO_SET (mgd, MGD_ERR_INTERNAL);
		return NULL;

	}

	const GValue *value =
		midgard_data_model_get_value_at(model, 0, 0);

	if (!value) {

		g_object_unref (model);
		return NULL;
	}

	if (G_VALUE_TYPE (value) == GDA_TYPE_NULL) {

		g_object_unref (model);
		g_warning ("Unexpected NULL type for returned value");
		return NULL;
	}

	GValue *new_value = g_new0(GValue, 1);
	g_value_init(new_value, G_VALUE_TYPE(value));
	g_value_copy(value, new_value);

	g_object_unref(model);

	return new_value;
}

guint 
midgard_core_query_get_id (MidgardConnection *mgd, const gchar *table, const gchar *guid)
{
	g_assert(mgd != NULL);
	g_assert(table != NULL);
	g_assert(guid != NULL);

	gchar *where = 
		midgard_core_query_where_guid(table, guid);

	GValue *value = 
		midgard_core_query_get_field_value(
				mgd, "id", table, where);

	g_free(where);
	
	if(!value)
		return 0;

	guint id = 0;
	gboolean value_not_int = FALSE;

	if(G_VALUE_HOLDS(value, G_TYPE_UINT)) {
		
		id = g_value_get_uint(value);
	
	} else if (G_VALUE_HOLDS(value, G_TYPE_INT)) {

		id = g_value_get_int(value);

	} else {

		value_not_int = TRUE;
	}

	g_value_unset(value);
	g_free(value);

	if(value_not_int) {
		
		g_error("Expected uint or int type for id field. Report a bug in database!");
	}

	return id;
}

static void 
_add_value_type (GString *str, GParameter parameter, gboolean add_comma)
{
	if (add_comma) 
		g_string_append (str, ", ");

	const gchar *type = "invalid";

	switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE(&parameter.value))) {
		
		case G_TYPE_STRING:
			type = "gchararray";
		break;
	
		case G_TYPE_UINT:
			type = "guint";
		break;

		case G_TYPE_INT:
			type = "gint";
		break;

		case G_TYPE_BOOLEAN:
			type = "gboolean";
		break;	
	}

	g_string_append_printf (str, "##/*name:'%s' type:%s*/", parameter.name, type);
}

GdaDataModel *
midgard_core_query_get_dbobject_model (MidgardConnection *mgd, MidgardDBObjectClass *klass, guint n_params, const GParameter *parameters)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (GDA_IS_CONNECTION (mgd->priv->connection), NULL);
	g_return_val_if_fail (klass != NULL, NULL);

	MgdSchemaTypeAttr *type_attr = midgard_core_class_get_type_attr (klass);
	g_return_val_if_fail (type_attr->sql_select_full != NULL, NULL);

	const gchar *table = midgard_core_class_get_table (klass);
	g_return_val_if_fail (table != NULL, NULL);

	GString *select = g_string_new ("SELECT ");
	g_string_append_printf (select, "%s FROM %s WHERE ", type_attr->sql_select_full, table);

	guint i;
	GString *where = g_string_new("");
	for (i = 0; i < n_params; i++) {
	
		g_string_append_printf (where, "%s %s = ", i > 0 ? " AND " : "", parameters[i].name);
		_add_value_type (where, parameters[i], FALSE);
	}

	g_string_append_printf (select, " %s ", where->str);

	GdaDict *dict = gda_dict_new ();
	gda_dict_set_connection (dict, mgd->priv->connection);

	GdaQuery *query = gda_query_new_from_sql (dict, select->str, NULL);

	g_string_free (select, TRUE);
	g_string_free (where, TRUE);

        if (!query) {
		g_warning ("Failed to create GdaQuery from sql");
		return NULL;
	}

	GdaParameter *param = NULL;
        GdaParameterList *plist = gda_query_get_parameter_list (query);

	for (i = 0; i < n_params; i++) {

        	param = gda_parameter_list_find_param (plist, parameters[i].name);
		gda_parameter_set_value (param, (const GValue *) &parameters[i].value);
	}

	gda_connection_clear_events_list (gda_dict_get_connection (dict));

	GError *error = NULL;
	GdaObject *exec_res = gda_query_execute (query, plist, FALSE, &error);

	g_debug("%s", gda_query_get_sql_text(query));

	if (!exec_res) {
		
		g_warning ("%s. Query %s failed", error && error->message ? error->message : "Unknown reason", gda_query_get_sql_text (query));
		g_object_unref (dict);
		g_object_unref (query);
		return NULL;
	}

	g_object_unref (query);
	g_object_unref (dict);	

	if (!GDA_IS_DATA_MODEL (exec_res)) {

		g_warning ("Failed to retrieve data model from query");
		return NULL;
	}
	
	return GDA_DATA_MODEL (exec_res);	
}

gboolean 
midgard_core_query_create_dbobject_record (MidgardDBObject *object)
{
	g_return_val_if_fail (object != NULL, FALSE);
	
	GdaConnection *cnc = (MGD_OBJECT_CNC (object))->priv->connection;
	gboolean bv;

	MidgardDBObjectClass *klass = g_type_class_peek (G_OBJECT_TYPE (object));
	if (!klass) {

		g_warning ("Can not find class pointer for %s instance", G_OBJECT_TYPE_NAME (object));
		return FALSE;
	}

	const gchar *table = midgard_core_class_get_table (klass);
	if (!table) {
		
		g_warning ("Empty table for %s class", G_OBJECT_TYPE_NAME (object));
		return FALSE;
	}

	guint n_prop;
	guint i;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (klass), &n_prop);

	if (!pspecs) {
		
		g_warning ("Properties not registered for %s class", G_OBJECT_TYPE_NAME (object));
		return FALSE;
	}

	GdaDict *dict = gda_dict_new ();
        gda_dict_set_connection (dict , (MGD_OBJECT_CNC (object))->priv->connection);

	GString *sqlquery = g_string_new ("INSERT INTO ");
	g_string_append_printf (sqlquery, "%s ( ", table);

	GString *values = g_string_new (" VALUES( ");
	MgdSchemaPropertyAttr *prop_attr = NULL;

	/* Create parameters from properties */
	GParameter *parameters = g_new0 (GParameter, n_prop);

	for (i = 0; i < n_prop; i++) {
		
		GValue value = {0, };
		g_value_init (&value, pspecs[i]->value_type);
		g_object_get_property (G_OBJECT (object), pspecs[i]->name, &value);

		/* Convert boolean to integer, it's safe for SQLite at least */
		if (pspecs[i]->value_type == G_TYPE_BOOLEAN) {

			bv = g_value_get_boolean (&value);
			g_value_unset (&value);

			g_value_init (&value, G_TYPE_UINT);
			g_value_set_uint (&value, bv ? 1 : 0);
		}

		/* Map property to storage field */
		prop_attr = midgard_core_class_get_property_attr (klass, pspecs[i]->name);
		parameters[i].name = prop_attr->field;
		parameters[i].value = (const GValue)value;
		g_string_append_printf (sqlquery, "%s %s", i > 0 ? "," : "", prop_attr->field);
		_add_value_type (values, parameters[i], i > 0 ? TRUE : FALSE);
		g_value_unset (&value);
	}

	g_string_append_printf (sqlquery, " ) %s )", values->str);

	GError *error = NULL;
        GdaQuery *query = gda_query_new_from_sql (dict, sqlquery->str, &error);

	g_string_free (sqlquery, TRUE);
	g_string_free (values, TRUE);

	if (query) 
		g_debug("%s", gda_query_get_sql_text(query));

	if (!query || error) {
		g_warning ("Failed to create new query from string. %s", error && error->message ? error->message : "Unknown reason");

		if (query)
			g_object_unref (query);

		g_object_unref (dict);
		g_free (pspecs);
		
		return FALSE;
	}

	GdaParameterList *plist = gda_query_get_parameter_list (query);
	GdaParameter *param;

	for (i = 0; i < n_prop; i++) {

		/* Map property to storage field */
		prop_attr = midgard_core_class_get_property_attr (klass, pspecs[i]->name);
		param = gda_parameter_list_find_param (plist, prop_attr->field);
		GValue val = {0, };
		g_value_init (&val, pspecs[i]->value_type);
		g_object_get_property (G_OBJECT(object), pspecs[i]->name, &val);

		/* Convert boolean to integer, it's safe for SQLite at least */
		if (pspecs[i]->value_type == G_TYPE_BOOLEAN) {

			bv = g_value_get_boolean (&val);
			g_value_unset (&val);

			g_value_init (&val, G_TYPE_UINT);
			g_value_set_uint (&val, bv ? 1 : 0);
		}

		gda_parameter_set_value (param, (const GValue*) &val);
		g_value_unset (&val);
	}

	gda_connection_clear_events_list (cnc);

	g_free (pspecs);
	g_object_unref (dict);

	if (error)
		g_clear_error (&error);
	error = NULL;

	/* TODO , debug query */
	GdaObject *res_object = gda_query_execute (query, plist, FALSE, &error);

	g_object_unref (query);
	g_object_unref (plist);

	if (!res_object)
		return FALSE;

	g_object_unref (res_object);

	if (error) {

		g_warning ("Failed to create object record. %s", error && error->message ? error->message : "Unknown reason");
		return FALSE;
	}

	return TRUE;
}

gboolean 
midgard_core_query_update_dbobject_record (MidgardDBObject *object)
{
	g_return_val_if_fail (object != NULL, FALSE);
	
	GdaConnection *cnc = (MGD_OBJECT_CNC (object))->priv->connection;
	g_return_val_if_fail (cnc != NULL, FALSE);

	MidgardDBObjectClass *klass = g_type_class_peek (G_OBJECT_TYPE (object));
	if (!klass) {

		g_warning ("Can not find class pointer for %s instance", G_OBJECT_TYPE_NAME (object));
		return FALSE;
	}

	const gchar *table = midgard_core_class_get_table (klass);
	if (!table) {
		
		g_warning ("Empty table for %s class", G_OBJECT_TYPE_NAME (object));
		return FALSE;
	}

	guint n_prop;
	guint i;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (klass), &n_prop);

	if (!pspecs) {
		
		g_warning ("Properties not registered for %s class", G_OBJECT_TYPE_NAME (object));
		return FALSE;
	}

	const gchar *guid = MGD_OBJECT_GUID (object);
	if (!guid || (guid && *guid == '\0')) {

		g_warning ("an not update object with empty guid");
		return FALSE;
	}

	GdaDict *dict = gda_dict_new ();
        gda_dict_set_connection (dict , (MGD_OBJECT_CNC (object))->priv->connection);

	GString *sqlquery = g_string_new ("UPDATE ");
	g_string_append_printf (sqlquery, "%s SET ", table);

	MgdSchemaPropertyAttr *prop_attr = NULL;
	gboolean bv;

	/* Create parameters from properties */
	GParameter *parameters = g_new0 (GParameter, n_prop);

	for (i = 0; i < n_prop; i++) {
		
		GValue value = {0, };
		g_value_init (&value, pspecs[i]->value_type);
		g_object_get_property (G_OBJECT (object), pspecs[i]->name, &value);
		/* Map property to storage field */
		prop_attr = midgard_core_class_get_property_attr (klass, pspecs[i]->name);

		/* Convert boolean to integer, it's safe for SQLite at least */
		if (pspecs[i]->value_type == G_TYPE_BOOLEAN) {

			bv = g_value_get_boolean (&value);
			g_value_unset (&value);

			g_value_init (&value, G_TYPE_UINT);
			g_value_set_uint (&value, bv ? 1 : 0);
		}

		parameters[i].name = prop_attr->field;
		parameters[i].value = (const GValue)value;
		g_string_append_printf (sqlquery, "%s %s=", i > 0 ? "," : "", prop_attr->field);
		_add_value_type (sqlquery, parameters[i], FALSE);
		g_value_unset (&value);
	}

	g_string_append_printf (sqlquery, " WHERE %s.guid = '%s'", table, guid);  	

	GError *error = NULL;
        GdaQuery *query = gda_query_new_from_sql (dict, sqlquery->str, &error);

	g_string_free (sqlquery, TRUE);

	if (query) 
		g_debug ("%s", gda_query_get_sql_text (query));

        if (!query || error) {
		
		g_warning ("Failed to create new query from string. %s", error && error->message ? error->message : "Unknown reason");

		if (query)
			g_object_unref (query);

		g_object_unref (dict);
		g_free (pspecs);
		
		return FALSE;
	}

	GdaParameterList *plist = gda_query_get_parameter_list (query);
	GdaParameter *param;

	for (i = 0; i < n_prop; i++) {

		/* Map property to storage field */
		prop_attr = midgard_core_class_get_property_attr (klass, pspecs[i]->name);
		param = gda_parameter_list_find_param (plist, prop_attr->field);
		GValue val = {0, };
		g_value_init (&val, pspecs[i]->value_type);
		g_object_get_property (G_OBJECT(object), pspecs[i]->name, &val);
		
		/* Convert boolean to integer, it's safe for SQLite at least */
		if (pspecs[i]->value_type == G_TYPE_BOOLEAN) {

			bv = g_value_get_boolean (&val);
			g_value_unset (&val);

			g_value_init (&val, G_TYPE_UINT);
			g_value_set_uint (&val, bv ? 1 : 0);
		}

		gda_parameter_set_value (param, (const GValue*) &val);
	}

	gda_connection_clear_events_list (cnc);

	g_free (pspecs);
	g_object_unref (dict);

	if (error)
		g_clear_error (&error);
	error = NULL;

	/* TODO , debug query */
	GdaObject *res_object = gda_query_execute (query, plist, FALSE, &error);

	g_object_unref (query);
	g_object_unref (plist);

	if (!res_object)
		return FALSE;

	g_object_unref (res_object);

	if (error) {

		g_warning ("Failed to update object record. %s", error && error->message ? error->message : "Unknown reason");
		return FALSE;
	}

	return TRUE;
}
gboolean midgard_core_query_update_object_fields(MidgardDBObject *object, const gchar *field, ...)
{
	g_assert(object != NULL);

	const gchar *name = field;
	va_list var_args;
	va_start(var_args, field);	
	GValue *value_arg;
	GSList *cols = NULL;
	GSList *values = NULL;
	
	while(name != NULL){
		cols = g_slist_append(cols, (gpointer)name);
		value_arg = va_arg(var_args, GValue*);
		values = g_slist_append(values, (gpointer)value_arg);
		name = va_arg(var_args, gchar*);
	}
	va_end(var_args);	

	MidgardConnection *mgd = MGD_OBJECT_CNC(object);
	const gchar *table = midgard_core_class_get_table(MIDGARD_DBOBJECT_GET_CLASS(object));

	gchar *where = midgard_core_query_where_guid(table, MGD_OBJECT_GUID(object));

#ifdef HAVE_LIBGDA_4
	gint rv = midgard_core_query_insert_records(mgd, table, 
			(GList *)cols, (GList *)values, GDA_SQL_STATEMENT_UPDATE, where);
#else
	gint rv = midgard_core_query_insert_records(mgd, table, 
			(GList *)cols, (GList *)values, GDA_QUERY_TYPE_UPDATE, where);
#endif

	g_slist_free(cols);
	g_slist_free(values);

	g_free (where);

	if(rv == 0)
		return TRUE;

	return FALSE;
}

static gchar *
__create_insert_query (const gchar *table, GList *cols, GList *values)
{
	gchar *escaped_str;
	GString *sql_query_cols = g_string_new ("(");
	GString *sql_query_values = g_string_new ("(");
	guint i = 0;

	while (cols != NULL) {

		if (i > 0) {
			g_string_append (sql_query_cols, ", ");
			g_string_append (sql_query_values, ", ");
		}

		i++;

		g_string_append (sql_query_cols, cols->data);	

		GValue *val = (GValue *) values->data;

		if (G_VALUE_TYPE (val) == G_TYPE_BOOLEAN){
			
			g_string_append_printf (sql_query_values, "%d", g_value_get_boolean (val));		

		} else if (G_VALUE_TYPE (val) == G_TYPE_FLOAT) {

			/* We use dot as decimal separator. Always. */
			GValue fval = {0, };
			g_value_init (&fval, G_TYPE_FLOAT);
			gchar *lstring = setlocale (LC_NUMERIC, "0");
			setlocale (LC_NUMERIC, "C");
			g_value_copy (val, &fval);
			g_string_append_printf (sql_query_values, "%g", g_value_get_float (val));		
			g_value_unset (&fval);
			setlocale (LC_ALL, lstring);

		} else if (G_VALUE_TYPE (val) == MGD_TYPE_TIMESTAMP) {
			
			gchar *ts_str = midgard_timestamp_get_string (val);
			g_string_append_printf (sql_query_values, "'%s'", ts_str);
			g_free (ts_str);

		} else if (G_VALUE_TYPE (val) == G_TYPE_STRING) {

		      	/* Escape string */	
			escaped_str = midgard_core_query_escape_string (g_value_get_string (val));
			if (!escaped_str)
				escaped_str = g_strdup ("");	
			g_string_append_printf (sql_query_values, "'%s'", escaped_str);
			g_free (escaped_str);

		} else if (G_VALUE_TYPE (val) == G_TYPE_UINT) { 

				g_string_append_printf (sql_query_values, "%d", g_value_get_uint (val));

		} else if (G_VALUE_TYPE (val) == G_TYPE_INT) {

				g_string_append_printf (sql_query_values, "%d", g_value_get_int (val));
	
		} else {

			/* FIXME, handle this */
		}

		cols = cols->next;
		values = values->next;
	}

	g_string_append (sql_query_cols, ")");
	g_string_append (sql_query_values, ")");

	gchar *query_str = g_strconcat ("INSERT INTO ", table, " ", sql_query_cols->str, " VALUES ", sql_query_values->str, NULL);

	g_string_free (sql_query_cols, TRUE);
	g_string_free (sql_query_values, TRUE);

	return query_str;
}

static gchar *
__create_update_query (const gchar *table, GList *cols, GList *values, const gchar *where)
{
	gchar *escaped_str;
	GString *sql_query = g_string_new ("");
	guint i = 0;

	while (cols != NULL) {

		if (i > 0) 
			g_string_append (sql_query, ", ");

		i++;

		g_string_append_printf (sql_query, "%s=", (gchar *)cols->data);	

		GValue *val = (GValue *) values->data;

		if (G_VALUE_TYPE (val) == G_TYPE_BOOLEAN){
			
			g_string_append_printf (sql_query, "%d", g_value_get_boolean (val));		

		} else if (G_VALUE_TYPE (val) == G_TYPE_FLOAT) {

			/* We use dot as decimal separator. Always. */
			GValue fval = {0, };
			g_value_init (&fval, G_TYPE_FLOAT);
			gchar *lstring = setlocale (LC_NUMERIC, "0");
			setlocale (LC_NUMERIC, "C");
			g_value_copy (val, &fval);
			g_string_append_printf (sql_query, "%g", g_value_get_float (val));		
			g_value_unset (&fval);
			setlocale (LC_ALL, lstring);

		} else if (G_VALUE_TYPE (val) == MGD_TYPE_TIMESTAMP) {
			
			gchar *ts_str = midgard_timestamp_get_string (val);
			g_string_append_printf (sql_query, "'%s'", ts_str);
			g_free (ts_str);

		} else if (G_VALUE_TYPE (val) == G_TYPE_STRING) {

		      	/* Escape string */	
			escaped_str = midgard_core_query_escape_string (g_value_get_string (val));
			if (!escaped_str)
				escaped_str = g_strdup ("");
			g_string_append_printf (sql_query, "'%s'", escaped_str);
			g_free (escaped_str);

		} else if (G_VALUE_TYPE (val) == G_TYPE_UINT) { 

				g_string_append_printf (sql_query, "%d", g_value_get_uint (val));

		} else if (G_VALUE_TYPE (val) == G_TYPE_INT) {

				g_string_append_printf (sql_query, "%d", g_value_get_int (val));
	
		} else {

			/* FIXME, handle this */
		}

		cols = cols->next;
		values = values->next;
	}

	gchar *query_str = g_strconcat ("UPDATE ", table, " SET ", sql_query->str, " WHERE ", where, NULL);

	g_string_free (sql_query, TRUE);

	return query_str;

}

gint midgard_core_query_insert_records(MidgardConnection *mgd, 
		const gchar *table, GList *cols, GList *values, 
		guint query_type, const gchar *where)
{
	g_assert(mgd != NULL);
	guint ci = g_list_length(cols);
	guint vi = g_list_length(values);

	if(ci != vi) {

		g_warning("%d != %d. Expected columns = values", ci, vi);
		return -1;
	}

#ifdef HAVE_LIBGDA_4
	/* create statement's parts */
	GdaSqlAnyPart *top;

	switch (query_type) {
		
		case GDA_SQL_STATEMENT_INSERT:
			top = (GdaSqlAnyPart*) g_new0 (GdaSqlStatementInsert, 1);
			break;
		
		case GDA_SQL_STATEMENT_UPDATE:
			top = (GdaSqlAnyPart*) g_new0 (GdaSqlStatementUpdate, 1);
			break;
		
		default:
			g_assert_not_reached();
	}

	top->type = query_type;

	GdaSqlTable *sql_table;
	GSList *fields_list = NULL;
	GSList *expr_list = NULL;

	sql_table = gda_sql_table_new (top);
	if (gda_sql_identifier_needs_quotes (table))
		sql_table->table_name = gda_sql_identifier_add_quotes (table);
	else
		sql_table->table_name = g_strdup (table);
	for (; cols; cols = cols->next, values = values->next) {
		GdaSqlField *field;
	
		field = gda_sql_field_new (top);
		fields_list = g_slist_prepend (fields_list, field);
		if (gda_sql_identifier_needs_quotes ((const gchar *) cols->data))
			field->field_name = gda_sql_identifier_add_quotes ((const gchar *) cols->data);
		else
			field->field_name = g_strdup ((const gchar *) cols->data);

		GdaSqlExpr *expr;
		expr = gda_sql_expr_new (top);
		expr_list = g_slist_prepend (expr_list, expr);
		if (values->data)
			expr->value = gda_value_copy ((GValue *) values->data);
		else
			expr->value = NULL;
	}
	
	/* finalize statement's creation */
	switch (query_type) {
	case GDA_SQL_STATEMENT_INSERT: {
		GdaSqlStatementInsert *ins = (GdaSqlStatementInsert*) top;
		ins->table = sql_table;
		ins->fields_list = g_slist_reverse (fields_list);
		ins->values_list = g_slist_prepend (NULL, g_slist_reverse (expr_list));
		break;
	}
	case GDA_SQL_STATEMENT_UPDATE: {
		GdaSqlStatementUpdate *upd = (GdaSqlStatementUpdate*) top;
		upd->table = sql_table;
		upd->fields_list = g_slist_reverse (fields_list);
		upd->expr_list = g_slist_reverse (expr_list);
		if (where) {
			gchar *tmp = g_strdup_printf ("SELECT * FROM a WHERE %s", where);
			GdaStatement *tmpstmt;

			/* create a parser if necessary */
			if (!mgd->priv->parser) {
				mgd->priv->parser = gda_connection_create_parser (mgd->priv->connection);
				if (!mgd->priv->parser)
					mgd->priv->parser = gda_sql_parser_new ();
			}
			tmpstmt = gda_sql_parser_parse_string (mgd->priv->parser, tmp, NULL, NULL);
			g_assert (tmpstmt);
			
			GdaSqlStatement *sqlst;
			g_object_get (tmpstmt, "structure", &sqlst, NULL);
			g_object_unref (tmpstmt);


			GdaSqlStatementSelect *sel = (GdaSqlStatementSelect*) sqlst->contents;
			if (sel->where_cond) {
				upd->cond = sel->where_cond;
				GDA_SQL_ANY_PART (upd->cond)->parent = GDA_SQL_ANY_PART (upd);
				sel->where_cond = NULL;
			}
			gda_sql_statement_free (sqlst);
		}
		break;
	}
	default:
		g_assert_not_reached();
	}
	
	GdaSqlStatement *sqlst;
	sqlst = gda_sql_statement_new (query_type);
	sqlst->contents = top;

	GdaStatement *stmt;
	stmt = (GdaStatement*) g_object_new (GDA_TYPE_STATEMENT, "structure", sqlst, NULL);
	gda_sql_statement_free (sqlst);

	gchar *debug_query = gda_statement_to_sql (stmt, NULL, NULL);
	g_debug("%s", debug_query);
	g_free (debug_query);

	gint retval;
	retval = gda_connection_statement_execute_non_select (mgd->priv->connection, stmt, NULL, NULL, NULL);
	g_object_unref (stmt);

	return retval == -1 ? -1 : 0;
#else	
	gchar *query_str = NULL;
	if (query_type == GDA_QUERY_TYPE_INSERT)
		query_str = __create_insert_query (table, cols, values);
	if (query_type == GDA_QUERY_TYPE_UPDATE)
		query_str = __create_update_query (table, cols, values, where);

	g_debug ("%s", query_str);

	GError *error = NULL;
	GdaConnection *connection = mgd->priv->connection;
	GdaCommand *command = gda_command_new (query_str, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	gda_connection_execute_non_select_command (connection, command, NULL, &error);
	gda_command_free (command);
	g_free (query_str);
	
	if(error){
		g_clear_error(&error);
		return -1;
	}
	
	return 0;
#endif
}

gboolean __table_exists(MidgardConnection *mgd, const gchar *tablename)
{
#ifdef HAVE_LIBGDA_4
	GdaMetaContext mcontext = {"_tables", 1, NULL, NULL};
        mcontext.column_names = g_new (gchar *, 1);
        mcontext.column_names[0] = "table_name";
        mcontext.column_values = g_new (GValue *, 1);
        g_value_set_string ((mcontext.column_values[0] = gda_value_new (G_TYPE_STRING)), tablename);
	GError *error = NULL;
	if (!gda_connection_update_meta_store (mgd->priv->connection, &mcontext, &error)) {
		gda_value_free (mcontext.column_values[0]);
		g_warning("Failed to update meta data for table '%s': %s", tablename, 
				error && error->message ? error->message : "No detail");
		if (error)
			g_error_free(error);
		return TRUE;
	}

	GdaDataModel *dm_schema =
		gda_connection_get_meta_store_data (mgd->priv->connection,
						    GDA_CONNECTION_META_TABLES, NULL, 1,
                                                   "name", mcontext.column_values[0],
                                                   NULL);
	gda_value_free (mcontext.column_values[0]);
	if(!dm_schema) {
		g_error("Failed to retrieve tables schema");
		return TRUE;
	}

	gboolean retval = TRUE;
	if (gda_data_model_get_n_rows (dm_schema) == 0)
		retval = FALSE;
	g_object_unref (dm_schema);
	return retval;
#else	
	GdaDataModel *dm_schema =
		gda_connection_get_schema(mgd->priv->connection,
				GDA_CONNECTION_SCHEMA_TABLES, NULL ,NULL);
	if(!dm_schema) {
		g_error("Failed to retrieve tables schema");
		return TRUE;
	}

	gint rows = gda_data_model_get_n_rows(dm_schema);
	guint j;
	const GValue *value;
	
	for(j = 0; j < rows; j++) {
		value = gda_data_model_get_value_at(dm_schema, 0, j);
		if(g_str_equal(g_value_get_string(value), tablename)){
			g_object_unref(dm_schema);
			return TRUE;
		}
	}
	
	g_object_unref(dm_schema);
#endif
	return FALSE;
}

gboolean midgard_core_table_exists(MidgardConnection *mgd, const gchar *tablename)
{
	        return __table_exists(mgd, tablename);
}

/* Add primary fieldname *only* when it's integer type, otherwise set to NULL */
gboolean midgard_core_query_create_table(MidgardConnection *mgd, 
		const gchar *descr, const gchar *tablename, const gchar *primary)
{
	if (primary == NULL)
		primary = "id";

	g_assert(mgd != NULL);
	g_assert(tablename != NULL);

	if(__table_exists(mgd, tablename)) {
		
		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
				"Table %s exists. Skipping create: OK", tablename);
		return TRUE;
	}

	GdaServerOperation *op;
	GdaServerProvider *server;
	GdaConnection *cnc = mgd->priv->connection;
	
#ifdef HAVE_LIBGDA_4
	server = gda_connection_get_provider(cnc);
#else
	server = gda_connection_get_provider_obj(cnc);
#endif

	GError *error = NULL;
	op = gda_server_provider_create_operation (server, cnc,
			GDA_SERVER_OPERATION_CREATE_TABLE, NULL, &error);

	if(!op) {
		
		g_warning("Can not prepare create statements for table '%s. %s", 
				tablename, error->message);
		g_clear_error(&error);
		return FALSE;
	}
	
	gda_server_operation_set_value_at(op, tablename, NULL, "/TABLE_DEF_P/TABLE_NAME");
	gda_server_operation_set_value_at(op, tablename, NULL, "/TABLE_DEF_P/TABLE_IFNOTEXISTS");
	
	/* Add primary field if defined */
	if(primary) {

		gda_server_operation_set_value_at(op, primary, NULL, "/FIELDS_A/@COLUMN_NAME/%d", 0);
	
		/* PostgreSQL requires 'SERIAL' identifier...  */
		if(mgd->priv->config->priv->dbtype == MIDGARD_DB_TYPE_POSTGRES) {
			
			gda_server_operation_set_value_at(op, "SERIAL", NULL, "/FIELDS_A/@COLUMN_TYPE/%d", 0);

		} else {

			gda_server_operation_set_value_at(op, "INTEGER", NULL, "/FIELDS_A/@COLUMN_TYPE/%d", 0);
		}

		gda_server_operation_set_value_at(op, "TRUE", NULL, "/FIELDS_A/@COLUMN_PKEY/%d", 0);
		gda_server_operation_set_value_at(op, "TRUE", NULL, "/FIELDS_A/@COLUMN_AUTOINC/%d", 0);	
	}

	gchar *_sql = gda_server_provider_render_operation(server, cnc, op, NULL);
	g_debug("Render create table: %s", _sql);

	gboolean created = 
		gda_server_provider_perform_operation (server, cnc, op, &error);

	if(!created) {

		g_clear_error(&error);
		g_object_unref(op);
		g_warning("Failed to create '%s' table", tablename);
		return FALSE;
	}

	g_object_unref(op);
	g_clear_error(&error);

	return TRUE;
}

static gboolean __mcq_column_exists(MidgardConnection *mgd,
		MidgardDBColumn *mdc) 
{
#ifdef HAVE_LIBGDA_4
	GdaMetaContext mcontext = {"_columns", 2, NULL, NULL};
        mcontext.column_names = g_new (gchar *, 2);
        mcontext.column_names[0] = "table_name";
        mcontext.column_names[1] = "column_name";
        mcontext.column_values = g_new (GValue *, 2);
        g_value_set_string ((mcontext.column_values[0] = gda_value_new (G_TYPE_STRING)), mdc->table_name);
        g_value_set_string ((mcontext.column_values[1] = gda_value_new (G_TYPE_STRING)), mdc->column_name);
	GError *error = NULL;
	if (!gda_connection_update_meta_store (mgd->priv->connection, &mcontext, &error)) {
		gda_value_free (mcontext.column_values[0]);
		g_warning("Failed to update meta data for table '%s': %s", mdc->table_name, 
				error && error->message ? error->message : "No detail");
		if (error)
			g_error_free(error);

		return TRUE;
	}

	GdaDataModel *dm_schema =
		gda_connection_get_meta_store_data (mgd->priv->connection,
						    GDA_CONNECTION_META_FIELDS, NULL, 2,
						    "name", mcontext.column_values[0],
						    "field_name", mcontext.column_values[1]);
	gda_value_free (mcontext.column_values[0]);
	if(!dm_schema) {
		g_error("Failed to retrieve tables schema");
		return TRUE;
	}

	gboolean retval = TRUE;
	if (gda_data_model_get_n_rows (dm_schema) == 0)
		retval = FALSE;
	g_object_unref (dm_schema);
	return retval;
#else
	GdaParameter *param, *col_param;
	GdaConnection *cnc = mgd->priv->connection;
	const GValue *value;
	GError *error = NULL;
	
	GdaParameterList *paramlist = gda_parameter_list_new (NULL);
	param = gda_parameter_new_string ("name", mdc->table_name);
	col_param = gda_parameter_new_string (mdc->column_name, mdc->column_name);
	gda_parameter_list_add_param (paramlist, param);
	//gda_parameter_list_add_param (paramlist, col_param);
	
	GdaDataModel *dm_schema = gda_connection_get_schema (cnc,
			GDA_CONNECTION_SCHEMA_FIELDS,
			paramlist, &error);
	
	g_object_unref(param);
	g_object_unref(paramlist);
	g_object_unref(col_param);
	
	if(!dm_schema) {
		g_debug("Couldn't find schema for %s", mdc->table_name);
		return TRUE;
	}

	gint nbcols, ncol;
	nbcols = gda_data_model_get_n_rows (dm_schema);
	for(ncol = 0; ncol < nbcols; ncol++) {

		value = gda_data_model_get_value_at(dm_schema , 0, ncol);
		if(g_str_equal (mdc->column_name, 
					g_value_get_string(value))) {
			g_debug("Check table '%s'. Column '%s' exists",
					mdc->table_name,
					mdc->column_name);
			return TRUE;
		}
	}
#endif
	return FALSE;
}

gboolean midgard_core_query_add_column(MidgardConnection *mgd,
		MidgardDBColumn *mdc)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(mdc->table_name != NULL, FALSE);
	g_return_val_if_fail(mdc->column_name != NULL, FALSE);

	if(!mdc->column_desc)
		mdc->column_desc = mdc->column_name;

	const gchar *column_name = mdc->column_name;
	const gchar *table_name = mdc->table_name;

	if (mdc->dbtype == NULL) {
		
		g_warning("GOT NULL dbtype for %s column (%s GType)", column_name, g_type_name(mdc->gtype));
	}

	if(g_str_equal(mdc->dbtype, "longtext"))
		mdc->dbtype = "text";

	const gchar *column_type_str;
	if ((mgd->priv->config->priv->dbtype == MIDGARD_DB_TYPE_POSTGRES)
			&& (g_str_equal(mdc->dbtype,"datetime"))
	) {

		column_type_str = "timestamp";

	} else {

		column_type_str = mdc->dbtype;
	}

	GError *error = NULL;
	GdaServerOperation *op;
	GdaServerProvider *server;
	GdaConnection *cnc = mgd->priv->connection;

	g_debug("Check if column exists");
	if(__mcq_column_exists(mgd, mdc))
		return TRUE;
#ifdef HAVE_LIBGDA_4
	server = gda_connection_get_provider(cnc);
#else
	server = gda_connection_get_provider_obj(cnc);
#endif
	
	op = gda_server_provider_create_operation (server, cnc,
			GDA_SERVER_OPERATION_ADD_COLUMN, NULL, &error);
	
	if(!op) {
		
		g_warning("Can not prepare add column statement for column '%s.%s'. %s",
				table_name, column_name, error->message);
		g_clear_error(&error);
		return FALSE;
	}
	
	gda_server_operation_set_value_at(op, table_name,
			NULL, "/COLUMN_DEF_P/TABLE_NAME");
	/* NOT NULL not implemented in PostgreSQL 8.1 ( 28.02.2007 ) */
	/* Disabled for now. Probably we won't need NOT NULL 
	if(mgd->priv->config->priv->dbtype != MIDGARD_DB_TYPE_POSTGRES)
		gda_server_operation_set_value_at(op, "true",
				NULL, "/COLUMN_DEF_P/COLUMN_NNUL");
	*/
	gda_server_operation_set_value_at(op, column_name,
				NULL, "/COLUMN_DEF_P/COLUMN_NAME");
	gda_server_operation_set_value_at(op, column_type_str,
			NULL, "/COLUMN_DEF_P/COLUMN_TYPE");
	
	gchar *dval = NULL; 

	if(mdc->dvalue) {

		switch(mdc->gtype) {
		
			case MGD_TYPE_UINT:
			case MGD_TYPE_INT:
				dval = g_strdup(mdc->dvalue);
				break;

			case MGD_TYPE_FLOAT:
				dval = g_strdup(mdc->dvalue);
				break;

			case MGD_TYPE_BOOLEAN:
				dval = g_strdup(mdc->dvalue);
				break;

			default:
				dval  = g_strdup_printf("'%s'", mdc->dvalue);
				break;
		}
	} 
	else {
		
		dval = g_strdup("\'\'");
	}

	/* Default value. Default value not implemented in PostgreSQL 8.1 ( 28.02.2007 ) */
	if(!mdc->autoinc && 
			mgd->priv->config->priv->dbtype != MIDGARD_DB_TYPE_POSTGRES) 
		gda_server_operation_set_value_at(op, dval,
				NULL, "/COLUMN_DEF_P/COLUMN_DEFAULT");

	g_free(dval);

	/* Primary key */
	if(mdc->primary)
		gda_server_operation_set_value_at(op, "true",
				NULL, "/COLUMN_DEF_P/COLUMN_PKEY");

	/* Unique key */
	if(mdc->autoinc) 
		gda_server_operation_set_value_at(op, "true",
				NULL, "/COLUMN_DEF_P/COLUMN_AUTOINC");

	/* Auto increment key */
	if(mdc->unique) 
		gda_server_operation_set_value_at(op, "true",
				NULL, "/COLUMN_DEF_P/COLUMN_UNIQUE");
	
	gchar *_sql = NULL;
	_sql = gda_server_provider_render_operation(server, cnc, op, &error);
	if(_sql) {
		g_debug("Rendering column add: %s", _sql);
		g_clear_error(&error);
	} else {
		g_warning("Can not prepare SQL query. %s", error->message);
		g_clear_error(&error);
		g_object_unref(op);
		return FALSE;
	}

	gboolean created =
		gda_server_provider_perform_operation (server, cnc, op, &error);

	if(!created) {
		
		g_warning("Can not add %s column to table '%s'. %s",
				column_name, table_name, error->message);
		g_clear_error(&error);
		g_object_unref(op);
		return FALSE;
	}

	g_debug("Created %s.%s", table_name, column_name);
	/*
	mdc->index = TRUE;
	midgard_core_query_add_index(mgd, mdc);
	*/
	g_clear_error(&error);
	g_object_unref(op);
	
	return TRUE;
}

gboolean __index_exists(MidgardConnection *mgd, MidgardDBColumn *mdc, const gchar *index_name)
{
#ifdef HAVE_LIBGDA_4
	TO_IMPLEMENT; /* Libgda can't do this yet, this is for V4.2 */
#else
	g_assert(mgd != NULL);
	g_assert(mdc != NULL);
	g_assert(index_name != NULL);

	GdaParameter *param;
	GdaConnection *cnc = mgd->priv->connection;
	GError *err = NULL;
	GdaParameterList *paramlist = gda_parameter_list_new (NULL);
	param = gda_parameter_new_string ("name", mdc->table_name);
	gda_parameter_list_add_param (paramlist, param);

	GdaDataModel *dm_schema =
		gda_connection_get_schema(cnc,
				GDA_CONNECTION_SCHEMA_FIELDS, paramlist ,&err);
	if(!dm_schema) {

		if(err)
			g_warning("%s", err->message);

		g_error("Failed to retrieve table schema");
		return FALSE;
	}

	gint rows = gda_data_model_get_n_rows(dm_schema);
	guint j;
	const GValue *value;
	gboolean index_exists = FALSE;

	for(j = 0; j < rows; j++) {
	
		value = gda_data_model_get_value_at (dm_schema, 0, j);
		if (!g_str_equal(mdc->column_name, g_value_get_string (value)))
			continue;

		value = gda_data_model_get_value_at(dm_schema, 4, j);
		index_exists = g_value_get_boolean (value);
		
		if (index_exists) {
			
			g_object_unref(dm_schema);
			return TRUE;
		}
	}
	
	g_object_unref(dm_schema);
#endif
	return FALSE;
}

gboolean midgard_core_query_add_index(MidgardConnection *mgd,
		MidgardDBColumn *mdc)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(mdc->table_name != NULL, FALSE);
	g_return_val_if_fail(mdc->column_name != NULL, FALSE);

	if(!mdc->index) {
		g_debug("Index not defined for %s.%s. Not created", mdc->table_name, mdc->column_name);
		return TRUE;
	}

	gchar *index_name = g_strconcat(mdc->table_name, "_", mdc->column_name, "_idx", NULL);
	if (__index_exists(mgd, mdc, (const gchar *)index_name)) {

		g_free(index_name);
		return TRUE;
	}

	g_free(index_name);

	if(!mdc->column_desc)
		mdc->column_desc = mdc->column_name;

	const gchar *column_name = mdc->column_name;
	const gchar *table_name = mdc->table_name;

	GdaServerOperation *op;
	GdaServerProvider *server;
	GdaConnection *cnc = mgd->priv->connection;
	
#ifdef HAVE_LIBGDA_4
	server = gda_connection_get_provider(cnc);
#else
	server = gda_connection_get_provider_obj(cnc);
#endif
	
	GError *error = NULL;
	op = gda_server_provider_create_operation (server, cnc,
			GDA_SERVER_OPERATION_CREATE_INDEX, NULL, &error);
	
	if(!op) {
		
		g_warning("Can not prepare create index statement for column '%s.%s'. %s",
				table_name, column_name, error->message);
		g_clear_error(&error);
		return FALSE;
	}

	if(mdc->index) {
		
		index_name = g_strconcat(mdc->table_name, "_", mdc->column_name, "_idx", NULL);

		gda_server_operation_set_value_at(op, index_name,
				NULL, "/INDEX_DEF_P/INDEX_NAME");
		g_free(index_name);

		gda_server_operation_set_value_at(op, table_name,
			NULL, "/INDEX_DEF_P/INDEX_ON_TABLE");

		gda_server_operation_set_value_at(op, mdc->column_name,
				NULL, "/INDEX_FIELDS_S/0/INDEX_FIELD");

		gda_server_operation_set_value_at(op, " ",
				NULL, "/INDEX_DEF_P/INDEX_TYPE");

		gchar *_sql = 
			gda_server_provider_render_operation(server, cnc, op, NULL);
		g_debug("Rendering create index: %s", _sql);
	
		/* Disconnect default error callback.
		   This is a hack, there's no way to check if index exists, so we make error be silent */
		midgard_core_connection_disconnect_error_callback (mgd);
	
		gboolean created =
			gda_server_provider_perform_operation (server, cnc, op, &error);
		
		/* Connect default error callback again */
		midgard_core_connection_connect_error_callback (mgd);

		if(!created) {
			
			g_debug("Can not create index on %s.%s ( %s )",
					column_name, table_name, error->message); 
			g_clear_error(&error);
			g_object_unref(op);
			return FALSE;
		}

		g_debug("Added index on %s.%s", table_name, column_name);

		g_clear_error(&error);
		g_object_unref(op);
		
		return TRUE;
	}

	return FALSE;
}

static gboolean __create_columns(MidgardConnection *mgd, xmlNode *tbln)
{

	xmlNode *node; 
	xmlChar *clmn_name = NULL, *autoinc = NULL, *unique = NULL, *primary = NULL;
	xmlChar *index = NULL, *dbtype = NULL, *gtype = NULL, *dvalue = NULL;
	MidgardDBColumn *mdc = NULL;

	xmlChar *tablename = xmlGetProp (tbln, (const xmlChar *)"name");

	for(node = tbln->children; node != NULL; node = node->next) {
		
		if(!g_str_equal(node->name, "column"))
			continue;

		clmn_name = xmlGetProp (node, (const xmlChar *)"name");
		if(clmn_name && g_str_equal(clmn_name, "id")) {
			xmlFree (clmn_name);
			continue;
		}

		mdc = midgard_core_dbcolumn_new();
		mdc->table_name = (const gchar *)tablename;

		if(clmn_name) 
			mdc->column_name = (const gchar *)clmn_name;

		autoinc = xmlGetProp (node, (const xmlChar *)"autoinc");
		if(autoinc) { 
			if(g_str_equal(autoinc, "true"))
				mdc->autoinc = TRUE;
			g_free(autoinc);
		}

		unique = xmlGetProp (node, (const xmlChar *)"unique");
		if(unique) {
			if(g_str_equal(unique, "true"))
				mdc->unique = TRUE;
			g_free(unique);
		}

		primary = xmlGetProp (node, (const xmlChar *)"primary");
		if(primary) {
			if(g_str_equal(primary, "true"))
				mdc->primary = TRUE;
			g_free(primary);
		}

		index = xmlGetProp (node, (const xmlChar *)"index");
		if(index) {
			if(g_str_equal(index, "true"))
				mdc->index = TRUE;
			g_free(index);
		}

		dbtype = xmlGetProp (node, (const xmlChar *)"dbtype");
		if(dbtype) {
			mdc->dbtype = (const gchar *)dbtype;
		}
		
		gtype = xmlGetProp (node, (const xmlChar *)"gtype");
		if(gtype) {
			
			if(g_str_equal(gtype, "string"))
				mdc->gtype = MGD_TYPE_STRING;
			
			if(g_str_equal(gtype, "integer"))
				mdc->gtype = MGD_TYPE_INT;

			if(g_str_equal(gtype, "float"))
				mdc->gtype = MGD_TYPE_FLOAT;
			
			if(g_str_equal(gtype, "boolean"))
				mdc->gtype = MGD_TYPE_BOOLEAN;
			
			if(g_str_equal(gtype, "bool")) 
				mdc->gtype = MGD_TYPE_BOOLEAN; 

			if(g_str_equal(gtype, "datetime"))
				mdc->gtype = MGD_TYPE_TIMESTAMP;

			if(g_str_equal(gtype, "text"))
				mdc->gtype = MGD_TYPE_LONGTEXT;			
		}

		dvalue = xmlGetProp (node, (const xmlChar *)"default");
		if(dvalue) {
			mdc->dvalue = (const gchar *)dvalue;
		}

		gboolean rv = midgard_core_query_add_column(mgd, mdc);

		xmlFree (clmn_name);
		xmlFree (dbtype);
		xmlFree (gtype);
		if (dvalue)
			xmlFree (dvalue);
	
		g_free (mdc);
		mdc = NULL;

		if (!rv) {
			xmlFree (tablename);
			return FALSE;
		}

	}

	xmlFree (tablename);

	return TRUE;
}

gboolean midgard_core_query_create_basic_db(MidgardConnection *mgd)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	
	gchar *dbxml = g_build_path(G_DIR_SEPARATOR_S,
			mgd->priv->config->sharedir, "midgard_initial_database.xml", NULL);

	if(!g_file_test((const gchar *)dbxml,
				G_FILE_TEST_EXISTS)) {

		g_error("Can not read initial database definition. %s, file doesn't exist.", dbxml);
		g_free(dbxml);
		return FALSE;
	}

	xmlDoc *doc = NULL;
	xmlNode *root = NULL, *node = NULL;

	LIBXML_TEST_VERSION;
	g_debug("Reading %s", dbxml);
	doc = xmlReadFile(dbxml, NULL, 0);
	
	if (doc == NULL) {

		g_warning("Can not parse %s", dbxml);
		g_free(dbxml);
		return FALSE;
	}
	
	root = xmlDocGetRootElement(doc);

	if(!root) {
		g_warning("Can not find root element in %s", dbxml);
		xmlFreeDoc(doc);
		return FALSE;
	}

	g_free(dbxml);
	
	xmlNode *_clmn;
	for (node = root; node != NULL ; node = node->next) {
		if (node->type == XML_ELEMENT_NODE) {

			for (_clmn = node->children; _clmn; _clmn = _clmn->next) {

				if(!g_str_equal(_clmn->name, "table"))
					continue;

				xmlChar *tablename = 
					xmlGetProp (_clmn, 
							(const xmlChar *)"name");
				midgard_core_query_create_table(mgd,
						(const gchar *)tablename,
						(const gchar *)tablename, 
						"id");
				__create_columns(mgd, _clmn);

				xmlChar *mdata = 
					xmlGetProp(_clmn, (const xmlChar *)"metadata");
				if(mdata != NULL) {

					if(g_str_equal(mdata, "yes"))
						midgard_core_query_create_metadata_columns(mgd, 
								(const gchar *)tablename);

					xmlFree(mdata);
				}

				xmlFree(tablename);
			}
		}
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return TRUE;
}

gboolean midgard_core_query_create_metadata_columns(
		MidgardConnection *mgd, const gchar *tablename)
{
	g_return_val_if_fail(mgd != NULL, FALSE);

	g_debug("CREATE metadata columns...");

	const gchar *guid_columns[] = {"metadata_creator", "metadata_revisor", 
		"metadata_approver" , "metadata_locker", "metadata_owner", NULL };

	const gchar *date_columns[] = {"metadata_created", "metadata_revised", 
		"metadata_approved", "metadata_locked", "metadata_schedule_start", 
		"metadata_schedule_end", "metadata_published", "metadata_exported", 
		"metadata_imported", NULL };

	const gchar *bool_columns[] = {"metadata_hidden", "metadata_nav_noentry", 
		"metadata_deleted", "metadata_islocked", "metadata_isapproved", NULL };

	const gchar *text_columns[] = {"metadata_authors", NULL };

	const gchar *size_columns[] = {"metadata_revision", "metadata_size", 
		"metadata_score", NULL };
	
	gboolean rv = FALSE;
	guint i = 0;
	MidgardDBColumn *mdc;
	
	while(guid_columns[i] != NULL) {
		
		mdc = midgard_core_dbcolumn_new();
		mdc->table_name = tablename;
		mdc->column_name = guid_columns[i];
		mdc->gtype = MGD_TYPE_STRING;
		mdc->dbtype = "varchar(80)";	

		rv = midgard_core_query_add_column(mgd, mdc);
		g_free(mdc);
		i++;
		if(!rv) 
			return FALSE;
	}

	i = 0;
	while(date_columns[i] != NULL) {
		
		 mdc = midgard_core_dbcolumn_new();
		 mdc->table_name = tablename;
		 mdc->column_name = date_columns[i];
		 mdc->gtype = MGD_TYPE_TIMESTAMP;
		 mdc->dbtype = "datetime";
		 mdc->dvalue = _DB_DEFAULT_DATETIME;	
		
		 rv = midgard_core_query_add_column(mgd, mdc);
		 g_free(mdc);
		 i++;
		 
		 if(!rv)
			 return FALSE;
	}

	i = 0;
	while(text_columns[i] != NULL) {

		mdc = midgard_core_dbcolumn_new();
		mdc->table_name = tablename;
		mdc->column_name = text_columns[i];
		mdc->gtype = MGD_TYPE_LONGTEXT;
		mdc->dbtype = "longtext";
		mdc->dvalue = "";
		
		rv = midgard_core_query_add_column(mgd, mdc);
		g_free(mdc);
		i++;
		if(!rv)
			return FALSE;
	}

	i = 0;
	while(bool_columns[i] != NULL) {

		mdc = midgard_core_dbcolumn_new();
		mdc->table_name = tablename;
		mdc->column_name = bool_columns[i];
		mdc->gtype = MGD_TYPE_INT;
		mdc->dbtype = "int";
		mdc->dvalue = "0";

		rv = midgard_core_query_add_column(mgd, mdc);
		g_free(mdc);
		i++;
		if(!rv)
			return FALSE;
	}
	
	i = 0;
	while(size_columns[i] != NULL) {

		mdc = midgard_core_dbcolumn_new();
		mdc->table_name = tablename;
		mdc->column_name = size_columns[i];
		mdc->gtype = MGD_TYPE_INT;
		mdc->dbtype = "int";
		mdc->dvalue = "0";
		
		rv = midgard_core_query_add_column(mgd, mdc);
		g_free(mdc);
		i++;
		if(!rv)
			return FALSE;
	}

	g_debug("CREATE metadata columns. OK.");
	
	return TRUE;
}

void __check_property_index(MidgardDBObjectClass *klass, MidgardReflectionProperty *mrp,
		const gchar *property, MidgardDBColumn *mdc) 
{
	g_assert(klass != NULL);
	g_assert(property != NULL);
	g_assert(mdc != NULL);

	if(mdc->index == TRUE)
		return;

	if (midgard_reflection_property_is_link(mrp, property)) {
		
		mdc->index = TRUE;
		return;
	}

	if (midgard_reflection_property_is_linked(mrp, property)) {

		mdc->index = TRUE;
		return;
	}

	const gchar *parent = NULL;
	const gchar *up = NULL;
	if (MIDGARD_IS_OBJECT_CLASS (klass)) {
		parent = midgard_object_class_get_property_parent(MIDGARD_OBJECT_CLASS(klass));
		up = midgard_object_class_get_property_up(MIDGARD_OBJECT_CLASS(klass));	
	}

	if (parent && g_str_equal(parent, property)) {

		mdc->index = TRUE;
		return;
	}
	
	if(up && g_str_equal(up, property)) {

		mdc->index = TRUE;
		return;
	}

	MgdSchemaPropertyAttr *prop_attr;
	prop_attr = g_hash_table_lookup(klass->dbpriv->storage_data->prophash, property);
	
	if (prop_attr != NULL && prop_attr->dbindex == TRUE) {

		mdc->index = TRUE;
		return;
	}
}

gboolean midgard_core_query_update_class_storage(MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(klass != NULL, FALSE);

	const gchar *tablename = midgard_core_class_get_table(klass);
	const gchar *classname = G_OBJECT_CLASS_NAME(G_OBJECT_CLASS(klass));

	/* Class with empty storage, ignore as silently as possible */
	if (!tablename) {
		g_debug("Can not update %s class table. None defined.", classname);
		return TRUE;
	}

	guint n_prop, i = 0;

	if(!__table_exists(mgd, tablename)) {
		
		g_warning("Table %s doesn't exist. Can not update", tablename);
		return FALSE;
	}

	GParamSpec **pspecs = 
		g_object_class_list_properties(G_OBJECT_CLASS(klass), &n_prop);

	if(!pspecs) {
		
		g_debug("Class %s has no properties registered", classname);
		return FALSE;
	}

	GValue pval = {0, };
	MidgardReflectionProperty *mrp = 
		midgard_reflection_property_new(MIDGARD_DBOBJECT_CLASS(klass));
	MidgardDBColumn *mdc;
	GType mgd_type;
	const gchar *nick;
	gchar **spltd;	

	for (i = 0; i < n_prop; i++) {
	
		nick = midgard_core_class_get_property_tablefield
			(MIDGARD_DBOBJECT_CLASS(klass), pspecs[i]->name);

		if (nick == NULL)
			continue;
		
		g_value_init(&pval, pspecs[i]->value_type);
		mdc = midgard_core_dbcolumn_new();
		
		spltd = g_strsplit(nick, ".", 0);
		
		mdc->column_name = spltd[1];
		mdc->table_name = tablename;

		mgd_type = midgard_reflection_property_get_midgard_type (mrp, pspecs[i]->name);
		mdc->gtype = mgd_type;
		
		switch(pspecs[i]->value_type) {

			case G_TYPE_STRING:
				if (mgd_type == MGD_TYPE_TIMESTAMP) {
					mdc->dvalue = _DB_DEFAULT_DATETIME;
					mdc->dbtype = "datetime";
				} else if (mgd_type == MGD_TYPE_LONGTEXT) {
					mdc->dvalue = "";
					mdc->dbtype = "longtext";
				} else if (mgd_type == MGD_TYPE_GUID) {
					mdc->dvalue = "";
					mdc->dbtype = "varchar(80)";
					mdc->index = TRUE;
				} else {
					mdc->dvalue = "";
					mdc->dbtype = "varchar(255)";
				}
				
				break;

			case G_TYPE_UINT:
			case G_TYPE_INT:
				mdc->dbtype = "integer";
				mdc->dvalue = "0";
				break;

			case G_TYPE_FLOAT:
				mdc->dbtype = "float";
				mdc->dvalue = "0.00";
				break;

			case G_TYPE_BOOLEAN:
				mdc->dbtype = "bool";
				mdc->dvalue = "0";
				break;

			case G_TYPE_BOXED:
				if (mgd_type == MGD_TYPE_TIMESTAMP) {
					mdc->dvalue = _DB_DEFAULT_DATETIME;
					mdc->dbtype = "datetime";
				}

			default:
				mdc->dbtype = "varchar(255)";
				mdc->dvalue = "";
				break;
		}

		/* add column */	
		midgard_core_query_add_column(mgd, mdc);

		/* add indexes */
		__check_property_index(klass, mrp, pspecs[i]->name, mdc);
		midgard_core_query_add_index(mgd, mdc);

		g_free(mdc);
		g_strfreev(spltd);
		g_value_unset(&pval);		
	}

	g_object_unref(mrp);
	g_free(pspecs);

	if (klass->dbpriv->has_metadata) {
	
		if(!midgard_core_query_create_metadata_columns(mgd, tablename)) {
		
			g_warning("Failed to create metadata columns"); 
			return FALSE;
		}
	}


	return TRUE;
}

gboolean midgard_core_query_create_class_storage(
		MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(klass != NULL, FALSE);

	const gchar *tablename = 
		midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(klass));
	const gchar *classname = 
		G_OBJECT_CLASS_NAME(G_OBJECT_CLASS(klass));

	/* Class with empty storage, ignore as silently as possible */
	if (!tablename) {
		g_debug("Can not create %s class table. None defined.", classname);
		return TRUE;
	}

	guint n_prop;
	guint i = 0;

	if(__table_exists(mgd, tablename)) {

		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_INFO,
				"Table %s exists. Skipping create: OK", tablename);
		return TRUE;
	}

	GParamSpec **pspecs = g_object_class_list_properties(
			G_OBJECT_CLASS(klass), &n_prop);

	if(!pspecs) {

		g_debug("Class %s has no properties registered", 
				classname);
		return FALSE;
	}

	const gchar *tables = 
		midgard_core_class_get_tables(MIDGARD_DBOBJECT_CLASS(klass));
	gchar **spltd = g_strsplit(tables, ",", -1);

	/* Get PRIMARY_KEY */
	const gchar *pprop = NULL;
	const gchar *pfield = NULL;
	const gchar *primary_field = NULL;
	if (MIDGARD_IS_OBJECT_CLASS (klass))
		pprop = midgard_object_class_get_primary_property(MIDGARD_OBJECT_CLASS(klass));
	
	if(pprop) {
	
		pfield = midgard_core_class_get_property_colname(
				MIDGARD_DBOBJECT_CLASS(klass), pprop);
		GParamSpec *pspec = 
			g_object_class_find_property(G_OBJECT_CLASS(klass), pprop);
		if(pspec->value_type == G_TYPE_UINT 
				|| pspec->value_type == G_TYPE_INT) {
			primary_field = pfield;
		}
	}

	while(spltd[i] != NULL) {
		
		if(!midgard_core_query_create_table(mgd,
					classname, spltd[i], primary_field)) {
			g_free(pspecs);
			g_strfreev(spltd);
			return FALSE;
		}
		i++;
	}

	g_strfreev(spltd);

	if (klass->dbpriv->has_metadata) {
	
		if(!midgard_core_query_create_metadata_columns(mgd, tablename)) {
		
			g_free(pspecs);
			g_warning("Failed to create metadata columns"); 
			return FALSE;
		}
	}
	
	GValue pval = {0, };
	MidgardReflectionProperty *mrp = 
		midgard_reflection_property_new(MIDGARD_DBOBJECT_CLASS(klass));
	MidgardDBColumn *mdc;
	GType mgd_type;
	const gchar *nick;

	for (i = 0; i < n_prop; i++) {
	
		nick = midgard_core_class_get_property_tablefield
			(MIDGARD_DBOBJECT_CLASS(klass), pspecs[i]->name);	

		if (nick == NULL)
			continue;

		if ( pprop && (g_str_equal(pprop, pspecs[i]->name)))
			continue;
		
		g_value_init(&pval, pspecs[i]->value_type);
		mdc = midgard_core_dbcolumn_new();
		
		spltd = g_strsplit(nick, ".", 0);
		
		mdc->column_name = spltd[1];
		mdc->table_name = tablename;

		mgd_type = midgard_reflection_property_get_midgard_type (mrp, pspecs[i]->name);
		mdc->gtype = mgd_type;

		switch(pspecs[i]->value_type) {

			case G_TYPE_STRING:
				if (mgd_type == MGD_TYPE_LONGTEXT) {
					mdc->dvalue = "";
					mdc->dbtype = "longtext";
				} else if (mgd_type == MGD_TYPE_GUID) {
					mdc->dvalue = "";
					mdc->dbtype = "varchar(80)";
					mdc->index = TRUE;
				} else {
					mdc->dvalue = "";
					mdc->dbtype = "varchar(255)";
				}
				
				break;

			case G_TYPE_BOXED:
				if (mgd_type == MGD_TYPE_TIMESTAMP) {
					mdc->dvalue = _DB_DEFAULT_DATETIME;
					mdc->dbtype = "datetime";
				}
				break;

			case G_TYPE_UINT:
			case G_TYPE_INT:
				if(pfield && (g_str_equal(pfield, pspecs[i]->name))) {
					mdc->dbtype = "integer";
					mdc->primary = TRUE;
					mdc->autoinc = TRUE;
				} else {
					mdc->dbtype = "integer";
					mdc->dvalue = "0";
				}

				break;

			case G_TYPE_FLOAT:
				mdc->dbtype = "float";
				mdc->dvalue = "0.00";
				break;

			case G_TYPE_BOOLEAN:
				mdc->dbtype = "bool";
				mdc->dvalue = "0";
				break;

			default:

				break;
		}

		/* Check not fundamental types */
		if (mgd_type == MGD_TYPE_TIMESTAMP) {
			
			mdc->dvalue = _DB_DEFAULT_DATETIME;
			mdc->dbtype = "datetime";
		}
		
		/* add column */
		g_debug("Add %s column of %s type (%s GType)", pspecs[i]->name, mdc->dbtype, g_type_name(pspecs[i]->value_type));
		midgard_core_query_add_column(mgd, mdc);

		/* add indexes */
		__check_property_index(klass, mrp, pspecs[i]->name, mdc);
		midgard_core_query_add_index(mgd, mdc);

		g_free(mdc);
		g_strfreev(spltd);
		g_value_unset(&pval);		
	}

	g_object_unref(mrp);
	g_free(pspecs);

	mgd_info ("%s - Table create: OK", g_type_name(G_OBJECT_CLASS_TYPE(klass)));

	return TRUE;
}

MidgardDBJoin*
midgard_core_dbjoin_new(void)
{
	MidgardDBJoin *mdbj = g_new(MidgardDBJoin, 1);

	mdbj->type = NULL;
	mdbj->typeid = 0;
	mdbj->klass = NULL;

	/* pointers references */
	mdbj->left = NULL;
	mdbj->right = NULL;

	return mdbj;
}

void midgard_core_dbjoin_free (MidgardDBJoin *mdbj)
{
	g_assert(mdbj != NULL);

	g_free(mdbj->type);
	mdbj->type = NULL;

	mdbj->typeid = 0;	

	g_free(mdbj);
}

/* Routine which is a workaround for gda_binary_stringify, 
 * which is also registered as transform function which converts 
 * binary value to string one.
 * With this function we get GdaBinary->data 'as is' */ 
gchar *  
midgard_core_query_binary_stringify (GValue *src_value)
{
	g_return_val_if_fail (src_value != NULL, NULL);

	const GdaBinary *binary = gda_value_get_binary ((const GValue*) src_value);

	if (binary == NULL || (binary && binary->binary_length == 0)
			|| (binary && binary->data == NULL))
		return g_strdup ("");

	return (gchar *) g_strndup((const gchar *) binary->data, binary->binary_length);
}

/* Modified gda_default_escape_string */
gchar *
midgard_core_query_escape_string (const gchar *string)
{
	gchar *ptr, *ret, *retptr;
	gint size;

	if (!string)
		return NULL;

	/* determination of the new string size */
	ptr = (gchar *) string;
	size = 1;
	while (*ptr) {
		if ((*ptr == '\'') ||(*ptr == '\\'))
			size += 2;
		else
			size += 1;
		ptr++;
	}

	ptr = (gchar *) string;
	ret = g_new0 (gchar, size);
	retptr = ret;
	
	while (*ptr) {

		if (*ptr == '\'') {
			*retptr = '\'';
			*(retptr+1) = *ptr;
			retptr += 2;
		}

		else if (*ptr == '\\') {
			*retptr = '\\';
			*(retptr+1) = *ptr;
			retptr += 2;
		}
		else {
			*retptr = *ptr;
			retptr ++;
		}
		ptr++;
	}
	*retptr = '\0';
	
	return ret;
}

