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
 */

#include "midgard_dbobject.h"
#include "schema.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include <sql-parser/gda-sql-parser.h>
#include "midgard_core_workspace.h"

static GObjectClass *parent_class= NULL;

/* Create GdaSqlSelectField for every property registered for the class. */
void
_add_fields_to_select_statement (MidgardDBObjectClass *klass, MidgardConnection *mgd, GdaSqlStatementSelect *select, const gchar *table_name)
{
	guint n_prop;
	guint i;
	GdaSqlSelectField *select_field;
	GdaSqlExpr *expr;
	GValue *val;
	gchar *table_field;
	GdaConnection *cnc = mgd->priv->connection;

	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (klass), &n_prop);
	if (!pspecs)
		return;

	const gchar *property_table = NULL;

	for (i = 0; i < n_prop; i++) {

		const gchar *property = pspecs[i]->name;
		const gchar *property_field = midgard_core_class_get_property_colname (klass, property);
		property_table = midgard_core_class_get_property_table (klass, property); 
		if (property_table && table_name)
			property_table = table_name;

		/* Ignore properties with NULL storage and those of object type */
		if (!property_table || pspecs[i]->value_type == G_TYPE_OBJECT)
			continue;

       		select_field = gda_sql_select_field_new (GDA_SQL_ANY_PART (select));
		/*select_field->field_name = g_strdup (property_field);
		select_field->table_name = g_strdup (property_table);*/
		select_field->as = gda_connection_quote_sql_identifier (cnc, property);
		select->expr_list = g_slist_append (select->expr_list, select_field);
		expr = gda_sql_expr_new (GDA_SQL_ANY_PART (select_field));
		val = g_new0 (GValue, 1);
		g_value_init (val, G_TYPE_STRING);
		gchar *q_table = gda_connection_quote_sql_identifier (cnc, property_table);
		gchar *q_field = gda_connection_quote_sql_identifier (cnc, property_field);
		table_field = g_strconcat (q_table, ".", q_field, NULL);
		g_value_set_string (val, table_field);
		g_free (q_table);
		g_free (q_field);
		g_free (table_field);
		expr->value = val;
		select_field->expr = expr;
	}	

	g_free (pspecs);

	if (!klass->dbpriv->has_metadata)
		return;

	/* Check if metadata provides own method to add fields. If not, use given class storage. */
	if (MIDGARD_IS_OBJECT_CLASS (klass)) {

		MidgardMetadataClass *mklass = MGD_DBCLASS_METADATA_CLASS (klass);
		if (!mklass)
			return;

		if (MIDGARD_DBOBJECT_CLASS (mklass)->dbpriv->add_fields_to_select_statement) {
			MIDGARD_DBOBJECT_CLASS (mklass)->dbpriv->add_fields_to_select_statement (MIDGARD_DBOBJECT_CLASS (mklass), mgd, select, table_name);
			return;
		}

		const gchar *table = midgard_core_class_get_table (klass);
		if (table_name)
			table = table_name;

		/* TODO, Once we stabilize use case, refactor this below to minimize code usage */
		GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (mklass), &n_prop);
		if (!pspecs)
			return;
		
		for (i = 0; i < n_prop; i++) {

			const gchar *property = pspecs[i]->name;
			const gchar *property_field = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (mklass), property);	

			if (pspecs[i]->value_type == G_TYPE_OBJECT || !property_field)
				continue;
			
			select_field = gda_sql_select_field_new (GDA_SQL_ANY_PART (select));
			/*select_field->field_name = g_strdup (property_field);
			select_field->table_name = g_strdup (table);*/
			select_field->as = g_strconcat ("metadata_", property, NULL);
			select->expr_list = g_slist_append (select->expr_list, select_field);
			expr = gda_sql_expr_new (GDA_SQL_ANY_PART (select_field));
			val = g_new0 (GValue, 1);
			g_value_init (val, G_TYPE_STRING);
			table_field = g_strconcat (table, ".", property_field, NULL);
			g_value_set_string (val, table_field);
			g_free (table_field);
			expr->value = val;
			select_field->expr = expr;
		}

		g_free (pspecs);
	}

	return;
}

gboolean
_midgard_dbobject_get_property (MidgardDBObject *self, const gchar *name, GValue *value)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);
	g_return_val_if_fail (G_IS_VALUE (value), FALSE);

	if (!self->dbpriv->datamodel)
		return FALSE;

	GdaDataModel *model = GDA_DATA_MODEL (self->dbpriv->datamodel);
	if (!model || (model && !GDA_IS_DATA_MODEL (model)))
		return FALSE;

	gint col_idx = gda_data_model_get_column_index (model, name);
	if (col_idx == -1)
		return FALSE;

	const GValue *src_val = gda_data_model_get_value_at (model, col_idx, self->dbpriv->row, NULL);
	if (!src_val)
		return FALSE;

	if (!G_IS_VALUE (src_val) 
			|| (G_IS_VALUE (src_val) 
				&& G_VALUE_TYPE (src_val) == GDA_TYPE_NULL)) {
		/* NULL fetched from underlying field */
		if (G_VALUE_HOLDS_STRING (value)) {
			g_value_set_string (value, "");
			return TRUE;
		}
		g_warning ("Can not find value for given '%s' property \n", name);
		return FALSE;
	}

	if (G_VALUE_TYPE (src_val) != G_VALUE_TYPE (value))
		g_value_transform (src_val, value);
	else 
		g_value_copy (src_val, value);

	if (G_VALUE_HOLDS_STRING (value) && g_value_get_string (value) == NULL)
		g_value_set_string (value, "");

	return TRUE;
}

gboolean
_midgard_dbobject_set_property (MidgardDBObject *self, const gchar *name, GValue *value)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);
	g_return_val_if_fail (G_IS_VALUE (value), FALSE);
	
	GdaDataModel *model = GDA_DATA_MODEL (self->dbpriv->datamodel);
	if (!model)
		return FALSE;

	gint col_idx = gda_data_model_get_column_index (model, name);
	if (col_idx == -1)
		return FALSE;

	GError *error = NULL;
	gboolean rv = gda_data_model_set_value_at (model, col_idx, self->dbpriv->row, (const GValue *) value, &error);


	return rv;
}

void
_midgard_dbobject_set_from_data_model (MidgardDBObject *self, GdaDataModel *model, gint row, guint column_id)
{
	g_return_if_fail (self != NULL);	
	g_return_if_fail (model != NULL);
	g_return_if_fail (row > -1);

	GError *error = NULL;

	/* Set user defined properties */
	guint n_props;
	guint i;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_GET_CLASS (self), &n_props);
	if (!pspecs)
		return;

	const GValue *pval;
	for (i = 1; i < n_props; i++) {
		const gchar *pname = pspecs[i]->name;

		if (!(pspecs[i]->flags & G_PARAM_WRITABLE)) {
			g_debug ("Ignoring read only property %s \n", pname);
			continue;
		}

		gint col_idx = gda_data_model_get_column_index (model, pname);
		if (col_idx == -1)
			continue;
		
		pval = gda_data_model_get_value_at (model, col_idx, row, &error);
		if (!pval) {
			g_warning ("Failed to get '%s' property value: %s", pname, 
					error && error->message ? error->message : "Unknown reason");
			continue;
		}
		/* Overwrite NULL values */
		if (G_VALUE_TYPE (pval) == GDA_TYPE_NULL && pspecs[i]->value_type == G_TYPE_STRING)
			g_object_set (G_OBJECT (self), pname, "", NULL);
		else
			g_object_set_property (G_OBJECT (self), pname, pval);

		column_id++;
	}

	g_free (pspecs);

	/* Set metadata */
	MidgardDBObject *dbobject = MIDGARD_DBOBJECT (self);
	MidgardMetadata *metadata = MGD_DBOBJECT_METADATA (dbobject);
	if (metadata)
		MIDGARD_DBOBJECT_GET_CLASS (MIDGARD_DBOBJECT (metadata))->dbpriv->set_from_data_model (
				MIDGARD_DBOBJECT (metadata), model, row, column_id);

	return;
}

static void
__statement_insert_add_metadata_fields (MidgardDBObjectClass *klass, GString *colnames, GString *values)
{
	MidgardMetadataClass *mklass = MGD_DBCLASS_METADATA_CLASS (klass);
	if (!mklass)
		return;

	guint i;
	guint n_prop;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (mklass), &n_prop);

	if (!pspecs)
		return;

	for (i = 0; i < n_prop; i++) {
		const gchar *col_name = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (mklass), pspecs[i]->name);
		if (!col_name)
			continue;
		g_string_append (colnames, ", ");
		g_string_append (values, ", ");
		g_string_append (colnames, col_name);
		const gchar *type_name = g_type_name (pspecs[i]->value_type);
		if (pspecs[i]->value_type == MIDGARD_TYPE_TIMESTAMP)
			type_name = "string";
		g_string_append_printf (values, "##%s::%s", col_name, type_name);
	}

	g_free (pspecs);
}

static void
__initialize_statement_insert_query_parameters (MidgardDBObjectClass *klass, const gchar *query_string, gboolean add_workspace)
{
	GdaSqlParser *parser = gda_sql_parser_new ();
	GdaStatement *stmt;
	GError *error = NULL;
	stmt = gda_sql_parser_parse_string (parser, query_string, NULL, &error);

	if (!stmt) {

		g_error ("Couldn't create %s class prepared statement. %s", 
				G_OBJECT_CLASS_NAME (klass), error && error->message ? error->message : "Unknown reason");
		return;
	}

	GdaSet *params; 
	if (!gda_statement_get_parameters (stmt, &params, &error)) {
		g_error ("Failed to create GdaSet for %s class. %s", 
				G_OBJECT_CLASS_NAME (klass), error && error->message ? error->message : "Unknown reason");
	}
	
	if (add_workspace) {
		klass->dbpriv->_workspace_statement_insert = stmt;
		klass->dbpriv->_workspace_statement_insert_params = params;
		return;
	}
	
	klass->dbpriv->_statement_insert = stmt;
	klass->dbpriv->_statement_insert_params = params;

	return;
}

static gchar *
__initialize_statement_insert_query_string (MidgardDBObjectClass *klass, gboolean add_workspace)
{
	GString *sql = g_string_new ("INSERT INTO ");
	guint n_props;
	guint i;
	const gchar *table = MGD_DBCLASS_TABLENAME (klass);
	g_return_val_if_fail (table != NULL, NULL);

	g_string_append (sql, table); 

	GParamSpec **pspecs = midgard_core_dbobject_class_list_properties (klass, &n_props);
	g_return_val_if_fail (pspecs != NULL, NULL);

	GString *colnames = g_string_new ("");
	GString *values = g_string_new ("");
	const gchar *pk = MGD_DBCLASS_PRIMARY (klass);
	gboolean add_coma = FALSE;

	for (i = 0; i < n_props; i++) {
		/* Ignore primary key */
		if (pk && g_str_equal (pspecs[i]->name, pk))
			continue;

		const gchar *col_name = midgard_core_class_get_property_colname (klass, pspecs[i]->name);
		if (add_coma) {
			g_string_append (colnames, ", ");
			g_string_append (values, ", ");
		}

		g_string_append (colnames, col_name);

		const gchar *type_name = g_type_name (pspecs[i]->value_type);
		if (pspecs[i]->value_type == MIDGARD_TYPE_TIMESTAMP)
			type_name = "string";
		g_string_append_printf (values, "##%s::%s", col_name, type_name);

		add_coma = TRUE;
	}

	/* Add workspace context columns */
	if (add_workspace) {
		g_string_append_printf (colnames, ", %s, %s", MGD_WORKSPACE_OID_FIELD, MGD_WORKSPACE_ID_FIELD);
		g_string_append_printf (values, ", ##%s::guint", MGD_WORKSPACE_OID_FIELD);
		g_string_append_printf (values, ", ##%s::guint", MGD_WORKSPACE_ID_FIELD);
	}

	__statement_insert_add_metadata_fields (klass, colnames, values);

	g_string_append_printf (sql, " (%s) VALUES (%s)", colnames->str, values->str);		
	
	g_string_free (colnames, TRUE);
	g_string_free (values, TRUE);

	return g_string_free (sql, FALSE);
}

static GdaStatement *
__get_statement_insert (MidgardDBObjectClass *klass, MidgardConnection *mgd)
{
	gchar *query = NULL;

	/* Try workspace statement first */
	if (klass->dbpriv->uses_workspace && (mgd && MGD_CNC_USES_WORKSPACE (mgd))) {
		
		if (!klass->dbpriv->_workspace_statement_insert) {
			query = __initialize_statement_insert_query_string (klass, TRUE);
			__initialize_statement_insert_query_parameters (klass, query, TRUE);
			g_free (query);
		}

		return klass->dbpriv->_workspace_statement_insert;
	}

	if (!klass->dbpriv->_statement_insert) {
		query = __initialize_statement_insert_query_string (klass, FALSE);
		__initialize_statement_insert_query_parameters (klass, query, FALSE);
		g_free (query);
	}

	return klass->dbpriv->_statement_insert;
}

static GdaSet *
__get_statement_insert_params (MidgardDBObjectClass *klass, MidgardConnection *mgd)
{
	GdaStatement *stmt = klass->dbpriv->get_statement_insert (klass, mgd);
	if (!stmt) {
		g_error ("Failed to get GdaStatement and GdaSet (%s)", G_OBJECT_CLASS_NAME (klass));
		return NULL;
	}

	if (klass->dbpriv->uses_workspace && (mgd && MGD_CNC_USES_WORKSPACE (mgd)))
		return klass->dbpriv->_workspace_statement_insert_params;

	return klass->dbpriv->_statement_insert_params;
}

static void
__statement_update_add_metadata_fields (MidgardDBObjectClass *klass, GString *sql)
{
	MidgardMetadataClass *mklass = MGD_DBCLASS_METADATA_CLASS (klass);
	if (!mklass)
		return;

	guint i;
	guint n_prop;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (mklass), &n_prop);

	if (!pspecs)
		return;

	for (i = 0; i < n_prop; i++) {
		const gchar *col_name = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (mklass), pspecs[i]->name);
		if (!col_name)
			continue;
		const gchar *type_name = g_type_name (pspecs[i]->value_type);
		if (pspecs[i]->value_type == MIDGARD_TYPE_TIMESTAMP)
			type_name = "string";
		g_string_append_printf (sql, ", %s=##%s::%s", col_name, col_name, type_name);
	}

	g_free (pspecs);
}

static void
__initialize_statement_update_query_parameters (MidgardDBObjectClass *klass, const gchar *query_string, gboolean add_workspace)
{
	GdaSqlParser *parser = gda_sql_parser_new ();
	GdaStatement *stmt;
	GError *error = NULL;
	stmt = gda_sql_parser_parse_string (parser, query_string, NULL, &error);

	if (!stmt) {
		g_error ("Couldn't create %s class prepared statement. %s", 
				G_OBJECT_CLASS_NAME (klass), error && error->message ? error->message : "Unknown reason");
		return;
	}

	GdaSet *params; 
	if (!gda_statement_get_parameters (stmt, &params, &error)) {
		g_error ("Failed to create GdaSet for %s class. %s", 
				G_OBJECT_CLASS_NAME (klass), error && error->message ? error->message : "Unknown reason");
	}
	
	if (add_workspace) {
		klass->dbpriv->_workspace_statement_update = stmt;
		klass->dbpriv->_workspace_statement_update_params = params;
		return;
	}
	
	klass->dbpriv->_statement_update = stmt;
	klass->dbpriv->_statement_update_params = params;

	return;
}

static gchar *
__initialize_statement_update_query_string (MidgardDBObjectClass *klass, gboolean add_workspace)
{
	GString *sql = g_string_new ("");
	guint n_props;
	guint i;
	const gchar *table = MGD_DBCLASS_TABLENAME (klass);
	g_return_val_if_fail (table != NULL, NULL);

	g_string_append_printf (sql, "UPDATE %s SET ", table); 

	GParamSpec **pspecs = midgard_core_dbobject_class_list_properties (klass, &n_props);
	g_return_val_if_fail (pspecs != NULL, NULL);

	const gchar *pk = MGD_DBCLASS_PRIMARY (klass);
	gboolean add_coma = FALSE;

	for (i = 0; i < n_props; i++) {
		/* Ignore primary key */
		if (pk && g_str_equal (pspecs[i]->name, pk))
			continue;

		const gchar *col_name = midgard_core_class_get_property_colname (klass, pspecs[i]->name);

		const gchar *type_name = g_type_name (pspecs[i]->value_type);
		if (pspecs[i]->value_type == MIDGARD_TYPE_TIMESTAMP)
			type_name = "string";
		g_string_append_printf (sql, "%s%s=##%s::%s", add_coma ? ", " : " ", col_name, col_name, type_name);
		add_coma = TRUE;
	}

	if (add_workspace) {
      		/* Add workspace context columns */
		g_string_append_printf (sql, ",%s=##%s::guint, %s=##%s::guint", 
				MGD_WORKSPACE_OID_FIELD, MGD_WORKSPACE_OID_FIELD,
				MGD_WORKSPACE_ID_FIELD, MGD_WORKSPACE_ID_FIELD);
	}

	__statement_update_add_metadata_fields (klass, sql);

	if (add_workspace) 
		g_string_append_printf (sql, " WHERE guid=##guid::string  AND %s=##%s::guint", 
				MGD_WORKSPACE_ID_FIELD, MGD_WORKSPACE_ID_FIELD);
	else 
		g_string_append (sql, " WHERE guid=##guid::string ");		

	GdaSqlParser *parser = gda_sql_parser_new ();
	GdaStatement *stmt;
	GError *error = NULL;
	stmt = gda_sql_parser_parse_string (parser, sql->str, NULL, &error);	

	return g_string_free (sql, FALSE);
}

static GdaStatement *
__get_statement_update (MidgardDBObjectClass *klass, MidgardConnection *mgd)
{
	gchar *query = NULL;

	/* Try workspace statement first */
	if (klass->dbpriv->uses_workspace && (mgd && MGD_CNC_USES_WORKSPACE (mgd))) {
		
		if (!klass->dbpriv->_workspace_statement_update) {
			query = __initialize_statement_update_query_string (klass, TRUE);
			__initialize_statement_update_query_parameters (klass, query, TRUE);
			g_free (query);
		}

		return klass->dbpriv->_workspace_statement_update;
	}

	if (!klass->dbpriv->_statement_update) {
		query = __initialize_statement_update_query_string (klass, FALSE);
		__initialize_statement_update_query_parameters (klass, query, FALSE);
		g_free (query);
	}

	return klass->dbpriv->_statement_update;
}

static GdaSet *
__get_statement_update_params (MidgardDBObjectClass *klass, MidgardConnection *mgd)
{	
	GdaStatement *stmt = klass->dbpriv->get_statement_update (klass, mgd);
	if (!stmt) {
		g_error ("Failed to get GdaStatement and GdaSet (%s)", G_OBJECT_CLASS_NAME (klass));
		return NULL;
	}

	if (klass->dbpriv->uses_workspace && (mgd && MGD_CNC_USES_WORKSPACE (mgd)))
		return klass->dbpriv->_workspace_statement_update_params;

	return klass->dbpriv->_statement_update_params;
}

GParamSpec**
midgard_core_dbobject_class_list_properties (MidgardDBObjectClass *klass, guint *n_props)
{
	g_return_val_if_fail (klass != NULL, NULL);

	MgdSchemaTypeAttr *type_attr = klass->dbpriv->storage_data;
	if (type_attr->params) {
		*n_props = g_slist_length (type_attr->_properties_list);
		return type_attr->params;
	}

	GSList *l;
	guint i = 0;
	guint n_params = g_slist_length (type_attr->_properties_list);
	type_attr->params = g_new (GParamSpec *, n_params);

	for (l = type_attr->_properties_list; l != NULL; l = l->next, i++) {
		type_attr->params[i] = g_object_class_find_property (G_OBJECT_CLASS (klass), (const gchar *) l->data);	
	}

	*n_props = n_params;
	return type_attr->params;
}

/* GOBJECT ROUTINES */

enum {
	PROPERTY_CONNECTION = 1
};

static void
__midgard_dbobject_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardDBObject *object = (MidgardDBObject *) instance;

	MIDGARD_DBOBJECT (object)->dbpriv = g_new(MidgardDBObjectPrivate, 1);
	MIDGARD_DBOBJECT (object)->dbpriv->mgd = NULL; /* read only */
	MIDGARD_DBOBJECT (object)->dbpriv->guid = NULL;
	MIDGARD_DBOBJECT (object)->dbpriv->datamodel = NULL;
	MIDGARD_DBOBJECT (object)->dbpriv->row = -1;	
	MIDGARD_DBOBJECT (object)->dbpriv->has_metadata = FALSE;
	MIDGARD_DBOBJECT (object)->dbpriv->metadata = NULL;
	MIDGARD_DBOBJECT (object)->dbpriv->is_in_storage = FALSE;

	MIDGARD_DBOBJECT (object)->dbpriv->storage_data =
		MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->storage_data;

}

static GObject *
midgard_dbobject_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties) 
{	
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return G_OBJECT(object);
}

static void __weak_ref_notify (gpointer data, GObject *object)
{
	if (data != NULL && G_IS_OBJECT (data)) {	
		MidgardDBObject *dbobject = MIDGARD_DBOBJECT (data);
		MGD_OBJECT_CNC (dbobject) = NULL;
	}
}

static void 
midgard_dbobject_dispose (GObject *object)
{
	MidgardDBObject *self = MIDGARD_DBOBJECT (object);
	if (self->dbpriv->datamodel && G_IS_OBJECT (self->dbpriv->datamodel))
		g_object_unref(self->dbpriv->datamodel);

	self->dbpriv->row = -1;

	MidgardMetadata *metadata = MGD_DBOBJECT_METADATA (self);
	if (metadata && G_IS_OBJECT (metadata)) {
		g_object_unref (metadata);
		MGD_DBOBJECT_METADATA (self) = NULL;
	}

	/* Nullify connection's pointer. */
	MidgardConnection *mgd = MIDGARD_DBOBJECT (self)->dbpriv->mgd;
	if (mgd != NULL && G_IS_OBJECT (mgd)) {
		g_object_weak_unref (G_OBJECT(self->dbpriv->mgd), __weak_ref_notify, self);	
		MIDGARD_DBOBJECT (self)->dbpriv->mgd = NULL;
	}

	parent_class->dispose (object);
}

static void 
midgard_dbobject_finalize (GObject *object)
{
	MidgardDBObject *self = MIDGARD_DBOBJECT(object);

	if (!self)
		return;

	if (!self->dbpriv)
		return;

	g_free((gchar *)self->dbpriv->guid);
	self->dbpriv->guid = NULL;

	g_free(self->dbpriv);
	self->dbpriv = NULL;

	parent_class->finalize (object);
}

static const MidgardConnection *__get_connection(MidgardDBObject *self)
{
	g_assert(self != NULL);
	return self->dbpriv->mgd;
}

static void
__midgard_dbobject_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	switch (property_id) {
		
		case PROPERTY_CONNECTION:			
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
__midgard_dbobject_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	GObject *mgd;
	switch (property_id) {

		case PROPERTY_CONNECTION:
			if (!G_VALUE_HOLDS_OBJECT (value)) 
				return;

			mgd = g_value_get_object (value);

			if (!MIDGARD_IS_CONNECTION (mgd))
				return;
	
			MIDGARD_DBOBJECT (object)->dbpriv->mgd = MIDGARD_CONNECTION (mgd);
			/* Add weak reference callback to connection. 
			 * Instead of keeping connection's alive (implicitly) we create sentinel 
			 * which guarantees connection's pointer to be null if connection is destroyed. */
			g_object_weak_ref (mgd, __weak_ref_notify, object);

			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

MidgardDBObjectPrivate *
midgard_core_dbobject_private_new ()
{
	MidgardDBObjectPrivate *dbpriv = g_new (MidgardDBObjectPrivate, 1);
	dbpriv->storage_data = NULL;
	dbpriv->create_storage = NULL;
	dbpriv->update_storage = NULL;
	dbpriv->storage_exists = NULL;
	dbpriv->delete_storage = NULL;
	dbpriv->add_fields_to_select_statement = NULL;
	dbpriv->get_property = NULL;
	dbpriv->set_property = NULL;
	dbpriv->set_from_data_model = NULL;
	dbpriv->_statement_insert = NULL;
	dbpriv->_statement_insert_params = NULL;
	dbpriv->_workspace_statement_insert = NULL;
	dbpriv->_workspace_statement_insert_params = NULL;
	dbpriv->get_statement_insert = NULL;
	dbpriv->get_statement_insert_params = NULL;	
	dbpriv->_statement_update = NULL;
	dbpriv->_statement_update_params = NULL;
	dbpriv->_workspace_statement_update = NULL;
	dbpriv->_workspace_statement_update_params = NULL;
	dbpriv->get_statement_update = NULL;
	dbpriv->get_statement_update_params = NULL;	
	dbpriv->set_static_sql_select = NULL;
	dbpriv->uses_workspace = FALSE;

	return dbpriv;
}

static void 
midgard_dbobject_class_init (MidgardDBObjectClass *klass, gpointer g_class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);
	
	object_class->constructor = midgard_dbobject_constructor;
	object_class->dispose = midgard_dbobject_dispose;
	object_class->finalize = midgard_dbobject_finalize;
	object_class->set_property = __midgard_dbobject_set_property;
	object_class->get_property = __midgard_dbobject_get_property;

	klass->get_connection = __get_connection;

	klass->dbpriv = g_new (MidgardDBObjectPrivate, 1);
	klass->dbpriv->storage_data = NULL;
	klass->dbpriv->create_storage = NULL;
	klass->dbpriv->update_storage = NULL;
	klass->dbpriv->storage_exists = NULL;
	klass->dbpriv->delete_storage = NULL;
	klass->dbpriv->add_fields_to_select_statement = _add_fields_to_select_statement;
	klass->dbpriv->get_property = _midgard_dbobject_get_property;
	klass->dbpriv->set_property = _midgard_dbobject_set_property;
	klass->dbpriv->set_from_data_model = _midgard_dbobject_set_from_data_model;
	klass->dbpriv->_statement_insert = NULL;
	klass->dbpriv->_statement_insert_params = NULL;
	klass->dbpriv->_workspace_statement_insert = NULL;
	klass->dbpriv->_workspace_statement_insert_params = NULL;
	klass->dbpriv->get_statement_insert = __get_statement_insert;
	klass->dbpriv->get_statement_insert_params = __get_statement_insert_params;
	klass->dbpriv->_statement_update = NULL;
	klass->dbpriv->_statement_update_params = NULL;
	klass->dbpriv->_workspace_statement_update = NULL;
	klass->dbpriv->_workspace_statement_update_params = NULL;
	klass->dbpriv->get_statement_update = __get_statement_update;
	klass->dbpriv->get_statement_update_params = __get_statement_update_params;	
	klass->dbpriv->set_static_sql_select = NULL;
	klass->dbpriv->uses_workspace = FALSE;

	/* Properties */
	GParamSpec *pspec = g_param_spec_object ("connection",
			"",
			"",
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardDBObject:connection:
	 * 
	 * Pointer to #MidgardConnection, given object has been initialized for
	 */  
	g_object_class_install_property (object_class, PROPERTY_CONNECTION, pspec);
}

/* Registers the type as a fundamental GType unless already registered. */ 
GType 
midgard_dbobject_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardDBObjectClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) midgard_dbobject_class_init, 
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardDBObject),
			0,              /* n_preallocs */  
			__midgard_dbobject_instance_init
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardDBObject", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
