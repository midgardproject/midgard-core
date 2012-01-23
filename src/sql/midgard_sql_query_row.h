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

#ifndef MIDGARD_SQL_QUERY_ROW_H
#define MIDGARD_SQL_QUERY_ROW_H

#include <glib-object.h>
#include <libgda/libgda.h>
#include "../midgard_connection.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_SQL_QUERY_ROW (midgard_sql_query_row_get_type())
#define MIDGARD_SQL_QUERY_ROW(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_SQL_QUERY_ROW, MidgardSqlQueryRow))
#define MIDGARD_SQL_QUERY_ROW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_SQL_QUERY_ROW, MidgardSqlQueryRowClass))
#define MIDGARD_IS_SQL_QUERY_ROW(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SQL_QUERY_ROW))
#define MIDGARD_IS_SQL_QUERY_ROW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_SQL_QUERY_ROW))
#define MIDGARD_SQL_QUERY_ROW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_SQL_QUERY_ROW, MidgardSqlQueryRowClass))

typedef struct _MidgardSqlQueryRow MidgardSqlQueryRow;
typedef struct _MidgardSqlQueryRowClass MidgardSqlQueryRowClass;

struct _MidgardSqlQueryRowClass{
	GObjectClass parent;
};

struct _MidgardSqlQueryRow{
	GObject parent;

	MidgardConnection *mgd;
	GObject *model;
	guint row;
};

GType 				midgard_sql_query_row_get_type		(void);
MidgardSqlQueryRow*		midgard_sql_query_row_new		(MidgardConnection *mgd, GObject *model, guint row);

G_END_DECLS

#endif /* MIDGARD_SQL_QUERY_ROW_H */
