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

#include "midgard_query_simple_constraint.h"

MidgardQuerySimpleConstraint**
midgard_query_simple_constraint_list_constraints (MidgardQuerySimpleConstraint *self, guint *n_objects)
{
	return MIDGARD_QUERY_SIMPLE_CONSTRAINT_GET_INTERFACE (self)->list_constraints (self, n_objects);
}

static void
simple_constraint_iface_init (gpointer g_iface, gpointer iface_data)
{
	MidgardQuerySimpleConstraintIFace *iface = (MidgardQuerySimpleConstraintIFace *)g_iface;
	iface->list_constraints = NULL;
}


GType
midgard_query_simple_constraint_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardQuerySimpleConstraintIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			NULL,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};

		static const GInterfaceInfo holder_info = {
			(GInterfaceInitFunc) simple_constraint_iface_init,    /* interface_init */
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};
		
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardQuerySimpleConstraint", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}
	return type;
}
