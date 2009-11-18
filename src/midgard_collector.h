/* 
 * Copyright (C) 2006, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_COLLECTOR_H
#define MIDGARD_COLLECTOR_H

#include "midgard_connection.h"
#include "query_builder.h"

/* convention macros */
#define MIDGARD_TYPE_COLLECTOR (midgard_collector_get_type())
#define MIDGARD_COLLECTOR(object)  \
	(G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_COLLECTOR, MidgardCollector))
#define MIDGARD_COLLECTOR_CLASS(klass)  \
	(G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_COLLECTOR, MidgardCollectorClass))
#define MIDGARD_IS_COLLECTOR(object)   \
	(G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_COLLECTOR))
#define MIDGARD_IS_COLLECTOR_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_COLLECTOR))
#define MIDGARD_COLLECTOR_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_COLLECTOR, MidgardCollectorClass))

typedef struct _MidgardCollectorPrivate MidgardCollectorPrivate;
typedef struct _MidgardCollector MidgardCollector;
typedef struct _MidgardCollector midgard_collector;
typedef struct MidgardCollectorClass MidgardCollectorClass;

struct _MidgardCollector{
	MidgardQueryBuilder parent;
	
	/* < private > */
	MidgardCollectorPrivate *priv;
};

struct MidgardCollectorClass{
	MidgardQueryBuilderClass parent;
	
	/* class members */
	gboolean 	(*set_key_property) 	(MidgardCollector *collector, const gchar *key, GValue *value);
	gboolean 	(*add_value_property) 	(MidgardCollector *collector, const gchar *value);
	gboolean 	(*set) 			(MidgardCollector *collector, const gchar *key, const gchar *subkey, GValue *value);
	GData 		*(*get) 		(MidgardCollector *collector, const gchar *key);
	GValue 		*(*get_subkey) 		(MidgardCollector *collector, const gchar *key, const gchar *subkey);
	gboolean 	(*merge) 		(MidgardCollector *collector, MidgardCollector *mc, gboolean overwrite);
	gchar 		**(*list_keys)		(MidgardCollector *collector);
	gboolean 	(*remove_key) 		(MidgardCollector *collector, const gchar *key);
	void 		(*destroy) 		(MidgardCollector *collector);
	gboolean 	(*add_constraint) 	(MidgardCollector *self, const gchar *name, const gchar *op, const GValue *value);
	gboolean 	(*add_constraint_with_property) (MidgardCollector *self, const gchar *property_a, const gchar *op, const gchar *property_b);
	gboolean 	(*begin_group) 		(MidgardCollector *self, const gchar *type);
	gboolean 	(*end_group) 		(MidgardCollector *self);
	gboolean 	(*add_order) 		(MidgardCollector *self, const gchar *name, const gchar *dir);
	void 		(*set_offset) 		(MidgardCollector *self, guint offset);
	void 		(*set_limit) 		(MidgardCollector *self, guint limit);
	void 		(*count) 		(MidgardCollector *self);
	void 		(*include_deleted) 	(MidgardCollector *self);
	gboolean 	(*execute) 		(MidgardCollector *self);
};

 
GType midgard_collector_get_type(void);
MidgardCollector 	*midgard_collector_new			(MidgardConnection *mgd, const gchar *typename, const gchar *domain, GValue *value);
gboolean 		midgard_collector_set_key_property	(MidgardCollector *self, const gchar *key, GValue *value); 
gboolean 		midgard_collector_add_value_property	(MidgardCollector *self, const gchar *value);
gboolean 		midgard_collector_set			(MidgardCollector *self, const gchar *key, const gchar *subkey, GValue *value);
GData 			*midgard_collector_get			(MidgardCollector *self, const gchar *key);
GValue 			*midgard_collector_get_subkey		(MidgardCollector *self, const gchar *key, const gchar *subkey);
gchar 			**midgard_collector_list_keys		(MidgardCollector *self);
gboolean 		midgard_collector_merge			(MidgardCollector *self, MidgardCollector *mc, gboolean overwrite);
gboolean 		midgard_collector_remove_key		(MidgardCollector *self, const gchar *key);
gboolean 		midgard_collector_add_constraint	(MidgardCollector *self, const gchar *name, const gchar *op, const GValue *value);
gboolean 		midgard_collector_add_constraint_with_property (MidgardCollector *self, const gchar *property_a, const gchar *op, const gchar *property_b);
gboolean 		midgard_collector_begin_group 		(MidgardCollector *self, const gchar *type);
gboolean 		midgard_collector_end_group		(MidgardCollector *self);
gboolean 		midgard_collector_add_order		(MidgardCollector *self, const gchar *name, const gchar *dir);
void 			midgard_collector_set_offset		(MidgardCollector *self, guint offset);
void 			midgard_collector_set_limit		(MidgardCollector *self, guint limit);
void 			midgard_collector_count			(MidgardCollector *self);
gboolean		midgard_collector_execute 		(MidgardCollector *self);
gboolean                midgard_collector_include_deleted       (MidgardCollector *self);

#endif /* MIDGARD_COLLECTOR_H */
