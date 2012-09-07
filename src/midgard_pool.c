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

#include "midgard_pool.h"

/**
 * midgard_pool_get_n_resources:
 * @self: #MidgardPool instance
 *
 * Returns: number of resources available in pool
 *
 * Since: 12.09
 */ 
guint
midgard_pool_get_n_resources (MidgardPool *self)
{
	return MIDGARD_POOL_GET_INTERFACE (self)->get_n_resources (self);
}

/**
 * midgard_pool_get_max_n_resources:
 * @self: #MidgardPool instance
 *
 * Returns: maximum number of resources which can be available in the pool
 *
 * Since: 12.09
 */ 
guint
midgard_pool_get_max_n_resources (MidgardPool *self)
{
	return MIDGARD_POOL_GET_INTERFACE (self)->get_max_n_resources (self);
}

/**
 * midgard_pool_set_max_n_resources:
 * @self: #MidgardPool instance
 * @n_resources: number of resources
 * @error: pointer to store returned error
 *
 * Sets maximum number of resources which can be available in pool
 *
 * Returns: (transfer none): #MidgardPool self reference 
 *
 * Since: 12.09
 */ 
MidgardPool*
midgard_pool_set_max_n_resources (MidgardPool *self, guint n_resources, GError **error)
{
	return MIDGARD_POOL_GET_INTERFACE (self)->set_max_n_resources (self, n_resources, error);
}

/**
 * midgard_pool_push:
 * @self: #MidgardPool instance
 * @object: #GObject to add to pool
 * @error: pointer to store returned error
 *
 * Adds an @object to pool
 *
 * Returns: (transfer none): #MidgardPool self reference 
 *
 * Since: 12.09
 */ 
MidgardPool*
midgard_pool_push (MidgardPool *self, GObject *object, GError **error)
{
	return MIDGARD_POOL_GET_INTERFACE (self)->push (self, object, error);
}

/* GOBJECT ROUTINES */

GType
midgard_pool_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardPoolIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardPool", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    	}
    	return type;
}
