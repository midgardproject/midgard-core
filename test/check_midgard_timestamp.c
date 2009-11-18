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

#include <check.h>
#include <string.h>
#include <stdlib.h>
#include "midgard/midgard_timestamp.h"

START_TEST(test_init)
{
        GValue *value = g_new0(GValue, 1);
        g_value_init(value, MIDGARD_TYPE_TIMESTAMP);
        fail_unless(G_VALUE_HOLDS(value, MIDGARD_TYPE_TIMESTAMP), NULL);
}
END_TEST

START_TEST(test_time)
{
        GValue *value = g_new0(GValue, 1);
        g_value_init(value, MIDGARD_TYPE_TIMESTAMP);
        fail_unless(midgard_timestamp_get_time(value) == 0, NULL);
        midgard_timestamp_set_time(value, 100);
        fail_unless(midgard_timestamp_get_time(value) == 100, NULL);
}
END_TEST

START_TEST(test_iso8601)
{
        GValue *value = g_new0(GValue, 1);
        g_value_init(value, MIDGARD_TYPE_TIMESTAMP);
        gchar *str = midgard_timestamp_dup_string(value);
        fail_unless(strcmp(str, "1970-01-01 00:00:00+0000") == 0, "%s", str);
        g_free(str);
        midgard_timestamp_set_string(value, "2005-09-15 11:46:00+0300");
        str = midgard_timestamp_dup_string(value);
        fail_unless(strcmp(str, "2005-09-15 11:46:00+0300"), "%s", str);
        g_free(str);
}
END_TEST

TCase *midgard_timestamp_test_case(void) {
        TCase *tc = tcase_create("midgard_timestamp");
        tcase_add_test(tc, test_init);
        tcase_add_test(tc, test_time);
        tcase_add_test(tc, test_iso8601);
        return tc;
}
