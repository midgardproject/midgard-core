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

struct _MidgardQueryGroupConstraint {
	GObject  parent;
	gchar *type;
	GSList *constraints;
};

MidgardQueryGroupConstraint *
midgard_query_group_constraint_new (const gchar *type, MidgardQuerySimpleConstraint *constraint, ...)
{
	g_return_val_if_fail (type != NULL, NULL);
	g_return_val_if_fail (constraint != NULL, NULL);

	MidgardQueryGroupConstraint *self = g_object_new (MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT, NULL);
	self->type = g_strdup (type);

	MidgardQuerySimpleConstraint *cnstr = constraint;
	va_list args;
	va_start (args, constraint);
	while (cnstr != NULL) {
		if (MIDGARD_IS_QUERY_SIMPLE_CONSTRAINT (cnstr))
			self->constraints = g_slist_append (self->constraints, cnstr);
		cnstr = va_arg (args, MidgardQuerySimpleConstraint*);
	}
	va_end (args);

	return self;
}

const gchar *
midgard_query_group_constraint_get_group_type (MidgardQueryGroupConstraint *self)
{
	g_return_val_if_fail (self != NULL, NULL);
	return (const gchar *)self->type;
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
	GSList *self_constraints = MIDGARD_QUERY_GROUP_CONSTRAINT (self)->constraints;
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
_midgard_query_group_add_conditions_to_statement (MidgardQueryExecutor *executor, MidgardQuerySimpleConstraint *self, GdaSqlStatement *stmt)
{	
	guint n_objects;
	guint i;
	MidgardQuerySimpleConstraint **constraints = 
		midgard_query_simple_constraint_list_constraints (MIDGARD_QUERY_SIMPLE_CONSTRAINT (self), &n_objects);
	if (!constraints)
		return;

	for (i = 0; i < n_objects; i++) {

		MIDGARD_QUERY_SIMPLE_CONSTRAINT_GET_INTERFACE (constraints[i])->priv->add_conditions_to_statement (executor, MIDGARD_QUERY_SIMPLE_CONSTRAINT (constraints[i]), stmt);
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
	self->type = NULL;
	self->constraints = NULL;

	return G_OBJECT(object);
}

static void
_midgard_query_group_constraint_dispose (GObject *object)
{
	MidgardQueryGroupConstraint *self = MIDGARD_QUERY_GROUP_CONSTRAINT (object);	

	parent_class->dispose (object);
}

static void
_midgard_query_group_constraint_finalize (GObject *object)
{
	MidgardQueryGroupConstraint *self = MIDGARD_QUERY_GROUP_CONSTRAINT (object);
	
	g_free (self->type);
	self->type = NULL;

	if (self->constraints)
		g_slist_free (self->constraints);
	self->constraints = NULL; 

	parent_class->finalize;
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

