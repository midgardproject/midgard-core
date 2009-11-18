/*
 * Copyright (C) 2005 Jukka Zitting <jz@yukatan.fi>
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
#ifndef MIDGARD_GUID_H
#define MIDGARD_GUID_H

#include "midgard_defs.h"
#include <glib.h>
#include "midgard_connection.h"

gboolean midgard_is_guid(const gchar *guid);

gchar *midgard_guid_new(MidgardConnection *mgd);

gchar *midgard_guid_external(MidgardConnection *mgd, const gchar *external);

gchar *mgd_create_guid(MidgardConnection *mgd, const gchar *table, guint id);

#endif
