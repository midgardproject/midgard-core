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

#include "midgard_query_selector.h"

struct _MidgardQuerySelector {
	GObject parent;
};

/**
 * midgard_query_selector_get_query_result:
 * @self: #MidgardQuerySelector instance
 * @error: pointer to store error 
 *
 * Returns: #MidgardQueryResult or %NULL on failure
 *
 * Since: 10.05.6
 */ 
MidgardQueryResult*
midgard_query_selector_get_query_result (MidgardQuerySelector *self, GError **error)
{
	MIDGARD_QUERY_SELECTOR_GET_INTERFACE (self)->get_query_result (self, error);
}

/**
 * midgard_query_selector_get_connection:
 * @self: #MidgardQuerySelector instance
 *
 * Returns: #MidgardConnection or %NULL
 *
 * Since: 10.05.6
 */ 
MidgardConnection*
midgard_query_selector_get_connection (MidgardQuerySelector *self)
{
	MIDGARD_QUERY_SELECTOR_GET_INTERFACE (self)->get_connection (self);
}

/* GOBJECT ROUTINES */

GType
midgard_query_selector_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQuerySelectorIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardQuerySelector", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    	}
    	return type;
}

