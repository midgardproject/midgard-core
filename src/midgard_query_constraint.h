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

#ifndef MIDGARD_QUERY_CONSTRAINT_H
#define MIDGARD_QUERY_CONSTRAINT_H

#include <glib-object.h>
#include "midgard_query_holder.h"
#include "midgard_query_property.h"
#include "midgard_query_storage.h"
#include "midgard_query_simple_constraint.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_QUERY_CONSTRAINT_TYPE (midgard_query_constraint_get_type()) 
#define MIDGARD_QUERY_CONSTRAINT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_CONSTRAINT, MidgardQueryConstraint))
#define MIDGARD_QUERY_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_QUERY_CONSTRAINT_TYPE, MidgardQueryConstraintClass))
#define MIDGARD_IS_QUERY_CONSTRAINT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_CONSTRAINT))
#define MIDGARD_IS_QUERY_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_QUERY_CONSTRAINT_TYPE))
#define MIDGARD_QUERY_CONSTRAINT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_QUERY_CONSTRAINT_TYPE, MidgardQueryConstraintClass))

typedef struct _MidgardQueryConstraint MidgardQueryConstraint; 
typedef struct _MidgardQueryConstraintClass MidgardQueryConstraintClass;

struct _MidgardQueryConstraintClass {
	GObjectClass parent;
	MidgardQuerySimpleConstraint	**(*list_constraints)	(MidgardQueryConstraint *self, guint *n_objects);
};

GType 			midgard_query_constraint_get_type	(void);
MidgardQueryConstraint  *midgard_query_constraint_new 		(MidgardQueryProperty *property, const gchar *op, MidgardQueryHolder *holder, MidgardQueryStorage *storage);
void			midgard_query_constraint_get_value	(MidgardQueryConstraint *self, GValue *value);
gboolean		midgard_query_constraint_set_value   	(MidgardQueryConstraint *self, const GValue *value);
MidgardQueryStorage	*midgard_query_constraint_get_storage	(MidgardQueryConstraint *self);
gboolean 		midgard_query_constraint_set_storage	(MidgardQueryConstraint *self, MidgardQueryStorage *storage);
MidgardQueryProperty	*midgard_query_constraint_get_property	(MidgardQueryConstraint *self);
gboolean 		midgard_query_constraint_set_property	(MidgardQueryConstraint *self, MidgardQueryProperty *property);
const gchar 		*midgard_query_constraint_get_operator	(MidgardQueryConstraint *self);
gboolean		midgard_query_constraint_set_operator	(MidgardQueryConstraint *self, const gchar *op);

G_END_DECLS

#endif /* MIDGARD_QUERY_CONSTRAINT_H */
