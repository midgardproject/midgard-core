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

#include "midgard_query_row.h"

struct _MidgardQueryRow {
	GObject parent;
};

/**
 * midgard_query_row_get_value:
 * @self: #MidgardQueryRow instance
 * @column_name: (allow-none): Column name which holds value
 * @error: pointer to store error
 *
 * Returns value for specified column.
 * @column_name can be ommited if column name is uniqe in the #MidgardQueryResult
 *
 * Since: 10.05.6
 */ 
const GValue*
midgard_query_row_get_value (MidgardQueryRow *self, const gchar *column_name, GError **error)
{
	MIDGARD_QUERY_ROW_GET_INTERFACE (self)->get_value (self, column_name, error);
}

/**
 * midgard_query_row_get_values:
 * @self: #MidgardQueryRow instance
 * @error: a pointer to store error
 *
 * Returns array of values stored in a row or %NULL.
 *
 * Since: 10.05.6
 */ 
GValueArray*
midgard_query_row_get_values (MidgardQueryRow *self, GError **error)
{
	MIDGARD_QUERY_ROW_GET_INTERFACE (self)->get_values (self, error);
}

/**
 * midgard_query_row_get_object:
 * @self: #MidgardQueryRow instance
 * @column_name: (allow-none): Column name which holds value
 * @error: pointer to store error
 *
 * Returns an object for specified column.
 * @column_name can be ommited if column name is uniqe in the #MidgardQueryResult
 *
 * Since: 10.05.6
 */ 
GObject*
midgard_query_row_get_object (MidgardQueryRow *self, const gchar *column_name, GError **error)
{
	MIDGARD_QUERY_ROW_GET_INTERFACE (self)->get_object (self, column_name, error);
}


/* GOBJECT ROUTINES */

GType
midgard_query_row_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryRowIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardQueryRow", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    	}
    	return type;
}

