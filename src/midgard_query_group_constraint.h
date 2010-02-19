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

#ifndef MIDGARD_QUERY_GROUP_CONSTRAINT_H
#define MIDGARD_QUERY_GROUP_CONSTRAINT_H

#include <glib-object.h>
#include "midgard_query_holder.h"
#include "midgard_query_property.h"
#include "midgard_query_storage.h"
#include "midgard_query_constraint.h"
#include "midgard_query_simple_constraint.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_QUERY_GROUP_CONSTRAINT_TYPE (midgard_query_group_constraint_get_type()) 
#define MIDGARD_QUERY_GROUP_CONSTRAINT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT, MidgardQueryGroupConstraint))
#define MIDGARD_QUERY_GROUP_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_QUERY_GROUP_CONSTRAINT_TYPE, MidgardQueryGroupConstraintClass))
#define MIDGARD_IS_QUERY_GROUP_CONSTRAINT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT))
#define MIDGARD_IS_QUERY_GROUP_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_QUERY_GROUP_CONSTRAINT_TYPE))
#define MIDGARD_QUERY_GROUP_CONSTRAINT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_QUERY_GROUP_CONSTRAINT_TYPE, MidgardQueryGroupConstraintClass))

typedef struct _MidgardQueryGroupConstraint MidgardQueryGroupConstraint; 
typedef struct _MidgardQueryGroupConstraintClass MidgardQueryGroupConstraintClass;

struct _MidgardQueryGroupConstraintClass {
	GObjectClass parent;
};

GType 				midgard_query_group_constraint_get_type		(void);
MidgardQueryGroupConstraint  	*midgard_query_group_constraint_new 		(const gchar *type, MidgardQuerySimpleConstraint *constraint, ...);
const gchar 			*midgard_query_group_constraint_get_group_type	(MidgardQueryGroupConstraint *self);
gboolean			midgard_query_group_constraint_set_group_type		(MidgardQueryGroupConstraint *self, const gchar *type);
gboolean			midgard_query_group_constraint_add_constraint	(MidgardQueryGroupConstraint *self, MidgardQuerySimpleConstraint *constraint);
MidgardQuerySimpleConstraint 	**midgard_query_group_constraint_list_constraints (MidgardQueryGroupConstraint *self, guint *n_objects);

G_END_DECLS

#endif /* MIDGARD_QUERY_GROUP_CONSTRAINT_H */
