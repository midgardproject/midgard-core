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

#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "midgard/midgard.h"
#include "midgard/midgard_object.h"
#include "check_query_builder.h"
#include "check_midgard_timestamp.h"
#include "check_uuid.h"
#include "check_guid.h"

/**
 * Test case for verifying the correct functioning of the
 * unit test environment.
 */
START_TEST(test_noop)
{
        /* do nothing, the test will just pass */
}
END_TEST

static MidgardConnection *mgd = NULL;

static void setup(void) { 
	GError *err = NULL;
        MidgardConnection *mgd = midgard_connection_new();
	midgard_connection_open(mgd, "midgard", err);
}

static void teardown(void) {
        g_object_unref(mgd);
}

START_TEST(test_version)
{
        fail_if(strcmp(MIDGARD_LIB_VERSION, midgard_version()) != 0, NULL);
}
END_TEST

Suite *midgard_suite(void) {
        TCase *tc_check = tcase_create("Check");
        tcase_add_test(tc_check, test_noop);

        TCase *tc_general = tcase_create("General");
        tcase_add_checked_fixture(tc_general, setup, teardown);
        tcase_add_test(tc_general, test_version);

        Suite *s = suite_create("Midgard");
        suite_add_tcase(s, tc_check);
        suite_add_tcase(s, tc_general);
        suite_add_tcase(s, midgard_query_builder_test_case());
        suite_add_tcase(s, midgard_uuid_test_case());
        suite_add_tcase(s, midgard_guid_test_case());
        suite_add_tcase(s, midgard_timestamp_test_case());

        return s;
}

int main(void) {
        Suite *s = midgard_suite();
        SRunner *sr = srunner_create(s);
        srunner_run_all(sr, CK_NORMAL);
        int nf = srunner_ntests_failed(sr);
        srunner_free(sr);
        return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
