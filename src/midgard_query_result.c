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

#include "midgard_query_result.h"

struct _MidgardQueryResult {
	GObject parent;
};

/**
 * midgard_query_result_get_objects:
 * @self: #MidgardQueryResult instance
 * @n_objects: pointer to store number of returned objects
 * @error: pointer to store returned error
 *
 * Returns: (element-type GObject) (array length=n_objects) (transfer full): array of objects available in this result or %NULL
 *
 * Since: 10.06
 */ 
GObject**
midgard_query_result_get_objects (MidgardQueryResult *self, guint *n_objects, GError **error)
{
	MIDGARD_QUERY_RESULT_GET_INTERFACE (self)->get_objects (self, n_objects, error);
}

/**
 * midgard_query_result_get_columns:
 * @self: #MidgardQueryResult instance
 * @n_objects: pointer to store number of returned objects
 * @error: pointer to store returned error
 *
 * Returns: (element-type MidgardQueryColumn) (array length=n_objects) (transfer full): array of #MidgardQueryColumn columns available in this result or %NULL
 *
 * Since: 10.06
 */ 
MidgardQueryColumn**
midgard_query_result_get_columns (MidgardQueryResult *self, guint *n_objects, GError **error)
{
	MIDGARD_QUERY_RESULT_GET_INTERFACE (self)->get_columns (self, n_objects, error);
}

/**
 * midgard_query_result_get_rows:
 * @self: #MidgardQueryResult instance
 * @n_objects: pointer to store number of returned objects
 * @error: pointer to store returned error
 *
 * Returns: (element-type MidgardQueryRow) (array length=n_objects) (transfer full): array of #MidgardQueryRow rows available in this result or %NULL
 *
 * Since: 10.06
 */ 
MidgardQueryRow**
midgard_query_result_get_rows (MidgardQueryResult *self, guint *n_objects, GError **error)
{
	MIDGARD_QUERY_RESULT_GET_INTERFACE (self)->get_rows (self, n_objects, error);
}

/**
 * midgard_query_result_get_column_names:
 * @self: #MidgardQueryResult instance
 * @n_names: pointer to store number of returned names
 * @error: pointer to store returned error
 *
 * Returns: (transfer container) (array length=n_names): array of column names available in this result or %NULL
 *
 * Since: 10.06
 */ 
gchar**
midgard_query_result_get_column_names (MidgardQueryResult *self, guint *n_names, GError **error)
{
	MIDGARD_QUERY_RESULT_GET_INTERFACE (self)->get_column_names (self, n_names, error);
}

/* GOBJECT ROUTINES */

GType
midgard_query_result_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryResultIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardQueryResult", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    	}
    	return type;
}
