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

#ifndef MIDGARD_QUERY_SELECT_H
#define MIDGARD_QUERY_SELECT_H

#include <glib-object.h>
#include "midgard_connection.h"
#include "midgard_query_storage.h"
#include "midgard_query_executor.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_SELECT (midgard_query_select_get_type()) 
#define MIDGARD_QUERY_SELECT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_SELECT, MidgardQuerySelect))
#define MIDGARD_QUERY_SELECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_QUERY_SELECT, MidgardQuerySelectClass))
#define MIDGARD_IS_QUERY_SELECT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_SELECT))
#define MIDGARD_IS_QUERY_SELECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_QUERY_SELECT))
#define MIDGARD_QUERY_SELECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_QUERY_SELECT, MidgardQuerySelectClass))

typedef struct _MidgardQuerySelect MidgardQuerySelect;
typedef struct _MidgardQuerySelectClass MidgardQuerySelectClass;

struct _MidgardQuerySelectClass {
	MidgardQueryExecutorClass parent;

	MidgardDBObject	**(*list_objects)		(MidgardQuerySelect *self, guint *n_objects);
	void		(*toggle_read_only)		(MidgardQuerySelect *self, gboolean toggle);
	void		(*include_deleted)		(MidgardQuerySelect *self, gboolean toggle);
};

struct _MidgardQuerySelect {
	MidgardQueryExecutor parent;
};

GType 			midgard_query_select_get_type		(void);
MidgardQuerySelect	*midgard_query_select_new 		(MidgardConnection *mgd, MidgardQueryStorage *storage);
MidgardDBObject		**midgard_query_select_list_objects	(MidgardQuerySelect *self, guint *n_objects);
void			midgard_query_select_toggle_read_only	(MidgardQuerySelect *self, gboolean toggle);	
void			midgard_query_select_include_deleted	(MidgardQuerySelect *self, gboolean toggle);

G_END_DECLS

#endif /* MIDGARD_QUERY_SELECT_H */
