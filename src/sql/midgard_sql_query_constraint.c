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

#include "midgard_sql_query_constraint.h"
#include "../midgard_query_holder.h"
#include "../midgard_query_constraint_simple.h"
#include "../midgard_dbobject.h"
#include "../midgard_core_query.h"
#include "../midgard_query_column.h"
#include "../midgard_core_object.h"
#include "../midgard_validable.h"

struct _MidgardSqlQueryConstraintPrivate {
	GObject  parent;
	MidgardSqlQueryColumn *column;
	gchar *op;
	GdaSqlOperatorType op_type;
	MidgardQueryHolder *holder;
	gboolean is_valid;
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

static gboolean __query_constraint_operator_is_valid(const gchar *op, GdaSqlOperatorType *op_type)
{
	guint i;
	for (i = 0; valid_operators[i].name != NULL; i++) {
		if (g_str_equal(op, valid_operators[i].name)) {
			if (op_type)
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


/**
 * midgard_sql_query_constraint_new:
 * @column: #MidgardSqlQuerycolumn instance
 * @op: constraint operator
 * @holder: #MidgardQueryHolder instance
 *
 * Returns: new #MidgardSqlQueryConstraint instance, or %NULL on failure
 * Since: 10.05.6
 */ 
MidgardSqlQueryConstraint *
midgard_sql_query_constraint_new (MidgardSqlQueryColumn *column, const gchar *op, 
		MidgardQueryHolder *holder)
{
	g_return_val_if_fail (column != NULL, NULL);
	g_return_val_if_fail (op != NULL, NULL);
	g_return_val_if_fail (holder != NULL, NULL);

	MidgardSqlQueryConstraint *self = g_object_new (MIDGARD_TYPE_SQL_QUERY_CONSTRAINT, 
			"column", column, "operator", op, "holder", holder, NULL);

	return self;
}

/**
 * midgard_sql_query_constraint_get_column:
 * @self: #MidgardSqlQueryConstraint instance
 *
 * Returns: (transfer full): #MidgardSqlQueryColumn associated with constraint or %NULL
 * Since: 10.05.6
 */  
MidgardSqlQueryColumn *
midgard_sql_query_constraint_get_column (MidgardSqlQueryConstraint *self)
{
	g_return_val_if_fail (self != NULL, NULL);
	return g_object_ref (self->priv->column);
}

/**
 * midgard_sql_query_constraint_set_column:
 * @self: #MidgardSqlQueryConstraint instance
 * @column: #MidgardSqlQueryColumn to associate with @self constraint
 * @error: pointer to store returned error
 *
 * Since: 10.05.6
 */ 
void 
midgard_sql_query_constraint_set_storage (MidgardSqlQueryConstraint *self, MidgardSqlQueryColumn *column, GError **error)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (column != NULL);

	if (self->priv->column)
		g_object_unref (self->priv->column);

	self->priv->column = g_object_ref (column);
}

/**
 * midgard_sql_query_constraint_get_holder:
 * @self: #MidgardSqlQueryConstraint instance
 *
 * Returns: (transfer full): #MidgardQueryHolder associated with @self constraint, or %NULL
 * Since: 10.05.6
 */ 
MidgardQueryHolder*
midgard_sql_query_constraint_get_property (MidgardSqlQueryConstraint *self)
{
	g_return_val_if_fail (self != NULL, NULL);

	return g_object_ref (self->priv->holder);
}

/**
 * midgard_sql_query_constraint_set_holder:
 * @self: #MidgardSqlQueryConstraint instance
 * @holder: #MidgardQueryHolder to associate with @self constraint
 * @error: pointer to store returned error
 *
 * Since: 10.05.6
 */ 
void       
midgard_sql_query_constraint_set_holder (MidgardSqlQueryConstraint *self, MidgardQueryHolder *holder, GError **error)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (holder != NULL);

	if (self->priv->holder)
		g_object_unref (self->priv->holder);

	self->priv->holder = g_object_ref (holder);
}

/**
 * midgard_sql_query_constraint_get_operator:
 * @self: #MidgardSqlQueryConstraint instance
 *
 * Returns: (transfer none): operator type associated with @self constraint, or %NULL
 * Since: 10.05.6
 */
const gchar *
midgard_sql_query_constraint_get_operator (MidgardSqlQueryConstraint *self)
{
	g_return_val_if_fail (self != NULL, NULL);
	return self->priv->op;
}

/**
 * midgard_sql_query_constraint_set_operator:
 * @self: #MidgardSqlQueryConstraint instance
 * @op: operator to associate with constraint
 * @error: pointer to store returned error
 * 
 * Check midgard_query_constraint_new() for valid operator types.
 *
 * Since: 10.05.6
 */ 
void
midgard_sql_query_constraint_set_operator (MidgardSqlQueryConstraint *self, const gchar *op, GError **error)
{
	g_return_if_fail (self != NULL);

 	GdaSqlOperatorType op_type;
	if (!__query_constraint_operator_is_valid (op, &op_type)) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_VALUE_INVALID, 
				"Invalid '%s' operator", op);
		return;
	}

        self->priv->op = g_strdup (op);
        self->priv->op_type = op_type;
}

/* GOBJECT ROUTINES */

enum {
	MIDGARD_SQL_QUERY_CONSTRAINT_COLUMN = 1,
	MIDGARD_SQL_QUERY_CONSTRAINT_OP,
	MIDGARD_SQL_QUERY_CONSTRAINT_HOLDER
};

static GObjectClass *parent_class = NULL;

MidgardQueryConstraintSimple**
_midgard_sql_query_constraint_list_constraints (MidgardQueryConstraintSimple *self, guint *n_objects)
{
	return NULL;
}

static void 
__get_expression_value (GdaConnection *cnc, GValue *src, GString *str)
{
	gchar *escaped_string = NULL;
	switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (src))) {
	
		case G_TYPE_STRING:
			escaped_string = gda_connection_value_to_sql_string(cnc, src);
			g_string_append_printf (str, "%s", escaped_string);
			g_free(escaped_string);
			break;

		case G_TYPE_UINT:
			  g_string_append_printf (str, "%d", g_value_get_uint (src));
			  break;

		case G_TYPE_INT:
			  g_string_append_printf (str, "%d", g_value_get_int (src));
			  break;

		case G_TYPE_FLOAT:
			  g_string_append_printf (str, "%.04f", g_value_get_float (src));
			  break;

		case G_TYPE_BOOLEAN:
			  g_string_append_printf (str, "%d", g_value_get_boolean (src));
			  break;

		case G_TYPE_BOXED:
			  if (G_VALUE_TYPE (src) == G_TYPE_VALUE_ARRAY) {
				GValueArray *array = (GValueArray *) g_value_get_boxed (src);
				if (!array) {
					/* FIXME, add this to validate */
					g_warning ("Empty array given");
					return;
				}
				guint i;
				for (i = 0; i < array->n_values; i++) {
					if (i > 0)
						g_string_append (str, ", ");
					__get_expression_value (cnc, g_value_array_get_nth (array, i), str);
				}
			} else {
				/*FIXME, add this to validate */
				g_warning ("BOXED type '%s' not implemented \n", G_VALUE_TYPE_NAME (src));
			}
			break;

		default:
			break;
	}

	return;
}

void 
_midgard_sql_query_constraint_add_conditions_to_statement (MidgardQueryExecutor *executor, MidgardQueryConstraintSimple *constraint_simple, GdaSqlStatement *stmt, GdaSqlExpr *where_expr_node, GError **error)
{	
	MidgardSqlQueryConstraint *self = MIDGARD_SQL_QUERY_CONSTRAINT (constraint_simple);
	GdaConnection *cnc = executor->priv->mgd->priv->connection;

	GdaSqlStatementSelect *select = stmt->contents;
	GdaSqlExpr *top_where, *where, *expr;
	GdaSqlOperation *top_operation, *cond;
	GValue *value;

	if (where_expr_node) {
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
	cond->operator_type = self->priv->op_type;

	/* Create table_alias.field name */
	MidgardSqlQueryColumn *column = midgard_sql_query_constraint_get_column (self);
	gchar *table_alias_field = g_strconcat ((const gchar *)midgard_query_column_get_qualifier (MIDGARD_QUERY_COLUMN (column), NULL),
				".", midgard_query_column_get_name (MIDGARD_QUERY_COLUMN (column), NULL), NULL);
	expr = gda_sql_expr_new (GDA_SQL_ANY_PART (cond));
	g_value_take_string ((value = gda_value_new (G_TYPE_STRING)), table_alias_field);
	expr->value = value;
	cond->operands = g_slist_append (cond->operands, expr);
	expr = gda_sql_expr_new (GDA_SQL_ANY_PART (cond));	

	/* Create value */
	GValue val = {0, };
	midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (self->priv->holder), &val);
	//FIXME, create parameter name::type */
	expr->value = gda_value_new (G_TYPE_STRING);
	GString *str = g_string_new ("");
	__get_expression_value (cnc, &val, str);
	g_value_take_string (expr->value, g_string_free (str, FALSE));
	g_value_unset (&val);
	cond->operands = g_slist_append (cond->operands, expr);

	/* increase executor's constraints number */
	executor->priv->n_constraints++;
}

static void
_midgard_sql_query_constraint_iface_init (MidgardQueryConstraintSimpleIFace *iface)
{
	iface->list_constraints = _midgard_sql_query_constraint_list_constraints;
	iface->priv = g_new (MidgardQueryConstraintSimplePrivate, 1);
	iface->priv->add_conditions_to_statement = _midgard_sql_query_constraint_add_conditions_to_statement;
	return;
}

static void
_midgard_sql_query_constraint_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardSqlQueryConstraint *self = MIDGARD_SQL_QUERY_CONSTRAINT (instance);
	self->priv = g_new (MidgardSqlQueryConstraintPrivate, 1);
	self->priv->column = NULL;
	self->priv->op = NULL;
	self->priv->holder = NULL;
	self->priv->is_valid = FALSE;
}

/* Validable iface */
static void
_midgard_sql_query_constraint_validable_iface_validate (MidgardValidable *iface, GError **error)
{
	g_return_if_fail (iface != NULL);
	MidgardSqlQueryConstraint *self = MIDGARD_SQL_QUERY_CONSTRAINT (iface);
	self->priv->is_valid = FALSE;

	/* Column, TODO */

	/* Storage, TODO */

	/* Value, TODO */

	/* Operator */
	if (!__query_constraint_operator_is_valid (self->priv->op, NULL)) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_TYPE_INVALID,
				"Invalid operator type '%s'", self->priv->op);
		return;
	}

	MIDGARD_SQL_QUERY_CONSTRAINT (self)->priv->is_valid = TRUE;
	return;
}

gboolean
_midgard_sql_query_constraint_validable_iface_is_valid (MidgardValidable *self)
{
	g_return_val_if_fail (self != NULL, FALSE);
	return MIDGARD_SQL_QUERY_CONSTRAINT (self)->priv->is_valid;
}

static void
_midgard_sql_query_constraint_validable_iface_init (MidgardValidableIFace *iface)
{
	iface->validate = _midgard_sql_query_constraint_validable_iface_validate;
	iface->is_valid = _midgard_sql_query_constraint_validable_iface_is_valid;
}

static GObject *
_midgard_sql_query_constraint_constructor (GType type,
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
_midgard_sql_query_constraint_dispose (GObject *object)
{	
	parent_class->dispose (object);

	MidgardSqlQueryConstraint *self = (MidgardSqlQueryConstraint *) object;
	if (self->priv->column) {
		g_object_unref (self->priv->column);
		self->priv->column = NULL;
	}

	if (self->priv->holder && G_IS_OBJECT (self->priv->holder)) {
		g_object_unref (self->priv->holder);
		self->priv->holder = NULL;
	}
}

static void
_midgard_sql_query_constraint_finalize (GObject *object)
{
	MidgardSqlQueryConstraint *self = MIDGARD_SQL_QUERY_CONSTRAINT (object);

	g_free (self->priv->op);
	self->priv->op = NULL;

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
__midgard_sql_query_constraint_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardSqlQueryConstraint *self = (MidgardSqlQueryConstraint *) object;

	switch (property_id) {
		
		case MIDGARD_SQL_QUERY_CONSTRAINT_COLUMN:
			g_value_set_object (value, self->priv->column);
			break;

		case MIDGARD_SQL_QUERY_CONSTRAINT_OP:
			g_value_set_string (value, self->priv->op);
			break;

		case MIDGARD_SQL_QUERY_CONSTRAINT_HOLDER:
			g_value_set_object (value, self->priv->holder);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
__midgard_sql_query_constraint_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardSqlQueryConstraint *self = (MidgardSqlQueryConstraint *) (object);
	GdaSqlOperatorType op_type;
	gchar *op; 

	switch (property_id) {

		case MIDGARD_SQL_QUERY_CONSTRAINT_COLUMN:
			if (self->priv->column)
				g_object_unref (self->priv->column);
			self->priv->column = g_value_dup_object (value);
			break;

		case MIDGARD_SQL_QUERY_CONSTRAINT_OP:
			op = (gchar *)g_value_get_string (value);
			GError *err = NULL;
			midgard_sql_query_constraint_set_operator (self, op, &err);
			if (err)
				g_warning ("Error while setting property. %s", err && err->message ? err->message : "Unknown reason");
			break;

		case MIDGARD_SQL_QUERY_CONSTRAINT_HOLDER:
			if (self->priv->holder)
				g_object_unref (self->priv->holder);
			self->priv->holder = g_value_dup_object (value);
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
_midgard_sql_query_constraint_class_init (MidgardSqlQueryConstraintClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_sql_query_constraint_constructor;
	object_class->dispose = _midgard_sql_query_constraint_dispose;
	object_class->finalize = _midgard_sql_query_constraint_finalize;

	object_class->set_property = __midgard_sql_query_constraint_set_property;
	object_class->get_property = __midgard_sql_query_constraint_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_object ("column",
			"",
			"",
			MIDGARD_TYPE_SQL_QUERY_COLUMN,
			G_PARAM_READWRITE);
	/**
	 * MidgardSqlQueryConstraint:column:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_SQL_QUERY_CONSTRAINT_COLUMN, pspec);

	pspec = g_param_spec_string ("operator",
			"", "", "",
			G_PARAM_READWRITE);
	/**
	 * MidgardSqlQueryConstraint:operator:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_SQL_QUERY_CONSTRAINT_OP, pspec);

	pspec = g_param_spec_object ("holder",
			"",
			"",
			MIDGARD_TYPE_QUERY_HOLDER,
			G_PARAM_READWRITE);
	/**
	 * MidgardSqlQueryConstraint:holder:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_SQL_QUERY_CONSTRAINT_HOLDER, pspec);
}

GType
midgard_sql_query_constraint_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardSqlQueryConstraintClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			(GClassInitFunc)_midgard_sql_query_constraint_class_init,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardSqlQueryConstraint),
			0,      /* n_preallocs */
			_midgard_sql_query_constraint_instance_init    /* instance_init */
		};
      
		static const GInterfaceInfo constraint_simple_info = {
			(GInterfaceInitFunc) _midgard_sql_query_constraint_iface_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

		static const GInterfaceInfo validable_info = {
			(GInterfaceInitFunc) _midgard_sql_query_constraint_validable_iface_init,
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSqlQueryConstraint", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_CONSTRAINT_SIMPLE, &constraint_simple_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_VALIDABLE, &validable_info);
    	}
    	return type;
}

