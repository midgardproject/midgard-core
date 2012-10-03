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
#include "midgard_query_constraint_simple.h"
#include "midgard_dbobject.h"
#include "midgard_core_query.h"
#include "midgard_core_object_class.h"
#include "midgard_core_object.h"
#include "midgard_validable.h"

struct _MidgardQueryConstraintPrivate {
	GObject  parent;
	MidgardQueryProperty *property_value;
	gchar *op;
	GdaSqlOperatorType op_type;
	MidgardQueryStorage *storage;
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
	{ "NOT REGEXP", GDA_SQL_OPERATOR_TYPE_NOT_REGEXP },
	{ "REGEXP", GDA_SQL_OPERATOR_TYPE_REGEXP },
	{ NULL, 0 }
};

gboolean __query_constraint_operator_is_valid(const gchar *op, GdaSqlOperatorType *op_type)
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
 * midgard_query_constraint_new:
 * @property: #MidgardQueryProperty instance
 * @op: constraint operator
 * @holder: #MidgardQueryHolder instance
 * @storage: (allow-none): optional #MidgardQueryStorage to use with constraint
 * 
 * Valid @op operators are: '=', '<', '>', '!=', '<>', '<=', '>=', 'LIKE', 'NOT LIKE', 'IN', 'NOT IN'
 *
 * Returns: new #MidgardQueryConstraint instance, or %NULL on failure
 * Since: 10.05
 */ 
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

	MidgardQueryConstraint *self = g_object_new (MIDGARD_TYPE_QUERY_CONSTRAINT, 
			"property", property, "operator", op, "holder", holder, NULL);

	/* Allow NULL storage */
	if (storage)
		midgard_query_constraint_set_storage (self, storage);

	return self;
}

/**
 * midgard_query_constraint_get_storage:
 * @self: #MidgardQueryStorage instance
 *
 * Returns: (transfer none): #MidgardQueryStorage associated with constraint or %NULL
 * Since: 10.05
 */  
MidgardQueryStorage *
midgard_query_constraint_get_storage (MidgardQueryConstraint *self)
{
	g_return_val_if_fail (self != NULL, NULL);
	return self->priv->storage;
}

/**
 * midgard_query_constraint_set_storage:
 * @self: #MidgardQueryConstraint instance
 * @storage: (allow-none): #MidgardQueryStorage to associate with @self constraint
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05
 */ 
gboolean          
midgard_query_constraint_set_storage (MidgardQueryConstraint *self, MidgardQueryStorage *storage)
{
	g_return_val_if_fail (self != NULL, FALSE);

	if (self->priv->storage)
		g_object_unref (self->priv->storage);

	if (!storage) {
		self->priv->storage = NULL;
		return TRUE;
	}

	g_return_val_if_fail (MIDGARD_IS_QUERY_STORAGE (storage), FALSE);
	self->priv->storage = g_object_ref (storage);
	return TRUE;
}

/**
 * midgard_query_constraint_get_property:
 * @self: #MidgardQueryConstraint instance
 *
 * Returns: (transfer none): #MidgardQueryProperty associated with @self constraint, or %NULL
 * Since: 10.05
 */ 
MidgardQueryProperty *
midgard_query_constraint_get_property (MidgardQueryConstraint *self)
{
	g_return_val_if_fail (self != NULL, NULL);

	return self->priv->property_value;
}

/**
 * midgard_query_constraint_set_property:
 * @self: #MidgardQueryConstraint instance
 * @property: #MidgardQueryProperty to associate with @self constraint
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05
 */ 
gboolean                
midgard_query_constraint_set_property (MidgardQueryConstraint *self, MidgardQueryProperty *property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (MIDGARD_IS_QUERY_PROPERTY (property), FALSE);

	if (self->priv->property_value)
		g_object_unref (self->priv->property_value);

	self->priv->property_value = g_object_ref (property);

	/* set default storage */
	if (!self->priv->storage) {
		MidgardQueryProperty *_p = self->priv->property_value;
		midgard_query_constraint_set_storage (self, _p->priv->storage);
	}

	return TRUE;
}

/**
 * midgard_query_constraint_get_operator:
 * @self: #MidgardQueryConstraint instance
 *
 * Returns: operator type associated with @self constraint, or %NULL
 * Since: 10.05
 */
const gchar *
midgard_query_constraint_get_operator (MidgardQueryConstraint *self)
{
	g_return_val_if_fail (self != NULL, NULL);

	return self->priv->op;
}

/**
 * midgard_query_constraint_set_operator:
 * @self: #MidgardQueryConstraint instance
 * @op: operator to associate with constraint
 * 
 * Check midgard_query_constraint_new() for valid operator types.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05
 */ 
gboolean
midgard_query_constraint_set_operator (MidgardQueryConstraint *self, const gchar *op)
{
	g_return_val_if_fail (self != NULL, FALSE);

 	GdaSqlOperatorType op_type;
	if (!__query_constraint_operator_is_valid (op, &op_type))
		return FALSE;

        self->priv->op = g_strdup (op);
        self->priv->op_type = op_type;
	return TRUE;
}

/* GOBJECT ROUTINES */

enum {
	MIDGARD_QUERY_CONSTRAINT_PROPERTY = 1,
	MIDGARD_QUERY_CONSTRAINT_OP,
	MIDGARD_QUERY_CONSTRAINT_HOLDER,
	MIDGARD_QUERY_CONSTRAINT_STORAGE
};

static GObjectClass *parent_class = NULL;

MidgardQueryConstraintSimple**
_midgard_query_constraint_list_constraints (MidgardQueryConstraintSimple *self, guint *n_objects)
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
_midgard_query_constraint_add_conditions_to_statement (MidgardQueryExecutor *executor, MidgardQueryConstraintSimple *constraint_simple, GdaSqlStatement *stmt, GdaSqlExpr *where_expr_node, GError **error)
{	
	MidgardQueryConstraint *self = MIDGARD_QUERY_CONSTRAINT (constraint_simple);
	GdaConnection *cnc = executor->priv->mgd->priv->connection;
	MidgardDBObjectClass *dbklass = NULL;	
       	if (self->priv->storage && (self->priv->storage != MIDGARD_QUERY_EXECUTOR (executor)->priv->storage)) {
	       dbklass = self->priv->storage->priv->klass;
	       MQE_SET_TABLE_ALIAS (executor, self->priv->storage);
	}
	if (!dbklass)
		dbklass = executor->priv->storage->priv->klass;
	g_return_if_fail (dbklass != NULL);

	/* Get table */
	//const gchar *table = midgard_core_class_get_table (dbklass);	

	/* Get field name */
	GValue field_value = {0, };
	midgard_query_holder_get_value (MIDGARD_QUERY_HOLDER (MIDGARD_QUERY_CONSTRAINT (constraint_simple)->priv->property_value), &field_value);

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
	gchar *table_alias_field;
	expr = gda_sql_expr_new (GDA_SQL_ANY_PART (cond));
	GError *err = NULL;
	table_alias_field = midgard_core_query_compute_constraint_property (executor, 
			MIDGARD_QUERY_CONSTRAINT (constraint_simple)->priv->storage, g_value_get_string (&field_value), &err);
	if (err) {
		g_propagate_error (error, err);
		return;
	}
	g_value_take_string ((value = gda_value_new (G_TYPE_STRING)), table_alias_field);
	expr->value = value;
	cond->operands = g_slist_append (cond->operands, expr);
	expr = gda_sql_expr_new (GDA_SQL_ANY_PART (cond));

	g_value_unset (&field_value);

	/* Create value */
	GValue val = {0, };
	midgard_query_holder_get_value (MIDGARD_QUERY_CONSTRAINT (constraint_simple)->priv->holder, &val);
	/*GType v_type = G_VALUE_TYPE (&val);
	//FIXME, create parameter name::type */
	//GValue *dval = gda_value_new (G_TYPE_STRING);
	//g_value_transform (&val, dval);
	//expr->param_spec = gda_sql_param_spec_new (dval);
	//expr->param_spec->g_type = v_type;
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
_midgard_query_constraint_iface_init (MidgardQueryConstraintSimpleIFace *iface)
{
	iface->list_constraints = _midgard_query_constraint_list_constraints;
	iface->priv = g_new (MidgardQueryConstraintSimplePrivate, 1);
	iface->priv->add_conditions_to_statement = _midgard_query_constraint_add_conditions_to_statement;
	return;
}

static void
_midgard_query_constraint_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardQueryConstraint *self = MIDGARD_QUERY_CONSTRAINT (instance);
	self->priv = g_new (MidgardQueryConstraintPrivate, 1);
	self->priv->property_value = NULL;
	self->priv->op = NULL;
	self->priv->storage = NULL;
	self->priv->holder = NULL;
	self->priv->is_valid = FALSE;
}

/* Validable iface */
static void
_midgard_query_constraint_validable_iface_validate (MidgardValidable *iface, GError **error)
{
	g_return_if_fail (iface != NULL);
	MidgardQueryConstraint *self = MIDGARD_QUERY_CONSTRAINT (iface);
	self->priv->is_valid = FALSE;

	/* Property */
	if (MIDGARD_IS_VALIDABLE (self->priv->property_value)) {
		GError *err = NULL;
		if (!midgard_validable_is_valid (MIDGARD_VALIDABLE (self->priv->property_value)))
			midgard_validable_validate (MIDGARD_VALIDABLE (self->priv->property_value), &err);
		if (err) {
			g_propagate_error (error, err);
			return;
		}
	}

	/* Storage, TODO */

	/* Value, TODO */

	/* Operator */
	if (!__query_constraint_operator_is_valid (self->priv->op, NULL)) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_TYPE_INVALID,
				"Invalid operator type '%s'", self->priv->op);
		return;
	}

	MIDGARD_QUERY_CONSTRAINT (self)->priv->is_valid = TRUE;
	return;
}

gboolean
_midgard_query_constraint_validable_iface_is_valid (MidgardValidable *self)
{
	g_return_val_if_fail (self != NULL, FALSE);
	return MIDGARD_QUERY_CONSTRAINT (self)->priv->is_valid;
}

static void
_midgard_query_constraint_validable_iface_init (MidgardValidableIFace *iface)
{
	iface->validate = _midgard_query_constraint_validable_iface_validate;
	iface->is_valid = _midgard_query_constraint_validable_iface_is_valid;
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

	return G_OBJECT(object);
}

static void
_midgard_query_constraint_dispose (GObject *object)
{	
	parent_class->dispose (object);

	MidgardQueryConstraint *self = (MidgardQueryConstraint *) object;
	if (self->priv->property_value) {
		g_object_unref (self->priv->property_value);
		self->priv->property_value = NULL;
	}

	if (self->priv->storage && G_IS_OBJECT (self->priv->storage)) {
		g_object_unref (self->priv->storage);
		self->priv->storage = NULL;
	}

	if (self->priv->holder && G_IS_OBJECT (self->priv->holder)) {
		g_object_unref (self->priv->holder);
		self->priv->holder = NULL;
	}
}

static void
_midgard_query_constraint_finalize (GObject *object)
{
	MidgardQueryConstraint *self = MIDGARD_QUERY_CONSTRAINT (object);

	g_free (self->priv->op);
	self->priv->op = NULL;

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
__midgard_query_constraint_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardQueryConstraint *self = (MidgardQueryConstraint *) object;

	switch (property_id) {
		
		case MIDGARD_QUERY_CONSTRAINT_PROPERTY:
			g_value_set_object (value, self->priv->property_value);
			break;

		case MIDGARD_QUERY_CONSTRAINT_OP:
			g_value_set_string (value, self->priv->op);
			break;

		case MIDGARD_QUERY_CONSTRAINT_HOLDER:
			g_value_set_object (value, self->priv->holder);
			break;

		case MIDGARD_QUERY_CONSTRAINT_STORAGE:
			g_value_set_object (value, self->priv->storage);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
__midgard_query_constraint_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardQueryConstraint *self = (MidgardQueryConstraint *) (object);
	GdaSqlOperatorType op_type;
	gchar *op; 

	switch (property_id) {

		case MIDGARD_QUERY_CONSTRAINT_PROPERTY:
			midgard_query_constraint_set_property (self, g_value_get_object (value));
			break;

		case MIDGARD_QUERY_CONSTRAINT_OP:
			op = (gchar *)g_value_get_string (value);
       			if (__query_constraint_operator_is_valid (op, &op_type)) {
				g_free (self->priv->op);
				self->priv->op = g_strdup (op);
				self->priv->op_type = op_type;
			}
			break;

		case MIDGARD_QUERY_CONSTRAINT_HOLDER:
			if (self->priv->holder)
				g_object_unref (self->priv->holder);
			self->priv->holder = g_value_dup_object (value);
			break;

		case MIDGARD_QUERY_CONSTRAINT_STORAGE:
			midgard_query_constraint_set_storage (self, g_value_get_object (value));
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
_midgard_query_constraint_class_init (MidgardQueryConstraintClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_query_constraint_constructor;
	object_class->dispose = _midgard_query_constraint_dispose;
	object_class->finalize = _midgard_query_constraint_finalize;

	object_class->set_property = __midgard_query_constraint_set_property;
	object_class->get_property = __midgard_query_constraint_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_object ("property",
			"",
			"",
			MIDGARD_TYPE_QUERY_PROPERTY,
			G_PARAM_READWRITE);
	/**
	 * MidgardQueryConstraint:property:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_QUERY_CONSTRAINT_PROPERTY, pspec);

	pspec = g_param_spec_string ("operator",
			"", "", "",
			G_PARAM_READWRITE);
	/**
	 * MidgardQueryConstraint:operator:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_QUERY_CONSTRAINT_OP, pspec);

	pspec = g_param_spec_object ("holder",
			"",
			"",
			MIDGARD_TYPE_QUERY_HOLDER,
			G_PARAM_READWRITE);
	/**
	 * MidgardQueryConstraint:holder:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_QUERY_CONSTRAINT_HOLDER, pspec);

	pspec = g_param_spec_object ("storage",
			"",
			"",
			MIDGARD_TYPE_QUERY_STORAGE,
			G_PARAM_READWRITE);
	/**
	 * MidgardQueryConstraint:storage:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_QUERY_CONSTRAINT_STORAGE, pspec);
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
			_midgard_query_constraint_instance_init    /* instance_init */
		};
      
		static const GInterfaceInfo property_info = {
			(GInterfaceInitFunc) _midgard_query_constraint_iface_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

		static const GInterfaceInfo validable_info = {
			(GInterfaceInitFunc) _midgard_query_constraint_validable_iface_init,
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryConstraint", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_CONSTRAINT_SIMPLE, &property_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_VALIDABLE, &validable_info);
    	}
    	return type;
}

