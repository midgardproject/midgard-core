/* 
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

#include "midgard_cr_core_query_select.h"
#include "midgard_cr_core_query_private.h"
#include "midgard_cr_core_query_holder.h"
#include "midgard_cr_core_query_constraint_group.h"
#include <sql-parser/gda-sql-parser.h>
#include "midgardcr.h"
#include "midgard_cr_core_sqlstoragemanager.h"

/**
 * midgard_cr_core_query_select_new:
 * @mgd: #MidgardCRCoreConnection instance
 * @storage: #MidgardCRCoreStorage instance
 *
 * #MidgardCRCoreStorage @storage represents storage which is queried during execution
 *
 * Returns: new #MidgardCRCoreQuerySelect instance or %NULL on failure
 * Since: 10.05
 */ 
MidgardCRCoreQuerySelect *
midgard_cr_core_query_select_new (MidgardCRSQLStorageManager *manager, MidgardCRCoreQueryStorage *storage)
{
	g_return_val_if_fail (manager != NULL, NULL);
	g_return_val_if_fail (storage != NULL, NULL);

	MidgardCRCoreQuerySelect *self = g_object_new (MIDGARD_CR_CORE_TYPE_QUERY_SELECT, 
		"manager", MIDGARD_CR_STORAGE_MANAGER (manager), "storage", storage, NULL);

	return self;
}

MidgardCRCoreQuerySelect *
midgard_cr_core_query_select_create_static (MidgardCRSQLStorageManager *manager, MidgardCRCoreQueryStorage *storage)
{
	return midgard_cr_core_query_select_new (manager, storage);
}

gboolean
_midgard_cr_core_query_select_set_constraint (MidgardCRCoreQueryExecutor *self, MidgardCRCoreQueryConstraintSimple *constraint)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (constraint != NULL, FALSE);

	if (self->priv->constraint)
		g_object_unref (self->priv->constraint);	

	self->priv->constraint = g_object_ref (constraint);

	return TRUE;
}

gboolean
_midgard_cr_core_query_select_set_limit (MidgardCRCoreQueryExecutor *self, guint limit)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (limit > 0, FALSE);

	self->priv->limit = limit;

	return TRUE;
}

gboolean 
_midgard_cr_core_query_select_set_offset (MidgardCRCoreQueryExecutor *self, guint offset)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (offset > 0, FALSE);

	self->priv->offset = offset;

	return TRUE;
}

typedef struct {
	MidgardCRCoreQueryProperty *property;
	gboolean asc;
} qso;

gboolean
_midgard_cr_core_query_select_add_order (MidgardCRCoreQueryExecutor *self, MidgardCRCoreQueryProperty *property, const gchar *type)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);

	gboolean asc = FALSE;

	gchar *lorder = g_ascii_strdown (type, -1);
	if (g_str_equal (lorder, "asc")) {
		asc = TRUE;
	} else if (g_str_equal (lorder, "desc")) {
		asc = FALSE;
	} else {
		g_warning ("Invalid order type '%s'. Expected ASC or DESC", type);
	}

	g_free (lorder);

	qso *_qs = g_new (qso, 1);
	_qs->asc = asc;
	_qs->property = property;

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

gboolean 
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
	MidgardCRCoreQueryProperty *left_property;
	MidgardCRCoreQueryProperty *right_property;	
	GdaSqlSelectJoinType join_type;
} qsj;

gboolean
_midgard_cr_core_query_select_add_join (MidgardCRCoreQueryExecutor *self, const gchar *join_type, 
		MidgardCRCoreQueryProperty *left_property, MidgardCRCoreQueryProperty *right_property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (join_type != NULL, FALSE);
	g_return_val_if_fail (left_property != NULL, FALSE);
	g_return_val_if_fail (right_property != NULL, FALSE);

	/* validate join type */
	GdaSqlSelectJoinType join_type_id;
	if (!__query_join_type_is_valid (join_type, &join_type_id)) 
		return FALSE;

	/* MidgardCRCoreQueryStorage *left_storage = left_property->priv->storage; */
	MidgardCRCoreQueryStorage *right_storage = right_property->priv->storage;

	/* We can not join the same table adding new implicit table alias */
	if (!right_storage) {
		g_warning ("Can not add join. Right property storage is NULL. ");
	       return FALSE;	
	}

	qsj *_sj = g_new (qsj, 1);
	_sj->left_property = left_property;
	_sj->right_property = right_property;

	_sj->join_type = join_type_id;

	self->priv->joins = g_slist_append (self->priv->joins, _sj);

	return TRUE;
}

gboolean 
__query_select_add_orders (MidgardCRCoreQueryExecutor *self)
{
	GSList *l = NULL;

	MidgardCRCoreQueryExecutor *executor = MIDGARD_CR_CORE_QUERY_EXECUTOR (self);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;	
	MidgardCRSQLStorageManager *mgd = self->priv->storage_manager;
	GdaSqlSelectOrder *order; 
	GValue *value;
	GdaSqlExpr *expr;

	if (!self->priv->orders)
		return TRUE;

	for (l = MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->orders; l != NULL; l = l->next) {

		qso *_so = (qso*) l->data;

		/* Create new order */
		order = gda_sql_select_order_new (GDA_SQL_ANY_PART (select));
		order->asc = _so->asc;
		MidgardCRCoreQueryProperty *property = _so->property;
		MidgardCRCoreQueryStorage *storage = property->priv->storage;

		/* Compute table.colname for given property name */
		GValue rval = {0, };
		midgard_cr_core_query_holder_get_value (MIDGARD_CR_CORE_QUERY_HOLDER (property), &rval);
		gchar *table_field = midgard_cr_core_query_compute_constraint_property (executor, storage, g_value_get_string (&rval));
		g_value_unset (&rval);

		value = g_new0 (GValue, 1);
		g_value_init (value, G_TYPE_STRING);
		g_value_take_string (value, table_field);

		/* Set order's expression and add new one to statement orders list */
		expr = gda_sql_expr_new (GDA_SQL_ANY_PART (order));
		expr->value = value;
		order->expr = expr;
		select->order_by = g_slist_append (select->order_by, order);
	}

	return TRUE;
}

gboolean 
__query_select_add_joins (MidgardCRCoreQuerySelect *self, GdaSqlOperation *operation)
{
	if (!MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->joins)
		return TRUE;

	GSList *l = NULL;

	MidgardCRCoreQueryExecutor *executor = MIDGARD_CR_CORE_QUERY_EXECUTOR (self);
	MidgardCRSQLStorageManager *manager = MIDGARD_CR_SQL_STORAGE_MANAGER (executor->priv->storage_manager);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;	
	GdaSqlSelectFrom *from = select->from;
	GdaSqlSelectJoin *join; 
	
	for (l = MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->joins; l != NULL; l = l->next) {

		qsj *_sj = (qsj*) l->data;

		join = gda_sql_select_join_new (GDA_SQL_ANY_PART (from));
		join->type = _sj->join_type;

		MidgardCRCoreQueryStorage *left_storage = _sj->left_property->priv->storage;
		MidgardCRCoreQueryStorage *right_storage = _sj->right_property->priv->storage;

		GValue lval = {0, };
		midgard_cr_core_query_holder_get_value (MIDGARD_CR_CORE_QUERY_HOLDER (_sj->left_property), &lval);
		gchar *left_table_field = 
			midgard_cr_core_query_compute_constraint_property (executor, left_storage, g_value_get_string (&lval));

		GValue rval = {0, };
		midgard_cr_core_query_holder_get_value (MIDGARD_CR_CORE_QUERY_HOLDER (_sj->right_property), &rval);
		gchar *right_table_field = 
			midgard_cr_core_query_compute_constraint_property (executor, right_storage, g_value_get_string (&rval));

		GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (join));
		expr->value = gda_value_new (G_TYPE_STRING);
		g_value_take_string (expr->value, g_strdup_printf ("%s = %s", left_table_field, right_table_field));

		join->expr = expr;
		join->position = ++executor->priv->joinid;

		/* Add right storage to targets */
		MQE_SET_TABLE_ALIAS (executor, right_storage);
		gda_sql_select_from_take_new_join (from , join);
		GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (from));
		s_target->table_name = g_strdup (MQE_GET_STORAGE_TABLE (manager, right_storage));
		s_target->as = g_strdup (right_storage->priv->table_alias);
		gda_sql_select_from_take_new_target (from, s_target);
	
		// Set target expression 
		GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
		GValue *tval = g_new0 (GValue, 1);
		g_value_init (tval, G_TYPE_STRING);
		g_value_set_string (tval, s_target->table_name);
		texpr->value = tval;
		s_target->expr = texpr;
	}

	return TRUE;
}

static gboolean
__include_deleted_from_target (MidgardCRCoreQueryExecutor *executor, GdaSqlSelectTarget *target)
{
	GSList *l = NULL;

	for (l = executor->priv->include_deleted_targets; l != NULL; l = l->next) {
		GdaSqlSelectTarget *t = (GdaSqlSelectTarget *) l->data;
		if (target == t)
			return TRUE;
	}

	return FALSE;
}

static void 
__add_exclude_deleted_constraints (MidgardCRCoreQueryExecutor *executor, GdaSqlStatementSelect *select, GdaSqlOperation *operation)
{
	GSList *l = select->from->targets;
	guint list_length = g_slist_length (l);

	guint i = 0;
	GdaSqlSelectTarget *target = NULL;
	/* Compute if there's a need to create new constraint group */
	for (l = select->from->targets; l != NULL; l = l->next) {
		target = (GdaSqlSelectTarget *) l->data;
		if (__include_deleted_from_target (executor, target))
			i++;
	}

	GdaSqlOperation *_op = operation;
	if (list_length > 1 && ((list_length - 1 > 1))) {
		/* Create new constraint group, (t1.deleted AND t2.deleted AND ...) */
		GdaSqlExpr *deleted_expr = gda_sql_expr_new (GDA_SQL_ANY_PART (operation));
		GdaSqlOperation *deleted_operation = gda_sql_operation_new (GDA_SQL_ANY_PART (deleted_expr));
		deleted_operation->operator_type = GDA_SQL_OPERATOR_TYPE_AND;
		deleted_expr->cond = deleted_operation;
		operation->operands = g_slist_append (operation->operands, deleted_expr);
		_op = deleted_operation;
	}

	/* Add metadata_deleted constraint for each statement's table */
	for (l = select->from->targets; l != NULL; l = l->next) {
		target = (GdaSqlSelectTarget *) l->data;
		if (__include_deleted_from_target (executor, target))
			continue;
		GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (operation));
        	expr->value = gda_value_new (G_TYPE_STRING);
        	g_value_take_string (expr->value, g_strdup_printf ("%s.metadata_deleted = 0", target->as));
        	_op->operands = g_slist_append (_op->operands, expr);
	}
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
__add_fields_to_select_statement (GdaConnection *cnc, GdaSqlStatementSelect *select, const gchar *table_name, 
		MidgardCRStorageModel *storage_model, guint *col_id, GError **error)
{
	guint n_models;
        guint i;
        GdaSqlSelectField *select_field;
        GdaSqlExpr *expr;
        GValue *val;
        gchar *table_field;
	GError *err = NULL;
	gchar *q_property;
	gchar *q_table;
	gchar *q_field;

	MidgardCRModel **models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (storage_model), &n_models); 
	if (!models) {
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL, 
				"'%s' sql table model doesn't contain any column model.", 
				midgard_cr_model_get_name (MIDGARD_CR_MODEL (storage_model)));
		return;
	}
	
	const gchar *property_table = table_name;
	for (i = 0; i < n_models; i++) {
		/* Storage model of object type, select fields from this model */
		if (midgard_cr_model_property_get_valuegtype (MIDGARD_CR_MODEL_PROPERTY (models[i])) == G_TYPE_OBJECT) {
			__add_fields_to_select_statement (cnc, select, table_name, MIDGARD_CR_STORAGE_MODEL (models[i]), col_id, &err);
			if (err) {
				g_propagate_error (error, err);
				return;
			}
			continue;
		}
		q_field = gda_connection_quote_sql_identifier (cnc, 
				midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (models[i])));
		q_table = gda_connection_quote_sql_identifier (cnc, table_name);
		select_field = gda_sql_select_field_new (GDA_SQL_ANY_PART (select));
		select_field->as = gda_connection_quote_sql_identifier (cnc, midgard_cr_model_get_name (models[i]));
		select->expr_list = g_slist_append (select->expr_list, select_field);
		expr = gda_sql_expr_new (GDA_SQL_ANY_PART (select_field));
		val = g_new0 (GValue, 1);
		g_value_init (val, G_TYPE_STRING);
		table_field = g_strconcat (q_table, ".", q_field, NULL);
		g_value_take_string (val, table_field);
		expr->value = val;
		select_field->expr = expr;
		MIDGARD_CR_SQL_COLUMN_MODEL (models[i])->_col_id = *col_id;
		g_free (q_field);
		g_free (q_table);
		(*col_id)++;
	}

	return;
}

gboolean 
_midgard_cr_core_query_select_execute (MidgardCRCoreQueryExecutor *self, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	if (!MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage) {
		/* FIXME, handle error */
		g_warning ("Missed QueryStorage associated with QuerySelect");
		return FALSE;
	}

	GError *err = NULL;
	GObjectClass *klass = g_type_class_peek (g_type_from_name (self->priv->storage->priv->classname));
	/* if (!klass->dbpriv->add_fields_to_select_statement) {
		// FIXME, handle error 
		g_warning ("Missed private DBObjectClass' fields to statement helper");
		return FALSE;
	} */

	g_object_ref (self);

	MidgardCRSQLStorageManager *mgd = (MidgardCRSQLStorageManager *) self->priv->storage_manager;
	g_return_val_if_fail (mgd != NULL, FALSE);
	MidgardCRSQLModelManager *model_manager = 
		(MidgardCRSQLModelManager *) midgard_cr_storage_manager_get_model_manager (MIDGARD_CR_STORAGE_MANAGER (mgd));
	MidgardCRSQLTableModel *table_model = 
		midgard_cr_sql_model_manager_get_table_model_by_name (model_manager, G_OBJECT_CLASS_NAME (G_OBJECT_CLASS (klass)));
	GdaConnection *cnc = (GdaConnection *) mgd->_cnc;
	GdaSqlParser *parser = (GdaSqlParser *) mgd->_parser;
	GdaSqlStatement *sql_stm;
	GdaSqlStatementSelect *sss;

	sql_stm = gda_sql_statement_new (GDA_SQL_STATEMENT_SELECT);
	sss = (GdaSqlStatementSelect*) sql_stm->contents;
	g_assert (GDA_SQL_ANY_PART (sss)->type == GDA_SQL_ANY_STMT_SELECT);
	MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->stmt = sql_stm;
	sss->from = gda_sql_select_from_new (GDA_SQL_ANY_PART (sss));

	/* Initialize top base expresion and operation with default AND operator type */
	GdaSqlExpr *base_where = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
	GdaSqlOperation *base_operation = gda_sql_operation_new (GDA_SQL_ANY_PART (base_where));
	base_operation->operator_type = GDA_SQL_OPERATOR_TYPE_AND;
	base_where->cond = base_operation;
	gda_sql_statement_select_take_where_cond (sql_stm, base_where);

	/* Create targets (FROM) */
	GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (sss->from));	
	s_target->table_name = g_strdup (midgard_cr_storage_model_get_location ((MIDGARD_CR_STORAGE_MODEL (table_model))));
	s_target->as = g_strdup_printf ("t%d", ++MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->tableid);
	MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->table_alias = g_strdup (s_target->as);
	gda_sql_select_from_take_new_target (sss->from, s_target);

	/* Set target expression */	
	GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
	GValue *tval = g_new0 (GValue, 1);
	g_value_init (tval, G_TYPE_STRING);
	g_value_set_string (tval, s_target->table_name);
	texpr->value = tval;
	s_target->expr = texpr;

	/* Add fields for all properties registered per class (SELECT a,b,c...) */
	guint _col_id = 0;
	__add_fields_to_select_statement (cnc, sss, s_target->as, MIDGARD_CR_STORAGE_MODEL (table_model), &_col_id, &err);
	if (err)
		goto return_false;


	GdaSqlExpr *where = sss->where_cond;
	GdaSqlOperation *operation = where->cond;

	/* Add joins, LEFT JOIN tbl2 ON... */
	if (!__query_select_add_joins (MIDGARD_CR_CORE_QUERY_SELECT (self), operation)) 
		goto return_false;

	/* Add constraints' conditions (WHERE a=1, b=2...) */
	if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->constraint) {
		MIDGARD_CR_CORE_QUERY_CONSTRAINT_SIMPLE_GET_INTERFACE (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->constraint)->priv->add_conditions_to_statement 			(MIDGARD_CR_CORE_QUERY_EXECUTOR (self), MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->constraint, sql_stm, base_where);
		if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->n_constraints == 1) 
			__add_second_dummy_constraint (sss, operation);
		/* FIXME, this is added to avoid 'Wrong number of operands error'.
		 * This is caused by either QuerySelect implementation or by MidgardCR&MidgardCRCore decorators */ 
		if (MIDGARD_CR_CORE_IS_QUERY_CONSTRAINT_GROUP (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->constraint))
			__add_dummy_constraint (sss, operation);
	} else { 
		/* no constraints, add dummy '1=1 AND 0<1' to satisfy top constraint group */
		__add_dummy_constraint (sss, operation); 
		__add_second_dummy_constraint (sss, operation); 
	}

	/* Add orders , ORDER BY t1.field... */
	if (!__query_select_add_orders (self)) 
		goto return_false;

	/* Exclude deleted */
	/* if (MGD_DBCLASS_METADATA_CLASS (klass) && !MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->include_deleted)
		__add_exclude_deleted_constraints (self, sss, operation); */

	/* Add limit, LIMIT x */
	if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->limit > 0) {
		GdaSqlExpr *limit_expr = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
		GValue *limit_val = g_new0 (GValue, 1);
		g_value_init (limit_val, G_TYPE_STRING);
		g_value_take_string (limit_val, g_strdup_printf ("%d", MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->limit));
		limit_expr->value = limit_val;
		sss->limit_count = limit_expr;
	}

	/* Add offset, OFFSET x */
	if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->offset >= 0) {
		GdaSqlExpr *offset_expr = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
		GValue *offset_val = g_new0 (GValue, 1);
		g_value_init (offset_val, G_TYPE_STRING);
		g_value_take_string (offset_val, g_strdup_printf ("%d", MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->offset));
		offset_expr->value = offset_val;
		sss->limit_offset = offset_expr;
	}

	/* Check structure */
	if (!gda_sql_statement_check_structure (sql_stm, &err)) {
		goto return_false;
	} 

	/* Create statement */
	GdaStatement *stmt = gda_statement_new ();	
	g_object_set (G_OBJECT (stmt), "structure", sql_stm, NULL);
	gda_sql_statement_free (sql_stm);
	sql_stm = NULL;

	__STORAGE_MANAGER_UPDATE_PROFILER_WITH_STMT(mgd, stmt, NULL);

	/* execute statement */
	GdaDataModel *model = gda_connection_statement_execute_select (cnc, stmt, NULL, &err);

	if (!model && !err)
		goto return_false;

	if (err) {
		if (model) 
			g_object_unref (model);
		goto return_false;
	}
	
	MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->results_count = gda_data_model_get_n_rows (model);
	if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->resultset && G_IS_OBJECT (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->resultset))
		g_object_unref (G_OBJECT (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->resultset));
	MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->resultset = (gpointer) model;
	g_object_unref (self);
	
	return TRUE;

return_false:
	if (err) {
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL, 
				"SQL query execution failed. %s ", err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
	}
	if (sql_stm)
		gda_sql_statement_free (sql_stm);
	g_object_unref (self);

	return FALSE;
}

guint
_midgard_cr_core_query_select_get_results_count (MidgardCRCoreQueryExecutor *self)
{
	g_return_val_if_fail (self != NULL, 0);

	return self->priv->results_count;
}

static void __set_object_from_data_model (MidgardCRModel *table_model, MidgardCRModel *object_model, GObjectClass *klass, GObject *object, GdaDataModel *data_model, guint row)
{
	guint n_models;
	MidgardCRSQLColumnModel **col_models = (MidgardCRSQLColumnModel**) midgard_cr_model_list_models (MIDGARD_CR_MODEL (table_model), &n_models);
	if (col_models == NULL)
		g_error ("No single column model associated with %s table model", midgard_cr_model_get_name (MIDGARD_CR_MODEL (table_model)));
	guint i;
	GParamSpec *pspec = NULL;
	const gchar *property_name = NULL;
	for (i = 0; i < n_models; i++) {
		property_name = midgard_cr_model_get_name (MIDGARD_CR_MODEL (col_models[i]));	
		pspec = g_object_class_find_property (klass, property_name);
		/* ReferenceObject, so get all its properties */
		if (midgard_cr_model_property_get_valuegtype (MIDGARD_CR_MODEL_PROPERTY (col_models[i])) == G_TYPE_OBJECT
				&& g_type_is_a (pspec->value_type, MIDGARD_CR_TYPE_REFERENCE_OBJECT)) {
			GObject *obj = NULL;
			g_object_get (object, property_name, &obj, NULL);
			if (obj == NULL) {
				MidgardCRModelProperty *ref_model = 
					MIDGARD_CR_MODEL_PROPERTY (midgard_cr_model_get_model_by_name (object_model, property_name));
				obj = g_object_new (pspec->value_type, 
						"classname", midgard_cr_model_property_get_refname (ref_model), NULL);
				g_object_set (object, property_name, obj, NULL);
			}			
			__set_object_from_data_model (MIDGARD_CR_MODEL (col_models[i]), NULL, G_OBJECT_GET_CLASS (obj), obj, data_model, row);
			continue;
		}
		/* g_print ("COLUMN MODEL %s (%d) (DataModel: '%s') \n", 
				midgard_cr_model_get_name (MIDGARD_CR_MODEL (col_models[i])), col_models[i]->_col_id,
				gda_data_model_get_column_name (data_model, col_models[i]->_col_id)); */
		/* READ ONLY PROPERTY */
		if (!(pspec->flags & G_PARAM_WRITABLE)) {
			/* FIXME, these setters should be much more efficient, and not such ugly */
			if (g_str_equal (property_name, "id"))
				MIDGARD_CR_REPOSITORY_OBJECT (object)->_id = 
					g_value_get_int (gda_data_model_get_value_at (data_model, col_models[i]->_col_id, row, NULL));
			else if (g_str_equal (property_name, "guid")) {
				g_free (MIDGARD_CR_REPOSITORY_OBJECT (object)->_guid);
				MIDGARD_CR_REPOSITORY_OBJECT (object)->_guid = 
					g_value_dup_string (gda_data_model_get_value_at (data_model, col_models[i]->_col_id, row, NULL));
			}
		/* CONSTRUCT ONLY PROPERTY */
		} else if ((pspec->flags & G_PARAM_CONSTRUCT_ONLY)) {
			/* TODO, yep, there's something to do with this case.
			 * Though, it is only related to ReferenceObject. */
			/* g_print ("Unhandled construct only property %s \n", property_name); */
		/* WRITE&READ PROPERTY, SET */
		} else {
			/* Every column model has column id set when select has been built. Use it instead of name. */
			g_object_set_property (object, property_name, gda_data_model_get_value_at (data_model, col_models[i]->_col_id, row, NULL));
		}
	}
}

MidgardCRStorable **
_midgard_cr_core_query_select_list_objects (MidgardCRCoreQuerySelect *self, guint *n_objects)
{
	g_return_val_if_fail (self != NULL, NULL);
	
	*n_objects = 0;
	GdaDataModel *data_model = GDA_DATA_MODEL (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->resultset);
	if (!data_model || (data_model && !GDA_IS_DATA_MODEL (data_model)))
		return NULL;

	guint i;
	guint rows = gda_data_model_get_n_rows (data_model);
	if (rows < 1)
		return NULL;

	MidgardCRSQLStorageManager *manager = MIDGARD_CR_SQL_STORAGE_MANAGER (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage_manager);
	const gchar *classname = MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage->priv->classname;
	GObjectClass *klass = g_type_class_peek (g_type_from_name (classname));
	MidgardCRSQLTableModel *table_model = midgard_cr_core_query_find_table_model_by_name (manager, classname);
	MidgardCRObjectModel *object_model = 
		midgard_cr_sql_model_manager_get_object_model_by_name (
				MIDGARD_CR_SQL_MODEL_MANAGER (midgard_cr_storage_manager_get_model_manager (MIDGARD_CR_STORAGE_MANAGER (manager))), classname);
	MidgardCRStorable **objects = g_new (MidgardCRStorable *, rows+1);	

	for (i = 0; i < rows; i++) {	
		objects[i] = g_object_new (G_OBJECT_CLASS_TYPE (klass), NULL);
		__set_object_from_data_model (MIDGARD_CR_MODEL (table_model), MIDGARD_CR_MODEL (object_model), klass, G_OBJECT (objects[i]), data_model, i);
		/* MGD_OBJECT_IN_STORAGE (objects[i]) = TRUE;
		MIDGARD_CR_CORE_DBOBJECT(objects[i])->dbpriv->datamodel = model;
		MIDGARD_CR_CORE_DBOBJECT(objects[i])->dbpriv->row = i;
		g_object_ref (model);

		if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->read_only) {
			gint col_idx = gda_data_model_get_column_index (model, "guid");
			const GValue *gval = gda_data_model_get_value_at (model, col_idx, i, NULL);
			// Set MidgardCRCoreDBObject data 
			MGD_OBJECT_GUID (objects[i]) = g_value_dup_string (gval);
		} else {
			MIDGARD_CR_CORE_DBOBJECT_GET_CLASS (objects[i])->dbpriv->set_from_data_model (
					MIDGARD_CR_CORE_DBOBJECT (objects[i]), model, i, 0);
		}*/
	}

	objects[i] = NULL;
	*n_objects = rows;
	
	return objects;
}

static void
_midgard_cr_core_query_select_include_deleted (MidgardCRCoreQuerySelect *self, gboolean toggle)
{
	g_return_if_fail (self != NULL);
	MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->include_deleted = toggle;
}

/**
 * midgard_cr_core_query_select_include_deleted:
 * @self: #MidgardCRCoreQuerySelect instance
 * @toggle: toggle to include or exclude deleted objects
 *
 * By default, #MidgardCRCoreQuerySelect ignores deleted objects.
 * With this method, you can set deleted objects toggle, so such can be 
 * included in execute results. This method may be called as many times 
 * as needed, to include (@TRUE) or exclude (@FALSE) deleted objects. 
 *
 * Since: 10.05.1
 */ 
void
midgard_cr_core_query_select_include_deleted (MidgardCRCoreQuerySelect *self, gboolean toggle)
{
	g_return_if_fail (self != NULL);
	MIDGARD_CR_CORE_QUERY_SELECT_GET_CLASS (self)->include_deleted (self, toggle);
}


/*
 * midgard_cr_core_query_select_list_objects:
 * @self: (in): #MidgardCRCoreQuerySelect instance
 * @n_objects: (out): pointer to store number of returned objects
 *
 * List all objects for which data has been returned during execution.
 *
 * Returns: (element-type MidgardCRCoreDBObject) (array length=1): newly allocated array of #MidgardCRCoreDBObject
 * Since: 10.05
 */ 
MidgardCRStorable **
midgard_cr_core_query_select_list_objects (MidgardCRCoreQuerySelect *self, guint *n_objects)
{
	return MIDGARD_CR_CORE_QUERY_SELECT_GET_CLASS (self)->list_objects (self, n_objects);
}

void 
_midgard_cr_core_query_select_toggle_read_only (MidgardCRCoreQuerySelect *self, gboolean toggle)
{
	g_return_if_fail (self != NULL);
	MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->read_only = toggle;
	return;
}

/**
 * midgard_cr_core_query_select_toggle_read_only:
 * @self: #MidgardCRCoreQuerySelect instance
 * @toggle: enables or disables read only mode
 *
 * This method switch #MidgardCRCoreQuerySelect to read only mode.
 * It should be enabled when returned objects will be used only to read properties.
 * It improves performance, but it's impossible to write returned object's properties.
 *
 * Since: 10.05
 */ 
void
midgard_cr_core_query_select_toggle_read_only (MidgardCRCoreQuerySelect *self, gboolean toggle)
{
	MIDGARD_CR_CORE_QUERY_SELECT_GET_CLASS (self)->toggle_read_only (self, toggle);
}

/* GOBJECT ROUTINES */

enum {
	PROPERTY_CONNECTION = 1,
	PROPERTY_STORAGE
};

static GObjectClass *parent_class= NULL;

static GObject *
_midgard_cr_core_query_select_constructor (GType type,
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
_midgard_cr_core_query_select_dispose (GObject *object)
{	
	parent_class->dispose (object);
}

static void 
_midgard_cr_core_query_select_finalize (GObject *object)
{
	MidgardCRCoreQuerySelect *self = MIDGARD_CR_CORE_QUERY_SELECT (object);

	/* DO NOT nullify resultset object, other objects might still hold 
	 * reference to it. */
	if (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->resultset && G_IS_OBJECT (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->resultset))
		g_object_unref (MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->resultset);

	parent_class->finalize (object);
}

static void
__midgard_cr_core_query_select_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardCRCoreQuerySelect *self = (MidgardCRCoreQuerySelect *) object;

	switch (property_id) {
		
		case PROPERTY_CONNECTION:
			/* write and constructor only */
			break;

		case PROPERTY_STORAGE:
			/* write and constructor only */
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
__midgard_cr_core_query_select_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardCRCoreQuerySelect *self = (MidgardCRCoreQuerySelect *) (object);
	GObject *mgd;
	GObject *storage;

	switch (property_id) {

		case PROPERTY_CONNECTION:
			if (!G_VALUE_HOLDS_OBJECT (value))
				return;
			MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage_manager = g_value_dup_object (value);	
			break;

		case PROPERTY_STORAGE:
			if (!G_VALUE_HOLDS_OBJECT (value))
				return;
			MIDGARD_CR_CORE_QUERY_EXECUTOR (self)->priv->storage = g_value_dup_object (value);
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void 
_midgard_cr_core_query_select_class_init (MidgardCRCoreQuerySelectClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);
	MidgardCRCoreQueryExecutorClass *executor_class = MIDGARD_CR_CORE_QUERY_EXECUTOR_CLASS (klass);	

	object_class->constructor = _midgard_cr_core_query_select_constructor;
	object_class->dispose = _midgard_cr_core_query_select_dispose;
	object_class->finalize = _midgard_cr_core_query_select_finalize;

	executor_class->set_constraint = _midgard_cr_core_query_select_set_constraint;
	executor_class->set_limit = _midgard_cr_core_query_select_set_limit;
	executor_class->set_offset = _midgard_cr_core_query_select_set_offset;
	executor_class->add_order = _midgard_cr_core_query_select_add_order;
	executor_class->add_join = _midgard_cr_core_query_select_add_join;
	executor_class->execute = _midgard_cr_core_query_select_execute;
	executor_class->get_results_count = _midgard_cr_core_query_select_get_results_count;
	
	klass->list_objects = _midgard_cr_core_query_select_list_objects;
	klass->toggle_read_only = _midgard_cr_core_query_select_toggle_read_only;
	klass->include_deleted = _midgard_cr_core_query_select_include_deleted;

	object_class->set_property = __midgard_cr_core_query_select_set_property;
	object_class->get_property = __midgard_cr_core_query_select_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_object ("manager",
			"", "", 
			MIDGARD_CR_TYPE_STORAGE_MANAGER,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardCRCoreQuerySelect:connection:
	 * 
	 * Pointer for a connection, #MidgardCRCoreQuerySelect has been initialized for
	 * 
	 */  
	g_object_class_install_property (object_class, PROPERTY_CONNECTION, pspec);

	pspec = g_param_spec_object ("storage",
			"", "", 
			MIDGARD_CR_CORE_TYPE_QUERY_STORAGE,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardCRCoreQuerySelect:storage:
	 * 
	 * Pointer for a connection, #MidgardCRCoreQuerySelect has been initialized for
	 * 
	 */  
	g_object_class_install_property (object_class, PROPERTY_STORAGE, pspec);
}

GType
midgard_cr_core_query_select_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardCRCoreQuerySelectClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_cr_core_query_select_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardCRCoreQuerySelect),
			0,              /* n_preallocs */
			NULL /* instance_init */
		};
		type = g_type_register_static (MIDGARD_CR_CORE_TYPE_QUERY_EXECUTOR, "MidgardCRCoreQuerySelect", &info, 0);
	}
	return type;
}
