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

#ifndef MIDGARD_QUERY_RESULT_H
#define MIDGARD_QUERY_RESULT_H

#include <glib-object.h>
#include "midgard_query_column.h"
#include "midgard_query_row.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_RESULT (midgard_query_select_get_type()) 
#define MIDGARD_QUERY_RESULT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_RESULT, MidgardQueryResult))
#define MIDGARD_IS_QUERY_RESULT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_RESULT))
#define MIDGARD_QUERY_RESULT_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_QUERY_RESULT, MidgardQueryResultIFace))

typedef struct _MidgardQueryResult MidgardQueryResult;
typedef struct _MidgardQueryResultIFace MidgardQueryResultIFace;

struct _MidgardQueryResultIFace {
	GTypeInterface parent;

	GObject**		(*get_objects)		(MidgardQueryResult *self, guint *n_objects);
	MidgardQueryColumn**	(*get_columns)		(MidgardQueryResult *self, guint *n_objects);
	MidgardQueryRow**	(*get_rows)		(MidgardQueryResult *self, guint *n_objects);
	gchar**			(*get_column_names)	(MidgardQueryResult *self, guint *n_names);
};

GType 			midgard_query_result_get_type		(void);
GObject**		midgard_query_result_get_objects	(MidgardQueryResult *self, guint *n_objects);
MidgardQueryColumn**	midgard_query_result_get_columns	(MidgardQueryResult *self, guint *n_objects);
MidgardQueryRow**	midgard_query_result_get_rows		(MidgardQueryResult *self, guint *n_objects);
gchar**			midgard_query_result_get_column_names	(MidgardQueryResult *self, guint *n_names);

G_END_DECLS

#endif /* MIDGARD_QUERY_RESULT_H */
