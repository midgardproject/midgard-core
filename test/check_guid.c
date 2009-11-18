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
#include "check_guid.h"
#include "midgard/guid.h"
#include "midgard/uuid.h"

START_TEST(test_is_guid)
{
	fail_if(midgard_is_guid(""), NULL);
        fail_if(midgard_is_guid("not-uuid"), NULL);
        fail_if(midgard_is_guid("1234567890"), NULL);
        fail_if(midgard_is_guid("1234567890abcdef"), NULL);
        fail_if(midgard_is_uuid("83209057-7acb-47b4-970f-gce4a38d3904"), NULL);
        fail_unless(midgard_is_guid("832090577acb47b4970fbce4a38d3904"), NULL);
        fail_unless(midgard_is_guid("83209057-7acb-47b4-970f-bce4a38d3904"), NULL);
        fail_unless(midgard_is_guid("83209057-7ACB-47B4-970F-BCE4A38D3904"), NULL);
        fail_unless(midgard_is_guid("83209057-7aCb-47B4-970f-BcE4a38D3904"), NULL);
        fail_unless(midgard_is_guid("01234567890123456789012345678901234567890123456789012345678901234567890123456789"), NULL);
        fail_unless(midgard_is_guid("012345678901234567890123456789012345678901234567890123456789012345678901234567890"), NULL);
}
END_TEST

START_TEST(test_guid_new)
{
	g_type_init();        
        MidgardConnection *mgd = midgard_connection_new();
        gchar *guid1 = midgard_guid_new(mgd);
        gchar *guid2 = midgard_guid_new(mgd);
        fail_unless(midgard_is_guid(guid1), "midgard_is_guid(%s)", guid1);
        fail_unless(midgard_is_guid(guid2), "midgard_is_guid(%s)", guid2);
        fail_if(g_str_equal(guid1, guid2), "g_str_equal(%s,%s)", guid1, guid2);
        g_free(guid1);
        g_free(guid2);
        g_object_unref(mgd);
}
END_TEST

START_TEST(test_guid_external)
{
        g_type_init();
        MidgardConnection *mgd = midgard_connection_new();
        gchar *guid1a = midgard_guid_external(mgd, "some-unique-name");
        gchar *guid1b = midgard_guid_external(mgd, "some-unique-name");
        gchar *guid2 = midgard_guid_external(mgd, "some-other-unique-name");
        fail_unless(midgard_is_guid(guid1a), "midgard_is_guid(%s)", guid1a);
        fail_unless(midgard_is_guid(guid1b), "midgard_is_guid(%s)", guid1b);
        fail_unless(midgard_is_guid(guid2), "midgard_is_guid(%s)", guid2);
        fail_unless(g_str_equal(guid1a, guid1b), "g_str_equal(%s,%s)", guid1a, guid1b);
        fail_if(g_str_equal(guid1a, guid2), "g_str_equal(%s,%s)", guid1a, guid2);
        g_free(guid1a);
        g_free(guid1b);
        g_free(guid2);
        g_object_unref(mgd);
       
}
END_TEST

START_TEST(test_create_guid)
{
        g_type_init();
        MidgardConnection *mgd = midgard_connection_new();
        gchar *guid1 = mgd_create_guid(mgd, "table", 123);
        gchar *guid2 = mgd_create_guid(mgd, "table", 123);
        fail_unless(midgard_is_guid(guid1), "midgard_is_guid(%s)", guid1);
        fail_unless(midgard_is_guid(guid2), "midgard_is_guid(%s)", guid2);
        fail_if(g_str_equal(guid1, guid2), "g_str_equal(%s,%s)", guid1, guid2);
        g_free(guid1);
        g_free(guid2);
        g_object_unref(mgd);
}
END_TEST

TCase *midgard_guid_test_case(void) {
        TCase *test_case = tcase_create("UUID");
        tcase_add_test(test_case, test_is_guid);
        tcase_add_test(test_case, test_guid_new);
        tcase_add_test(test_case, test_guid_external);
        tcase_add_test(test_case, test_create_guid);
        return test_case;
}
