/* 
 * Copyright (C) 2006, 2007, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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
#include "midgard_error.h"
#include "midgard_core_connection.h"
#include "midgard_timestamp.h"
#ifdef HAVE_LIBGDA_4
#include <sql-parser/gda-sql-parser.h>
#endif
#include "midgard_metadata.h"
#include "midgard_reflector_object.h"

/* Do not use _DB_DEFAULT_DATETIME.
 * Some databases (like MySQL) fails to create datetime column with datetime which included timezone. 
 */ 
#define _DB_DEFAULT_DATETIME "0001-01-01 00:00:00"

#ifdef HAVE_LIBGDA_4

#define __get_query_string(__cnc, __str) \
	__str = NULL; \
	const GList *__events = gda_connection_get_events (__cnc); \
	const GList *__list; \
	for (__list = __events; __list != NULL; __list = __list->next) { \
		GdaConnectionEvent *__event = (GdaConnectionEvent *) __list->data; \
		if (gda_connection_event_get_event_type (__event) == GDA_CONNECTION_EVENT_COMMAND) { \
			__str = gda_connection_event_get_description (__event); \
		} \
	}

#endif 

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

	if (MGD_CNC_DEBUG (mgd))
		g_debug("Model query = %s", query);

	if (lerror) {
	
		g_warning("Model query failed: %s", lerror && lerror->message ? lerror->message : "No description");
		g_error_free(lerror);
	}

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
_add_value_type_update (GString *str, const gchar *name, GValue *value, gboolean add_comma)
{
	if (add_comma) 
		g_string_append (str, ", ");

	const gchar *type = "invalid";

	switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE(value))) {
		
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

		case G_TYPE_DOUBLE:
			type = "gfloat";
			break;	

		case G_TYPE_FLOAT:
			type = "gfloat";
			break;

		case G_TYPE_BOXED:
			if (G_VALUE_TYPE (value) == GDA_TYPE_TIMESTAMP) {
				type = "timestamp";
			} else {
				g_warning ("_add_value_type_update: unhandled boxed value type (%s)", G_VALUE_TYPE_NAME (value));
			}
			break;

		case G_TYPE_OBJECT:
			/*do nothing, ignore */	
			break;

		default:
			g_warning ("_add_value_type_update: unhandled value type (%s)", G_VALUE_TYPE_NAME (value));
			break;
	}

	g_string_append_printf (str, "%s=##%s::%s", name, name, type);
}

#ifdef HAVE_LIBGDA_4

static void 
_add_value_type (GString *str, const gchar *name, GValue *value, gboolean add_comma)
{
	if (add_comma) 
		g_string_append (str, ", ");

	const gchar *type = "invalid";

	switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE(value))) {
		
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

		case G_TYPE_BOXED:
			if (G_VALUE_TYPE (value) == GDA_TYPE_TIMESTAMP) {
				type = "timestamp";
			} else {
				g_warning ("_add_value_type: unhandled boxed value type (%s)", G_VALUE_TYPE_NAME (value));
			}
			break;	

		default:
			g_warning ("_add_value_type: unhandled value type (%s)", G_VALUE_TYPE_NAME (value));
			break;
	}

	g_string_append_printf (str, "##%s::%s", name, type);
}

#else 

static void 
_add_value_type (GString *str, const gchar *name, GValue *value, gboolean add_comma)
{
	if (add_comma) 
		g_string_append (str, ", ");

	const gchar *type = "invalid";

	switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE(value))) {
		
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

		case G_TYPE_BOXED:
			if (G_VALUE_TYPE (value) == GDA_TYPE_TIMESTAMP) {
				type = "timestamp";
			} else {
				g_warning ("_add_value_type: unhandled boxed value type (%s)", G_VALUE_TYPE_NAME (value));
			}
			break;	

		default:
			g_warning ("_add_value_type: unhandled value type (%s)", G_VALUE_TYPE_NAME (value));
			break;
	}

	g_string_append_printf (str, "## /*name:'%s' type:%s*/", name, type);
}

#endif /* HAVE_LIBGDA_4 */

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
		_add_value_type (where, parameters[i].name, (GValue *)&parameters[i].value, FALSE);
	}

	g_string_append_printf (select, " %s ", where->str);
	g_string_free (where, TRUE);

#ifdef HAVE_LIBGDA_4
	
	GError *error = NULL;
	GdaDataModel *exec_res;
	GdaStatement *stmt;
	GdaSet *params;
	GdaSqlParser *parser = mgd->priv->parser;
 
	/* Create statement */
        stmt = gda_sql_parser_parse_string (parser, select->str, NULL, NULL);

	g_string_free (select, TRUE);

	/* create GdaDataSelect */
	if (!gda_statement_get_parameters (stmt, &params, &error)) {

		g_warning ("Failed to create statement parameters. %s", error && error->message ? error->message : "Unknown reason");
		return NULL;
	}

	/* Add values to query */
	for (i = 0; i < n_params; i++) {

		switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (&parameters[i].value))) {

			case G_TYPE_STRING:
				gda_set_set_holder_value (params, &error, parameters[i].name, g_value_get_string (&parameters[i].value));
				break;

			case G_TYPE_UINT:
				gda_set_set_holder_value (params, &error, parameters[i].name, g_value_get_uint (&parameters[i].value));
				break;

			case G_TYPE_INT:
				gda_set_set_holder_value (params, &error, parameters[i].name, g_value_get_int (&parameters[i].value));
				break;

			case G_TYPE_BOOLEAN:
				gda_set_set_holder_value (params, &error, parameters[i].name, g_value_get_boolean (&parameters[i].value));
				break;

			case G_TYPE_FLOAT:
				gda_set_set_holder_value (params, &error, parameters[i].name, g_value_get_float (&parameters[i].value));
				break;
		}
	}

	GType *col_types = g_new (GType, type_attr->num_properties+1);
	
	for (i = 0; i < type_attr->num_properties; i++) {
		if (type_attr->params[i]->value_type == G_TYPE_UINT)
			col_types[i] = G_TYPE_INT;	
		else
			col_types[i] = type_attr->params[i]->value_type;	
	}
	/* terminate array with 0 */
	col_types[i] = G_TYPE_NONE;

	exec_res = gda_connection_statement_execute_select_full (mgd->priv->connection, stmt, params,
			GDA_STATEMENT_MODEL_RANDOM_ACCESS, col_types, &error);

	if (MGD_CNC_DEBUG (mgd)) {
		gchar *debug_query = gda_statement_serialize (stmt);
		g_debug ("%s", debug_query);
		g_free (debug_query);
	}

	g_free (col_types);
	g_object_unref (params);
	g_object_unref (stmt);

	return exec_res;
#else 
	GdaDict *dict = gda_dict_new ();
	gda_dict_set_connection (dict, mgd->priv->connection);

	GdaQuery *query = gda_query_new_from_sql (dict, select->str, NULL);

	g_string_free (select, TRUE);

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

	if (MGD_CNC_DEBUG (mgd))
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

#endif /* HAVE_LIBGDA_4 */

}

#define _SET_HOLDER_VALUE \
	gchar *pstr; 									\
	guint puint; 									\
	gint pint;									\
	gfloat pfloat; 									\
	MidgardTimestamp *mt;								\
	gboolean pbool;									\
	gchar *pname = pspecs[i]->name; 						\
	switch (G_TYPE_FUNDAMENTAL (pspecs[i]->value_type)) { 				\
		case G_TYPE_STRING:							\
			g_object_get (object, pname, &pstr, NULL);			\
			if (pstr == NULL) pstr = g_strdup ("");				\
			gda_set_set_holder_value (params, NULL, col_name, pstr);	\
			g_free (pstr);							\
		break;									\
		case G_TYPE_UINT:							\
			g_object_get (object, pname, &puint, NULL);			\
			gda_set_set_holder_value (params, NULL, col_name, puint);	\
		break;									\
		case G_TYPE_INT:							\
			g_object_get (object, pname, &pint, NULL);			\
			gda_set_set_holder_value (params, NULL, col_name, pint);	\
		break;									\
		case G_TYPE_FLOAT:							\
			g_object_get (object, pname, &pfloat, NULL);			\
			gda_set_set_holder_value (params, NULL, col_name, pfloat);	\
		break;									\
		case G_TYPE_BOOLEAN:							\
			g_object_get (object, pname, &pbool, NULL);			\
			gda_set_set_holder_value (params, NULL, col_name, pbool);	\
		break;									\
		case G_TYPE_BOXED:							\
			if (pspecs[i]->value_type == MIDGARD_TYPE_TIMESTAMP) {		\
				g_object_get (object, pname, &mt, NULL);		\
				pstr = midgard_timestamp_get_string (mt);		\
				gda_set_set_holder_value (params, NULL, col_name, pstr);\
				midgard_timestamp_free (mt);				\
				g_free (pstr);						\
			}								\
		break;									\
		default:								\
			g_warning ("Can not add %s parameter to statement. "		\
					"Can not handle %s (%s) type", 			\
					pname, g_type_name (pspecs[i]->value_type),	\
					g_type_name (G_TYPE_FUNDAMENTAL (pspecs[i]->value_type)));\
		break;									\
	}											

static void 
__add_metadata_parameters (MidgardDBObject *dbobject, MidgardDBObjectClass *klass, GdaSet *params)
{
	MidgardMetadataClass *mklass = MGD_DBCLASS_METADATA_CLASS (klass);
	if (!mklass)
		return;

	guint i;
	guint n_prop;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (mklass), &n_prop);

	if (!pspecs)
		return;

	GObject *object = (GObject *) MGD_DBOBJECT_METADATA (dbobject);

	for (i = 0; i < n_prop; i++) {
		/* Ignore property which holds object */
		if (G_TYPE_FUNDAMENTAL (pspecs[i]->value_type) == G_TYPE_OBJECT)
			continue;
		const gchar *col_name = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (mklass), pspecs[i]->name);
		_SET_HOLDER_VALUE;
	}

	g_free (pspecs);
}

gboolean 
midgard_core_query_create_dbobject_record (MidgardDBObject *object)
{
	g_return_val_if_fail (object != NULL, FALSE);
	
	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	GdaConnection *cnc = mgd->priv->connection;
	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_GET_CLASS (object);
	if (!klass) {
		g_warning ("Can not find class pointer for %s instance", G_OBJECT_TYPE_NAME (object));
		return FALSE;
	}

	GdaStatement *insert = MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->statement_insert;
	g_return_val_if_fail (insert != NULL, FALSE);
	GdaSet *params = MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->statement_insert_params;

	guint n_props;
	guint i;	
	GParamSpec **pspecs = midgard_core_dbobject_class_list_properties (klass, &n_props);
	g_return_val_if_fail (pspecs != NULL, FALSE);

	const gchar *pk = midgard_reflector_object_get_property_primary (G_OBJECT_CLASS_NAME (klass));
	GError *error = NULL;

	for (i = 0; i < n_props; i++) {
		/* Ignore primary key and property of GObject type*/
		if ((pk && g_str_equal (pspecs[i]->name, pk)) || 
				G_TYPE_FUNDAMENTAL(pspecs[i]->value_type) == G_TYPE_OBJECT)
			continue;

		const gchar *col_name = midgard_core_class_get_property_colname (klass, pspecs[i]->name);
		_SET_HOLDER_VALUE;
	}

	__add_metadata_parameters (object, klass, params);

	if (MGD_CNC_DEBUG (mgd)) {
		gchar *debug_sql = gda_connection_statement_to_sql (cnc, insert, params, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
		g_debug ("DBObject create: %s", debug_sql);
		g_free (debug_sql);
	}

	gboolean inserted = (gda_connection_statement_execute_non_select (cnc, insert, params, NULL, &error) == -1) ? FALSE : TRUE;

	if (!inserted) {
		g_warning ("Query failed. %s", error && error->message ? error->message : "Unknown reason");
		return FALSE;
	}

	return inserted;
}

gboolean 
midgard_core_query_update_dbobject_record (MidgardDBObject *object)
{
	g_return_val_if_fail (object != NULL, FALSE);
	
	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	GdaConnection *cnc = mgd->priv->connection;
	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_GET_CLASS (object);
	if (!klass) {
		g_warning ("Can not find class pointer for %s instance", G_OBJECT_TYPE_NAME (object));
		return FALSE;
	}

	GdaStatement *update = MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->statement_update;
	g_return_val_if_fail (update != NULL, FALSE);
	GdaSet *params = MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->statement_update_params;

	guint n_props;
	guint i;	
	GParamSpec **pspecs = midgard_core_dbobject_class_list_properties (klass, &n_props);
	g_return_val_if_fail (pspecs != NULL, FALSE);

	const gchar *pk = midgard_reflector_object_get_property_primary (G_OBJECT_CLASS_NAME (klass));
	GError *error = NULL;

	for (i = 0; i < n_props; i++) {
		/* Ignore primary key and property of GObject type*/
		if ((pk && g_str_equal (pspecs[i]->name, pk)) || 
				G_TYPE_FUNDAMENTAL(pspecs[i]->value_type) == G_TYPE_OBJECT)
			continue;

		const gchar *col_name = midgard_core_class_get_property_colname (klass, pspecs[i]->name);
		_SET_HOLDER_VALUE;
	}

	__add_metadata_parameters (object, klass, params);

	if (MGD_CNC_DEBUG (mgd)) {
		gchar *debug_sql = gda_connection_statement_to_sql (cnc, update, params, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
		g_debug ("DBObject update: %s", debug_sql);
		g_free (debug_sql);
	}

	gboolean updated = (gda_connection_statement_execute_non_select (cnc, update, params, NULL, &error) == -1) ? FALSE : TRUE;

	if (!updated) {
		g_warning ("Query failed. %s", error && error->message ? error->message : "Unknown reason");
		return FALSE;
	}

	return updated;
}

#ifdef HAVE_LIBGDA_4

gboolean 
midgard_core_query_update_object_fields (MidgardDBObject *object, const gchar *field, ...)
{
	g_return_val_if_fail (object != NULL, FALSE);
	g_return_val_if_fail (field != NULL, FALSE);

	/* Create two single list and validate arguments' list */

	const gchar *name = field;
	va_list var_args;
	va_start(var_args, field);	
	GValue *value_arg;
	GSList *cols = NULL;
	GSList *values = NULL;
	
	while (name != NULL) {

		g_return_val_if_fail (name != NULL, FALSE);
		cols = g_slist_append (cols, (gpointer)name);
		
		value_arg = va_arg (var_args, GValue*);
		g_return_val_if_fail (value_arg != NULL, FALSE);

		values = g_slist_append (values, (gpointer)value_arg);
		name = va_arg (var_args, gchar*);
	}
	va_end (var_args);	

	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	const gchar *guid = MGD_OBJECT_GUID (object);
	const gchar *table = midgard_core_class_get_table (MIDGARD_DBOBJECT_GET_CLASS (object));

	/* Build SQL query string */
	GString *sql = g_string_new ("UPDATE ");
	g_string_append_printf (sql, "%s SET ", table);
	guint i = 0;
	GSList *cl, *vl;

	for (cl = cols, vl = values; cl != NULL; cl = cl->next, vl = vl->next) {

		_add_value_type_update (sql, (const gchar *) cl->data, (GValue *) vl->data, i > 0 ? TRUE : FALSE);
		i++;
	}

	g_string_append_printf (sql, " WHERE %s.guid = '%s'", table, guid);

	/* Create statement and set parameters */
	GdaConnection *cnc = mgd->priv->connection;
	GdaSqlParser *parser = mgd->priv->parser;
	GdaStatement *stmt;
	GdaSet *params;
	GdaHolder *p;
	GError *error = NULL;

	stmt = gda_sql_parser_parse_string (parser, sql->str, NULL, &error);

	if (!stmt || error) {

		g_warning ("%s. Failed to create SQL statement for given query %s", 
				error && error->message ? error->message : "Unknown reason", sql->str);
		if (stmt)
			g_object_unref (stmt);

		g_clear_error (&error);
		return FALSE;
	}

	g_string_free (sql, TRUE);
	if (!gda_statement_get_parameters (stmt, &params, &error)) {
	
		g_warning ("Failed to get query parameters. %s", error && error->message ? error->message : "Unknown reason");
		g_object_unref (stmt);
		if (error) g_clear_error (&error);
		return FALSE;
	}

	for (cl = cols, vl = values; cl != NULL; cl = cl->next, vl = vl->next) {
	
		p = gda_set_get_holder (params, (gchar *) cl->data);
	
		if (!p)
			g_warning ("Failed to get holder for %s column", (gchar *) cl->data);
	
		if (!gda_holder_set_value (p, (GValue *) vl->data, &error)) {
			g_warning ("Failed to set holder's value. %s", 
					error && error->message ? error->message : "Unknown reason");
			g_object_unref (stmt);
			if (error) g_clear_error (&error);
			return FALSE;
		}
	}

	gchar *debug_sql = gda_connection_statement_to_sql (cnc, stmt, params, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
	g_free (debug_sql);

	gint retval = gda_connection_statement_execute_non_select (cnc, stmt, params, NULL, &error);

	if (error) {
		g_warning ("Failed to execute statement. %s", error && error->message ? error->message : "Unknown reason");
		g_clear_error (&error);
	}
	
	g_object_unref (params);
	g_object_unref (stmt);

	if (retval == -1) {

		/* FIXME, provider error, handle this */
		return FALSE;
	}

	if (retval >= 0)
		return TRUE;

	return FALSE;
}

#else 

gboolean 
midgard_core_query_update_object_fields (MidgardDBObject *object, const gchar *field, ...)
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
#endif /* HAVE_LIBGDA_4 */

#ifdef HAVE_LIBGDA_4

gint 
midgard_core_query_insert_records (MidgardConnection *mgd, 
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
	/* Build SQL query string */
	GString *sql = g_string_new ("UPDATE ");
	g_string_append_printf (sql, "%s SET ", table);
	guint i = 0;
	GList *cl, *vl;

	for (cl = cols, vl = values; cl != NULL; cl = cl->next, vl = vl->next) {
	
		_add_value_type_update (sql, (const gchar *) cl->data, (GValue *) vl->data, i > 0 ? TRUE : FALSE);
		i++;
	}
	
	g_string_append_printf (sql, " WHERE %s", where);

	/* Create statement and set parameters */
	GdaConnection *cnc = mgd->priv->connection;
	GdaSqlParser *parser = mgd->priv->parser;
	GdaStatement *stmt;
	GdaSet *params;
	GdaHolder *p;
	GError *error = NULL;

	stmt = gda_sql_parser_parse_string (parser, sql->str, NULL, &error);

	if (!stmt || error) {

		g_warning ("%s. Failed to create SQL statement for given query %s", 
				error && error->message ? error->message : "Unknown reason", sql->str);
		if (stmt)
			g_object_unref (stmt);

		g_clear_error (&error);
		return FALSE;
	}

	g_string_free (sql, TRUE);
	if (!gda_statement_get_parameters (stmt, &params, &error)) {
	
		g_warning ("Failed to get query parameters. %s", error && error->message ? error->message : "Unknown reason");
		g_object_unref (stmt);
		if (error) g_clear_error (&error);
		return FALSE;
	}

	for (cl = cols, vl = values; cl != NULL; cl = cl->next, vl = vl->next) {
	
		p = gda_set_get_holder (params, (gchar *) cl->data);
	
		if (!p)
			g_warning ("Failed to get holder for %s column", (gchar *) cl->data);

		if (G_VALUE_HOLDS_STRING ((GValue *) vl->data) && !g_value_get_string ((GValue *) vl->data))
			g_value_set_string ((GValue *) vl->data, "");

		if (!gda_holder_set_value (p, (GValue *) vl->data, &error)) {
			g_warning ("Failed to set holder's value. %s", 
					error && error->message ? error->message : "Unknown reason");
			g_object_unref (stmt);
			if (error) g_clear_error (&error);
			return FALSE;
		}
	}

	if (MGD_CNC_DEBUG (mgd)) {
		gchar *debug_sql = gda_connection_statement_to_sql (cnc, stmt, params, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
		g_debug ("%s", debug_sql);
		g_free (debug_sql);
	}

	gint retval = gda_connection_statement_execute_non_select (cnc, stmt, params, NULL, &error);

	if (error) {
		g_warning ("Failed to execute statement. %s", error && error->message ? error->message : "Unknown reason");
		g_clear_error (&error);
	}
	
	g_object_unref (params);
	g_object_unref (stmt);

	if (retval == -1) {

		/* FIXME, provider error, handle this */
		return FALSE;
	}

	if (retval >= 0)
		return TRUE;

	return FALSE;
}

#else 

gint 
midgard_core_query_insert_records (MidgardConnection *mgd, 
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

	/* Initialize GdaQuery */
	GdaDict *dict = gda_dict_new();
	gda_dict_set_connection(dict , mgd->priv->connection);
	
	GdaQuery *query = gda_query_new(dict);
	gda_query_set_query_type(query, query_type);
	
	GdaQueryTarget *target = gda_query_target_new(query, table);
	gda_query_add_target(query, target, NULL);
	
	while(cols != NULL) {

		GdaQueryField *field = 
			gda_query_field_field_new(query, 
					(const gchar *) cols->data);
		gda_entity_add_field (GDA_ENTITY(query), GDA_ENTITY_FIELD(field));
		
		GValue *val = (GValue *) values->data;
		GdaQueryField *value;

		if(G_VALUE_TYPE(val) == G_TYPE_BOOLEAN){
			
			GValue nval = {0, };
			g_value_init(&nval, G_TYPE_UINT);
			g_value_set_uint(&nval, 0);
			if(g_value_get_boolean(val))
				g_value_set_uint(&nval, 1);
			
			value = gda_query_field_value_new(query, G_TYPE_UINT);
			gda_query_field_value_set_value(
					GDA_QUERY_FIELD_VALUE(value), &nval);

		} else if(G_VALUE_TYPE(val) == G_TYPE_FLOAT) {

			/* We use dot as decimal separator. Always. */
			GValue fval = {0, };
			g_value_init(&fval, G_TYPE_FLOAT);
			gchar *lstring = setlocale(LC_NUMERIC, "0");
			setlocale(LC_NUMERIC, "C");
			g_value_copy(val, &fval);
			
			value = gda_query_field_value_new(query, G_TYPE_FLOAT);
			gda_query_field_value_set_value(
					GDA_QUERY_FIELD_VALUE(value), &fval);

			g_value_unset(&fval);
			setlocale(LC_ALL, lstring);
	
		} else if (G_VALUE_TYPE(val) == MGD_TYPE_TIMESTAMP) {
			
			GValue tval = {0, };

			/* GDA3 & SQLite workaround */
			if (mgd->priv->config->priv->dbtype == MIDGARD_DB_TYPE_SQLITE)
				g_value_init(&tval, G_TYPE_STRING);
			else
				g_value_init(&tval, GDA_TYPE_TIMESTAMP);
			
			g_value_transform(val, &tval);
		
			/* GDA3 & SQLite workaround */
			if (mgd->priv->config->priv->dbtype == MIDGARD_DB_TYPE_SQLITE)
				value = gda_query_field_value_new (query, G_TYPE_STRING);
			else 
				value = gda_query_field_value_new (query, GDA_TYPE_TIMESTAMP);

			gda_query_field_value_set_value (GDA_QUERY_FIELD_VALUE (value), &tval);

			g_value_unset(&tval);

		} else if (G_VALUE_TYPE(val) == G_TYPE_STRING
				|| G_VALUE_TYPE(val) == G_TYPE_UINT
				|| G_VALUE_TYPE(val) == G_TYPE_INT) { 
	
			value = gda_query_field_value_new(query, G_VALUE_TYPE(val));
			
			if (!value)
				g_warning("Can not create new field value for given %s value type", G_VALUE_TYPE_NAME(val));
			else
				gda_query_field_value_set_value(GDA_QUERY_FIELD_VALUE(value), val);

		} else {

			value = NULL;
			/*
			g_warning("Unsupported %s value type", G_VALUE_TYPE_NAME(val));
			value = gda_query_field_value_new(query, G_VALUE_TYPE(val));
			if (!value)
				g_warning("Can not create new field value for given %s value type", G_VALUE_TYPE_NAME(val));
			else
				gda_query_field_value_set_value(GDA_QUERY_FIELD_VALUE(value), val);
				*/
		}

		if (value != NULL) {
			
			gda_entity_add_field(GDA_ENTITY(query), GDA_ENTITY_FIELD(value));
			g_object_set (field, "value-provider", value, NULL);
			gda_query_field_set_visible (GDA_QUERY_FIELD (value), FALSE);
			
			g_object_unref(field);
			g_object_unref(value);
		}

		cols = cols->next;
		values = values->next;
	}

	gchar *full_query = NULL;

	if(where != NULL) {		
		
		const gchar *sql = gda_query_get_sql_text (query);
		full_query = g_strconcat (sql, " WHERE ", where, NULL);
		gda_query_set_sql_text (query, full_query, NULL);
	}

	if (full_query) {

		g_debug ("%s", full_query);
		g_free (full_query);
	}

	GError *error = NULL;
	gda_query_execute(query, NULL, FALSE, &error);

	g_object_unref(target);
	g_object_unref(dict);
	g_object_unref(query); 
	
	if(error){
		g_clear_error(&error);
		return -1;
	}
	
	return 0;
}

#endif /* HAVE_LIBGDA_4 */

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
	g_free (mcontext.column_names);
	g_free (mcontext.column_values);

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
	if (primary) {
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

#ifdef HAVE_LIBGDA_4

	/* Update meta store */
	GdaMetaStruct *mstruct;
	GdaMetaDbObject *dbo;
	GValue *table_value;
	GdaMetaStore *store = gda_connection_get_meta_store (cnc);

	if (!gda_connection_update_meta_store (cnc, NULL, &error)) {
		    /* FIXME, error message */
		    return FALSE;
	}

	mstruct = gda_meta_struct_new (store, GDA_META_STRUCT_FEATURE_NONE);
	table_value = gda_value_new (G_TYPE_STRING);
	g_value_set_string (table_value, tablename);
	dbo = gda_meta_struct_complement (mstruct, GDA_META_DB_TABLE, NULL, NULL, table_value, &error);
	gda_value_free (table_value);
	g_object_unref (mstruct);

	if (!dbo) {
		g_warning ("Failed to update %s table meta store.", error && error->message ? error->message : "Unknown reason");
		return FALSE;
	}

#endif /* HAVE_LIBGDA_4 */

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
	/*if (!gda_connection_update_meta_store (mgd->priv->connection, &mcontext, &error)) {
		gda_value_free (mcontext.column_values[0]);
		//FIXME, there should be warning, but for some reason 
		//either SQLite or gda-sqlite provider is buggy 
		g_message ("Failed to update meta data for table '%s': %s", mdc->table_name, 
				error && error->message ? error->message : "No detail");
		if (error)
			g_error_free(error);

		return FALSE;
	}*/

	GdaDataModel *dm_schema =
		gda_connection_get_meta_store_data (mgd->priv->connection,
						    GDA_CONNECTION_META_FIELDS, NULL, 2,
						    "name", mcontext.column_values[0],
						    "field_name", mcontext.column_values[1]);
	gda_value_free (mcontext.column_values[0]);
	g_free (mcontext.column_names);
	g_free (mcontext.column_values);

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

	g_debug("Check if column '%s.%s' exists", mdc->table_name, mdc->column_name);
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
	if(mgd->priv->config->priv->dbtype != MIDGARD_DB_TYPE_POSTGRES)
		gda_server_operation_set_value_at(op, "true", NULL, "/COLUMN_DEF_P/COLUMN_NNUL");

	gda_server_operation_set_value_at(op, column_name, NULL, "/COLUMN_DEF_P/COLUMN_NAME");
	gda_server_operation_set_value_at(op, column_type_str, NULL, "/COLUMN_DEF_P/COLUMN_TYPE");
	
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
	
		/* FIXME, there should be warning, but for some reason 
		 * either SQLite or gda-sqlite provider is buggy */	
		g_message ("Can not add %s column to table '%s'. %s",
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

	GdaConnection *cnc = mgd->priv->connection;
	GdaMetaStore *store = gda_connection_get_meta_store (cnc);
	const gchar *sql = "SELECT index_name FROM _table_indexes WHERE table_schema=##schema::string AND table_name=##tname::string";
	gchar *dbname = mgd->priv->config->database;
	gboolean index_exists = FALSE;

	/* Database name - table_schema */
	GValue tsval = {0, };
	g_value_init (&tsval, G_TYPE_STRING);
	g_value_set_string (&tsval, dbname);

	/* Table name - table_name */
	GValue tnval = {0, };
	g_value_init (&tnval, G_TYPE_STRING);
	g_value_set_string (&tnval, mdc->table_name);

	GdaDataModel *data_model = gda_meta_store_extract (store, sql, NULL, "schema", &tsval, "tname", &tnval, NULL);

	g_value_unset (&tsval);
	g_value_unset (&tnval);

	if (!data_model) {

		g_debug ("No indexes found in meta store for table %s", mdc->table_name);
		return index_exists;
	}

	gint rows = gda_data_model_get_n_rows (data_model);
	gint i = 0;

	if (rows == -1) {

		g_warning ("Unknown number of rows for given indexes data model");
		g_object_unref (data_model);
		return index_exists;
	}

	for (i = 0; i < rows; i++) {

		const GValue *inval = gda_data_model_get_value_at (data_model, 0, i, NULL);	
		if (g_str_equal (index_name, g_value_get_string (inval)))
			index_exists = TRUE;
	}

	g_object_unref (data_model);

	return index_exists;

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

static gboolean
__create_repligard_table (MidgardConnection *mgd)
{
	/* Create table with primary key */
	const gchar *table = "repligard";
	if (!midgard_core_query_create_table (mgd, table, table, "id"))
		return FALSE;
	
	/* Create columns */
	MidgardDBColumn *mdc = NULL;

	/* GUID */
	mdc = midgard_core_dbcolumn_new ();
	mdc->table_name = table;
	mdc->column_name = "guid";
	mdc->index = TRUE;
	mdc->dbtype = "varchar(80)";
	mdc->gtype = MGD_TYPE_STRING;
	mdc->unique = FALSE;

	gboolean rv = midgard_core_query_add_column (mgd, mdc);
	g_free (mdc);
	if (!rv) 
		return FALSE;

	/* TYPENAME */	
	mdc = midgard_core_dbcolumn_new ();
	mdc->table_name = table;
	mdc->column_name = "typename";
	mdc->index = FALSE;
	mdc->dbtype = "varchar(255)";
	mdc->gtype = MGD_TYPE_STRING;
	mdc->unique = FALSE;

	rv = midgard_core_query_add_column (mgd, mdc);
	g_free (mdc);
	if (!rv) 
		return FALSE;

	/* OBJECT ACTION */
	mdc = midgard_core_dbcolumn_new ();
	mdc->table_name = table;
	mdc->column_name = "object_action";
	mdc->index = FALSE;
	mdc->dbtype = "int";
	mdc->gtype = MGD_TYPE_INT;
	mdc->unique = FALSE;
	mdc->dvalue = "0";

	rv = midgard_core_query_add_column (mgd, mdc);
	g_free (mdc);
	if (!rv) 
		return FALSE;

	/* OBJECT ACTION DATE */
	mdc = midgard_core_dbcolumn_new ();
	mdc->table_name = table;
	mdc->column_name = "object_action_date";
	mdc->index = FALSE;
	mdc->dbtype = "datetime";
	mdc->gtype = MGD_TYPE_TIMESTAMP;
	mdc->unique = FALSE;
	mdc->dvalue = "0001-01-01 00:00:00";

	rv = midgard_core_query_add_column (mgd, mdc);
	g_free (mdc);
	if (!rv) 
		return FALSE;

	return TRUE;
}

gboolean 
midgard_core_query_create_basic_db (MidgardConnection *mgd)
{
	g_return_val_if_fail(mgd != NULL, FALSE);

	return __create_repligard_table (mgd);	
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
	const gchar *classname = G_OBJECT_CLASS_NAME (klass);
	if (MIDGARD_IS_OBJECT_CLASS (klass)) {
		parent = midgard_reflector_object_get_property_parent (classname);
		up = midgard_reflector_object_get_property_up (classname);	
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

		/* Ignore properties of object type */
		if (G_TYPE_FUNDAMENTAL (pspecs[i]->value_type) == G_TYPE_OBJECT)
			continue;

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
	
		if (!midgard_core_query_create_metadata_columns(mgd, tablename)) {
	
			/* FIXME, there should be warning, but for some reason 
			 * either SQLite or gda-sqlite provider is buggy */	
			g_message ("Failed to create metadata columns"); 
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
		pprop = midgard_reflector_object_get_property_primary (classname);
	
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

		/* Override dbtype if explicitly defined */
		MgdSchemaPropertyAttr *prop_attr = midgard_core_class_get_property_attr (klass, pspecs[i]->name);
		if (prop_attr && prop_attr->dbtype)
			mdc->dbtype = prop_attr->dbtype;

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
	mdbj->left_table = NULL;
	mdbj->left_field = NULL;
	mdbj->left_klass = NULL;
	mdbj->right_table = NULL;
	mdbj->right_field = NULL;
	mdbj->right_klass = NULL;
	mdbj->left_tablefield = NULL;
	mdbj->right_tablefield = NULL;

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

	g_free (mdbj->left_table);
	mdbj->left_table = NULL;
	g_free (mdbj->left_field);
	mdbj->left_field = NULL;
	g_free (mdbj->right_table);
	mdbj->right_table = NULL;
	g_free (mdbj->right_field);
	mdbj->right_field = NULL;
	g_free (mdbj->left_tablefield);
	mdbj->left_tablefield = NULL;
	g_free (mdbj->right_tablefield);
	mdbj->right_tablefield = NULL;

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

	const GdaBinary *binary;
	if (G_VALUE_TYPE (src_value) == GDA_TYPE_BLOB) {
		const GdaBlob *blob = gda_value_get_blob (src_value);
		binary = &blob->data;
	}

	if (G_VALUE_TYPE (src_value) == GDA_TYPE_BINARY) 
		binary = gda_value_get_binary ((const GValue*) src_value);

	if (binary == NULL || (binary && binary->binary_length == 0)
			|| (binary && binary->data == NULL))
		return g_strdup ("");

	return (gchar *) g_strndup((const gchar *) binary->data, binary->binary_length);
}

#define _RESERVED_BLOB_NAME "attachment"
#define _RESERVED_BLOB_TABLE "blobs"
#define _RESERVED_PARAM_NAME "parameter"
#define _RESERVED_PARAM_TABLE "record_extension"
#define _RESERVED_METADATA_NAME "metadata"
#define _RESERVED_GUID_NAME "guid"

typedef struct {
	const MidgardDBObjectClass *klass;
	const gchar *table;
	const gchar *table_alias;
	const gchar *target_table;
	const gchar *target_table_alias;
	const gchar *colname;
	const gchar *target_colname;
	MidgardQueryExecutor *executor;
} Psh;

static void 
__add_join (Psh *holder) 
{
	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (holder->executor);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;
	GdaSqlSelectFrom *from = select->from;
	GdaSqlSelectJoin *join = gda_sql_select_join_new (GDA_SQL_ANY_PART (from));
	join->type = GDA_SQL_SELECT_JOIN_LEFT;

	GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (join));
        expr->value = gda_value_new (G_TYPE_STRING);
	g_value_take_string (expr->value, g_strdup_printf ("%s.%s = %s.%s", 
				holder->table_alias, holder->colname, holder->target_table_alias, holder->target_colname));
	join->expr = expr;
	join->position = ++executor->priv->joinid;

	gda_sql_select_from_take_new_join (from , join);

	GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (from));
	s_target->table_name = g_strdup (holder->target_table);
	s_target->as = g_strdup (holder->target_table_alias);
	gda_sql_select_from_take_new_target (from, s_target);

	/* Set target expression */     
	GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
	GValue *tval = g_new0 (GValue, 1);
	g_value_init (tval, G_TYPE_STRING);
	g_value_set_string (tval, s_target->table_name);
	texpr->value = tval;
	s_target->expr = texpr;
}

void 
__exclude_deleted_constraint (MidgardQueryExecutor *executor, const gchar *table_alias)
{
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;
	GdaSqlExpr *where = select->where_cond;
	GdaSqlOperation *operation = where->cond;

	GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (operation));
	expr->value = gda_value_new (G_TYPE_STRING);
	g_value_set_string (expr->value, "t1.metadata_deleted = 0");
	operation->operands = g_slist_append (operation->operands, expr);
}

gboolean 
__compute_reserved_property_constraint (Psh *holder, const gchar *token_1, const gchar *token_2)
{
	/* metadata */
	if (g_str_equal (_RESERVED_METADATA_NAME, token_1)) {
		MidgardMetadataClass *mklass = g_type_class_peek (MIDGARD_TYPE_METADATA);
		const gchar *property_field = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (mklass), token_2);
		if (!property_field)
			return FALSE;
		holder->colname = property_field;
		return TRUE;
	}

	/* parameter */
	if (g_str_equal (_RESERVED_PARAM_NAME, token_1)) {
		holder->klass = g_type_class_peek (g_type_from_name ("midgard_parameter"));
		holder->target_colname = "parent_guid";
		holder->target_table = _RESERVED_PARAM_TABLE;
		holder->target_table_alias = g_strdup_printf ("t%d", ++holder->executor->priv->tableid);
		holder->colname = _RESERVED_GUID_NAME;
		__add_join (holder);
		holder->colname = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (holder->klass), token_2);
		holder->table = holder->target_table;
		holder->table_alias = holder->target_table_alias;
		return TRUE;
	}

	/* attachment */
	if (g_str_equal (_RESERVED_BLOB_NAME, token_1)) {
		holder->klass = g_type_class_peek (g_type_from_name ("midgard_attachment"));
		holder->target_colname = "parent_guid";
		holder->target_table = _RESERVED_BLOB_TABLE;
		holder->target_table_alias = g_strdup_printf ("t%d", ++holder->executor->priv->tableid);
		holder->colname = _RESERVED_GUID_NAME;
		__add_join (holder);
		holder->colname = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (holder->klass), token_2);
		holder->table = holder->target_table;
		holder->table_alias = holder->target_table_alias;
		return TRUE;
	}

	/* fallback to default */
	MidgardReflectionProperty *mrp = midgard_reflection_property_new (MIDGARD_DBOBJECT_CLASS (holder->klass));
	holder->colname = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (holder->klass), token_1);
	/* Add implicit join if property is a link */
	if (midgard_reflection_property_is_link(mrp, token_1)) {
		holder->klass = midgard_reflection_property_get_link_class (mrp, token_1);
		const gchar *target_property = midgard_reflection_property_get_link_target (mrp, token_1);
		holder->target_colname = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (holder->klass), target_property);
		holder->target_table = midgard_core_class_get_table (MIDGARD_DBOBJECT_CLASS (holder->klass));
		holder->target_table_alias = g_strdup_printf ("t%d", ++holder->executor->priv->tableid);
		__add_join (holder);
		holder->table = holder->target_table;
		holder->table_alias = holder->target_table_alias;
	}

	return TRUE;
}

gchar *
midgard_core_query_compute_constraint_property (MidgardQueryExecutor *executor,
		                MidgardQueryStorage *storage, const gchar *name)
{
	g_return_val_if_fail (executor != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	GdaConnection *cnc = executor->priv->mgd->priv->connection;

	/* Set table alias if it's not set */
	if (storage)
		MQE_SET_TABLE_ALIAS (executor, storage);

	gchar *table_field = NULL;
	gchar *table_alias = executor->priv->table_alias;
	const gchar *table = executor->priv->storage->priv->table;
       	MidgardDBObjectClass *klass = executor->priv->storage->priv->klass; 
	if (storage) {
		table = executor->priv->storage->priv->table;
		table_alias = storage->priv->table_alias;
		klass = storage->priv->klass;	
	}

      	gchar **spltd = g_strsplit(name, ".", 0);
	guint i = 0;
	guint j = 0;

	/* We can support max 3 tokens */
	while(spltd[i] != NULL)
		i++;

	gchar *q_table = NULL;
	gchar *q_field = NULL;

	/* case: property */
	if (i == 1) {
		const gchar *property_field = midgard_core_class_get_property_colname (klass, name);
		if (!property_field) {
			g_warning ("%s doesn't seem to be registered for %s", name, G_OBJECT_CLASS_NAME (klass));
			g_strfreev (spltd);
			return NULL;
		}
		q_table = gda_connection_quote_sql_identifier (cnc, table_alias);
		q_field = gda_connection_quote_sql_identifier (cnc, property_field);
		table_field = g_strdup_printf ("%s.%s", q_table, q_field);
		g_free (q_table);
		g_free (q_field);
	} else if (i < 4) {
		/* Set all pointers we need to generate valid tables' names, aliases or joins */
		Psh holder = {NULL, };
		holder.table = table;
		holder.table_alias = table_alias;
		holder.klass = klass;
		holder.executor = MIDGARD_QUERY_EXECUTOR (executor);
		holder.colname = NULL;
		
		while (spltd[j] != NULL) {
			if (spltd[j+1] == NULL)
				break;
			/* Set all pointers we need to generate valid tables' names, aliases or joins */
			/* case: metadata.property, attachment.property, property.link, etc */	
			if (!__compute_reserved_property_constraint (&holder, spltd[j], spltd[j+1]))
				break;
			j++;
		}	
		
		if (holder.table_alias && holder.colname) {
			q_table = gda_connection_quote_sql_identifier (cnc, holder.table_alias);
			q_field = gda_connection_quote_sql_identifier (cnc, holder.colname);
			table_field = g_strdup_printf ("%s.%s", q_table, q_field);
			g_free (q_table);
			g_free (q_field);
		}

	} else {
		  g_warning("Failed to parse '%s'. At most 3 tokens allowed", name);
	}

	g_strfreev (spltd);

	return table_field;
}

gchar*
midgard_core_query_unescape_string (MidgardConnection *mgd, const gchar *string)
{
	glong total;
	gchar *ptr;
	gchar *retval;
	glong offset = 0;
	
	if (!string) 
		return NULL;

	total = strlen (string);
	retval = g_memdup (string, total+1);
	ptr = (gchar *) retval;
	while (offset < total) {

		/* we accept the "''" as a synonym of "\'" */
		if (*ptr == '\'') {
			if (*(ptr+1) == '\'') {
				g_memmove (ptr+1, ptr+2, total - offset);
				offset += 2;
			}
		}
		if (*ptr == '\\') {
			if (*(ptr+1) == '\\') {
				g_memmove (ptr+1, ptr+2, total - offset);
				offset += 2;
			}
			else {
				if (*(ptr+1) == '\'') {
					*ptr = '\'';
					g_memmove (ptr+1, ptr+2, total - offset);
					offset += 2;
				}				
			}
		}
		else
			offset ++;

		ptr++;
	}

	return retval;		
}

