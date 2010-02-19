/* 
 * Copyright (C) 2005 Jukka Zitting <jz@yukatan.fi>
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
#ifndef _GROUP_CONSTRAINT_H
#define _GROUP_CONSTRAINT_H

#include <glib.h>
#include <glib-object.h>
#include "query_constraint.h"

#define MIDGARD_TYPE_GROUP_CONSTRAINT            (midgard_group_constraint_get_type())
#define MIDGARD_GROUP_CONSTRAINT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MIDGARD_TYPE_GROUP_CONSTRAINT, MidgardGroupConstraint))
#define MIDGARD_GROUP_CONSTRAINT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MIDGARD_TYPE_GROUP_CONSTRAINT, MidgardGroupConstraintClass))
#define MIDGARD_IS_GROUP_CONSTRAINT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MIDGARD_TYPE_GROUP_CONSTRAINT))
#define MIDGARD_IS_GROUP_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MIDGARD_TYPE_GROUP_CONSTRAINT))
#define MIDGARD_GROUP_CONSTRAINT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MIDGARD_TYPE_GROUP_CONSTRAINT, MidgardGroupConstraintClass))

typedef struct _MidgardGroupConstraint MidgardGroupConstraint;
typedef struct _MidgardGroupConstraintClass MidgardGroupConstraintClass;

struct _MidgardGroupConstraint{
        MidgardCoreQueryConstraint parent;

        /* private fields */
        gchar *type;
	GSList *constraints;
	GSList *nested;
	MidgardGroupConstraint *parent_group;
};

struct _MidgardGroupConstraintClass{
        MidgardCoreQueryConstraintClass parent;

	void (*add_sql) (MidgardGroupConstraint *group, GString *sql);
};

GType midgard_group_constraint_get_type(void);

MidgardGroupConstraint *midgard_group_constraint_new(const gchar *type);

void midgard_group_constraint_add_constraint(
        MidgardGroupConstraint *group, MidgardCoreQueryConstraint *constraint);

#endif
