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

#ifndef MIDGARD_CR_CORE_QUERY_STORAGE_H
#define MIDGARD_CR_CORE_QUERY_STORAGE_H

#include <glib-object.h>

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_CR_CORE_TYPE_QUERY_STORAGE (midgard_cr_core_query_storage_get_type()) 
#define MIDGARD_CR_CORE_QUERY_STORAGE(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_CR_CORE_TYPE_QUERY_STORAGE, MidgardCRCoreQueryStorage))
#define MIDGARD_CR_CORE_QUERY_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_CR_CORE_TYPE_QUERY_STORAGE, MidgardCRCoreQueryStorageClass))
#define MIDGARD_CR_CORE_IS_QUERY_STORAGE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_CR_CORE_TYPE_QUERY_STORAGE))
#define MIDGARD_CR_CORE_IS_QUERY_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_CR_CORE_TYPE_QUERY_STORAGE))
#define MIDGARD_CR_CORE_QUERY_STORAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_CR_CORE_TYPE_QUERY_STORAGE, MidgardCRCoreQueryStorageClass))

typedef struct _MidgardCRCoreQueryStorage MidgardCRCoreQueryStorage;
typedef struct _MidgardCRCoreQueryStoragePrivate MidgardCRCoreQueryStoragePrivate;
typedef struct _MidgardCRCoreQueryStorageClass MidgardCRCoreQueryStorageClass; 

struct _MidgardCRCoreQueryStorage {
	GObject parent;

	/* < private > */
	MidgardCRCoreQueryStoragePrivate *priv;
};

struct _MidgardCRCoreQueryStorageClass {
	GObjectClass parent;
};

GType 			midgard_cr_core_query_storage_get_type	(void);
MidgardCRCoreQueryStorage	*midgard_cr_core_query_storage_new	(const gchar *classname);

G_END_DECLS

#endif /* MIDGARD_CR_CORE_QUERY_STORAGE_H */
