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

#ifndef MIDGARD_CR_CORE_QUERY_CONSTRAINT_H
#define MIDGARD_CR_CORE_QUERY_CONSTRAINT_H

#include <glib-object.h>
#include "midgard_cr_core_query_holder.h"
#include "midgard_cr_core_query_property.h"
#include "midgard_cr_core_query_storage.h"
#include "midgard_cr_core_query_constraint_simple.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT (midgard_cr_core_query_constraint_get_type()) 
#define MIDGARD_CR_CORE_QUERY_CONSTRAINT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT, MidgardCRCoreQueryConstraint))
#define MIDGARD_CR_CORE_QUERY_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_CR_CORE_QUERY_CONSTRAINT_TYPE, MidgardCRCoreQueryConstraintClass))
#define MIDGARD_CR_CORE_IS_QUERY_CONSTRAINT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_CR_CORE_TYPE_QUERY_CONSTRAINT))
#define MIDGARD_CR_CORE_IS_QUERY_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_CR_CORE_QUERY_CONSTRAINT_TYPE))
#define MIDGARD_CR_CORE_QUERY_CONSTRAINT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_CR_CORE_QUERY_CONSTRAINT_TYPE, MidgardCRCoreQueryConstraintClass))

typedef struct _MidgardCRCoreQueryConstraint MidgardCRCoreQueryConstraint;
typedef struct _MidgardCRCoreQueryConstraintPrivate MidgardCRCoreQueryConstraintPrivate;
typedef struct _MidgardCRCoreQueryConstraintClass MidgardCRCoreQueryConstraintClass;

struct _MidgardCRCoreQueryConstraint {
	GObject parent;

	/* < private > */
	MidgardCRCoreQueryConstraintPrivate *priv;
};	

struct _MidgardCRCoreQueryConstraintClass {
	GObjectClass parent;
	MidgardCRCoreQueryConstraintSimple	**(*list_constraints)	(MidgardCRCoreQueryConstraint *self, guint *n_objects);
};

GType 			midgard_cr_core_query_constraint_get_type	(void);
MidgardCRCoreQueryConstraint  *midgard_cr_core_query_constraint_new 		(MidgardCRCoreQueryProperty *property, const gchar *op, MidgardCRCoreQueryHolder *holder, MidgardCRCoreQueryStorage *storage);
void			midgard_cr_core_query_constraint_get_value	(MidgardCRCoreQueryConstraint *self, GValue *value);
gboolean		midgard_cr_core_query_constraint_set_value   	(MidgardCRCoreQueryConstraint *self, const GValue *value);
MidgardCRCoreQueryStorage	*midgard_cr_core_query_constraint_get_storage	(MidgardCRCoreQueryConstraint *self);
gboolean 		midgard_cr_core_query_constraint_set_storage	(MidgardCRCoreQueryConstraint *self, MidgardCRCoreQueryStorage *storage);
MidgardCRCoreQueryProperty	*midgard_cr_core_query_constraint_get_property	(MidgardCRCoreQueryConstraint *self);
gboolean 		midgard_cr_core_query_constraint_set_property	(MidgardCRCoreQueryConstraint *self, MidgardCRCoreQueryProperty *property);
const gchar 		*midgard_cr_core_query_constraint_get_operator	(MidgardCRCoreQueryConstraint *self);
gboolean		midgard_cr_core_query_constraint_set_operator	(MidgardCRCoreQueryConstraint *self, const gchar *op);

G_END_DECLS

#endif /* MIDGARD_CR_CORE_QUERY_CONSTRAINT_H */
