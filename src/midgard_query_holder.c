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

#include "midgard_query_holder.h"

struct _MidgardQueryHolder {
	GObject parent;
};

/**
 * midgard_query_holder_get_value:
 * @self: #MidgardQueryHolder instance
 * @value: (out): pointer to store returned value
 *
 * Since: 10.05
 */ 
void
midgard_query_holder_get_value (MidgardQueryHolder *self, GValue *value)
{
	MIDGARD_QUERY_HOLDER_GET_INTERFACE (self)->get_value (self, value);
}

/**
 * midgard_query_holder_set_value:
 * @self: #MidgardQueryHolder instance
 * @value: value to set
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_query_holder_set_value (MidgardQueryHolder *self, const GValue *value)
{
	return MIDGARD_QUERY_HOLDER_GET_INTERFACE (self)->set_value (self, value);
}

/* GOBJECT ROUTINES */

GType
midgard_query_holder_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryHolderIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardQueryHolder", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    	}
    	return type;
}

