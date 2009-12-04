/* 
 * Copyright (C) 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_OBJECT_ATTACHMENT_H
#define MIDGARD_OBJECT_ATTACHMENT_H

#include "midgard_object.h"

G_BEGIN_DECLS

MidgardObject	**midgard_object_list_attachments	(MidgardObject *self);
MidgardObject 	*midgard_object_create_attachment	(MidgardObject *self, const gchar *name, const gchar *title, const gchar *mimetype);
gboolean 	midgard_object_delete_attachments	(MidgardObject *self, guint n_params, const GParameter *parameters);
gboolean 	midgard_object_purge_attachments	(MidgardObject *self, gboolean delete_blob, guint n_params, const GParameter *parameters);
MidgardObject 	**midgard_object_find_attachments	(MidgardObject *self, guint n_params, const GParameter *parameters);
gboolean 	midgard_object_has_attachments		(MidgardObject *self);

G_END_DECLS

#endif /* MIDGARD_OBJECT_ATTACHMENT_H */
