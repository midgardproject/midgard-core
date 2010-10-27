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
#include "midgard_cr_core_uuid.h"
#include "midgard_cr_core_md5.h"

#include <string.h>
#include <inttypes.h>

gboolean midgard_cr_core_is_uuid(const gchar *uuid) {
        g_assert(uuid != NULL);
        static const char format[] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
        int i;
        for (i = 0; i < sizeof(format); i++) {
                if (format[i] == 'x') {
                        if (!g_ascii_isxdigit(uuid[i])) {
                                return FALSE;
                        }
                } else if (uuid[i] != format[i]) {
                        return FALSE;
                }
        }
        return TRUE;
}

G_LOCK_DEFINE_STATIC(uuid);

gchar *midgard_cr_core_uuid_new(void) {
        static gboolean initialized = FALSE;
        static guint64 timestamp;
        static guint32 advance;
        static guint16 clockseq;
        static guint16 node_high;
        static guint32 node_low;
        guint64 time;
        guint16 nowseq;
        GTimeVal tv;

        G_LOCK(uuid);
        g_get_current_time(&tv);
        time = ((guint64) tv.tv_sec) * G_USEC_PER_SEC + ((guint64) tv.tv_usec);
        time = time * 10 + G_GINT64_CONSTANT(0x01B21DD213814000U);
        if (!initialized) {
                timestamp = time;
                advance = 0;
                GRand *rand = g_rand_new();
                guint32 r = g_rand_int(rand);
                clockseq = r >> 16;
                node_high = r | 0x0100;
                node_low = g_rand_int(rand);
                g_rand_free(rand);
                initialized = TRUE;
        } else if (time < timestamp) {
                timestamp = time;
                advance = 0;
                clockseq++;
        } else if (time == timestamp) {
                advance++;
                time += advance;
        } else {
                timestamp = time;
                advance = 0;
        }
        nowseq = clockseq;
        G_UNLOCK(uuid);

        return g_strdup_printf(
                "%08" PRIx32 "-%04x-%04x-%04x-%04x%08" PRIx32,
                (guint32) time,
                (unsigned int) (time >> 32) & 0xffff,
                (unsigned int) ((time >> 48) & 0x0ffff) | 0x1000,
                (unsigned int) (nowseq & 0x3fff) | 0x8000,
                (unsigned int) node_high,
                (guint32) node_low);
}

static const unsigned char namespace_uuid[] = {
        0x00, 0xdc, 0x46, 0xa0, 0x0e, 0x0c, 0x10, 0x85,
        0x82, 0xbb, 0x00, 0x02, 0xa5, 0xd5, 0xfd, 0x2e
};

gchar *midgard_cr_core_uuid_external(const gchar *external) {
        g_assert(external != NULL);
        unsigned char uuid[16];

        MIDGARD_CR_CORE_MD5_CTX md5;
        MIDGARD_CR_CORE_MD5_Init(&md5);
        MIDGARD_CR_CORE_MD5_Update(&md5, namespace_uuid, sizeof(namespace_uuid));
        MIDGARD_CR_CORE_MD5_Update(&md5, (unsigned char *) external, strlen(external));
        MIDGARD_CR_CORE_MD5_Final(uuid, &md5);

        return g_strdup_printf(
                "%08lx-%04x-%04x-%04x-%04x%08lx",
                (unsigned long) uuid[0] << 24 | uuid[1] << 16 | uuid[2] << 8 | uuid[3],
                (unsigned int) uuid[4] << 8 | uuid[5],
                (unsigned int) ((uuid[6] & 0x0f) | 0x30) << 8 | uuid[7],
                (unsigned int) ((uuid[8] & 0x3f) | 0x80) << 8 | uuid[9],
                (unsigned int) uuid[10] << 8 | uuid[11],
                (unsigned long) uuid[12] << 24 | uuid[13] << 16 | uuid[14] << 8 | uuid[15]);
}
