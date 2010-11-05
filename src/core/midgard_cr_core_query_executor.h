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

#ifndef MIDGARD_CR_CORE_QUERY_EXECUTOR_H
#define MIDGARD_CR_CORE_QUERY_EXECUTOR_H

#include <glib-object.h>
//#include "midgard_cr_core_defs.h"
#include "midgard_cr_core_query_storage.h"
#include "midgard_cr_core_query_constraint_simple.h"
#include "midgard_cr_core_query_property.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_CR_CORE_TYPE_QUERY_EXECUTOR (midgard_cr_core_query_executor_get_type()) 
#define MIDGARD_CR_CORE_QUERY_EXECUTOR(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_CR_CORE_TYPE_QUERY_EXECUTOR, MidgardCRCoreQueryExecutor))
#define MIDGARD_CR_CORE_QUERY_EXECUTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_CR_CORE_TYPE_QUERY_EXECUTOR, MidgardCRCoreQueryExecutorClass))
#define MIDGARD_CR_CORE_IS_QUERY_EXECUTOR(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_CR_CORE_TYPE_QUERY_EXECUTOR))
#define MIDGARD_CR_CORE_IS_QUERY_EXECUTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_CR_CORE_TYPE_QUERY_EXECUTOR))
#define MIDGARD_CR_CORE_QUERY_EXECUTOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_CR_CORE_TYPE_QUERY_EXECUTOR, MidgardCRCoreQueryExecutorClass))

typedef struct _MidgardCRCoreQueryExecutor MidgardCRCoreQueryExecutor;
typedef struct _MidgardCRCoreQueryExecutorClass MidgardCRCoreQueryExecutorClass;
typedef struct _MidgardCRCoreQueryExecutorPrivate MidgardCRCoreQueryExecutorPrivate; 

struct _MidgardCRCoreQueryExecutorClass {
	GObjectClass parent;

	/* virtual methods */
	gboolean	(*set_constraint)		(MidgardCRCoreQueryExecutor *self, MidgardCRCoreQueryConstraintSimple *constraint);
	gboolean	(*set_limit)			(MidgardCRCoreQueryExecutor *self, guint limit);
	gboolean	(*set_offset)			(MidgardCRCoreQueryExecutor *self, guint offset);
	gboolean	(*add_order)			(MidgardCRCoreQueryExecutor *self, MidgardCRCoreQueryProperty *property, const gchar *type);
	gboolean        (*add_join)			(MidgardCRCoreQueryExecutor *self, const gchar *join_type, 
								MidgardCRCoreQueryProperty *left_property, MidgardCRCoreQueryProperty *right_property);
	gboolean	(*execute)			(MidgardCRCoreQueryExecutor *self);
	guint 		(*get_results_count)		(MidgardCRCoreQueryExecutor *self);
};

struct _MidgardCRCoreQueryExecutor {
	GObject parent;

	/* < private > */
	MidgardCRCoreQueryExecutorPrivate *priv;
};

GType 			midgard_cr_core_query_executor_get_type		(void);
MidgardCRCoreQueryExecutor	*midgard_cr_core_query_executor_new 		(GObject *manager, MidgardCRCoreQueryStorage *storage);
gboolean		midgard_cr_core_query_executor_set_constraint	(MidgardCRCoreQueryExecutor *self, MidgardCRCoreQueryConstraintSimple *constraint);
gboolean		midgard_cr_core_query_executor_set_limit	(MidgardCRCoreQueryExecutor *self, guint limit);
gboolean 		midgard_cr_core_query_executor_set_offset	(MidgardCRCoreQueryExecutor *self, guint offset);
gboolean		midgard_cr_core_query_executor_add_order	(MidgardCRCoreQueryExecutor *self, MidgardCRCoreQueryProperty *property, const gchar *type);
gboolean		midgard_cr_core_query_executor_add_join		(MidgardCRCoreQueryExecutor *self, const gchar *join_type, MidgardCRCoreQueryProperty *left_property, MidgardCRCoreQueryProperty *right_property);
gboolean		midgard_cr_core_query_executor_execute		(MidgardCRCoreQueryExecutor *self);
guint 			midgard_cr_core_query_executor_get_results_count	(MidgardCRCoreQueryExecutor *self);

G_END_DECLS

#endif /* MIDGARD_CR_CORE_QUERY_EXECUTOR_H */
