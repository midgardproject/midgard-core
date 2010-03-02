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

#include "midgard_query_constraint.h"
#include "midgard_query_property.h"
#include "midgard_query_storage.h"
#include "midgard_query_holder.h"
#include "midgard_query_simple_constraint.h"
#include "midgard_dbobject.h"
#include "midgard_core_query.h"
#include "midgard_core_object_class.h"
#include "midgard_core_object.h"

struct _MidgardQueryConstraint {
	GObject  parent;
	MidgardQueryProperty *property_value;
	gchar *op;
	GdaSqlOperatorType op_type;
	MidgardQueryStorage *storage;
	MidgardQueryHolder *holder;
};

static struct {
	const gchar *name;
	GdaSqlOperatorType op_type;
} valid_operators[] = {
	{ "=", GDA_SQL_OPERATOR_TYPE_EQ },
	{ ">", GDA_SQL_OPERATOR_TYPE_GT },
	{ "<", GDA_SQL_OPERATOR_TYPE_LT },
	{ "!=", GDA_SQL_OPERATOR_TYPE_DIFF },
	{ "<>", GDA_SQL_OPERATOR_TYPE_DIFF },
	{ "<=", GDA_SQL_OPERATOR_TYPE_LEQ },
	{ ">=", GDA_SQL_OPERATOR_TYPE_GEQ },
	{ "LIKE", GDA_SQL_OPERATOR_TYPE_LIKE },
	{ "NOT LIKE", GDA_SQL_OPERATOR_TYPE_GT },
	{ "IN", GDA_SQL_OPERATOR_TYPE_IN },
	{ "NOT IN", GDA_SQL_OPERATOR_TYPE_NOTIN },
	{ NULL, 0 }
};

gboolean __query_constraint_operator_is_valid(const gchar *op, GdaSqlOperatorType *op_type)
{
	guint i;
	for (i = 0; valid_operators[i].name != NULL; i++) {
		if (g_str_equal(op, valid_operators[i].name)) {
			*op_type = valid_operators[i].op_type;
			return TRUE;
		}
	}

	if (valid_operators[i].name == NULL) {
		g_warning("Invalid comparison operator %s", op);
		return FALSE;
	}

	return TRUE;
}


MidgardQueryConstraint *
midgard_query_constraint_new (MidgardQueryProperty *property, const gchar *op, 
		MidgardQueryHolder *holder, MidgardQueryStorage *storage)
{
	g_return_val_if_fail (property != NULL, NULL);
	g_return_val_if_fail (op != NULL, NULL);
	g_return_val_if_fail (holder != NULL, NULL);

	GdaSqlOperatorType op_type;
	if (!__query_constraint_operator_is_valid (op, &op_type)) 
		return NULL;	

	MidgardQueryConstraint *self = g_object_new (MIDGARD_TYPE_QUERY_CONSTRAINT, NULL);
	self->property_value = property;
	self->op = g_strdup (op);
	self->op_type = op_type;
	self->holder = holder;

	/* Allow NULL storage */
	if (storage)
		self->storage = storage;
	
	return self;
}

void
midgard_query_constraint_get_value (MidgardQueryConstraint *self, GValue *value)
{
	return;
}

gboolean
midgard_query_constraint_set_value (MidgardQueryConstraint *self, const GValue *value)
{
	return FALSE;
}

MidgardQueryStorage *
midgard_query_constraint_get_storage (MidgardQueryConstraint *self)
{
	return NULL;
}

gboolean          
midgard_query_constraint_set_storage (MidgardQueryConstraint *self, MidgardQueryStorage *storage)
{
	return FALSE;
}

MidgardQueryProperty *
midgard_query_constraint_get_property (MidgardQueryConstraint *self)
{
	return NULL;
}

gboolean                
midgard_query_constraint_set_property (MidgardQueryConstraint *self, MidgardQueryProperty *property)
{
	return FALSE;
}

const gchar *
midgard_query_constraint_get_operator (MidgardQueryConstraint *self)
{
	return NULL;
}

gboolean
midgard_query_constraint_set_operator (MidgardQueryConstraint *self, const gchar *op)
{
	return FALSE;
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class = NULL;

MidgardQuerySimpleConstraint**
_midgard_query_constraint_list_constraints (MidgardQuerySimpleConstraint *self, guint *n_objects)
{
	return NULL;
}

static void 
__set_expression_value (GValue *dest, GValue *src)
{
	gchar *str;

	switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (src))) {
	
		case G_TYPE_STRING:
			  str = g_strdup_printf ("'%s'", g_value_get_string (src));
			  break;

		case G_TYPE_UINT:
			  str = g_strdup_printf ("%d", g_value_get_uint (src));
			  break;

		case G_TYPE_INT:
			  str = g_strdup_printf ("%d", g_value_get_int (src));
			  break;

		case G_TYPE_FLOAT:
			  str = g_strdup_printf ("%.04f", g_value_get_float (src));
			  break;

		case G_TYPE_BOOLEAN:
			  str = g_strdup_printf ("%d", g_value_get_boolean (src));
			  break;

		case G_TYPE_BOXED:
			  g_print ("BOXED type not implemented \n");
			  break;

		default:
			  break;
	}

	g_value_take_string (dest, str);
}

void 
_midgard_query_constraint_add_conditions_to_statement (MidgardQueryExecutor *executor, MidgardQuerySimpleConstraint *simple_constraint, GdaSqlStatement *stmt, GdaSqlExpr *where_expr_node)
{	
	MidgardQueryConstraint *self = MIDGARD_QUERY_CONSTRAINT (simple_constraint);
	GdaConnection *cnc = executor->priv->mgd->priv->connection;
	MidgardDBObjectClass *dbklass = NULL;
       	if (self->storage) {
	       dbklass = self->storage->klass;
	       MQE_SET_TABLE_ALIAS (executor, self->storage->table_alias);
	}
	if (!dbklass)
		dbklass = executor->priv->storage->klass;
	g_return_if_fail (dbklass != NULL);

	/* Get table */
	const gchar *table = midgard_core_class_get_table (dbklass);	

	/* Get field name */
	GValue field_value = {0, };
	midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (MIDGARD_QUERY_CONSTRAINT (simple_constraint)->property_value), &field_value);

	GdaSqlStatementSelect *select = stmt->contents;
	GdaSqlExpr *top_where, *where, *expr;
	GdaSqlOperation *top_operation, *cond;
	GValue *value;
	gboolean take_where_condition = FALSE;

	if (!select->where_cond) {	
		top_where = gda_sql_expr_new (GDA_SQL_ANY_PART (select));
		take_where_condition = TRUE;
		top_operation = gda_sql_operation_new (GDA_SQL_ANY_PART (top_where));
		top_operation->operator_type = GDA_SQL_OPERATOR_TYPE_AND;
		top_where->cond = top_operation;
	} else if (where_expr_node) {
		top_where = where_expr_node;
		top_operation = top_where->cond;
	} else {
		top_where = select->where_cond;
		top_operation = top_where->cond;
	}

	where = gda_sql_expr_new (GDA_SQL_ANY_PART (top_operation));
	top_operation->operands = g_slist_append (top_operation->operands, where);

	cond = gda_sql_operation_new (GDA_SQL_ANY_PART (where));
	where->cond = cond;	
	cond->operator_type = self->op_type;

	/* Create table_alias.field name */
	gchar *table_alias_field;
	expr = gda_sql_expr_new (GDA_SQL_ANY_PART (cond));
	table_alias_field = midgard_core_query_compute_constraint_property (executor, 
			MIDGARD_QUERY_CONSTRAINT (simple_constraint)->storage, g_value_get_string (&field_value));
	/* TODO, handle error case when table_alias_field is NULL */
	g_value_take_string ((value = gda_value_new (G_TYPE_STRING)), table_alias_field);
	expr->value = value;
	cond->operands = g_slist_append (cond->operands, expr);
	expr = gda_sql_expr_new (GDA_SQL_ANY_PART (cond));

	/* Create value */
	GValue val = {0, };
	midgard_query_holder_get_value (MIDGARD_QUERY_CONSTRAINT (simple_constraint)->holder, &val);
	GType v_type = G_VALUE_TYPE (&val);
	//GValue *dval = gda_value_new (G_TYPE_STRING);
	//g_value_transform (&val, dval);
	//expr->param_spec = gda_sql_param_spec_new (dval);
	//expr->param_spec->g_type = v_type;
	expr->value = gda_value_new (G_TYPE_STRING);
	__set_expression_value (expr->value, &val);
	g_value_unset (&val);
	cond->operands = g_slist_append (cond->operands, expr);

	if (take_where_condition)
		gda_sql_statement_select_take_where_cond (stmt, top_where);
}

static void
midgard_query_constraint_init (MidgardQuerySimpleConstraintIFace *iface)
{
	iface->list_constraints = _midgard_query_constraint_list_constraints;
	iface->priv = g_new (MidgardQuerySimpleConstraintPrivate, 1);
	iface->priv->add_conditions_to_statement = _midgard_query_constraint_add_conditions_to_statement;
	return;
}

static GObject *
_midgard_query_constraint_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	MidgardQueryConstraint *self = MIDGARD_QUERY_CONSTRAINT (object);
	self->property_value = NULL;
	self->op = NULL;
	self->storage = NULL;
	self->holder = NULL;

	return G_OBJECT(object);
}

static void
_midgard_query_constraint_dispose (GObject *object)
{
	MidgardQueryConstraint *self = MIDGARD_QUERY_CONSTRAINT (object);
	parent_class->dispose (object);
}

static void
_midgard_query_constraint_finalize (GObject *object)
{
	MidgardQueryConstraint *self = MIDGARD_QUERY_CONSTRAINT (object);

	g_free (self->op);
	self->op = NULL;

	parent_class->finalize;
}

static void
_midgard_query_constraint_class_init (MidgardQueryConstraintClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_query_constraint_constructor;
	object_class->dispose = _midgard_query_constraint_dispose;
	object_class->finalize = _midgard_query_constraint_finalize;
}

GType
midgard_query_constraint_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryConstraintClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			(GClassInitFunc)_midgard_query_constraint_class_init,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardQueryConstraint),
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
      
		static const GInterfaceInfo property_info = {
			(GInterfaceInitFunc) midgard_query_constraint_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryConstraint", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_SIMPLE_CONSTRAINT, &property_info);
    	}
    	return type;
}

