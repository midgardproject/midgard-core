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

struct _MidgardQueryConstraint {
	GObject  parent;
	MidgardQueryProperty *property_value;
	const gchar *op;
	MidgardQueryStorage *storage;
	MidgardQueryHolder *holder;
};

MidgardQueryConstraint *
midgard_query_constraint_new (MidgardQueryProperty *property, const gchar *op, 
		MidgardQueryHolder *holder, MidgardQueryStorage *storage)
{
	g_return_val_if_fail (property != NULL, NULL);
	g_return_val_if_fail (op != NULL, NULL);
	g_return_val_if_fail (holder != NULL, NULL);

	MidgardQueryConstraint *self = g_object_new (MIDGARD_QUERY_CONSTRAINT_TYPE, NULL);
	self->property_value = property;
	self->op = op;
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

static void
midgard_query_constraint_init (MidgardQuerySimpleConstraintIFace *iface)
{
	iface->constraints = NULL;
	return;
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
			NULL,   /* class_init */
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
		g_type_add_interface_static (type, MIDGARD_QUERY_SIMPLE_CONSTRAINT_TYPE, &property_info);
    	}
    	return type;
}

