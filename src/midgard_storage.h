/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_STORAGE_H
#define MIDGARD_STORAGE_H

#include <glib.h>
#include <glib-object.h>
#include "midgard_defs.h"
#include "midgard_connection.h"
#include "midgard_dbobject.h"

/* convention macros */
#define MIDGARD_TYPE_STORAGE (midgard_storage_get_type())
#define MIDGARD_STORAGE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_STORAGE, MidgardStorage))
#define MIDGARD_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_STORAGE, MidgardStorageClass))
#define MIDGARD_IS_STORAGE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_STORAGE))
#define MIDGARD_IS_STORAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_STORAGE))
#define MIDGARD_STORAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_STORAGE, MidgardStorageClass))

typedef struct _MidgardStorage MidgardStorage;
typedef struct _MidgardStorage midgard_storage;
typedef struct _MidgardStorageClass MidgardStorageClass;

GType	midgard_storage_get_type	(void);

gboolean midgard_storage_create_base_storage	(MidgardConnection *mgd);
gboolean midgard_storage_create		 	(MidgardConnection *mgd, const gchar *name);
gboolean midgard_storage_update 		(MidgardConnection *mgd, const gchar *name);
gboolean midgard_storage_exists 		(MidgardConnection *mgd, const gchar *name);
gboolean midgard_storage_delete 		(MidgardConnection *mgd, const gchar *name);

#endif /* MIDGARD_STORAGE_H */
