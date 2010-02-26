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

#ifndef MIDGARD_QUERY_SIMPLE_CONSTRAINT_H
#define MIDGARD_QUERY_SIMPLE_CONSTRAINT_H

#include <glib-object.h>

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_SIMPLE_CONSTRAINT (midgard_query_simple_constraint_get_type()) 
#define MIDGARD_QUERY_SIMPLE_CONSTRAINT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_SIMPLE_CONSTRAINT, MidgardQuerySimpleConstraint))
#define MIDGARD_IS_QUERY_SIMPLE_CONSTRAINT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_SIMPLE_CONSTRAINT))
#define MIDGARD_QUERY_SIMPLE_CONSTRAINT_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_QUERY_SIMPLE_CONSTRAINT , MidgardQuerySimpleConstraintIFace))

typedef struct _MidgardQuerySimpleConstraint MidgardQuerySimpleConstraint; /* dummy */
typedef struct _MidgardQuerySimpleConstraintIFace MidgardQuerySimpleConstraintIFace;
typedef struct _MidgardQuerySimpleConstraintPrivate MidgardQuerySimpleConstraintPrivate;

struct _MidgardQuerySimpleConstraintIFace {
	GTypeInterface	parent;
	MidgardQuerySimpleConstraint	**(*list_constraints)	(MidgardQuerySimpleConstraint *self, guint *n_objects); 
	MidgardQuerySimpleConstraintPrivate *priv;
};

GType 				midgard_query_simple_constraint_get_type		(void);
MidgardQuerySimpleConstraint	**midgard_query_simple_constraint_list_constraints 	(MidgardQuerySimpleConstraint *self, guint *n_objects);

G_END_DECLS

#endif /* MIDGARD_QUERY_SIMPLE_CONSTRAINT_H */
