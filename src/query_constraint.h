/* 
 * Copyright (C) 2005 Jukka Zitting <jz@yukatan.fi>
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
#ifndef CORE_QUERY_CONSTRAINT_H
#define CORE_QUERY_CONSTRAINT_H

#include "midgard_object.h"
#include "schema.h"

#define MIDGARD_CORE_TYPE_QUERY_CONSTRAINT            (midgard_core_query_constraint_get_type())
#define MIDGARD_CORE_QUERY_CONSTRAINT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MIDGARD_CORE_TYPE_QUERY_CONSTRAINT, MidgardCoreQueryConstraint))
#define MIDGARD_CORE_QUERY_CONSTRAINT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MIDGARD_CORE_TYPE_QUERY_CONSTRAINT, MidgardCoreQueryConstraintClass))
#define MIDGARD_IS_QUERY_CONSTRAINT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MIDGARD_CORE_TYPE_QUERY_CONSTRAINT))
#define MIDGARD_IS_QUERY_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MIDGARD_CORE_TYPE_QUERY_CONSTRAINT))
#define MIDGARD_CORE_QUERY_CONSTRAINT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MIDGARD_CORE_TYPE_QUERY_CONSTRAINT, MidgardCoreQueryConstraintClass))

typedef struct _MidgardCoreQueryConstraint MidgardCoreQueryConstraint;
typedef struct _MidgardCoreQueryConstraintPrivate MidgardCoreQueryConstraintPrivate;

struct _MidgardCoreQueryConstraintPrivate{
        
	GSList *joins; /* list of MidgardCoreQueryConstraintPrivate */
	gchar *condition_operator;
	MgdSchemaPropertyAttr *prop_left;
	MgdSchemaPropertyAttr *prop_right;
	MgdSchemaPropertyAttr *current;
	GValue *value;
	gchar *condition;
	gchar *order_dir;
	GObjectClass *klass;
	GParamSpec *pspec;
	MidgardQueryBuilder *builder;
	const gchar *propname;
};

struct _MidgardCoreQueryConstraint{
        GObject parent;

	MidgardCoreQueryConstraintPrivate *priv;
};

typedef struct {
        GObjectClass parent;
        void (*add_sql)(MidgardCoreQueryConstraint *constraint, GString *sql);
} MidgardCoreQueryConstraintClass;

GType midgard_core_query_constraint_get_type(void);

MidgardCoreQueryConstraint *midgard_core_query_constraint_new(void);

gboolean 	midgard_core_query_constraint_add_operator		(MidgardCoreQueryConstraint *self, const gchar *op);
void 		midgard_core_query_constraint_add_sql			(MidgardCoreQueryConstraint *self, GString *sql);
gboolean 	midgard_core_query_constraint_operator_is_valid		(const gchar *op);
gboolean 	midgard_core_query_constraint_add_value			(MidgardCoreQueryConstraint *self, const GValue *value);
gboolean 	midgard_core_query_constraint_build_condition		(MidgardCoreQueryConstraint *self);
gboolean	midgard_core_query_constraint_parse_property		(MidgardCoreQueryConstraint **self, MidgardDBObjectClass *klass, const gchar *name);
void		midgard_core_query_constraint_set_builder		(MidgardCoreQueryConstraint *self, MidgardQueryBuilder *builder);
void		midgard_core_query_constraint_set_class			(MidgardCoreQueryConstraint *self, MidgardDBObjectClass *klass);

/* PRIVATE */
MidgardCoreQueryConstraintPrivate *midgard_core_query_constraint_private_new(void);
void midgard_core_query_constraint_private_free(MidgardCoreQueryConstraintPrivate *mqcp);

#endif /* CORE_QUERY_CONSTRAINT_H */
