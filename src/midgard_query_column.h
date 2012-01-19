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

#ifndef MIDGARD_QUERY_COLUMN_H
#define MIDGARD_QUERY_COLUMN_H

#include <glib-object.h>
#include "midgard_query_column.h"
#include "midgard_query_row.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_COLUMN (midgard_query_select_get_type()) 
#define MIDGARD_QUERY_COLUMN(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_COLUMN, MidgardQueryColumn))
#define MIDGARD_IS_QUERY_COLUMN(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_COLUMN))
#define MIDGARD_QUERY_COLUMN_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_QUERY_COLUMN, MidgardQueryColumnIFace))

typedef struct _MidgardQueryColumn MidgardQueryColumn;
typedef struct _MidgardQueryColumnIFace MidgardQueryColumnIFace;

struct _MidgardQueryColumnIface {
	GTypeInterface parent;

	MidgardQueryProperty*	(*get_query_property)	(MidgardQueryColumn *self);
	gchar*			(*get_name)		(MidgardQueryColumn *self);
};

GType 			midgard_query_column_get_type		(void);
MidgardQueryProperty*	midgard_query_column_get_query_property	(MidgardQueryColumn *self);
gchar*			midgard_query_column_get_name		(MidgardQueryColumn *self);

G_END_DECLS

#endif /* MIDGARD_QUERY_COLUMN_H */
