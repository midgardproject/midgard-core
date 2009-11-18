/* 
 * Copyright (C) 2007 Piotr Pokora <piotrek.pokora@gmail.com>
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
#ifndef QUERY_GROUP_CONSTRAINT_H
#define QUERY_GROUP_CONSTRAINT_H

#include "midgard_object.h"
#include "schema.h"

#define MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT            (midgard_query_group_constraint_get_type())
#define MIDGARD_QUERY_GROUP_CONSTRAINT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT, MidgardQueryGroupConstraint))
#define MIDGARD_QUERY_GROUP_CONSTRAINT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT, MidgardQueryGroupConstraintClass))
#define MIDGARD_IS_QUERY_GROUP_CONSTRAINT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT))
#define MIDGARD_IS_QUERY_GROUP_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT))
#define MIDGARD_QUERY_GROUP_CONSTRAINT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MIDGARD_TYPE_QUERY_GROUP_CONSTRAINT, MidgardQueryGroupConstraintClass))

typedef struct _MidgardQueryGroupConstraint MidgardQueryGroupConstraint;
typedef struct _MidgardQueryGroupConstraintPrivate MidgardQueryGroupConstraintPrivate;

struct _MidgardQueryGroupConstraintPrivate{
        
	GSList *constraints;
	gchar *group_type;
};

struct _MidgardQueryGroupConstraint{
        GObject parent;

	MidgardQueryGroupConstraintPrivate *priv;
};

typedef struct {
        GObjectClass parent;
        void (*add_sql)(MidgardQueryGroupConstraint *self, GString *sql);
} MidgardQueryGroupConstraintClass;

GType midgard_query_group_constraint_get_type(void);

MidgardQueryGroupConstraint *midgard_query_group_constraint_new(void);

void midgard_query_group_constraint_add_sql(
        MidgardQueryGroupConstraint *self, GString *sql);

gboolean midgard_query_group_constraint_type_is_valid(const gchar *type);

/* PRIVATE */
MidgardQueryGroupConstraintPrivate *midgard_query_group_constraint_private_new(void);
void midgard_query_group_constraint_private_free(MidgardQueryGroupConstraintPrivate *mqcp);

#endif /* QUERY_CONSTRAINT_H */
