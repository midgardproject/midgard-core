/*
 * Copyright (c) 2005 Jukka Zitting <jz@yukatan.fi>
 * Copyright (c) 2005, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_QUERY_BUILDER_H
#define MIDGARD_QUERY_BUILDER_H

#include "midgard_connection.h"

/* convention macros */
#define MIDGARD_TYPE_QUERY_BUILDER (midgard_query_builder_get_type())
#define MIDGARD_QUERY_BUILDER(object)  \
	(G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_BUILDER, MidgardQueryBuilder))
#define MIDGARD_QUERY_BUILDER_CLASS(klass)  \
	(G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_QUERY_BUILDER, MidgardQueryBuilderClass))
#define MIDGARD_IS_QUERY_BUILDER(object)   \
	(G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_QUERY_BUILDER))
#define MIDGARD_IS_QUERY_BUILDER_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_QUERY_BUILDER))
#define MIDGARD_QUERY_BUILDER_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_QUERY_BUILDER, MidgardQueryBuilderClass))

typedef struct _MidgardQueryBuilder MidgardQueryBuilder;
typedef struct _MidgardQueryBuilder midgard_query_builder;
typedef struct _MidgardQueryBuilderPrivate MidgardQueryBuilderPrivate;

struct _MidgardQueryBuilder {
	GObject parent;

	MidgardQueryBuilderPrivate *priv;
};

typedef struct MidgardQueryBuilderClass MidgardQueryBuilderClass;

struct MidgardQueryBuilderClass{
	GObjectClass parent;

	gboolean 	(*add_constraint) 	(MidgardQueryBuilder *builder, const gchar *name, const gchar *op, const GValue *value);
	gboolean 	(*add_constraint_with_property) (MidgardQueryBuilder *builder, const gchar *property_a, const gchar *op, const gchar *property_b);
	gboolean 	(*begin_group) 		(MidgardQueryBuilder *builder, const gchar *type);
	gboolean 	(*end_group) 		(MidgardQueryBuilder *builder);
	gboolean 	(*add_order) 		(MidgardQueryBuilder *builder, const gchar *name, const gchar *dir);
	void 		(*set_offset) 		(MidgardQueryBuilder *builder, guint offset);
	void 		(*set_limit) 		(MidgardQueryBuilder *builder, guint limit);
	void 		(*include_deleted) 	(MidgardQueryBuilder *builder);
	GObject 	**(*execute) 		(MidgardQueryBuilder *builder, guint *n_objects);
	guint 		(*count) 		(MidgardQueryBuilder *builder);
};

GType			midgard_query_builder_get_type		(void);
MidgardQueryBuilder	*midgard_query_builder_new		(MidgardConnection *mgd, const gchar *classname);
gboolean 		midgard_query_builder_add_constraint	(MidgardQueryBuilder *builder, const gchar *name, const gchar *op, const GValue *value);
gboolean 		midgard_query_builder_add_constraint_with_property	(MidgardQueryBuilder *builder, const gchar *property_a, const gchar *op, const gchar *property_b);
gboolean 		midgard_query_builder_begin_group	(MidgardQueryBuilder *builder, const gchar *type);
gboolean 		midgard_query_builder_end_group		(MidgardQueryBuilder *builder);
gboolean 		midgard_query_builder_add_order		(MidgardQueryBuilder *builder, const gchar *name, const gchar *dir);
void 			midgard_query_builder_set_offset	(MidgardQueryBuilder *builder, guint offset);
void 			midgard_query_builder_set_limit		(MidgardQueryBuilder *builder, guint limit);
GObject 		**midgard_query_builder_execute		(MidgardQueryBuilder *builder, guint *n_objects);
guint 			midgard_query_builder_count		(MidgardQueryBuilder *builder); 
const gchar 		*midgard_query_builder_get_type_name	(MidgardQueryBuilder *builder);
void 			midgard_query_builder_include_deleted	(MidgardQueryBuilder *builder);

#endif
