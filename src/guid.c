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
#include <config.h>
#include "guid.h"
#include "uuid.h"

gboolean midgard_is_guid(const gchar *guid) {

	if (!guid || (guid && *guid == '\0'))
		return FALSE;

        int i;
        for (i = 0; guid[i] != 0 && i < 80; i++) {
                if (guid[i] != '-' && !g_ascii_isxdigit(guid[i])) {
                        return FALSE;
                }
        }
        return (i >= 21);
}

static void uuid2guid(gchar *uuid) {
        gchar *guid = uuid;
        do {
                if (*uuid == '-') {
                        uuid++;
                }
                *guid++ = *uuid++;
        } while (*uuid);
}

gchar *midgard_guid_new(MidgardConnection *mgd) {
        g_assert(mgd != NULL);
        gchar *uuid = midgard_uuid_new();
        uuid2guid(uuid);
        return uuid;
}

gchar *midgard_guid_external(MidgardConnection *mgd, const gchar *external) {
        g_assert(mgd != NULL);
        g_assert(external != NULL);
        gchar *uuid = midgard_uuid_external(external);
        uuid2guid(uuid);
        return uuid;
}

gchar *mgd_create_guid(MidgardConnection *mgd, const gchar *table, guint id) {
        g_assert(mgd != NULL);
        g_assert(table != NULL);
        return midgard_guid_new(mgd);
}
