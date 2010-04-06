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

#include "midgard_query_group_constraint.h"
#include "midgard_query_property.h"
#include "midgard_query_storage.h"
#include "midgard_query_holder.h"
#include "midgard_query_simple_constraint.h"
#include "midgard_dbobject.h"
#include "midgard_core_query.h"

struct _MidgardQueryGroupConstraintPrivate {
	gchar *type;
	GdaSqlOperatorType op_type;
	GSList *constraints;
};

MidgardQueryGroupConstraint *
midgard_query_group_constraint_new (const gchar *type, MidgardQuerySimpleConstraint *constraint, ...)
{
	g_return_val_if_fail (type != NULL, NULL);
	g_return_val_if_fail (constraint != NULL, NULL);

	GdaSqlOperatorType op_type;

	/* Validate given type. We expect type to be NULL terminated. */
	gchar *valid_type = g_ascii_strdown (type, -1);
	if (g_str_equal (valid_type, "and"))
		op_type = GDA_SQL_OPERATOR_TYPE_AND;
	else if (g_str_equal (valid_type, "or"))
		op_type = GDA_SQL_OPERATOR_TYPE_OR;
	else {
		/* FIXME, handle catchable error */
		g_warning ("Invalid group type. Expected 'AND' or 'OR'. '%s' given", type);
		g_free (valid_type);
		return NULL;
	}

	MidgardQueryGroupConstraint *self = g_object_new (MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT, NULL);
	self->priv->type = valid_type;
	self->priv->op_type = op_type;

	MidgardQuerySimpleConstraint *cnstr = constraint;
	va_list args;
	va_start (args, constraint);
	while (cnstr != NULL) {
		if (MIDGARD_IS_QUERY_SIMPLE_CONSTRAINT (cnstr))
			self->priv->constraints = g_slist_append (self->priv->constraints, cnstr);
		cnstr = va_arg (args, MidgardQuerySimpleConstraint*);
	}
	va_end (args);

	return self;
}

const gchar *
midgard_query_group_constraint_get_group_type (MidgardQueryGroupConstraint *self)
{
	g_return_val_if_fail (self != NULL, NULL);
	return (const gchar *)self->priv->type;
}

gboolean
midgard_query_group_constraint_set_group_type (MidgardQueryGroupConstraint *self, const gchar *type)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (type != NULL, FALSE);

	return TRUE;
}

gboolean
midgard_query_group_constraint_add_constraint (MidgardQueryGroupConstraint *self, MidgardQuerySimpleConstraint *constraint, ...)
{
	return FALSE;
}

MidgardQuerySimpleConstraint **
midgard_query_group_constraint_list_constraints (MidgardQueryGroupConstraint *self, guint *n_objects)
{
	return FALSE;
}

/* GOBJECT ROUTINES */

GObjectClass *parent_class = NULL;

MidgardQuerySimpleConstraint**
_midgard_query_group_constraint_list_constraints (MidgardQuerySimpleConstraint *self, guint *n_objects)
{
	g_return_val_if_fail (self != NULL, NULL);

	GSList *l;
	GSList *self_constraints = MIDGARD_QUERY_GROUP_CONSTRAINT (self)->priv->constraints;
	guint i = 0;

	/* count constraints */
	for (l = self_constraints; l != NULL; l = l->next)
		i++;

	MidgardQuerySimpleConstraint **constraints = g_new (MidgardQuerySimpleConstraint*, i);

	i = 0;
	for (l = self_constraints; l != NULL; l = l->next)
	{
		constraints[i] = MIDGARD_QUERY_SIMPLE_CONSTRAINT (l->data);
		i++;
	}

	*n_objects = i;
	return constraints;
}

void
_midgard_query_group_add_conditions_to_statement (MidgardQueryExecutor *executor, MidgardQuerySimpleConstraint *self, 
		GdaSqlStatement *stmt, GdaSqlExpr *where_expr_node)
{	
	guint n_objects;
	guint i;
	MidgardQuerySimpleConstraint **constraints = 
		midgard_query_simple_constraint_list_constraints (MIDGARD_QUERY_SIMPLE_CONSTRAINT (self), &n_objects);
	if (!constraints)
		return;

	GdaSqlStatementSelect *select = stmt->contents;
	GdaSqlExpr *top_where = NULL, *where;
	GdaSqlOperation *top_operation, *operation;	
	
	/* Create base top expression and operation */
	if (!select->where_cond) {
		top_where = gda_sql_expr_new (GDA_SQL_ANY_PART (select));
		top_operation = gda_sql_operation_new (GDA_SQL_ANY_PART (top_where));
		top_operation->operator_type = MIDGARD_QUERY_GROUP_CONSTRAINT (self)->priv->op_type;
		top_where->cond = top_operation;
	     	gda_sql_statement_select_take_where_cond (stmt, top_where);	
	} else if (where_expr_node) {
		/* This is nested groups case: '... AND (f2=1 OR f2=2)...' */
		where = where_expr_node;
		operation = where->cond;
		top_where = gda_sql_expr_new (GDA_SQL_ANY_PART (operation));
		top_operation = gda_sql_operation_new (GDA_SQL_ANY_PART (where_expr_node));
		top_operation->operator_type = MIDGARD_QUERY_GROUP_CONSTRAINT (self)->priv->op_type;
		top_where->cond = top_operation;
		operation->operands = g_slist_append (operation->operands, top_where);
	}

	for (i = 0; i < n_objects; i++) {

		MIDGARD_QUERY_SIMPLE_CONSTRAINT_GET_INTERFACE (constraints[i])->priv->add_conditions_to_statement (executor, MIDGARD_QUERY_SIMPLE_CONSTRAINT (constraints[i]), stmt, top_where);
	}

	g_free (constraints);
}

static void
_midgard_query_group_constraint_iface_init (MidgardQuerySimpleConstraintIFace *iface)
{
	iface->list_constraints = _midgard_query_group_constraint_list_constraints;
	iface->priv = g_new (MidgardQuerySimpleConstraintPrivate, 1);
	iface->priv->add_conditions_to_statement = _midgard_query_group_add_conditions_to_statement;
	
	return;
}

static GObject *
_midgard_query_group_constraint_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	MidgardQueryGroupConstraint *self = MIDGARD_QUERY_GROUP_CONSTRAINT (object);
	self->priv = g_new (MidgardQueryGroupConstraintPrivate, 1);
	self->priv->type = NULL;
	self->priv->constraints = NULL;

	return G_OBJECT(object);
}

static void
_midgard_query_group_constraint_dispose (GObject *object)
{
	parent_class->dispose (object);
}

static void
_midgard_query_group_constraint_finalize (GObject *object)
{
	MidgardQueryGroupConstraint *self = MIDGARD_QUERY_GROUP_CONSTRAINT (object);
	
	g_free (self->priv->type);
	self->priv->type = NULL;

	if (self->priv->constraints)
		g_slist_free (self->priv->constraints);
	self->priv->constraints = NULL; 

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
_midgard_query_group_constraint_class_init (MidgardQueryGroupConstraintClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_query_group_constraint_constructor;
	object_class->dispose = _midgard_query_group_constraint_dispose;
	object_class->finalize = _midgard_query_group_constraint_finalize;
}

GType
midgard_query_group_constraint_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryGroupConstraintClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			(GClassInitFunc) _midgard_query_group_constraint_class_init, /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardQueryGroupConstraint),
			0,      /* n_preallocs */
			    /* instance_init */
		};
      
		static const GInterfaceInfo property_info = {
			(GInterfaceInitFunc)_midgard_query_group_constraint_iface_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryGroupConstraint", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_SIMPLE_CONSTRAINT, &property_info);
    	}
    	return type;
}

