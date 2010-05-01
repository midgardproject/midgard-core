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

#ifndef MIDGARD_QUERY_CONSTRAINT_GROUP_H
#define MIDGARD_QUERY_CONSTRAINT_GROUP_H

#include <glib-object.h>
#include "midgard_query_holder.h"
#include "midgard_query_property.h"
#include "midgard_query_storage.h"
#include "midgard_query_constraint.h"
#include "midgard_query_constraint_simple.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_CONSTRAINT_GROUP (midgard_query_constraint_group_get_type()) 
#define MIDGARD_QUERY_CONSTRAINT_GROUP(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object), MIDGARD_TYPE_QUERY_CONSTRAINT_GROUP, MidgardQueryConstraintGroup))
#define MIDGARD_QUERY_CONSTRAINT_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_QUERY_CONSTRAINT_GROUP, MidgardQueryConstraintGroupClass))
#define MIDGARD_IS_QUERY_CONSTRAINT_GROUP(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_CONSTRAINT_GROUP))
#define MIDGARD_IS_QUERY_CONSTRAINT_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_QUERY_CONSTRAINT_GROUP))
#define MIDGARD_QUERY_CONSTRAINT_GROUP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_QUERY_CONSTRAINT_GROUP, MidgardQueryConstraintGroupClass))

typedef struct _MidgardQueryConstraintGroup MidgardQueryConstraintGroup; 
typedef struct _MidgardQueryConstraintGroupPrivate MidgardQueryConstraintGroupPrivate; 
typedef struct _MidgardQueryConstraintGroupClass MidgardQueryConstraintGroupClass;

struct _MidgardQueryConstraintGroup {
	GObject parent;

	/* < private > */
	MidgardQueryConstraintGroupPrivate *priv;
};

struct _MidgardQueryConstraintGroupClass {
	GObjectClass parent;
};

GType 				midgard_query_constraint_group_get_type		(void);
MidgardQueryConstraintGroup  	*midgard_query_constraint_group_new 		(const gchar *type, MidgardQueryConstraintSimple *constraint, ...);
const gchar 			*midgard_query_constraint_group_get_group_type	(MidgardQueryConstraintGroup *self);
gboolean			midgard_query_constraint_group_set_group_type		(MidgardQueryConstraintGroup *self, const gchar *type);
gboolean			midgard_query_constraint_group_add_constraint	(MidgardQueryConstraintGroup *self, MidgardQueryConstraintSimple *constraint, ...);

G_END_DECLS

#endif /* MIDGARD_QUERY_CONSTRAINT_GROUP_H */
