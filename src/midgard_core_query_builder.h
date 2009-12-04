/* 
 * Copyright (C) 2006 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_CORE_QB_H
#define MIDGARD_CORE_QB_H

#include "midgard_object.h"
#include "midgard_collector.h"
#include "schema.h"
#include "query_constraint.h"
#include "query_order.h"
#include "query_group_constraint.h"
#include "group_constraint.h"

struct _MidgardQueryBuilderPrivate {
	
	GSList *constraints;
	GSList *groups;
	gboolean is_grouping;
	guint grouping_ref;
	MidgardGroupConstraint *group_constraint;
	GSList *joins;
	GHashTable *tables;
	GSList *orders;
	GValue *value;

	/* constants */
	MidgardConnection *mgd;
	MgdSchemaTypeAttr *schema;
	GType type;

	guint offset;
	guint limit;
	gboolean include_deleted;
	gboolean select_distinct;
	gint error;

	/* references for reserved joins */
	gboolean param_join_exists;
	gboolean blob_join_exists;
};

MidgardQueryBuilderPrivate *midgard_query_builder_private_new(void);
void midgard_query_builder_private_free(MidgardQueryBuilderPrivate *mqbp);

#define MQB_SELECT_OBJECT 0 
#define MQB_SELECT_GUID 1
#define MQB_SELECT_FIELD 2
#define MQB_SELECT_COUNT  3

void			midgard_core_qb_add_table		(MidgardQueryBuilder *builder, const gchar *table);
gchar 			*midgard_core_qb_get_sql		(MidgardQueryBuilder *builder, guint mode, gchar *select);
gchar 			*midgard_core_qb_get_sql_as		(MidgardQueryBuilder *builder, const gchar *name);
void 			midgard_core_qb_add_table		(MidgardQueryBuilder *builder, const gchar *table);
void 			midgard_core_qb_add_constraint		(MidgardQueryBuilder *builder, MidgardQueryConstraint *constraint);
void 			midgard_core_qb_add_order		(MidgardQueryBuilder *builder, MidgardQueryOrder *order);
void 			midgard_core_qb_add_group		(MidgardQueryBuilder *builder, MidgardQueryGroupConstraint *group);
void 			midgard_core_qb_add_group_constraint	(MidgardQueryBuilder *builder, MidgardQueryConstraint *constraint);
gboolean 		midgard_core_qb_is_grouping		(MidgardQueryBuilder *builder);
GObject			**midgard_core_qb_execute		(MidgardQueryBuilder *builder, guint *n_objects);
GList 			*midgard_core_qb_set_object_from_query	(MidgardQueryBuilder *builder, guint select_type, MidgardObject *object);
#endif /* MIDGARD_CORE_QB_H */
