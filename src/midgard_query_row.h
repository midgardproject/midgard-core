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

#ifndef MIDGARD_QUERY_ROW_H
#define MIDGARD_QUERY_ROW_H

#include <glib-object.h>

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_ROW (midgard_query_row_get_type()) 
#define MIDGARD_QUERY_ROW(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_ROW, MidgardQueryRow))
#define MIDGARD_IS_QUERY_ROW(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_ROW))
#define MIDGARD_QUERY_ROW_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_QUERY_ROW, MidgardQueryRowIFace))

typedef struct _MidgardQueryRow MidgardQueryRow;
typedef struct _MidgardQueryRowIFace MidgardQueryRowIFace;

struct _MidgardQueryRowIFace {
	GTypeInterface parent;

	const GValue*		(*get_value)		(MidgardQueryRow *self, const gchar *column_name, GError **error);
	GValueArray*	(*get_values)		(MidgardQueryRow *self);
	GObject*	(*get_object)		(MidgardQueryRow *self, const gchar *column_name, GError **error);
};

GType 			midgard_query_row_get_type		(void);
const GValue*		midgard_query_row_get_value		(MidgardQueryRow *self, const gchar *column_name, GError **error);
GValueArray*		midgard_query_row_get_values		(MidgardQueryRow *self);
GObject*		midgard_query_row_get_object		(MidgardQueryRow *self, const gchar *column_name, GError **error);

G_END_DECLS

#endif /* MIDGARD_QUERY_ROW_H */
