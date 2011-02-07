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

#include "midgard_query_constraint_simple.h"
#include "midgard_core_query.h"

/**
 * midgard_query_constraint_simple_list_constraints:
 * @self: #MidgardQueryConstraintSimple instance
 * @n_objects: pointer to store numer of returned objects
 *
 * Returns: (transfer container): array of #MidgardQueryConstraintSimple instances
 *
 * Since: 10.05
 */
MidgardQueryConstraintSimple**
midgard_query_constraint_simple_list_constraints (MidgardQueryConstraintSimple *self, guint *n_objects)
{
	return MIDGARD_QUERY_CONSTRAINT_SIMPLE_GET_INTERFACE (self)->list_constraints (self, n_objects);
}

GType
midgard_query_constraint_simple_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardQueryConstraintSimpleIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,	
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardQueryConstraintSimple", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}
	return type;
}
