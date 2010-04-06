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

#ifndef MIDGARD_QUERY_STORAGE_H
#define MIDGARD_QUERY_STORAGE_H

#include <glib-object.h>
#include "midgard_dbobject.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_STORAGE (midgard_query_storage_get_type()) 
#define MIDGARD_QUERY_STORAGE(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_STORAGE, MidgardQueryStorage))
#define MIDGARD_QUERY_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_QUERY_STORAGE, MidgardQueryStorageClass))
#define MIDGARD_IS_QUERY_STORAGE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_STORAGE))
#define MIDGARD_IS_QUERY_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_QUERY_STORAGE))
#define MIDGARD_QUERY_STORAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_QUERY_STORAGE, MidgardQueryStorageClass))

typedef struct _MidgardQueryStorage MidgardQueryStorage;
typedef struct _MidgardQueryStoragePrivate MidgardQueryStoragePrivate;
typedef struct _MidgardQueryStorageClass MidgardQueryStorageClass; 

struct _MidgardQueryStorage {
	GObject parent;

	/* < private > */
	MidgardQueryStoragePrivate *priv;
};

struct _MidgardQueryStorageClass {
	GObjectClass parent;
};

GType 			midgard_query_storage_get_type	(void);
MidgardQueryStorage	*midgard_query_storage_new	(const gchar *classname);

G_END_DECLS

#endif /* MIDGARD_QUERY_STORAGE_H */
