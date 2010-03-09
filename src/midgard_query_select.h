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

#ifndef MIDGARD_QUERY_SELECT_H
#define MIDGARD_QUERY_SELECT_H

#include <glib-object.h>
#include "midgard_connection.h"
#include "midgard_query_storage.h"
#include "midgard_query_executor.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_SELECT (midgard_query_select_get_type()) 
#define MIDGARD_QUERY_SELECT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_SELECT, MidgardQuerySelect))
#define MIDGARD_QUERY_SELECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_QUERY_SELECT, MidgardQuerySelectClass))
#define MIDGARD_IS_QUERY_SELECT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_SELECT))
#define MIDGARD_IS_QUERY_SELECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_QUERY_SELECT))
#define MIDGARD_QUERY_SELECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_QUERY_SELECT, MidgardQuerySelectClass))

typedef struct _MidgardQuerySelect MidgardQuerySelect;
typedef struct _MidgardQuerySelectClass MidgardQuerySelectClass;
typedef struct _MidgardQuerySelectPrivate MidgardQuerySelectPrivate; 

struct _MidgardQuerySelectClass {
	GObjectClass parent;

	/* virtual methods */
	gboolean	(*set_constraint)		(MidgardQuerySelect *self, MidgardQuerySimpleConstraint *constraint);
	gboolean	(*set_limit)			(MidgardQuerySelect *self, guint limit);
	gboolean	(*set_offset)			(MidgardQuerySelect *self, guint offset);
	gboolean	(*add_order)			(MidgardQuerySelect *self, MidgardQueryProperty *property, const gchar *type);
	gboolean        (*add_join)			(MidgardQuerySelect *self, const gchar *join_type, 
								MidgardQueryProperty *left_property, MidgardQueryProperty *right_property);
	gboolean	(*execute)			(MidgardQuerySelect *self);
	guint           (*get_results_count)            (MidgardQuerySelect *self);
	MidgardDBObject	**(*list_objects)		(MidgardQuerySelect *self);
};

struct _MidgardQuerySelect {
	GObject parent;

	/* < private > */
	MidgardQueryExecutorPrivate *priv;
};

GType 			midgard_query_select_get_type		(void);
MidgardQuerySelect	*midgard_query_select_new 		(MidgardConnection *mgd, MidgardQueryStorage *storage);
MidgardDBObject		**midgard_query_select_list_objects	(MidgardQuerySelect *self);

G_END_DECLS

#endif /* MIDGARD_QUERY_SELECT_H */
