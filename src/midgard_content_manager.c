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

#include "midgard_content_manager.h"

/**
 * midgard_content_manager_error_quark: (skip)
 * 
 * Returns: MIDGARD_CONTENT_MANAGER_ERROR quark
 */
GQuark
midgard_content_manager_error_quark (void)
{
	static GQuark q = 0;
	if (q == 0)
		q = g_quark_from_static_string ("midgard-content-manager-error-quark");
	return q;
}

struct _MidgardContentManager {
	GObject parent;
};

/**
 * midgard_content_manager_get_connection:
 * @self: #MidgardContentManager instance
 * @error: pointer to store returned error
 *
 * Returns: (transfer full): #MidgardConnection or %NULL 
 *
 * Since: 10.05.8
 */ 
MidgardConnection*
midgard_content_manager_get_connection (MidgardContentManager *self, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->get_connection (self, error);
}

/**
 * midgard_content_manager_create_job:
 * @self: #MidgardContentManager instance
 * @type: #MidgardContentManagerJobType
 * @content: #GObject object which holds content data
 * @reference: #MidgardObjectReference instance
 * @model: (allow-none): #MidgardModel instance
 * @error: pointer to store returned error
 *
 * Returns: (transfer full): #MidgardContentManagerJob job or %NULL on failure 
 *
 * Since: 10.05.8
 */ 
MidgardContentManagerJob*
midgard_content_manager_create_job (MidgardContentManager *self, MidgardContentManagerJobType type, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_GET_INTERFACE (self)->create_job (self, type, content, reference, model, error);
}

/* GOBJECT ROUTINES */

GType
midgard_content_manager_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardContentManagerIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardContentManager", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    	}
    	return type;
}
