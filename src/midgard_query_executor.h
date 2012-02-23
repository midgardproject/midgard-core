/* 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_QUERY_EXECUTOR_H
#define MIDGARD_QUERY_EXECUTOR_H

#include <glib-object.h>
#include "midgard_connection.h"
#include "midgard_query_storage.h"
#include "midgard_query_constraint_simple.h"
#include "midgard_query_property.h"
#include "midgard_query_holder.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_EXECUTOR (midgard_query_executor_get_type()) 
#define MIDGARD_QUERY_EXECUTOR(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_EXECUTOR, MidgardQueryExecutor))
#define MIDGARD_QUERY_EXECUTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_QUERY_EXECUTOR, MidgardQueryExecutorClass))
#define MIDGARD_IS_QUERY_EXECUTOR(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_EXECUTOR))
#define MIDGARD_IS_QUERY_EXECUTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_QUERY_EXECUTOR))
#define MIDGARD_QUERY_EXECUTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_QUERY_EXECUTOR, MidgardQueryExecutorClass))

typedef struct _MidgardQueryExecutor MidgardQueryExecutor;
typedef struct _MidgardQueryExecutorClass MidgardQueryExecutorClass;
typedef struct _MidgardQueryExecutorPrivate MidgardQueryExecutorPrivate; 

struct _MidgardQueryExecutorClass {
	GObjectClass parent;

	/* virtual methods */
	gboolean	(*set_constraint)		(MidgardQueryExecutor *self, MidgardQueryConstraintSimple *constraint);
	gboolean	(*set_limit)			(MidgardQueryExecutor *self, guint limit);
	gboolean	(*set_offset)			(MidgardQueryExecutor *self, guint offset);
	gboolean	(*add_order)			(MidgardQueryExecutor *self, MidgardQueryProperty *property, const gchar *type);
	gboolean        (*add_join)			(MidgardQueryExecutor *self, const gchar *join_type, 
								MidgardQueryHolder *left_holder, MidgardQueryHolder *right_holder);
	guint 		(*get_results_count)		(MidgardQueryExecutor *self);

	/* signals */
	void            (*execution_start)      (GObject *object);
	void            (*execution_end)        (GObject *object);

	/* signals IDs */
	guint           signal_id_execution_start;
	guint           signal_id_execution_end;
};

struct _MidgardQueryExecutor {
	GObject parent;

	/* < private > */
	MidgardQueryExecutorPrivate *priv;
};

GType 			midgard_query_executor_get_type		(void);
MidgardQueryExecutor	*midgard_query_executor_new 		(MidgardConnection *mgd, MidgardQueryStorage *storage);
gboolean		midgard_query_executor_set_constraint	(MidgardQueryExecutor *self, MidgardQueryConstraintSimple *constraint);
gboolean		midgard_query_executor_set_limit	(MidgardQueryExecutor *self, guint limit);
gboolean 		midgard_query_executor_set_offset	(MidgardQueryExecutor *self, guint offset);
gboolean		midgard_query_executor_add_order	(MidgardQueryExecutor *self, MidgardQueryProperty *property, const gchar *type);
gboolean		midgard_query_executor_add_join		(MidgardQueryExecutor *self, const gchar *join_type, MidgardQueryHolder *left_holder, MidgardQueryHolder *right_holder);
guint 			midgard_query_executor_get_results_count	(MidgardQueryExecutor *self);

G_END_DECLS

#endif /* MIDGARD_QUERY_EXECUTOR_H */
