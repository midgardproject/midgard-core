/* 
 * Copyright (C) 2005, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_QUOTA_H
#define MIDGARD_QUOTA_H

#include "midgard_object.h"

guint 		midgard_quota_get_object_size	(MidgardObject *object);
gboolean 	midgard_quota_create		(MidgardObject *object);
gboolean 	midgard_quota_update		(MidgardObject *object, guint init_size);
void 		midgard_quota_remove		(MidgardObject *object, guint size);
void 		midgard_quota_set_type_size	(MidgardConnection *mgd, const gchar *typename,	guint size, guint records);
guint 		midgard_quota_get_type_size	(MidgardConnection *mgd, const gchar *typename);
gboolean 	midgard_quota_size_is_reached	(MidgardObject *object, gint size);

#endif /* MIDGARD_QUOTA_H */
