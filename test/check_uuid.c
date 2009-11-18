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
#include "check_uuid.h"
#include "midgard/uuid.h"

START_TEST(test_is_uuid)
{
        fail_if(midgard_is_uuid(""), NULL);
        fail_if(midgard_is_uuid("not-uuid"), NULL);
        fail_unless(midgard_is_uuid("83209057-7acb-47b4-970f-bce4a38d3904"), NULL);
        fail_unless(midgard_is_uuid("83209057-7ACB-47B4-970F-BCE4A38D3904"), NULL);
        fail_unless(midgard_is_uuid("83209057-7aCb-47B4-970f-BcE4a38D3904"), NULL);
        fail_if(midgard_is_uuid("83209057-7acb-47b4-970f-bce4a38d390"), NULL);
        fail_if(midgard_is_uuid("83209057-7acb-47b4-970f-gce4a38d3904"), NULL);
        fail_if(midgard_is_uuid("83209057-7acb-47b4-970f0bce4a38d3904"), NULL);
        fail_if(midgard_is_uuid("83209057-7acb-47b4-970f+bce4a38d3904"), NULL);
        fail_if(midgard_is_uuid("83209057-7acb-47b4-970f-bce4a38d39041"), NULL);
}
END_TEST

START_TEST(test_uuid_new)
{
        gchar *uuid1 = midgard_uuid_new();
        gchar *uuid2 = midgard_uuid_new();
        fail_unless(midgard_is_uuid(uuid1), "midgard_is_uuid(%s)", uuid1);
        fail_unless(midgard_is_uuid(uuid2), "midgard_is_uuid(%s)", uuid2);
        fail_if(g_str_equal(uuid1, uuid2), "g_str_equal(%s,%s)", uuid1, uuid2);
        g_free(uuid1);
        g_free(uuid2);
}
END_TEST

START_TEST(test_uuid_external)
{
        static const char *test_uuid = "fa51e40a-898b-3a10-a49f-d79121aa4ca5";
        gchar *uuid0 = midgard_uuid_external("test-name");
        gchar *uuid1a = midgard_uuid_external("some-unique-name");
        gchar *uuid1b = midgard_uuid_external("some-unique-name");
        gchar *uuid2 = midgard_uuid_external("some-other-unique-name");
        fail_unless(midgard_is_uuid(uuid0), "midgard_is_uuid(%s)", uuid0);
        fail_unless(midgard_is_uuid(uuid1a), "midgard_is_uuid(%s)", uuid1a);
        fail_unless(midgard_is_uuid(uuid1b), "midgard_is_uuid(%s)", uuid1b);
        fail_unless(midgard_is_uuid(uuid2), "midgard_is_uuid(%s)", uuid2);
        fail_unless(g_str_equal(uuid0, test_uuid), "g_str_equal(%s,%s)", uuid0, test_uuid);
        fail_unless(g_str_equal(uuid1a, uuid1b), "g_str_equal(%s,%s)", uuid1a, uuid1b);
        fail_if(g_str_equal(uuid1a, uuid2), "g_str_equal(%s,%s)", uuid1a, uuid2);
        g_free(uuid0);
        g_free(uuid1a);
        g_free(uuid1b);
        g_free(uuid2);
}
END_TEST

TCase *midgard_uuid_test_case(void) {
        TCase *test_case = tcase_create("UUID");
        tcase_add_test(test_case, test_is_uuid);
        tcase_add_test(test_case, test_uuid_new);
        tcase_add_test(test_case, test_uuid_external);
        return test_case;
}
