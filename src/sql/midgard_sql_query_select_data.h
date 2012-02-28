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

#ifndef MIDGARD_SQL_QUERY_SELECT_DATA_H
#define MIDGARD_SQL_QUERY_SELECT_DATA_H

#include "../midgard_connection.h"
#include "../midgard_query_executor.h"
#include "midgard_sql_query_column.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_SQL_QUERY_SELECT_DATA (midgard_sql_query_select_data_get_type())
#define MIDGARD_SQL_QUERY_SELECT_DATA(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_SQL_QUERY_SELECT_DATA, MidgardSqlQuerySelectData))
#define MIDGARD_SQL_QUERY_SELECT_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_SQL_QUERY_SELECT_DATA, MidgardSqlQuerySelectDataClass))
#define MIDGARD_IS_SQL_QUERY_SELECT_DATA(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SQL_QUERY_SELECT_DATA))
#define MIDGARD_IS_SQL_QUERY_SELECT_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_SQL_QUERY_SELECT_DATA))
#define MIDGARD_SQL_QUERY_SELECT_DATA_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_SQL_QUERY_SELECT_DATA, MidgardSqlQuerySelectDataClass))

typedef struct _MidgardSqlQuerySelectData MidgardSqlQuerySelectData;
typedef struct _MidgardSqlQuerySelectDataClass MidgardSqlQuerySelectDataClass;

struct _MidgardSqlQuerySelectDataClass{
	MidgardQueryExecutorClass parent;
};

struct _MidgardSqlQuerySelectData{
	MidgardQueryExecutor parent;

	GSList *columns;
	gchar *query_string;
};

GType 				midgard_sql_query_select_data_get_type		(void);
MidgardSqlQuerySelectData*	midgard_sql_query_select_data_new		(MidgardConnection *mgd);
void				midgard_sql_query_select_data_add_column	(MidgardSqlQuerySelectData *self, MidgardSqlQueryColumn *column);
MidgardSqlQueryColumn**		midgard_sql_query_select_data_get_columns	(MidgardSqlQuerySelectData *self, guint *n_objects, GError **error);

G_END_DECLS

#endif /* MIDGARD_SQL_QUERY_SELECT_DATA_H */
