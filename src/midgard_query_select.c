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

#include "midgard_query_select.h"
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include "midgard_query_holder.h"

MidgardQuerySelect *
midgard_query_select_new (MidgardConnection *mgd, MidgardQueryStorage *storage)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (storage != NULL, NULL);

	MidgardQuerySelect *self = g_object_new (MIDGARD_TYPE_QUERY_SELECT, NULL);
	self->priv->mgd = mgd;
	self->priv->storage = storage;

	return self;
}

gboolean
_midgard_query_select_set_constraint (MidgardQuerySelect *self, MidgardQuerySimpleConstraint *constraint)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (constraint != NULL, FALSE);
	
	self->priv->constraint = constraint;

	return TRUE;
}

gboolean
_midgard_query_select_set_limit (MidgardQuerySelect *self, guint limit)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (limit > 0, FALSE);

	self->priv->limit = limit;

	return TRUE;
}

gboolean 
_midgard_query_select_set_offset (MidgardQuerySelect *self, guint offset)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (offset > 0, FALSE);

	self->priv->offset = offset;

	return TRUE;
}

typedef struct {
	MidgardQueryProperty *property;
	gboolean asc;
} qso;

gboolean
_midgard_query_select_add_order (MidgardQuerySelect *self, MidgardQueryProperty *property, const gchar *type)
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
	MidgardQueryProperty *left_property;
	MidgardQueryProperty *right_property;	
	GdaSqlSelectJoinType join_type;
} qsj;

gboolean
_midgard_query_select_add_join (MidgardQuerySelect *self, const gchar *join_type, 
		MidgardQueryProperty *left_property, MidgardQueryProperty *right_property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (join_type != NULL, FALSE);
	g_return_val_if_fail (left_property != NULL, FALSE);
	g_return_val_if_fail (right_property != NULL, FALSE);

	/* validate join type */
	GdaSqlSelectJoinType join_type_id;
	if (!__query_join_type_is_valid (join_type, &join_type_id)) 
		return FALSE;

	MidgardQueryStorage *left_storage = left_property->storage;
	MidgardQueryStorage *right_storage = right_property->storage;

	/* We can not join the same table adding new implicit table alias */
	if (!right_storage) {
		g_warning ("Can not add join. Right property storage is NULL. ");
	       return FALSE;	
	}
	
	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (self);

	qsj *_sj = g_new (qsj, 1);
	_sj->left_property = left_property;
	_sj->right_property = right_property;

	_sj->join_type = join_type_id;

	executor->priv->joins = g_slist_append (executor->priv->joins, _sj);

	return TRUE;
}

gboolean __query_select_add_orders (MidgardQuerySelect *self)
{
	if (!self->priv->orders)
		return TRUE;

	GSList *l = NULL;

	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (self);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;	
	GdaSqlSelectOrder *order; 
	
	for (l = self->priv->orders; l != NULL; l = l->next) {

		qso *_so = (qso*) l->data;

		/* Create new order */
		order = gda_sql_select_order_new (GDA_SQL_ANY_PART (select));
		order->asc = _so->asc;
		MidgardQueryProperty *property = _so->property;
		MidgardQueryStorage *storage = property->storage;

		/* Compute table.colname for given property name */
		GValue rval = {0, };
		midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (property), &rval);
		gchar *table_field = midgard_core_query_compute_constraint_property (executor, storage, g_value_get_string (&rval));
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

void 
_midgard_core_query_select_add_deleted_condition (GdaConnection *cnc, MidgardDBObjectClass *klass, GdaSqlStatement *stmt)
{
	const gchar *table = midgard_core_class_get_table (klass);

	GdaSqlStatementSelect *select = stmt->contents;
	GdaSqlExpr *where, *expr;
	GdaSqlOperation *cond;
	GValue *value;
	where = gda_sql_expr_new (GDA_SQL_ANY_PART (select));
	cond = gda_sql_operation_new (GDA_SQL_ANY_PART (where));
	where->cond = cond;
	cond->operator_type = GDA_SQL_OPERATOR_TYPE_EQ;
	expr = gda_sql_expr_new (GDA_SQL_ANY_PART (cond));
	g_value_take_string ((value = gda_value_new (G_TYPE_STRING)), g_strdup ("metadata_deleted"));
	expr->value = value;
	cond->operands = g_slist_append (NULL, expr);
	gchar *str;
	str = g_strdup_printf ("%s", "0");
	expr = gda_sql_expr_new (GDA_SQL_ANY_PART (cond));
	g_value_take_string ((value = gda_value_new (G_TYPE_STRING)), str);
	expr->value = value;
	cond->operands = g_slist_append (cond->operands, expr);	
	gda_sql_statement_select_take_where_cond (stmt, where);

	GError *error = NULL;
	if (gda_sql_statement_check_structure (stmt, &error) == FALSE) {
		g_warning (_("Can't build SELECT statement: %s)"),
				error && error->message ? error->message : _("No detail"));
		if (error)
			g_error_free (error);
		return;
	}
}

gboolean __query_select_add_joins (MidgardQuerySelect *self)
{
	if (!self->priv->joins)
		return TRUE;

	GSList *l = NULL;

	MidgardQueryExecutor *executor = MIDGARD_QUERY_EXECUTOR (self);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;	
	GdaSqlSelectFrom *from = select->from;
	GdaSqlSelectJoin *join; 
	
	for (l = self->priv->joins; l != NULL; l = l->next) {

		qsj *_sj = (qsj*) l->data;

		join = gda_sql_select_join_new (GDA_SQL_ANY_PART (from));
		join->type = _sj->join_type;

		MidgardQueryStorage *left_storage = _sj->left_property->storage;
		MidgardQueryStorage *right_storage = _sj->right_property->storage;

		GValue lval = {0, };
		midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (_sj->left_property), &lval);
		gchar *left_table_field = 
			midgard_core_query_compute_constraint_property (executor, left_storage, g_value_get_string (&lval));

		GValue rval = {0, };
		midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (_sj->right_property), &rval);
		gchar *right_table_field = 
			midgard_core_query_compute_constraint_property (executor, right_storage, g_value_get_string (&rval));

		GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (join));
		expr->value = gda_value_new (G_TYPE_STRING);
		g_value_take_string (expr->value, g_strdup_printf ("%s = %s", left_table_field, right_table_field));

		join->expr = expr;
		join->position = ++executor->priv->joinid;

		/* Add right storage to targets */
		MQE_SET_TABLE_ALIAS (executor, right_storage->table_alias);
		gda_sql_select_from_take_new_join (from , join);
		GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (from));
		s_target->table_name = g_strdup (right_storage->table);
		s_target->as = g_strdup (right_storage->table_alias);
		gda_sql_select_from_take_new_target (from, s_target);
	
		// Set target expression 
		GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
		GValue *tval = g_new0 (GValue, 1);
		g_value_init (tval, G_TYPE_STRING);
		g_value_set_string (tval, right_storage->table);
		texpr->value = tval;
		s_target->expr = texpr;
	}
}

gboolean 
_midgard_query_select_execute (MidgardQuerySelect *self)
{
	g_return_val_if_fail (self != NULL, FALSE);

	if (!self->priv->storage) {
		/* FIXME, handle error */
		g_warning ("Missed QueryStorage associated with QuerySelect");
		return FALSE;
	}
	
	MidgardDBObjectClass *klass = self->priv->storage->klass;
	if (!klass->dbpriv->add_fields_to_select_statement) {
		/* FIXME, handle error */
		g_warning ("Missed private DBObjectClass' fields to statement helper");
		return FALSE;
	}

	g_object_ref (self);

	GdaConnection *cnc = self->priv->mgd->priv->connection;
	GdaSqlStatement *sql_stm;
	GdaSqlStatementSelect *sss;

	sql_stm = gda_sql_statement_new (GDA_SQL_STATEMENT_SELECT);
	sss = (GdaSqlStatementSelect*) sql_stm->contents;
	g_assert (GDA_SQL_ANY_PART (sss)->type == GDA_SQL_ANY_STMT_SELECT);
	self->priv->stmt = sql_stm;
	sss->from = gda_sql_select_from_new (GDA_SQL_ANY_PART (sss));

	/* Create targets (FROM) */
	GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (sss->from));
	s_target->table_name = g_strdup (midgard_core_class_get_table (klass));
	s_target->as = g_strdup_printf ("t%d", ++self->priv->tableid);
	self->priv->table_alias = g_strdup (s_target->as);
	gda_sql_select_from_take_new_target (sss->from, s_target);

	/* Set target expression */	
	GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
	GValue *tval = g_new0 (GValue, 1);
	g_value_init (tval, G_TYPE_STRING);
	g_value_set_string (tval, s_target->table_name);
	texpr->value = tval;
	s_target->expr = texpr;

	/* Add fields for all properties registered per class (SELECT a,b,c...) */
	klass->dbpriv->add_fields_to_select_statement (klass, sss, s_target->as);

	//_midgard_core_query_select_add_deleted_condition (cnc, klass, sql_stm);

	/* Add joins */
	if (!__query_select_add_joins (self)) 
		goto return_false;

	/* Add constraints' conditions (WHERE a=1, b=2...) */
	if (self->priv->constraint)
		MIDGARD_QUERY_SIMPLE_CONSTRAINT_GET_INTERFACE (self->priv->constraint)->priv->add_conditions_to_statement (
				MIDGARD_QUERY_EXECUTOR (self), self->priv->constraint, sql_stm, NULL);

	GError *error = NULL;
	if (!gda_sql_statement_check_structure (sql_stm, &error)) {
		g_warning (_("Can't build SELECT statement: %s)"),
				error && error->message ? error->message : _("Unknown reason"));
		if (error)
			g_error_free (error);
		goto return_false;
	}

	/* Add orders */
	if (!__query_select_add_orders (self)) 
		goto return_false;

	/* Add limit */
	if (self->priv->limit > 0) {
		GdaSqlExpr *limit_expr = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
		GValue *limit_val = g_new0 (GValue, 1);
		g_value_init (limit_val, G_TYPE_STRING);
		g_value_take_string (limit_val, g_strdup_printf ("%d", self->priv->limit));
		limit_expr->value = limit_val;
		sss->limit_count = limit_expr;
	}

	/* Add offset */
	if (self->priv->offset >= 0) {
		GdaSqlExpr *offset_expr = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
		GValue *offset_val = g_new0 (GValue, 1);
		g_value_init (offset_val, G_TYPE_STRING);
		g_value_take_string (offset_val, g_strdup_printf ("%d", self->priv->offset));
		offset_expr->value = offset_val;
		sss->limit_offset = offset_expr;
	}

	/* Create statement */
	GdaStatement *stmt = gda_statement_new ();	
	g_object_set (G_OBJECT (stmt), "structure", sql_stm, NULL);
	gda_sql_statement_free (sql_stm);
	sql_stm = NULL;

	gchar *debug_sql = gda_connection_statement_to_sql (cnc, stmt, NULL, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
	g_print ("%s", debug_sql);
	g_free (debug_sql);

	/* execute statement */
	GdaDataModel *model = gda_connection_statement_execute_select (cnc, stmt, NULL, &error);

	if (!model && !error)
		goto return_false;

	if (error) {
		/* FIXME, set domain error code */
		g_warning ("Execute error - %s", error->message);
		g_error_free (error);
		goto return_false;
	}
	
	self->priv->results_count = gda_data_model_get_n_rows (model);
	if (self->priv->resultset && G_IS_OBJECT (self->priv->resultset))
		g_object_unref (G_OBJECT (self->priv->resultset));
	self->priv->resultset = (gpointer) model;
	g_object_unref (self);
	
	return TRUE;

return_false:
	if (sql_stm)
		gda_sql_statement_free (sql_stm);
	g_object_unref (self);

	return FALSE;
}

guint
_midgard_query_select_get_results_count (MidgardQuerySelect *self)
{
	g_return_val_if_fail (self != NULL, 0);

	return self->priv->results_count;
}

MidgardDBObject **
_midgard_query_select_list_objects (MidgardQuerySelect *self, guint *n_objects)
{
	g_return_val_if_fail (self != NULL, NULL);
	
	GdaDataModel *model = GDA_DATA_MODEL (MIDGARD_QUERY_EXECUTOR (self)->priv->resultset);
	if (!model || (model && !GDA_IS_DATA_MODEL (model)))
		return NULL;

	guint i;
	guint rows = gda_data_model_get_n_rows (model);
	if (rows < 1)
		return NULL;


	MidgardConnection *mgd = self->priv->mgd;
	MidgardDBObjectClass *klass = MIDGARD_QUERY_EXECUTOR (self)->priv->storage->klass;
	MidgardDBObject **objects = g_new (MidgardDBObject *, rows+1);

	for (i = 0; i < rows; i++) {
		objects[i] = g_object_new (G_OBJECT_CLASS_TYPE (klass), NULL);
		gint col_idx = gda_data_model_get_column_index (model, "guid");
		const GValue *gval = gda_data_model_get_value_at (model, col_idx, i, NULL);
		/* Set MidgardDBObject data */
		MGD_OBJECT_GUID (objects[i]) = g_value_dup_string (gval);
		MIDGARD_DBOBJECT(objects[i])->dbpriv->mgd = mgd;
		MIDGARD_DBOBJECT(objects[i])->dbpriv->datamodel = model;
		MIDGARD_DBOBJECT(objects[i])->dbpriv->row = i;
		g_object_ref (model);
	}

	objects[i] = NULL;
	*n_objects = rows;

	return objects;
}

MidgardDBObject **
midgard_query_select_list_objects (MidgardQuerySelect *self, guint *n_objects)
{
	return MIDGARD_QUERY_SELECT_GET_CLASS (self)->list_objects (self, n_objects);
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class= NULL;

static GObject *
_midgard_query_select_constructor (GType type,
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
_midgard_query_select_dispose (GObject *object)
{
	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (object);
	parent_class->dispose (object);
}

static void 
_midgard_query_select_finalize (GObject *object)
{
	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (object);

	/* DO NOT nullify resultset object, other objects might still hold 
	 * reference to it. */
	if (self->priv->resultset && G_IS_OBJECT (self->priv->resultset))
		g_object_unref (self->priv->resultset);

	parent_class->finalize;
}

static void 
_midgard_query_select_class_init (MidgardQuerySelectClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_query_select_constructor;
	object_class->dispose = _midgard_query_select_dispose;
	object_class->finalize = _midgard_query_select_finalize;

	klass->set_constraint = _midgard_query_select_set_constraint;
	klass->set_limit = _midgard_query_select_set_limit;
	klass->set_offset = _midgard_query_select_set_offset;
	klass->add_order = _midgard_query_select_add_order;
	klass->add_join = _midgard_query_select_add_join;
	klass->execute = _midgard_query_select_execute;
	klass->get_results_count = _midgard_query_select_get_results_count;
	klass->list_objects = _midgard_query_select_list_objects;
}

GType
midgard_query_select_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardQuerySelectClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_query_select_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardQuerySelect),
			0,              /* n_preallocs */
			NULL /* instance_init */
		};
		type = g_type_register_static (MIDGARD_TYPE_QUERY_EXECUTOR, "MidgardQuerySelect", &info, 0);
	}
	return type;
}
