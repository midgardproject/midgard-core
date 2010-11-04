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

#ifndef MIDGARD_CR_CORE_QUERY_CONSTRAINT_GROUP_H
#define MIDGARD_CR_CORE_QUERY_CONSTRAINT_GROUP_H

#include <glib-object.h>
#include "midgard_cr_core_query_holder.h"
#include "midgard_cr_core_query_property.h"
#include "midgard_cr_core_query_storage.h"
#include "midgard_cr_core_query_constraint.h"
#include "midgard_cr_core_query_constraint_simple.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT_GROUP (midgard_cr_core_query_constraint_group_get_type()) 
#define MIDGARD_CR_CORE_QUERY_CONSTRAINT_GROUP(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object), MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT_GROUP, MidgardCRCoreQueryConstraintGroup))
#define MIDGARD_CR_CORE_QUERY_CONSTRAINT_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT_GROUP, MidgardCRCoreQueryConstraintGroupClass))
#define MIDGARD_CR_CORE_IS_QUERY_CONSTRAINT_GROUP(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT_GROUP))
#define MIDGARD_CR_CORE_IS_QUERY_CONSTRAINT_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT_GROUP))
#define MIDGARD_CR_CORE_QUERY_CONSTRAINT_GROUP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT_GROUP, MidgardCRCoreQueryConstraintGroupClass))

typedef struct _MidgardCRCoreQueryConstraintGroup MidgardCRCoreQueryConstraintGroup; 
typedef struct _MidgardCRCoreQueryConstraintGroupPrivate MidgardCRCoreQueryConstraintGroupPrivate; 
typedef struct _MidgardCRCoreQueryConstraintGroupClass MidgardCRCoreQueryConstraintGroupClass;

struct _MidgardCRCoreQueryConstraintGroup {
	GObject parent;

	/* < private > */
	MidgardCRCoreQueryConstraintGroupPrivate *priv;
};

struct _MidgardCRCoreQueryConstraintGroupClass {
	GObjectClass parent;
};

GType 				midgard_cr_core_query_constraint_group_get_type		(void);
MidgardCRCoreQueryConstraintGroup  	*midgard_cr_core_query_constraint_group_new 		(void);
MidgardCRCoreQueryConstraintGroup  	*midgard_cr_core_query_constraint_group_new_valist	(const gchar *type, MidgardCRCoreQueryConstraintSimple *constraint, ...);
MidgardCRCoreQueryConstraintGroup  	*midgard_cr_core_query_constraint_group_new_with_constraints	(const gchar *type, MidgardCRCoreQueryConstraintSimple **constraints, guint n_constraints);
const gchar 			*midgard_cr_core_query_constraint_group_get_group_type	(MidgardCRCoreQueryConstraintGroup *self);
gboolean			midgard_cr_core_query_constraint_group_set_group_type		(MidgardCRCoreQueryConstraintGroup *self, const gchar *type);
gboolean			midgard_cr_core_query_constraint_group_add_constraint	(MidgardCRCoreQueryConstraintGroup *self, MidgardCRCoreQueryConstraintSimple *constraint, ...);

G_END_DECLS

#endif /* MIDGARD_CR_CORE_QUERY_CONSTRAINT_GROUP_H */
