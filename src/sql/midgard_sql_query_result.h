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

#ifndef MIDGARD_SQL_QUERY_RESULT_H
#define MIDGARD_SQL_QUERY_RESULT_H

#include <glib-object.h>
#include "midgard_sql_query_row.h"
#include "midgard_sql_query_column.h"
#include "../midgard_query_selector.h"


G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_SQL_QUERY_RESULT (midgard_sql_query_result_get_type())
#define MIDGARD_SQL_QUERY_RESULT(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_SQL_QUERY_RESULT, MidgardSqlQueryResult))
#define MIDGARD_SQL_QUERY_RESULT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_SQL_QUERY_RESULT, MidgardSqlQueryResultClass))
#define MIDGARD_IS_SQL_QUERY_RESULT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SQL_QUERY_RESULT))
#define MIDGARD_IS_SQL_QUERY_RESULT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_SQL_QUERY_RESULT))
#define MIDGARD_SQL_QUERY_RESULT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_SQL_QUERY_RESULT, MidgardSqlQueryResultClass))

typedef struct _MidgardSqlQueryResult MidgardSqlQueryResult;
typedef struct _MidgardSqlQueryResultClass MidgardSqlQueryResultClass;

struct _MidgardSqlQueryResultClass{
	GObjectClass parent;
};

struct _MidgardSqlQueryResult{
	GObject parent;

	MidgardQuerySelector *selector;
	GObject *model;
	MidgardSqlQueryRow** rows;
	guint n_rows;
	MidgardSqlQueryColumn **columns;	
	guint n_columns;
};

GType 				midgard_sql_query_result_get_type	(void);
MidgardSqlQueryResult*		midgard_sql_query_result_new		(MidgardQuerySelector *selector, GObject *model);
void				midgard_sql_query_result_set_columns	(MidgardSqlQueryResult *self, MidgardSqlQueryColumn **columns, guint n_columns, GError **error);

G_END_DECLS

#endif /* MIDGARD_SQL_QUERY_RESULT_H */
