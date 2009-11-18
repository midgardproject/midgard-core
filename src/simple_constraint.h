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
#ifndef _SIMPLE_CONSTRAINT_H
#define _SIMPLE_CONSTRAINT_H

#include <glib.h>
#include <glib-object.h>
#include "query_constraint.h"
#include "midgard_defs.h"
#include "midgard_object.h"

#define MIDGARD_TYPE_SIMPLE_CONSTRAINT            (midgard_simple_constraint_get_type())
#define MIDGARD_SIMPLE_CONSTRAINT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MIDGARD_TYPE_SIMPLE_CONSTRAINT, MidgardSimpleConstraint))
#define MIDGARD_SIMPLE_CONSTRAINT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MIDGARD_TYPE_SIMPLE_CONSTRAINT, MidgardSimpleConstraintClass))
#define MIDGARD_IS_SIMPLE_CONSTRAINT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MIDGARD_TYPE_SIMPLE_CONSTRAINT))
#define MIDGARD_IS_SIMPLE_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MIDGARD_TYPE_SIMPLE_CONSTRAINT))
#define MIDGARD_SIMPLE_CONSTRAINT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MIDGARD_TYPE_SIMPLE_CONSTRAINT, MidgardSimpleConstraintClass))

typedef struct {
        MidgardQueryConstraint parent;

        /* private fields */
        const MidgardConnection *mgd;
        const GParamSpec *spec;
        const gchar *op;
        GValue *value;
	GObjectClass *klass;
	GObjectClass *initial_klass;
	GType ext_type;
	GType parent_type;
	GType initial_type;
	/* Initial klass initizlized with QB must be known all the time */
	GList *types;
} MidgardSimpleConstraint;

typedef struct {
        MidgardQueryConstraintClass parent;
} MidgardSimpleConstraintClass;

GType midgard_simple_constraint_get_type(void);

MidgardSimpleConstraint *midgard_simple_constraint_new(
        MidgardConnection *mgd, GObjectClass *klass,
        const gchar *name, const gchar *op, const GValue *value);

#endif
