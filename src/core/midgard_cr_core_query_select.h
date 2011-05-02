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

#ifndef MIDGARD_CR_CORE_QUERY_SELECT_H
#define MIDGARD_CR_CORE_QUERY_SELECT_H

#include <glib-object.h>
#include "midgard_cr_core_query_storage.h"
#include "midgard_cr_core_query_executor.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_CR_CORE_TYPE_QUERY_SELECT (midgard_cr_core_query_select_get_type()) 
#define MIDGARD_CR_CORE_QUERY_SELECT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_CR_CORE_TYPE_QUERY_SELECT, MidgardCRCoreQuerySelect))
#define MIDGARD_CR_CORE_QUERY_SELECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_CR_CORE_TYPE_QUERY_SELECT, MidgardCRCoreQuerySelectClass))
#define MIDGARD_CR_CORE_IS_QUERY_SELECT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_CR_CORE_TYPE_QUERY_SELECT))
#define MIDGARD_CR_CORE_IS_QUERY_SELECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_CR_CORE_TYPE_QUERY_SELECT))
#define MIDGARD_CR_CORE_QUERY_SELECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_CR_CORE_TYPE_QUERY_SELECT, MidgardCRCoreQuerySelectClass))

typedef struct _MidgardCRCoreQuerySelect MidgardCRCoreQuerySelect;
typedef struct _MidgardCRCoreQuerySelectClass MidgardCRCoreQuerySelectClass;

struct _MidgardCRCoreQuerySelectClass {
	MidgardCRCoreQueryExecutorClass parent;

	struct _MidgardCRRepositoryObject	**(*list_objects)		(MidgardCRCoreQuerySelect *self, guint *n_objects);
	void		(*toggle_read_only)		(MidgardCRCoreQuerySelect *self, gboolean toggle);
	void		(*include_deleted)		(MidgardCRCoreQuerySelect *self, gboolean toggle);
};

struct _MidgardCRCoreQuerySelect {
	MidgardCRCoreQueryExecutor parent;
};

struct _MidgardCRSQLStorageManager;
struct _MidgardCRSQLQueryStorage;
struct _MidgardCRSQLRepositoryObject;

GType 			midgard_cr_core_query_select_get_type		(void);
MidgardCRCoreQuerySelect	*midgard_cr_core_query_select_new 		(struct _MidgardCRSQLStorageManager *manager, struct _MidgardCRCoreQueryStorage *storage);
MidgardCRCoreQuerySelect	*midgard_cr_core_query_create_static 		(struct _MidgardCRSQLStorageManager *manager, struct _MidgardCRCoreQueryStorage *storage);
struct _MidgardCRRepositoryObject			**midgard_cr_core_query_select_list_objects	(MidgardCRCoreQuerySelect *self, guint *n_objects);
void			midgard_cr_core_query_select_toggle_read_only	(MidgardCRCoreQuerySelect *self, gboolean toggle);	
void			midgard_cr_core_query_select_include_deleted	(MidgardCRCoreQuerySelect *self, gboolean toggle);

G_END_DECLS

#endif /* MIDGARD_CR_CORE_QUERY_SELECT_H */
