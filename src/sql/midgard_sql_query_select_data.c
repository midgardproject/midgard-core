/* 
 * Copyright (C) 2012 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "../midgard_query_selector.h"
#include "midgard_sql_query_select_data.h"
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include "midgard_query_holder.h"
#include "midgard_dbobject.h"
#include "midgard_validable.h"
#include "midgard_executable.h"
#include <sql-parser/gda-sql-parser.h>
#include "midgard_core_workspace.h"
#include "midgard_workspace_storage.h"
#include "midgard_core_config.h"
#include "../midgard_query_result.h"
#include "midgard_sql_query_result.h"
#include "midgard_sql_query_constraint.h"

/**
 * midgard_sql_query_select_data_new:
 * @mgd: #MidgardConnection instance
 *
 * Returns: new #MidgardSqlQuerySelectData instance or %NULL on failure
 * Since: 10.05.6
 */ 
MidgardSqlQuerySelectData *
midgard_sql_query_select_data_new (MidgardConnection *mgd)
{
	g_return_val_if_fail (mgd != NULL, NULL);

	MidgardSqlQuerySelectData *self = g_object_new (MIDGARD_TYPE_SQL_QUERY_SELECT_DATA, "connection", mgd, NULL);
	return self;
}

/**
 * midgard_sql_query_select_data_add_column:
 * @self: #MidgardSqlQuerySelectData instance
 * @column: #MidgardSqlQueryColumn to add
 *
 * Adds a new column, which will be available in #MidgardSqlQueryResult 
 *
 * Since: 10.05.6
 */ 
void
midgard_sql_query_select_data_add_column (MidgardSqlQuerySelectData *self, MidgardSqlQueryColumn *column)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (column != NULL);

	self->columns = g_slist_append (self->columns, g_object_ref (column));
}

/**
 * midgard_sql_query_select_data_get_columns:
 * @self: #MidgardSqlQuerySelectData instance
 * @n_objects: pointer to store number of returned columns
 * @error: pointer to store returned error
 *
 * Returns all collumns added to given instance.
 *
 * Returns: (element-type MidgardSqlQueryColumn) (array length=n_objects) (transfer full): array of #MidgardSqlQueryColumn objects or %NULL
 *
 * Since: 10.05.6
 */
MidgardSqlQueryColumn**         
midgard_sql_query_select_data_get_columns (MidgardSqlQuerySelectData *self, guint *n_objects, GError **error)
{
	g_return_val_if_fail (self != NULL, NULL);
	*n_objects = 0;
	
	if (self->columns == NULL)
		return NULL;

	guint n = g_slist_length (self->columns);
	if (n == 0)
		return NULL;

	MidgardSqlQueryColumn **columns = g_new (MidgardSqlQueryColumn*, n);
	GSList *l;
	guint i = 0;
	for (l = self->columns; l != NULL; l = l->next, i++) {
		columns[i] = g_object_ref (G_OBJECT (l->data));
	}

	*n_objects = n;
	return columns;
}

gboolean
_midgard_sql_query_select_data_set_constraint (MidgardQueryExecutor *self, MidgardQueryConstraintSimple *constraint)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (constraint != NULL, FALSE);

	if (self->priv->constraint)
		g_object_unref (self->priv->constraint);	

	self->priv->constraint = g_object_ref (constraint);

	return TRUE;
}

gboolean
_midgard_sql_query_select_data_set_limit (MidgardQueryExecutor *self, guint limit)
{
	g_return_val_if_fail (self != NULL, FALSE);

	self->priv->limit = limit;

	return TRUE;
}

gboolean 
_midgard_sql_query_select_data_set_offset (MidgardQueryExecutor *self, guint offset)
{
	g_return_val_if_fail (self != NULL, FALSE);
	self->priv->offset = offset;

	return TRUE;
}

typedef struct {
	MidgardQueryProperty *property;
	gboolean asc;
	gchar *order_type;
} qso;

gboolean
_midgard_sql_query_select_data_add_order (MidgardQueryExecutor *self, MidgardQueryProperty *property, const gchar *type)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);

	qso *_qs = g_new (qso, 1);
	_qs->order_type = g_strdup (type);
	_qs->asc = FALSE;
	_qs->property = g_object_ref (property);

	self->priv->orders = g_slist_append (self->priv->orders, _qs);

	return FALSE;
}

static struct {
	const gchar *name;
	GdaSqlSelectJoinType type;
} valid_joins[] = {
	{ "cross", GDA_SQL_SELECT_JOIN_CROSS },
	{ "natural", GDA_SQL_SELECT_JOIN_NATURAL },
	{ "inner", GDA_SQL_SELECT_JOIN_INNER },
	{ "left", GDA_SQL_SELECT_JOIN_LEFT },
	{ "right", GDA_SQL_SELECT_JOIN_RIGHT },
	{ "full", GDA_SQL_SELECT_JOIN_FULL },
	{ NULL,0 }
};

static gboolean 
__query_join_type_is_valid(const gchar *type, GdaSqlSelectJoinType *join_type)
{
	guint i;
	gchar *ltype = g_ascii_strdown (type, -1);
	for (i = 0; valid_joins[i].name != NULL; i++) {
		if (g_str_equal(ltype, valid_joins[i].name)) {
			*join_type = valid_joins[i].type;
			g_free (ltype);
			return TRUE;
		}
	}
	
	g_free (ltype);

	if (valid_joins[i].name == NULL) {
		g_warning("Invalid join type %s", type);
		return FALSE;
	}
	
	return TRUE;
}

typedef struct {
	MidgardQueryProperty *left_property;
	MidgardQueryProperty *right_property;	
	GdaSqlSelectJoinType join_type;
} qsj;

gboolean
_midgard_sql_query_select_data_add_join (MidgardQueryExecutor *self, const gchar *join_type, 
		MidgardQueryHolder *left_holder, MidgardQueryHolder *right_holder)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (join_type != NULL, FALSE);
	g_return_val_if_fail (left_holder != NULL, FALSE);
	g_return_val_if_fail (right_holder != NULL, FALSE);

	/* validate join type */
	GdaSqlSelectJoinType join_type_id;
	if (!__query_join_type_is_valid (join_type, &join_type_id)) 
		return FALSE;

	qsj *_sj = g_new (qsj, 1);
	_sj->left_property = g_object_ref (left_holder);
	_sj->right_property = g_object_ref (right_holder);

	_sj->join_type = join_type_id;

	self->priv->joins = g_slist_append (self->priv->joins, _sj);

	return TRUE;
}

gboolean __query_select_data_add_orders (MidgardQueryExecutor *self, GError **error)
{
	if (!self->priv->orders)
		return TRUE;

	GSList *l = NULL;

	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (self);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;	
	GdaSqlSelectOrder *order; 
	
	for (l = MIDGARD_QUERY_EXECUTOR (self)->priv->orders; l != NULL; l = l->next) {

		/* Proper asc type is set during validation phase */
		qso *_so = (qso*) l->data;

		/* Create new order */
		order = gda_sql_select_order_new (GDA_SQL_ANY_PART (select));
		order->asc = _so->asc;
		MidgardQueryProperty *property = _so->property;
		MidgardQueryStorage *storage = NULL;
		
		if (property->priv && property->priv->storage)
			storage = property->priv->storage;

		/* Compute table.colname for given property name */
		GValue rval = {0, };
		midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (property), &rval);
		GError *err = NULL;
		gchar *table_field = midgard_core_query_compute_constraint_property (executor, storage, g_value_get_string (&rval), &err);
		if (err) {
			g_propagate_error (error, err);
			g_free (table_field);
			gda_sql_select_order_free (order);
			return FALSE;
		}

		if (!table_field) {
			g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_LOCATION_INVALID,
					"Can not find table and column name for given '%s' property name", g_value_get_string (&rval));
			g_value_unset (&rval);
			return FALSE;	
		}

		g_value_unset (&rval);

		GValue *value = g_new0 (GValue, 1);
		g_value_init (value, G_TYPE_STRING);
		g_value_take_string (value, table_field);

		/* Set order's expression and add new one to statement orders list */
		GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (order));
		expr->value = value;
		order->expr = expr;
		select->order_by = g_slist_append (select->order_by, order);
	}

	return TRUE;
}

#define __BOOL_VALUE(__bool)(__bool ? "0" : "FALSE")

static void
__add_implicit_workspace_join (MidgardSqlQuerySelectData *self, GdaSqlOperation *operation)
{
	g_return_if_fail (self != NULL);
	
	MidgardDBObjectClass *klass = MIDGARD_QUERY_EXECUTOR (self)->priv->storage->priv->klass;
	if (!g_type_is_a (G_OBJECT_CLASS_TYPE (klass), MIDGARD_TYPE_OBJECT))
		return;

	MidgardConnection *mgd = MIDGARD_QUERY_EXECUTOR (self)->priv->mgd;
	/* Do not take worskpace into account if if it's disabled 
	 * or enabled and none set */
	if (!MGD_CNC_USES_WORKSPACE (mgd))
		return;
	if (!MGD_CNC_HAS_WORKSPACE (mgd))
		return;

	gboolean bool_is_int = MIDGARD_QUERY_EXECUTOR (self)->priv->bool_is_int;

	guint ws_id = MGD_CNC_WORKSPACE_ID (mgd);
	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (self);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;
	GdaSqlSelectFrom *from = select->from;
	GdaSqlSelectJoin *join;
	const gchar *klass_table = MGD_DBCLASS_TABLENAME (klass);

	gchar *left_table = executor->priv->table_alias;
	//gchar *right_table = g_strdup_printf ("t%d", ++executor->priv->tableid);
	gchar *right_table = g_strdup ("midgard_ws_tmp_table");

	join = gda_sql_select_join_new (GDA_SQL_ANY_PART (from));
	join->type = GDA_SQL_SELECT_JOIN_INNER;

	GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (join));
	expr->value = gda_value_new (G_TYPE_STRING);
	g_value_take_string (expr->value, g_strdup_printf ("%s.%s = %s.%s",
				left_table, MGD_WORKSPACE_ID_FIELD, right_table, MGD_WORKSPACE_ID_FIELD));

	join->expr = expr;
	join->position = ++executor->priv->joinid;

	MidgardConfig *config = mgd->priv->config;
	const gchar *sql_part_bool_func_start = "";
	const gchar *sql_part_bool_func_end = "";
	const gchar *deleted_field = midgard_core_object_get_deleted_field (klass);	
	GString *table = g_string_new ("(SELECT DISTINCT MAX");
	if (config->priv->dbtype == MIDGARD_DB_TYPE_POSTGRES && deleted_field) {
		sql_part_bool_func_start = " bool_or(";
		sql_part_bool_func_end = ") AS metadata_deleted";
	}
	g_string_append_printf (table, "(%s) AS %s, %s %s%s%s%s FROM %s WHERE %s IN (0,",
			MGD_WORKSPACE_ID_FIELD, MGD_WORKSPACE_ID_FIELD, MGD_WORKSPACE_OID_FIELD, 
			deleted_field ? "," : "",
			sql_part_bool_func_start,
			deleted_field ? deleted_field : "",
			sql_part_bool_func_end,
			klass_table, MGD_WORKSPACE_ID_FIELD);

	const MidgardWorkspaceStorage *ws = midgard_connection_get_workspace (mgd);
	GSList *list = MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (ws)->priv->list_ids (mgd, MIDGARD_WORKSPACE_STORAGE (ws));
	GSList *l = NULL;
	guint i = 0;
	guint id;
	for (l = list; l != NULL; l = l->next, i++) {
		GValue *id_val = (GValue *) l->data;
		if (G_VALUE_HOLDS_UINT (id_val))
			id = g_value_get_uint (id_val);
		else
			id = (guint) g_value_get_int (id_val);
		g_string_append_printf (table, "%s%d", i > 0 ? "," : "", id);
	}
	if (!list) 
		g_string_append (table, "0");
	g_string_append (table, ") ");  
	if (deleted_field)
		g_string_append_printf (table, "AND %s = %s ", deleted_field, __BOOL_VALUE (bool_is_int));
	g_string_append_printf (table, "GROUP BY %s)", MGD_WORKSPACE_OID_FIELD);
	g_slist_free (list);

	gda_sql_select_from_take_new_join (from , join);
	GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (from));
	s_target->table_name = g_string_free (table, FALSE);
	s_target->as = right_table;
	gda_sql_select_from_take_new_target (from, s_target);

	GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
	GValue *tval = g_new0 (GValue, 1);
	g_value_init (tval, G_TYPE_STRING);
	g_value_set_string (tval, g_strdup (s_target->table_name));
	texpr->value = tval;
	s_target->expr = texpr;

	/* Add workspace object id constraint */
	GdaSqlExpr *ws_expr = gda_sql_expr_new (GDA_SQL_ANY_PART (operation));
	ws_expr->value = gda_value_new (G_TYPE_STRING);
	g_value_take_string (ws_expr->value, g_strdup_printf ("%s.%s = %s.%s", 
				left_table, MGD_WORKSPACE_OID_FIELD, right_table, MGD_WORKSPACE_OID_FIELD));
	operation->operands = g_slist_append (operation->operands, ws_expr);

}

gboolean __query_select_data_add_joins (MidgardSqlQuerySelectData *self, GdaSqlOperation *operation, GError **error)
{
	//__add_implicit_workspace_join (self, operation);

	if (!MIDGARD_QUERY_EXECUTOR (self)->priv->joins)
		return TRUE;

	GSList *l = NULL;

	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (self);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;	
	GdaSqlSelectFrom *from = select->from;
	GdaSqlSelectJoin *join; 
	
	for (l = MIDGARD_QUERY_EXECUTOR (self)->priv->joins; l != NULL; l = l->next) {

		qsj *_sj = (qsj*) l->data;

		join = gda_sql_select_join_new (GDA_SQL_ANY_PART (from));
		join->type = _sj->join_type;

		MidgardQueryStorage *left_storage = _sj->left_property->priv->storage;
		MidgardQueryStorage *right_storage = _sj->right_property->priv->storage;

		/*
		GError *err = NULL;
		GValue lval = {0, };
		midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (_sj->left_property), &lval);
		gchar *left_table_field = 
			midgard_core_query_compute_constraint_property (executor, left_storage, g_value_get_string (&lval), &err);
		if (err) {
			g_propagate_error (error, err);
			g_free (left_table_field);
		}

		GValue rval = {0, };
		midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (_sj->right_property), &rval);
		gchar *right_table_field = 
			midgard_core_query_compute_constraint_property (executor, right_storage, g_value_get_string (&rval), &err);
		if (err) {
			g_propagate_error (error, err);
			g_free (right_table_field);
		}*/
	       
		GValue lval = {0, };
		midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (_sj->left_property), &lval);
		
		GValue rval = {0, };
                midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (_sj->right_property), &rval);

		GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (join));
		expr->value = gda_value_new (G_TYPE_STRING);
		g_value_take_string (expr->value, g_strdup_printf ("%s = %s", g_value_get_string (&lval), g_value_get_string (&rval)));

		g_value_unset (&lval);
		g_value_unset (&rval);

		join->expr = expr;
		join->position = ++executor->priv->joinid;

		const gchar *qualifier = midgard_query_column_get_qualifier (MIDGARD_QUERY_COLUMN (_sj->right_property), NULL);

		/* Add right qualifier to targets */	
		gda_sql_select_from_take_new_join (from , join);
		GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (from));
		s_target->table_name = g_strdup (qualifier);
		//s_target->as = g_strdup (right_storage->priv->table_alias);
		gda_sql_select_from_take_new_target (from, s_target);
	
		/* Set target expression */
		GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
		GValue *tval = g_new0 (GValue, 1);
		g_value_init (tval, G_TYPE_STRING);
		g_value_set_string (tval, qualifier);
		texpr->value = tval;
		s_target->expr = texpr;
	}

	return TRUE;
}

static void
__add_dummy_constraint (GdaSqlStatementSelect *select, GdaSqlOperation *top_operation)
{
	GdaSqlExpr *dexpr = gda_sql_expr_new (GDA_SQL_ANY_PART (top_operation));
	dexpr->value = gda_value_new (G_TYPE_STRING);
	g_value_take_string (dexpr->value, g_strdup ("1=1"));
	top_operation->operands = g_slist_append (top_operation->operands, dexpr);

	return;
}

static void
__add_second_dummy_constraint (GdaSqlStatementSelect *select, GdaSqlOperation *top_operation)
{
	GdaSqlExpr *dexpr = gda_sql_expr_new (GDA_SQL_ANY_PART (top_operation));
	dexpr->value = gda_value_new (G_TYPE_STRING);
	g_value_take_string (dexpr->value, g_strdup ("0<1"));
	top_operation->operands = g_slist_append (top_operation->operands, dexpr);

	return;
}

static void
_get_all_constraints (MidgardSqlQuerySelectData *self, MidgardQueryConstraintSimple *constr, GSList **list)
{
	if (!constr)
		if (self && MIDGARD_QUERY_EXECUTOR (self)->priv->constraint)
			constr = MIDGARD_QUERY_EXECUTOR (self)->priv->constraint;

	if (!constr)
		return;

	*list = g_slist_prepend (*list, constr);
	guint n_objects;
	MidgardQueryConstraintSimple **constraints = midgard_query_constraint_simple_list_constraints (constr, &n_objects);
	if (!constraints)
		return;

	guint i;
	for (i = 0; i < n_objects; i++) {
		_get_all_constraints (NULL, constraints[i], list);
	}

	g_free (constraints);
}

#define __FREE() \
	g_object_unref (storage); \
	g_object_unref (columns[i]); \
	free (columns);

static void
_midgard_sql_query_select_data_validable_iface_validate (MidgardValidable *iface, GError **error)
{
	g_return_if_fail (iface != NULL);
	MidgardSqlQuerySelectData *self = MIDGARD_SQL_QUERY_SELECT_DATA (iface);
	MIDGARD_QUERY_EXECUTOR (self)->priv->is_valid = FALSE;

	GError *err = NULL;
	guint n_objects;
	guint i;
	MidgardSqlQueryColumn **columns = midgard_sql_query_select_data_get_columns (self, &n_objects, NULL);
	
	/* No single column added to selector */
	if (!columns) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_LOCATION_INVALID,
				"No single column added to selector");
		return;
	}

	for (i = 0; i < n_objects; i++) {

		MidgardQueryProperty *qprop = midgard_query_column_get_query_property (MIDGARD_QUERY_COLUMN(columns[i]), NULL);
		MidgardQueryStorage *storage = NULL;
		g_object_get (G_OBJECT (qprop), "storage", &storage, NULL);
		g_object_unref (qprop);
		
		/* No storage */
		if (!storage) {
			g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_LOCATION_INVALID,
					"No QueryStorage associated with QuerySelectData");
			__FREE();	
			return;
		}

		/* No GObject derived class */
		GObjectClass *klass = (GObjectClass *) storage->priv->klass;
		if (klass == NULL || (!G_IS_OBJECT_CLASS(klass))) {
			g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_TYPE_INVALID,
					"Invalid '%s' classname associated with QueryStorage", storage->priv->classname);
			__FREE();
			return;
		}
		
		const gchar *table = midgard_core_class_get_table (MIDGARD_DBOBJECT_CLASS (klass));
		
		/* No class table */
		if (!table) {
			g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_LOCATION_INVALID,
					"No SQL table defined for '%s' class", G_OBJECT_CLASS_NAME (klass));
			__FREE();
			return;
		}

		g_object_unref (columns[i]);
	}
	g_free (columns);

	/* COMMON WITH QUERYEXECUTOR */
	/* TODO: Consider executor->validate() */

	/* Limit */
	if (MIDGARD_QUERY_EXECUTOR (self)->priv->limit == 0) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_VALUE_INVALID,
				"Invalid limit value (0). Expected > 0");
		return;
	}

	/* Orders */
	GSList *l = NULL;
	for (l = MIDGARD_QUERY_EXECUTOR (self)->priv->orders; l != NULL; l = l->next) {
		qso *_so = (qso*) l->data;
		gboolean asc = FALSE;
		gchar *lorder = g_ascii_strdown (_so->order_type, -1);
		if (g_str_equal (lorder, "asc")) {
			_so->asc = TRUE;
		} else if (g_str_equal (lorder, "desc")) {
			_so->asc = FALSE;
		} else {
			g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_VALUE_INVALID,
					"Invalid order type '%s'. Expected ASC or DESC", _so->order_type);
			g_free (lorder); 
			return;
		}		
		g_free (lorder);
	}

	/* Gather all objects required for execution */
	GSList *o_list = NULL;
	//o_list = g_slist_prepend (o_list, storage);
	_get_all_constraints (self, NULL, &o_list);

	for (l = o_list; l != NULL; l = l->next) {

		if (MIDGARD_IS_SQL_QUERY_CONSTRAINT (l->data)
				/* || MIDGARD_IS_SQL_QUERY_CONSTRAINT_GROUP (l->data)) */
				&& (!midgard_validable_is_valid (MIDGARD_VALIDABLE (l->data)))) {
			//midgard_validable_validate (MIDGARD_VALIDABLE (l->data), &err);
			if (err) {
				g_propagate_error (error, err);
				g_slist_free (o_list);
				return;
			}
		} else {
			g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_TYPE_INVALID,
					"Invalid constraint type '%s'. Expected SqlQueryConstraint or SqlQueryConstraintGroup", 
					G_OBJECT_TYPE_NAME (G_OBJECT (l->data)));
			g_slist_free (o_list);
			return;
		}
	}

	if (o_list)
		g_slist_free (o_list);

	MIDGARD_QUERY_EXECUTOR (self)->priv->is_valid = TRUE;
	return;
}

gboolean
_midgard_sql_query_select_data_validable_iface_is_valid (MidgardValidable *self)
{
	g_return_val_if_fail (self != NULL, FALSE);
	return MIDGARD_QUERY_EXECUTOR (self)->priv->is_valid;
}

static void
_midgard_sql_query_select_data_executable_iface_execute (MidgardExecutable *iface, GError **error)
{
	g_return_if_fail (iface != NULL);
	MidgardSqlQuerySelectData *self = MIDGARD_SQL_QUERY_SELECT_DATA (iface);
	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (self);

	g_signal_emit (self, MIDGARD_QUERY_EXECUTOR_GET_CLASS (self)->signal_id_execution_start, 0);

	GError *err = NULL;
	midgard_validable_validate (MIDGARD_VALIDABLE (self), &err);
	if (err) {	
		g_propagate_error (error, err);
		return;
	} 	

	g_object_ref (self);
	
	MidgardConnection *mgd = executor->priv->mgd;
	GdaConnection *cnc = mgd->priv->connection;
	GdaSqlStatement *sql_stm;
	GdaSqlStatementSelect *sss;

	sql_stm = gda_sql_statement_new (GDA_SQL_STATEMENT_SELECT);
	sss = (GdaSqlStatementSelect*) sql_stm->contents;
	g_assert (GDA_SQL_ANY_PART (sss)->type == GDA_SQL_ANY_STMT_SELECT);
	MIDGARD_QUERY_EXECUTOR (self)->priv->stmt = sql_stm;
	sss->from = gda_sql_select_from_new (GDA_SQL_ANY_PART (sss));

	/* Initialize top base expresion and operation with default AND operator type */
	GdaSqlExpr *base_where = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
	GdaSqlOperation *base_operation = gda_sql_operation_new (GDA_SQL_ANY_PART (base_where));
	base_operation->operator_type = GDA_SQL_OPERATOR_TYPE_AND;
	base_where->cond = base_operation;
	gda_sql_statement_select_take_where_cond (sql_stm, base_where);

	/* Create SELECT a.t, b.t... FROM ... */
	GSList *table_list = NULL;
	guint n_objects;
	guint i;
	MidgardSqlQueryColumn **columns = midgard_sql_query_select_data_get_columns (self, &n_objects, NULL);

	for (i = 0; i < n_objects; i++) {

		MidgardQueryProperty *qprop = midgard_query_column_get_query_property (MIDGARD_QUERY_COLUMN(columns[i]), NULL);
		MidgardQueryStorage *storage = NULL;
		g_object_get (G_OBJECT (qprop), "storage", &storage, NULL);
		gchar *property;
		g_object_get (qprop, "property", &property, NULL);
		g_object_unref (qprop);
		MidgardDBObjectClass *dbklass = storage->priv->klass;
		const gchar *table_alias = midgard_query_column_get_qualifier (MIDGARD_QUERY_COLUMN(columns[i]), NULL);
		const gchar *property_table = midgard_core_class_get_table (dbklass);
		const gchar *property_field = midgard_core_class_get_property_colname (dbklass, property);

		/* Create SELECT a.t, b.t... */
		GdaSqlSelectField *select_field = gda_sql_select_field_new (GDA_SQL_ANY_PART (sss));
		select_field->as = gda_connection_quote_sql_identifier (cnc, midgard_query_column_get_name (MIDGARD_QUERY_COLUMN(columns[i]), NULL));
		sss->expr_list = g_slist_append (sss->expr_list, select_field);
		GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (select_field));
		GValue *val = g_new0 (GValue, 1);
		g_value_init (val, G_TYPE_STRING);
		gchar *q_table = gda_connection_quote_sql_identifier (cnc, table_alias);
		gchar *q_field = gda_connection_quote_sql_identifier (cnc, property_field);
		g_value_take_string (val, g_strconcat (q_table, ".", q_field, NULL));
		g_free (q_table);
		g_free (q_field);
		expr->value = val;
		select_field->expr = expr;

		g_free (property);
		g_object_unref (storage);

		if (table_list != NULL && g_slist_find_custom (table_list, property_table, 
					(GCompareFunc) g_strcmp0) != NULL)
			continue;

		/* Create targets, FROM */
		GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (sss->from));
		s_target->table_name = g_strdup (property_table);
		s_target->as = g_strdup (midgard_query_column_get_qualifier (MIDGARD_QUERY_COLUMN(columns[i]), NULL));
		MIDGARD_QUERY_EXECUTOR (self)->priv->table_alias = g_strdup (s_target->as);
		gda_sql_select_from_take_new_target (sss->from, s_target);
		g_object_unref (columns[i]);

		/* Set target expression */	
		GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
		GValue *tval = g_new0 (GValue, 1);
		g_value_init (tval, G_TYPE_STRING);
		g_value_set_string (tval, s_target->table_name);
		texpr->value = tval;
		s_target->expr = texpr;

		table_list = g_slist_append (table_list, (gpointer) property_table);
	}

	if (table_list)
		g_slist_free (table_list);

	GdaSqlExpr *where = sss->where_cond;
	GdaSqlOperation *operation = where->cond;
	/* Add joins, LEFT JOIN tbl2 ON... */
	__query_select_data_add_joins (MIDGARD_SQL_QUERY_SELECT_DATA (self), operation, &err);
	if (err) {
	 	g_propagate_error (error, err);
		goto return_false;
	}

	/* Add constraints' conditions (WHERE a=1, b=2...) */
	if (MIDGARD_QUERY_EXECUTOR (self)->priv->constraint) {
		MIDGARD_QUERY_CONSTRAINT_SIMPLE_GET_INTERFACE (MIDGARD_QUERY_EXECUTOR (self)->priv->constraint)->priv->add_conditions_to_statement 			(MIDGARD_QUERY_EXECUTOR (self), MIDGARD_QUERY_EXECUTOR (self)->priv->constraint, sql_stm, base_where, &err);
		if (err) {
			g_set_error(error, err->domain, err->code, "%s", err->message ? err->message : "Unknown error");
			g_clear_error(&err);
		}
		if (MIDGARD_QUERY_EXECUTOR (self)->priv->n_constraints == 1) 
			__add_second_dummy_constraint (sss, operation);
		/* Add dummy constraint if operation has only one operand */
		if (operation->operands && (g_slist_length (operation->operands) == 1))
			__add_dummy_constraint (sss, operation);
	} else { 
		/* no constraints, add dummy '1=1 AND 0<1' to satisfy top constraint group */
		__add_dummy_constraint (sss, operation); 
		__add_second_dummy_constraint (sss, operation); 
	}

	/* Add orders , ORDER BY t1.field... */
	if (!__query_select_data_add_orders (executor, &err)) { 
		if (err)
			g_propagate_error (error, err);
		goto return_false;
	}


	/* Add limit, LIMIT x */
	if (executor->priv->limit > 0) {
		GdaSqlExpr *limit_expr = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
		GValue *limit_val = g_new0 (GValue, 1);
		g_value_init (limit_val, G_TYPE_STRING);
		g_value_take_string (limit_val, g_strdup_printf ("%d", executor->priv->limit));
		limit_expr->value = limit_val;
		sss->limit_count = limit_expr;
	}

	/* Add offset, OFFSET x */
	if (executor->priv->offset >= 0) {
		GdaSqlExpr *offset_expr = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
		GValue *offset_val = g_new0 (GValue, 1);
		g_value_init (offset_val, G_TYPE_STRING);
		g_value_take_string (offset_val, g_strdup_printf ("%d", executor->priv->offset));
		offset_expr->value = offset_val;
		sss->limit_offset = offset_expr;
	}	

	/* Check structure */
	if (!gda_sql_statement_check_structure (sql_stm, &err)) {
		g_set_error (error, MIDGARD_EXECUTION_ERROR, MIDGARD_EXECUTION_ERROR_INTERNAL,
				"Can't build SELECT statement: %s)", err && err->message ? err->message : _("Unknown reason"));
		if (err)
			g_clear_error (&err);
		goto return_false;
	} 

	/* Create statement */
	GdaStatement *stmt = gda_statement_new ();	
	g_object_set (G_OBJECT (stmt), "structure", sql_stm, NULL);
	gda_sql_statement_free (sql_stm);
	sql_stm = NULL;

	g_free (self->query_string);
	self->query_string = gda_connection_statement_to_sql (cnc, stmt, NULL, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
	if (MGD_CNC_DEBUG (mgd)) 
		g_debug ("QuerySelectData: %s", self->query_string);

	/* execute statement */
	GdaDataModel *model = gda_connection_statement_execute_select (cnc, stmt, NULL, &err);
	g_object_unref (stmt);

	if (!model && !err) {
		g_set_error (error, MIDGARD_EXECUTION_ERROR, MIDGARD_EXECUTION_ERROR_INTERNAL,
				"Execute error - Unknown reason, underlying error is NULL");
		goto return_false;
	}

	if (err) {
		g_set_error (error, MIDGARD_EXECUTION_ERROR, MIDGARD_EXECUTION_ERROR_INTERNAL,
				"Execute error - %s", err->message);
		g_error_free (err);
		goto return_false;
	}
	
	executor->priv->results_count = gda_data_model_get_n_rows (model);
	if (executor->priv->resultset && G_IS_OBJECT (executor->priv->resultset))
		g_object_unref (G_OBJECT (executor->priv->resultset));
	executor->priv->resultset = (gpointer) model;
	g_object_unref (self);
	g_signal_emit (self, MIDGARD_QUERY_EXECUTOR_GET_CLASS (self)->signal_id_execution_end, 0);
	return;

return_false:
	if (sql_stm)
		gda_sql_statement_free (sql_stm);

	g_signal_emit (self, MIDGARD_QUERY_EXECUTOR_GET_CLASS (self)->signal_id_execution_end, 0);
	g_object_unref (self);
	return;
}

guint
_midgard_sql_query_select_data_get_results_count (MidgardQueryExecutor *self)
{
	g_return_val_if_fail (self != NULL, 0);

	return self->priv->results_count;
}

MidgardQueryResult *
_midgard_sql_query_select_data_selector_get_query_result (MidgardQuerySelector *selector, GError **error)
{
	g_return_val_if_fail (selector != NULL, NULL);
	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (selector);
	MidgardSqlQueryResult *query_result = midgard_sql_query_result_new (selector, G_OBJECT (executor->priv->resultset));
	/* Workaround to set columns */
	guint n_objects; 
	MidgardSqlQueryColumn **columns = midgard_sql_query_select_data_get_columns ((MidgardSqlQuerySelectData *)selector, &n_objects, NULL);
	if (columns != NULL)
		midgard_sql_query_result_set_columns (query_result, columns, n_objects, NULL);

	return  (MidgardQueryResult *) query_result;
}

MidgardConnection *
_midgard_sql_query_select_data_selector_get_connection (MidgardQuerySelector *self)
{
	g_return_val_if_fail (self != NULL, NULL);
	/* Get connection marked as transfer full. Return new reference to a connection object */
	return g_object_ref (MIDGARD_QUERY_EXECUTOR(self)->priv->mgd);
}

const gchar *
_midgard_sql_query_select_data_selector_get_query_string (MidgardQuerySelector *selector)
{
	MidgardSqlQuerySelectData *self = MIDGARD_SQL_QUERY_SELECT_DATA (selector);
	g_return_val_if_fail (selector != NULL, NULL);
	return self->query_string;
}

/* GOBJECT ROUTINES */

enum {
	PROPERTY_CONNECTION = 1,
	PROPERTY_STORAGE
};

static GObjectClass *parent_class= NULL;

static GObject *
_midgard_sql_query_select_data_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return G_OBJECT(object);
}

static void
_midgard_sql_query_select_data_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardSqlQuerySelectData *self = MIDGARD_SQL_QUERY_SELECT_DATA (instance);
	self->columns = NULL;
	self->query_string = NULL;
}

static void
_midgard_sql_query_select_data_dispose (GObject *object)
{	
	parent_class->dispose (object);

	MidgardSqlQuerySelectData *self = MIDGARD_SQL_QUERY_SELECT_DATA (object);
	if (self->columns != NULL) {
		GSList *l = NULL;
		for (l = self->columns; l != NULL; l = l->next) {
			g_object_ref (G_OBJECT (l->data));
		}
	}
	self->columns = NULL;
}

static void 
_midgard_sql_query_select_data_finalize (GObject *object)
{
	MidgardSqlQuerySelectData *self = MIDGARD_SQL_QUERY_SELECT_DATA (object);

	g_free (self->query_string);

	/* DO NOT nullify resultset object, other objects might still hold 
	 * reference to it. */
	if (MIDGARD_QUERY_EXECUTOR (self)->priv->resultset && G_IS_OBJECT (MIDGARD_QUERY_EXECUTOR (self)->priv->resultset))
		g_object_unref (MIDGARD_QUERY_EXECUTOR (self)->priv->resultset);

	/* Drop the reference to QueryProperty object. Orders are freed in parent class */
	GSList *l;
	for (l = MIDGARD_QUERY_EXECUTOR (self)->priv->orders; l != NULL; l = l->next) {
		qso *_so = (qso*) l->data;
		g_free (_so->order_type);
		g_object_unref (_so->property);
	}

	parent_class->finalize (object);
}

static void
__midgard_sql_query_select_data_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardSqlQuerySelectData *self = (MidgardSqlQuerySelectData *) object;

	switch (property_id) {
		
		case PROPERTY_CONNECTION:
			/* write and constructor only */
			break;

		case PROPERTY_STORAGE:
			g_value_set_object (value, g_object_ref (MIDGARD_QUERY_EXECUTOR (self)->priv->storage));	
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
__midgard_sql_query_select_data_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardSqlQuerySelectData *self = (MidgardSqlQuerySelectData *) (object);
	GObject *mgd;
	GObject *storage;

	switch (property_id) {

		case PROPERTY_CONNECTION:
			if (!G_VALUE_HOLDS_OBJECT (value))
				return;
			MIDGARD_QUERY_EXECUTOR (self)->priv->mgd = g_value_dup_object (value);
			/* In case of SQLite provider, use integer instead of boolean type,
			 * which is not supported in SQLite */ 
			MidgardConfig *config = MIDGARD_QUERY_EXECUTOR (self)->priv->mgd->priv->config;
			if (config->priv->dbtype == MIDGARD_DB_TYPE_SQLITE)
				MIDGARD_QUERY_EXECUTOR (self)->priv->bool_is_int = TRUE;	
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void 
_midgard_sql_query_select_data_class_init (MidgardSqlQuerySelectDataClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);
	MidgardQueryExecutorClass *executor_class = MIDGARD_QUERY_EXECUTOR_CLASS (klass);	

	object_class->constructor = _midgard_sql_query_select_data_constructor;
	object_class->dispose = _midgard_sql_query_select_data_dispose;
	object_class->finalize = _midgard_sql_query_select_data_finalize;

	executor_class->set_constraint = _midgard_sql_query_select_data_set_constraint;
	executor_class->set_limit = _midgard_sql_query_select_data_set_limit;
	executor_class->set_offset = _midgard_sql_query_select_data_set_offset;
	executor_class->add_order = _midgard_sql_query_select_data_add_order;
	executor_class->add_join = _midgard_sql_query_select_data_add_join;
	executor_class->get_results_count = _midgard_sql_query_select_data_get_results_count;

	object_class->set_property = __midgard_sql_query_select_data_set_property;
	object_class->get_property = __midgard_sql_query_select_data_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_object ("connection",
			"", "", 
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardSqlQuerySelectData:connection:
	 * 
	 * Pointer to a connection, #MidgardSqlQuerySelectData has been initialized for
	 * 
	 */  
	g_object_class_install_property (object_class, PROPERTY_CONNECTION, pspec);
}

/* Executable iface */
static void
_midgard_sql_query_select_data_executable_iface_init (MidgardExecutableIFace *iface)
{
	iface->execute = _midgard_sql_query_select_data_executable_iface_execute;
}

/* Validable iface */
static void
_midgard_sql_query_select_data_validable_iface_init (MidgardValidableIFace *iface)
{
	iface->is_valid = _midgard_sql_query_select_data_validable_iface_is_valid;
	iface->validate = _midgard_sql_query_select_data_validable_iface_validate;
}

/* Selector iface */
static void
_midgard_sql_query_select_data_selector_iface_init (MidgardQuerySelectorIFace *iface)
{
	iface->get_query_result = _midgard_sql_query_select_data_selector_get_query_result;
	iface->get_connection = _midgard_sql_query_select_data_selector_get_connection;
	iface->get_query_string = _midgard_sql_query_select_data_selector_get_query_string;
}

GType
midgard_sql_query_select_data_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardSqlQuerySelectDataClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_query_select_data_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlQuerySelectData),
			0,              /* n_preallocs */
			_midgard_sql_query_select_data_instance_init /* instance_init */
		};

		static const GInterfaceInfo executable_info = {
			(GInterfaceInitFunc) _midgard_sql_query_select_data_executable_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo validable_info = {
			(GInterfaceInitFunc) _midgard_sql_query_select_data_validable_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo selector_info = {
			(GInterfaceInitFunc) _midgard_sql_query_select_data_selector_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		type = g_type_register_static (MIDGARD_TYPE_QUERY_EXECUTOR, "MidgardSqlQuerySelectData", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_EXECUTABLE, &executable_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_VALIDABLE, &validable_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_SELECTOR, &selector_info);
	}
	return type;
}
