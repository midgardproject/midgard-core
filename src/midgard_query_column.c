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

#include "midgard_query_column.h"

struct _MidgardQueryColumn {
	GObject parent;
};

/**
 * midgard_query_column_get_query_property:
 * @self: #MidgardQueryColumn instance
 * @error: pointer to store error
 *
 * Returns: (transfer full): #MidgardQueryProperty associated with the column or %NULL
 * Since: 10.06
 */ 
MidgardQueryProperty*
midgard_query_column_get_query_property (MidgardQueryColumn *self, GError **error)
{
	MIDGARD_QUERY_COLUMN_GET_INTERFACE (self)->get_query_property (self, error);
}

/**
 * midgard_query_column_get_name:
 * @self: #MidgardQueryColumn instance
 * @error:pointer to store error
 *
 * Returns: (transfer none): the name of the column
 *
 * Since: 10.06
 */ 
gchar*
midgard_query_column_get_name (MidgardQueryColumn *self, GError **error)
{
	return MIDGARD_QUERY_COLUMN_GET_INTERFACE (self)->get_name (self, error);
}

/* GOBJECT ROUTINES */

GType
midgard_query_column_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryColumnIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardQueryColumn", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    	}
    	return type;
}

