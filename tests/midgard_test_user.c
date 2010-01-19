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

#define USER_TEST_NAME "john"
#define USER_TEST_PASS "secret"
#define USER_TEST_AUTH_TYPE "Legacy"

void 
midgard_test_user_init (MidgardUserTest *mut, gconstpointer data)
{
	MidgardConnection *mgd = mut->mgd;

	MidgardUser *user = midgard_user_new (mgd, 0, NULL);
	g_assert (user != NULL);

	g_assert (midgard_user_is_admin (user) == FALSE);
	g_assert (midgard_user_is_user (user) == FALSE);
}

void 
midgard_test_user_create (MidgardUserTest *mut, gconstpointer data)
{
	MidgardConnection *mgd = mut->mgd;

	MidgardUser *user = midgard_user_new (mgd, 0, NULL);
	
	g_assert (user != NULL);
	g_assert (MIDGARD_IS_USER (user));
	g_object_set (user, 
			"login", USER_TEST_NAME,
			"password", USER_TEST_PASS,
			"authtype", USER_TEST_AUTH_TYPE, 
			NULL);

	/* Create user in current test sitegroup */
	gboolean user_created = midgard_user_create (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (user_created != FALSE);

	g_assert (midgard_user_is_user (user) == FALSE);
	g_assert (midgard_user_is_admin (user) == FALSE);

	/* Check if can create already created */
	gboolean user_created_again = midgard_user_create (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
	g_assert (user_created_again == FALSE);

	/* Check duplicate */
	MidgardUser *duser = midgard_user_new (mgd, 0, NULL);
	g_assert (duser != NULL);
	g_assert (MIDGARD_IS_USER (duser));
	g_object_set (duser, 
			"login", USER_TEST_NAME,
			"password", USER_TEST_PASS,
			"authtype", USER_TEST_AUTH_TYPE, 
			NULL);

	user_created = midgard_user_create (duser);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_DUPLICATE);
	g_assert (user_created != TRUE);

	/* Check empty authtype */
	g_object_set (duser, 
			"login", "alice",
			"authtype", "", NULL);
	gboolean user_created_with_empty_authtype = midgard_user_create (duser);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
	g_assert (user_created_with_empty_authtype != TRUE);

	/* Check invalid authtype */
	g_object_set (duser, "authtype", "NoSuchAuthType", NULL);
	gboolean user_created_with_wrong_authtype = midgard_user_create (duser);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INTERNAL);
	g_assert (user_created_with_wrong_authtype != TRUE);

	g_object_unref (user);
	g_object_unref (duser);
}

void 
midgard_test_user_update (MidgardUserTest *mut, gconstpointer data)
{
	MidgardConnection *mgd = mut->mgd;
	
	/* Update empty object */
	MidgardUser *empty = midgard_user_new (mgd, 0, NULL);
	gboolean empty_user_updated = midgard_user_update (empty);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
	g_assert (empty_user_updated != TRUE);
	g_object_unref (empty);

	guint n_params = 3;
	GParameter *parameters = g_new (GParameter, n_params);

	/* login */
	GValue lval = {0, };
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, USER_TEST_NAME);
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* password */
	GValue pval = {0, };
	g_value_init (&pval, G_TYPE_STRING);
	g_value_set_string (&pval, USER_TEST_PASS);
	parameters[1].name = "password";
	parameters[1].value = pval;

	/* authtype */
	GValue atval = {0, };
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, USER_TEST_AUTH_TYPE);
	parameters[2].name = "authtype";
	parameters[2].value = atval;

	MidgardUser *user = midgard_user_get (mgd, n_params, parameters);
	g_assert (user != NULL);
	
	g_value_unset (&lval);
	g_value_unset (&pval);
	g_value_unset (&atval);
	g_free (parameters);

	/* update */
	g_object_set (user, "login", "alice", NULL);
	gboolean user_login_updated = midgard_user_update (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (user_login_updated != FALSE);

	/* update and revert name*/
	g_object_set (user, "login", USER_TEST_NAME, NULL);
	user_login_updated = midgard_user_update (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (user_login_updated != FALSE);

	/* Check empty authtype */
	g_object_set (user, 
			"login", "alice",
			"authtype", "", NULL);
	gboolean user_updated_with_empty_authtype = midgard_user_update (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
	g_assert (user_updated_with_empty_authtype != TRUE);

	/* Check invalid authtype */
	g_object_set (user, "authtype", "NoSuchAuthType", NULL);
	gboolean user_updated_with_wrong_authtype = midgard_user_update (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INTERNAL);
	g_assert (user_updated_with_wrong_authtype != TRUE);

	/* check duplicate */
	MidgardUser *duser = midgard_user_new (mgd, 0, NULL);
	g_object_set (duser,
		"login", "Foo",
		"authtype", "Legacy", NULL);
	gboolean duplicated_user_created = midgard_user_create (duser);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (duplicated_user_created != FALSE);

	g_object_set (duser, "login", USER_TEST_NAME, NULL);
	gboolean duplicated_user_updated = midgard_user_update (duser);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_DUPLICATE);
	g_assert (duplicated_user_updated == FALSE);

	g_object_unref (duser);
	g_object_unref (user);
}

void 
midgard_test_user_get (MidgardUserTest *mut, gconstpointer data)
{
	MidgardConnection *mgd = mut->mgd;

	guint n_params = 3;
	GParameter *parameters = g_new (GParameter, n_params);

	/* Check valid user */
	/* login */
	GValue lval = {0, };
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, USER_TEST_NAME);
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* password */
	GValue pval = {0, };
	g_value_init (&pval, G_TYPE_STRING);
	g_value_set_string (&pval, USER_TEST_PASS);
	parameters[1].name = "password";
	parameters[1].value = pval;

	/* authtype */
	GValue atval = {0, };
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, USER_TEST_AUTH_TYPE);
	parameters[2].name = "authtype";
	parameters[2].value = atval;

	MidgardUser *user = midgard_user_get (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (user != NULL);
	
	g_value_unset (&lval);
	g_value_unset (&pval);
	g_value_unset (&atval);
	g_free (parameters);

	/* Check user with invalid password */
	n_params = 3;
	parameters = g_new (GParameter, n_params);

	/* login */	
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, USER_TEST_NAME);
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* password */
	g_value_init (&pval, G_TYPE_STRING);
	g_value_set_string (&pval, "wrongpassword");
	parameters[1].name = "password";
	parameters[1].value = pval;

	/* authtype */
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, USER_TEST_AUTH_TYPE);
	parameters[2].name = "authtype";
	parameters[2].value = atval;

	user = midgard_user_get (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_NOT_EXISTS);
	g_assert (user == NULL);
	
	g_value_unset (&lval);
	g_value_unset (&pval);
	g_value_unset (&atval);
	g_free (parameters);

	/* Check user which doesn't exist*/
	n_params = 2;
	parameters = g_new (GParameter, n_params);

	/* login */
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, "Alice");
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* authtype */
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, USER_TEST_AUTH_TYPE);
	parameters[1].name = "authtype";
	parameters[1].value = atval;

	user = midgard_user_get (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_NOT_EXISTS);
	g_assert (user == NULL);
	
	g_value_unset (&lval);
	g_value_unset (&atval);
	g_free (parameters);

	/* Check empty authtype */
	n_params = 2;
	parameters = g_new (GParameter, n_params);

	/* login */
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, "Alice");
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* authtype */
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, "");
	parameters[1].name = "authtype";
	parameters[1].value = atval;

	user = midgard_user_get (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INTERNAL);
	g_assert (user == NULL);
	
	g_value_unset (&lval);
	g_value_unset (&atval);
	g_free (parameters);

	/* Check invalid authtype */
	n_params = 2;
	parameters = g_new (GParameter, n_params);

	/* login */
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, "Alice");
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* authtype */
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, "Invalidtype");
	parameters[1].name = "authtype";
	parameters[1].value = atval;

	user = midgard_user_get (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INTERNAL);
	g_assert (user == NULL);
	
	g_value_unset (&lval);
	g_value_unset (&atval);
	g_free (parameters);
}

void 
midgard_test_user_query (MidgardUserTest *mut, gconstpointer data)
{
	MidgardConnection *mgd = mut->mgd;

	guint n_params = 1;
	GParameter *parameters = g_new (GParameter, n_params);

	/* Check valid user */
	/* login */
	GValue lval = {0, };
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, USER_TEST_NAME);
	parameters[0].name = "login";
	parameters[0].value = lval;

	MidgardUser **users = midgard_user_query (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (users != NULL);
	g_assert (users[0] != NULL);

	g_object_unref (users[0]);
	g_free (users);
	g_value_unset (&lval);
	g_free (parameters);

	/* Check if duplicates exist */
	n_params = 2;
	parameters = g_new (GParameter, n_params);

	/* login */
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, USER_TEST_NAME);
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* authtype */
	GValue atval = {0, };
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, USER_TEST_AUTH_TYPE);
	parameters[1].name = "authtype";
	parameters[1].value = atval;

	users = midgard_user_query (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (users != NULL);

	guint users_count = 0;
	while (users[users_count] != NULL)
		users_count++;

	g_assert_cmpint (users_count, == , 1);
	
	g_object_unref (users[0]);
	g_free (users);
	g_value_unset (&lval);
	g_value_unset (&atval);
	g_free (parameters);

	/* Check invalid authtype */
	n_params = 2;
	parameters = g_new (GParameter, n_params);

	/* login */
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, "Alice");
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* authtype */
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, "Invalidtype");
	parameters[1].name = "authtype";
	parameters[1].value = atval;

	users = midgard_user_query (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INTERNAL);
	g_assert (users == NULL);
	
	g_value_unset (&lval);
	g_value_unset (&atval);
	g_free (parameters);
}

void 
midgard_test_user_login (MidgardUserTest *mut, gconstpointer data)
{
	MidgardConnection *mgd = mut->mgd;
	
	guint n_params = 3;
	GParameter *parameters = g_new (GParameter, n_params);

	/* Log in valid user */
	/* login */
	GValue lval = {0, };
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, USER_TEST_NAME);
	parameters[0].name = "login";
	parameters[0].value = lval;

	/* password */
	GValue pval = {0, };
	g_value_init (&pval, G_TYPE_STRING);
	g_value_set_string (&pval, USER_TEST_PASS);
	parameters[1].name = "password";
	parameters[1].value = pval;

	/* authtype */
	GValue atval = {0, };
	g_value_init (&atval, G_TYPE_STRING);
	g_value_set_string (&atval, USER_TEST_AUTH_TYPE);
	parameters[2].name = "authtype";
	parameters[2].value = atval;

	MidgardUser *user = midgard_user_get (mgd, n_params, parameters);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (user != NULL);
	
	g_value_unset (&lval);
	g_value_unset (&pval);
	g_value_unset (&atval);
	g_free (parameters);

	gboolean logged_in = midgard_user_login (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (logged_in != FALSE);	

	/* Check empty user */
	MidgardUser *empty = midgard_user_new (mgd, 0, NULL);
	gboolean empty_logged_in = midgard_user_login (empty);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
	g_assert (empty_logged_in == FALSE);
	g_object_unref (empty);
}

void 
midgard_test_user_logout (MidgardUserTest *mut, gconstpointer data)
{
	MidgardConnection *mgd = mut->mgd;

	/* Get current user and logout */
	MidgardUser *user = midgard_connection_get_user (mgd);
	g_assert (user != NULL);

	gboolean user_logout = midgard_user_logout (user);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (user_logout != FALSE);
	g_object_unref (user);

	/* Check empty user logout */
	if (g_test_trap_fork(0, G_TEST_TRAP_SILENCE_STDERR)) {
		MidgardUser *empty = midgard_user_new (mgd, 0, NULL);
		gboolean empty_user_logout = midgard_user_logout (empty);
	}
	// this test is supposed to "abort", which is equivalent to failure in g_test terms
	g_test_trap_assert_failed();
}
