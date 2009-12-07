/* 
 * Copyright (C) 2006, 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef _MIDGARD_BLOB_H
#define _MIDGARD_BLOB_H

#include "midgard_connection.h"
#include "midgard_object.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_BLOB (midgard_blob_get_type())
#define MIDGARD_BLOB(object)  \
	(G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_BLOB, MidgardBlob))
#define MIDGARD_BLOB_CLASS(klass)  \
	(G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_BLOB, MidgardBlobClass))
#define MIDGARD_IS_BLOB(object)   \
	(G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_BLOB))
#define MIDGARD_IS_BLOB_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_BLOB))
#define MIDGARD_BLOB_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_BLOB, MidgardBlobClass))

typedef struct MidgardBlob MidgardBlob;
typedef struct MidgardBlobClass MidgardBlobClass;
typedef struct _MidgardBlobPrivate MidgardBlobPrivate;

struct MidgardBlobClass {
	GObjectClass parent;

	gchar *(*read_content) (MidgardBlob *self, gsize *bytes_read);
	gboolean (*write_content) (MidgardBlob *self, const gchar *content);
	gboolean (*remove_file) (MidgardBlob *self);
	gboolean (*exists) (MidgardBlob *self);
	const gchar *(*get_path) (MidgardBlob *self);
	GIOChannel *(*get_handler) (MidgardBlob *self, const gchar *mode);
};

struct MidgardBlob {
	GObject parent;

	MidgardBlobPrivate *priv;
};

GType 		midgard_blob_get_type		(void);
MidgardBlob 	*midgard_blob_new		(MidgardObject *attachment, const gchar *encoding);
gchar 		*midgard_blob_read_content	(MidgardBlob *self, gsize *bytes_read);
gboolean 	midgard_blob_write_content	(MidgardBlob *self, const gchar *content);
GIOChannel 	*midgard_blob_get_handler	(MidgardBlob *self, const gchar *mode);
const gchar 	*midgard_blob_get_path		(MidgardBlob *self);
gboolean 	midgard_blob_exists		(MidgardBlob *self);
gboolean 	midgard_blob_remove_file	(MidgardBlob *self);

G_END_DECLS
#endif /* _MIDGARD_BLOB_H */
