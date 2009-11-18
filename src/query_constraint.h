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
#ifndef QUERY_CONSTRAINT_H
#define QUERY_CONSTRAINT_H

#include "midgard_object.h"
#include "schema.h"

#define MIDGARD_TYPE_QUERY_CONSTRAINT            (midgard_query_constraint_get_type())
#define MIDGARD_QUERY_CONSTRAINT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), MIDGARD_TYPE_QUERY_CONSTRAINT, MidgardQueryConstraint))
#define MIDGARD_QUERY_CONSTRAINT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), MIDGARD_TYPE_QUERY_CONSTRAINT, MidgardQueryConstraintClass))
#define MIDGARD_IS_QUERY_CONSTRAINT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), MIDGARD_TYPE_QUERY_CONSTRAINT))
#define MIDGARD_IS_QUERY_CONSTRAINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MIDGARD_TYPE_QUERY_CONSTRAINT))
#define MIDGARD_QUERY_CONSTRAINT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), MIDGARD_TYPE_QUERY_CONSTRAINT, MidgardQueryConstraintClass))

typedef struct _MidgardQueryConstraint MidgardQueryConstraint;
typedef struct _MidgardQueryConstraintPrivate MidgardQueryConstraintPrivate;

struct _MidgardQueryConstraintPrivate{
        
	GSList *joins; /* list of MidgardQueryConstraintPrivate */
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

struct _MidgardQueryConstraint{
        GObject parent;

	MidgardQueryConstraintPrivate *priv;
};

typedef struct {
        GObjectClass parent;
        void (*add_sql)(MidgardQueryConstraint *constraint, GString *sql);
} MidgardQueryConstraintClass;

GType midgard_query_constraint_get_type(void);

MidgardQueryConstraint *midgard_query_constraint_new(void);

gboolean 	midgard_query_constraint_add_operator		(MidgardQueryConstraint *self, const gchar *op);
void 		midgard_query_constraint_add_sql		(MidgardQueryConstraint *self, GString *sql);
gboolean 	midgard_query_constraint_operator_is_valid	(const gchar *op);
gboolean 	midgard_query_constraint_add_value		(MidgardQueryConstraint *self, const GValue *value);
gboolean 	midgard_query_constraint_build_condition	(MidgardQueryConstraint *self);
gboolean	midgard_query_constraint_parse_property		(MidgardQueryConstraint **self, MidgardDBObjectClass *klass, const gchar *name);
void		midgard_query_constraint_set_builder		(MidgardQueryConstraint *self, MidgardQueryBuilder *builder);
void		midgard_query_constraint_set_class		(MidgardQueryConstraint *self, MidgardDBObjectClass *klass);

/* PRIVATE */
MidgardQueryConstraintPrivate *midgard_query_constraint_private_new(void);
void midgard_query_constraint_private_free(MidgardQueryConstraintPrivate *mqcp);

#endif /* QUERY_CONSTRAINT_H */
