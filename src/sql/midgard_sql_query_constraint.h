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

#ifndef MIDGARD_SQL_QUERY_CONSTRAINT_H
#define MIDGARD_SQL_QUERY_CONSTRAINT_H

#include <glib-object.h>
#include "../midgard_query_holder.h"
#include "../midgard_query_constraint_simple.h"
#include "midgard_sql_query_column.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_SQL_QUERY_CONSTRAINT (midgard_sql_query_constraint_get_type()) 
#define MIDGARD_SQL_QUERY_CONSTRAINT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_SQL_QUERY_CONSTRAINT, MidgardSqlQueryConstraint))
#define MIDGARD_SQL_QUERY_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_SQL_QUERY_CONSTRAINT_TYPE, MidgardSqlQueryConstraintClass))
#define MIDGARD_IS_SQL_QUERY_CONSTRAINT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SQL_QUERY_CONSTRAINT))
#define MIDGARD_IS_SQL_QUERY_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_SQL_QUERY_CONSTRAINT_TYPE))
#define MIDGARD_SQL_QUERY_CONSTRAINT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_SQL_QUERY_CONSTRAINT_TYPE, MidgardSqlQueryConstraintClass))

typedef struct _MidgardSqlQueryConstraint MidgardSqlQueryConstraint;
typedef struct _MidgardSqlQueryConstraintPrivate MidgardSqlQueryConstraintPrivate;
typedef struct _MidgardSqlQueryConstraintClass MidgardSqlQueryConstraintClass;

struct _MidgardSqlQueryConstraint {
	GObject parent;

	/* < private > */
	MidgardSqlQueryConstraintPrivate *priv;
};	

struct _MidgardSqlQueryConstraintClass {
	GObjectClass parent;
	MidgardQueryConstraintSimple	**(*list_constraints)	(MidgardSqlQueryConstraint *self, guint *n_objects);
};

GType 				midgard_sql_query_constraint_get_type	(void);
MidgardSqlQueryConstraint*	midgard_sql_query_constraint_new 		(MidgardSqlQueryColumn *column, const gchar *op, MidgardQueryHolder *holder);
MidgardSqlQueryColumn*		midgard_sql_query_constraint_get_column		(MidgardSqlQueryConstraint *self);
void	 			midgard_sql_query_constraint_set_column		(MidgardSqlQueryConstraint *self, MidgardSqlQueryColumn *column, GError **error);
const gchar*			midgard_sql_query_constraint_get_operator	(MidgardSqlQueryConstraint *self);
void				midgard_sql_query_constraint_set_operator	(MidgardSqlQueryConstraint *self, const gchar *op, GError **error);
MidgardQueryHolder*		midgard_sql_query_constraint_get_holder		(MidgardSqlQueryConstraint *self);
void	 			midgard_sql_query_constraint_set_holder		(MidgardSqlQueryConstraint *self, MidgardQueryHolder *holder, GError **error);

G_END_DECLS

#endif /* MIDGARD_SQL_QUERY_CONSTRAINT_H */
