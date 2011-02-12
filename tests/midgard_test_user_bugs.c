/* 
 * Copyright (C) 2008, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_test.h"

#define USER_TEST_BUG_NAME "john_bug"
#define USER_TEST_BUG_PASS "secret_bug"
#define USER_TEST_BUG_AUTH_TYPE "Legacy"

void 
midgard_test_user_bugs_73 (MidgardUserTest *mut, gconstpointer data)
{
	g_test_bug_base (MIDGARD_GITHUB_ISSUES_BASE_URL);
	g_test_bug ("73");

	MidgardSchema *schema = g_object_new(MIDGARD_TYPE_SCHEMA, NULL);
	midgard_schema_init(schema, NULL);
	midgard_schema_read_dir(schema, NULL);

	MidgardConfig *config = NULL;
	MidgardConnection *mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);

	MidgardUser *user = midgard_user_new (mgd, 0, NULL);
	g_assert (user != NULL);
	g_assert (MIDGARD_IS_USER (user));

	g_object_set (user,
			"login", USER_TEST_BUG_NAME,
			"password", USER_TEST_BUG_PASS,
			"authtype", USER_TEST_BUG_AUTH_TYPE,
			NULL);

	/* Create user */
	gboolean user_created = midgard_user_create (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (user_created != FALSE);

       	gboolean logged_in = midgard_user_log_in (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (logged_in != FALSE);

	g_object_unref (mgd);
	g_assert (!MIDGARD_IS_CONNECTION (mgd));

	g_assert (G_OBJECT (user)->ref_count == 1);
	
	g_object_unref (user);
	g_assert (!MIDGARD_IS_USER (user));

	/* Create connection once again and cleanup */
	config = NULL;
	mgd = midgard_test_connection_open_user_config(CONFIG_CONFIG_NAME, &config);
	g_assert (mgd != NULL);
	g_assert (MIDGARD_IS_CONNECTION (mgd));

	guint n_params = 3;
        GParameter *parameters = g_new (GParameter, n_params);
 
        /* login */
        GValue lval = {0, };
        g_value_init (&lval, G_TYPE_STRING);
        g_value_set_string (&lval, USER_TEST_BUG_NAME);
        parameters[0].name = "login";
        parameters[0].value = lval;

        /* password */
        GValue pval = {0, };
        g_value_init (&pval, G_TYPE_STRING);
        g_value_set_string (&pval, USER_TEST_BUG_PASS);
        parameters[1].name = "password";
        parameters[1].value = pval;

        /* authtype */
        GValue atval = {0, };
        g_value_init (&atval, G_TYPE_STRING);
        g_value_set_string (&atval, USER_TEST_BUG_AUTH_TYPE);
        parameters[2].name = "authtype";
        parameters[2].value = atval;

        user = midgard_user_get (mgd, n_params, parameters);
        MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
        g_assert (user != NULL);

	g_value_unset (&lval);
	g_value_unset (&pval);
	g_value_unset (&atval);
	g_free (parameters);

	gboolean user_deleted = midgard_user_delete (user);
	g_assert (user_deleted == TRUE);

	g_object_unref (mgd);
	g_object_unref (user);
}
