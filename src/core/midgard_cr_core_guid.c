/*
 * Copyright (C) 2005 Jukka Zitting <jz@yukatan.fi>
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
#include <config.h>
#include "midgard_cr_core_guid.h"
#include "midgard_cr_core_uuid.h"

gboolean 
midgard_cr_core_is_guid (const gchar *guid) {
        
	/* Handle the case when object's guid property might be 
	 * initialized with NULL or empty string by default */
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

static void 
uuid2guid (gchar *uuid) {
        gchar *guid = uuid;
        do {
                if (*uuid == '-') {
                        uuid++;
                }
                *guid++ = *uuid++;
        } while (*uuid);
}

gchar *
midgard_cr_core_guid_new (void) {
        gchar *uuid = midgard_cr_core_uuid_new();
        uuid2guid(uuid);
        return uuid;
}
