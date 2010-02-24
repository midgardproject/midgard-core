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

MidgardQuerySimpleConstraint**
_midgard_query_group_constraint_list_constraints (MidgardQueryGroupConstraint *self, guint *n_objects)
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

static void
midgard_query_group_constraint_init (MidgardQuerySimpleConstraintIFace *iface)
{
	iface->list_constraints = _midgard_query_group_constraint_list_constraints;
	return;
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
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardQueryGroupConstraint),
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
      
		static const GInterfaceInfo property_info = {
			(GInterfaceInitFunc) midgard_query_group_constraint_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryGroupConstraint", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_SIMPLE_CONSTRAINT, &property_info);
    	}
    	return type;
}

