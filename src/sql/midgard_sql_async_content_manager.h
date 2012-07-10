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

#ifndef MIDGARD_SQL_ASYNC_CONTENT_MANAGER_H
#define MIDGARD_SQL_ASYNC_CONTENT_MANAGER_H

#include "../midgard_connection.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_SQL_ASYNC_CONTENT_MANAGER (midgard_sql_async_content_manager_get_type()) 
#define MIDGARD_SQL_ASYNC_CONTENT_MANAGER(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_SQL_ASYNC_CONTENT_MANAGER, MidgardSqlAsyncContentManager))
#define MIDGARD_SQL_ASYNC_CONTENT_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_SQL_ASYNC_CONTENT_MANAGER, MidgardSqlAsyncContentManagerClass))
#define MIDGARD_IS_SQL_ASYNC_CONTENT_MANAGER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SQL_ASYNC_CONTENT_MANAGER))
#define MIDGARD_IS_SQL_ASYNC_CONTENT_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_SQL_ASYNC_CONTENT_MANAGER_TYPE))
#define MIDGARD_SQL_ASYNC_CONTENT_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_SQL_ASYNC_CONTENT_MANAGER_TYPE, MidgardSqlAsyncContentManagerClass))

typedef struct _MidgardSqlAsyncContentManager MidgardSqlAsyncContentManager;
typedef struct _MidgardSqlAsyncContentManagerPrivate MidgardSqlAsyncContentManagerPrivate;
typedef struct _MidgardSqlAsyncContentManagerClass MidgardSqlAsyncContentManagerClass;

struct _MidgardSqlAsyncContentManager {
	GObject parent;

	/* < private > */
	MidgardSqlAsyncContentManagerPrivate *priv;
};	

struct _MidgardSqlAsyncContentManagerClass {
	GObjectClass parent;
};

GType 				midgard_sql_async_content_manager_get_type	(void);
MidgardSqlAsyncContentManager  *midgard_sql_async_content_manager_new		(MidgardConnection *mgd);

G_END_DECLS

#endif /* MIDGARD_SQL_ASYNC_CONTENT_MANAGER_H */
