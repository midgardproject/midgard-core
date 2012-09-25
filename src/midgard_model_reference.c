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

#include "midgard_model_reference.h"
#include "midgard_model.h"

/**
 * midgard_model_reference_get_id:
 * @self: #MidgardModelReference instance
 * @error: pointer to store returned error
 *
 * Returns: Unique id of the model, or NULL in case of error
 *
 * Since: 12.09
 */ 
const gchar*
midgard_model_reference_get_id (MidgardModelReference *self, GError **error)
{
	return MIDGARD_MODEL_REFERENCE_GET_INTERFACE (self)->get_id (self, error);
}

/**
 * midgard_model_reference_get_id_value:
 * @self: #MidgardModelReference instance
 * @error: pointer to store returned error
 *
 * The purpose of this method is to return generic GValue which can hold either 
 * string or integer id. 
 *
 * Returns: New GValue which holds id of the model, or NULL in case of error.
 *
 * Since: 12.09
 */ 
GValue*
midgard_model_reference_get_id_value (MidgardModelReference *self, GError **error)
{
	return MIDGARD_MODEL_REFERENCE_GET_INTERFACE (self)->get_id_value (self, error);
}

/* GOBJECT ROUTINES */

GType
midgard_model_reference_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardModelReferenceIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardModelReference", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
		g_type_interface_add_prerequisite (type, MIDGARD_TYPE_MODEL);
    	}
    	return type;
}
