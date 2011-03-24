/* 
 * Copyright (C) 2007, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "config.h"
#include <libgda/libgda.h>
/* stddef included due to bug in openssl package.
 * In theory 0.9.8g fix this, but on some systems stddef is not included.
 * Remove this include when stddef is included in openssl dev headers. */
#include <stddef.h>
#include "midgard_md5.h"
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_dbobject.h"
#include "midgard_core_query.h"
#include "midgard_user.h"
#include "midgard_error.h"
#include "midgard_replicator.h"
#include "guid.h"
#include "midgard_core_object_class.h"
#include "midgard_core_connection.h"

#if HAVE_CRYPT_H
# include <crypt.h>
#else
# define _XOPEN_SOURCE
# include <unistd.h>
#endif

/* declarations for virtual methods */
MidgardUser     	*__midgard_user_get               (MidgardConnection *mgd, guint n_params, const GParameter *parameters);
MidgardUser     	**__midgard_user_query            (MidgardConnection *mgd, guint n_params, const GParameter *parameters);
gboolean        	__midgard_user_create             (MidgardUser *self);
gboolean        	__midgard_user_update             (MidgardUser *self);
gboolean		__midgard_user_delete		  (MidgardUser *self);
gboolean        	__midgard_user_is_user            (MidgardUser *self);
gboolean        	__midgard_user_is_admin           (MidgardUser *self);
MidgardObject		*__midgard_user_get_person        (MidgardUser *self);
gboolean		__midgard_user_set_person         (MidgardUser *self, MidgardObject *person);
gboolean        	__midgard_user_login              (MidgardUser *self);
gboolean        	__midgard_user_logout             (MidgardUser *self);

struct _MidgardUserPrivate {
	MidgardObject *person;
	gchar *person_guid;
	guint user_type;
	gchar *login;
	gchar *password;
	gboolean active;
	guint auth_type_id;
	gchar *auth_type;
	gboolean fetched;
	gboolean is_logged;
};

#define MIDGARD_USER_TABLE "midgard_user"
#define MIDGARD_USER_PROP_LOGIN "login"
#define MIDGARD_USER_PROP_AUTHTYPE "authtype"
#define MIDGARD_USER_PROP_GUID "guid"

/* MidgardUser object properties */
enum {
	MIDGARD_USER_GUID = 1,
	MIDGARD_USER_LOGIN, 
	MIDGARD_USER_PASS, 
	MIDGARD_USER_ACTIVE,	
	MIDGARD_USER_AUTH_TYPE,
	MIDGARD_USER_AUTH_TYPE_ID,
	MIDGARD_USER_TYPE,
	MIDGARD_USER_PERSON_GUID
};

static GObjectClass *__parent_class= NULL;

/**
 * midgard_user_new:
 * @mgd: #MidgardConnection instance
 * @n_params: number of parameters
 * @parameters: #GParameter with #MidgardUser properties
 *
 * @parameters and @n_params arguments are optional. midgard_user_get() will be invoked 
 * in constructor if @parameters argument will be set to not %NULL value.
 *
 * Returns: #MidgardUser object or %NULL on failure
 */
MidgardUser *
midgard_user_new (MidgardConnection *mgd, guint n_params, const GParameter *parameters)
{
	g_return_val_if_fail (mgd != NULL, NULL);

	if (parameters && n_params > 0) {

		MidgardUserClass *klass = g_type_class_peek (MIDGARD_TYPE_USER);
		return klass->get (mgd, n_params, parameters);
	}

	MidgardUser *self = g_object_new (MIDGARD_TYPE_USER, "connection", mgd, NULL);

	return self;
}

static gboolean 
__validate_parameters (MidgardConnection *mgd, guint n_params, const GParameter *parameters, guint valid)
{
	guint i;
	const gchar *pname;
	gint pvalid = 0;

	for (i = 0; i < n_params; i++) {

		pname = parameters[i].name;

		if (g_str_equal (pname, MIDGARD_USER_PROP_LOGIN)
				|| g_str_equal (pname, MIDGARD_USER_PROP_AUTHTYPE)
				|| g_str_equal (pname, MIDGARD_USER_PROP_GUID))
			pvalid++;
	}

	if (pvalid < valid) {

		MIDGARD_ERRNO_SET (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
		return FALSE;
	}

	return TRUE;
}	

static GParameter *
__convert_to_storage_parameters (MidgardConnection *mgd, guint n_params, const GParameter *parameters, MidgardUserClass *klass, GValue idval)
{
	GParameter *tbl_params = g_new0 (GParameter, n_params);

	guint i;
	const gchar *pname;
		
	for (i = 0; i < n_params; i++ ) {
	
		pname = parameters[i].name;
		GParamSpec *pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), pname);

		if (pspec) {
			
			if (g_str_equal (pspec->name, "authtype")) {

				tbl_params[i].name = "auth_type_id";	

				/* Validate authentication type */
				guint id = midgard_core_auth_type_id_from_name (mgd, g_value_get_string ((const GValue *)&parameters[i].value));
				if (id == 0) {

					MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INTERNAL, "Unknown authentication type");
					g_free (tbl_params);
					return NULL;
				}

				g_value_set_uint (&idval, id);
				tbl_params[i].value = idval;

			} else {

				tbl_params[i].value = parameters[i].value;
				tbl_params[i].name = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (klass), pname);	
			}
		}
	}

	return tbl_params;
}

/**
 * midgard_user_get:
 * @mgd: #MidgardConnection instance
 * @n_params: number of parameters
 * @parameters: #GParameter with #MidgardUser properties
 *
 * Fetch #MidgardUser object from storage. 
 * At least 'login' and 'authtype' property are required to be set in parameters.
 *
 * Cases to return %NULL:
 * <itemizedlist>
 * <listitem><para>
 * 'login' or 'authtype' properties do not exist in given parameters (MGD_ERR_INVALID_PROPERTY_VALUE)
 * </para></listitem>
 * <listitem><para>
 * There's no user object which match given parameters (MGD_ERR_NOT_EXISTS)
 * </para></listitem>
 * <listitem><para>
 * More than one record found in database (MGD_ERR_INTERNAL)
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: (transfer full): new #MidgardUser instance or %NULL in case of failure
 * 
 * Since 9.09
 */
MidgardUser *
midgard_user_get (MidgardConnection *mgd, guint n_params, const GParameter *parameters)
{
	MidgardUserClass *klass = g_type_class_peek (MIDGARD_TYPE_USER);
	return klass->get (mgd, n_params, parameters);
}

MidgardUser *
__midgard_user_get (MidgardConnection *mgd, guint n_params, const GParameter *parameters)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (parameters != NULL, NULL);

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);
	MidgardUserClass *klass = g_type_class_peek (MIDGARD_TYPE_USER);
	
	/* Validate properties and find required ones */
	if (!__validate_parameters(mgd, n_params, parameters, 2)) 
		return NULL;

	/* Create new parameters. We need to pass column names instead of properties */
	GValue idval = {0, };
	g_value_init (&idval, G_TYPE_UINT);
	GParameter *tbl_params = __convert_to_storage_parameters (mgd, n_params, parameters, klass, idval);
	if (!tbl_params) {

		g_value_unset (&idval);
		return NULL;
	}
	GdaDataModel *model = midgard_core_query_get_dbobject_model (mgd, MIDGARD_DBOBJECT_CLASS (klass), n_params, tbl_params);

	g_free (tbl_params);
	g_value_unset (&idval);

	if (!model) {

		MIDGARD_ERRNO_SET (mgd, MGD_ERR_NOT_EXISTS);
		return NULL;
	}

	guint rows = gda_data_model_get_n_rows(model);

	if (rows > 1) {
		
		g_warning ("midgard_user storage inconsistency. Found %d records. Expected one.", rows);
		MIDGARD_ERRNO_SET (mgd, MGD_ERR_INTERNAL);
	}

	if(rows == 0) {
		
		MIDGARD_ERRNO_SET (mgd, MGD_ERR_NOT_EXISTS);
		g_object_unref(model);
		return NULL;
	}

	MidgardUser *self = g_object_new (MIDGARD_TYPE_USER, "connection", mgd, NULL);

	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->__set_from_sql (MIDGARD_DBOBJECT (self), model, 0);
	g_object_unref (model);

	return self;
}

/** 
 * midgard_user_query:
 * @mgd: #MidgardConnection instance
 * @n_params: number of parameters
 * @parameters: #GParameter with #MidgardUser properties
 *
 * Fetch #MidgardUser objects from storage. 
 * At least 'login' and 'authtype' property are required to be set in parameters.
 *
 * Cases to return %NULL:
 * <itemizedlist> 
 * <listitem><para>
 * There's attempt to query users in anonymous mode or logged in user is not admin (MGD_ERR_ACCESS_DENIED)
 * </para></listitem>
 * <listitem><para>
 * 'login' or 'authtype' properties do not exist in given parameters (MGD_ERR_INVALID_PROPERTY_VALUE)
 * </para></listitem>
 * <listitem><para>
 * There are no user objects which match given parameters (MGD_ERR_NOT_EXISTS)
 * </para></listitem>
 * </itemizedlist>
 *
 * Returned array should be freed (g_free()) when no longer needed.
 * 
 * Returns: (transfer full): newly allocated and NULL terminated array of #MidgardUser objects or %NULL
 * 
 * Since 9.09
 */
MidgardUser **
midgard_user_query (MidgardConnection *mgd, guint n_params, const GParameter *parameters)
{
	MidgardUserClass *klass = g_type_class_peek (MIDGARD_TYPE_USER);
	return klass->query (mgd, n_params, parameters);
}

MidgardUser **
__midgard_user_query (MidgardConnection *mgd, guint n_params, const GParameter *parameters)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (parameters != NULL, NULL);

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	/* Deny queries for non admin user*/
	MidgardUser *cuser = midgard_connection_get_user (mgd);
	if (!cuser || (cuser && !midgard_user_is_admin (cuser))) {

		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_ACCESS_DENIED, "Not allowed to query users for non admin users.");
		return NULL;
	}

	guint i;
	MidgardUserClass *klass = g_type_class_peek (MIDGARD_TYPE_USER);

	/* Validate properties and find required ones */
	if (!__validate_parameters(mgd, n_params, parameters, 1)) 
		return NULL;

	/* Create new parameters. We need to pass column names instead of properties */
	GValue idval = {0, };
	g_value_init (&idval, G_TYPE_UINT);
	GParameter *tbl_params = __convert_to_storage_parameters (mgd, n_params, parameters, klass, idval);
	if (!tbl_params) {
		
		g_value_unset (&idval);
		return NULL;
	}
	GdaDataModel *model = midgard_core_query_get_dbobject_model (mgd, MIDGARD_DBOBJECT_CLASS (klass), n_params, tbl_params);

	g_free (tbl_params);
	g_value_unset (&idval);

	if (!model) {

		MIDGARD_ERRNO_SET (mgd, MGD_ERR_NOT_EXISTS);
		return NULL;
	}

	guint rows = gda_data_model_get_n_rows(model);
	if(rows == 0) {

		if (model) {
			g_object_unref(model);
			model = NULL;
		}

		MIDGARD_ERRNO_SET (mgd, MGD_ERR_NOT_EXISTS);
		return NULL;
	}

	MidgardUser **users = g_new (MidgardUser*, rows+1);

	for (i = 0; i < rows; i++) {
	
		users[i] = g_object_new (MIDGARD_TYPE_USER, "connection", mgd, NULL);
		MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->__set_from_sql (MIDGARD_DBOBJECT (users[i]), model, i);
	}

	users[i] = NULL;

	return users;
}	

/** 
 * midgard_user_create:
 * @self: #MidgardUser instance
 *
 * Creates database record for given user.
 * 
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * User with such login and authentication type already exists ( MGD_ERR_DUPLICATE ) 
 * </para></listitem>
 * <listitem><para>
 * User's guid is already set ( MGD_ERR_INVALID_PROPERTY_VALUE )
 * </para></listitem>
 * <listitem><para>
 * 'authtype' property is empty or NULL ( MGD_ERR_INVALID_PROPERTY_VALUE )
 * </para></listitem>
 * <listitem><para>
 * 'authtype' property value is invalid ( MGD_ERR_INTERNAL )
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 9.09
 */
gboolean 
midgard_user_create (MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);

	MidgardUserClass *klass = MIDGARD_USER_GET_CLASS (self);
	return klass->create (self);
}

gboolean 
__midgard_user_create (MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (MIDGARD_IS_USER (self), FALSE);
	
	MidgardConnection *mgd = MGD_OBJECT_CNC (self);
	const gchar *guid = MGD_OBJECT_GUID (self);

	g_return_val_if_fail (mgd != NULL, FALSE);

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	/* Validate guid */
	if (guid || (guid && !midgard_is_guid (guid))) {

		MIDGARD_ERRNO_SET (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
		return FALSE;
	}

	/* Validate auth type */	
	gchar *auth_type;
	GValue aval = {0, };
	g_value_init (&aval, G_TYPE_STRING);

	g_object_get_property (G_OBJECT (self), "authtype", &aval);
	auth_type = (gchar *) g_value_get_string (&aval);

	if (auth_type == NULL || (auth_type && *auth_type == '\0')) {
	
		g_value_unset (&aval);
		MIDGARD_ERRNO_SET (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
		return FALSE;
	}

	guint id = midgard_core_auth_type_id_from_name (mgd, auth_type);

	if (id == 0) {
		
		g_value_unset (&aval);
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INTERNAL, "Unknown authentication type");
		return FALSE;
	}

	self->priv->auth_type_id = id;

	/* Create parameters to get user */
	GValue lval = {0, };
	g_value_init (&lval, G_TYPE_STRING);
	g_object_get_property (G_OBJECT (self), "login", &lval);

	GParameter *parameters = g_new0 (GParameter, 2);
	parameters[0].name = "login";
	parameters[0].value = lval;
	parameters[1].name = "authtype";
	parameters[1].value = aval;
	
	/* Check if user with given login and authtype already exists */
	MidgardUser *user = midgard_user_get (mgd, 2, (const GParameter *) parameters);

	/* Reset error, it might be changed by midgard_user_get */
	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	g_free (parameters);
	g_value_unset (&aval);
	g_value_unset (&lval);

	if (user) {
		
		g_object_unref (user);
		MIDGARD_ERRNO_SET (mgd, MGD_ERR_DUPLICATE);
		return FALSE;
	}

	/* Set correct auth type id */

	/* Create record */
	MIDGARD_DBOBJECT (self)->dbpriv->guid = midgard_guid_new (mgd);

	if (midgard_core_query_create_dbobject_record (MIDGARD_DBOBJECT (self)))
		return TRUE;

	g_free ( (gchar *)MIDGARD_DBOBJECT (self)->dbpriv->guid);
	MIDGARD_DBOBJECT (self)->dbpriv->guid = NULL;

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_INTERNAL);

	return FALSE;
}

/**
 * midgard_user_update:
 * @self: #MidgardUser instance
 *
 * Updates user storage record
 * 
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * User with such login and authentication type already exists ( MGD_ERR_DUPLICATE ) 
 * </para></listitem>
 * <listitem><para>
 * User's guid is not set ( MGD_ERR_INVALID_PROPERTY_VALUE )
 * </para></listitem>
 * <listitem><para>
 * 'authtype' property is empty or NULL ( MGD_ERR_INVALID_PROPERTY_VALUE )
 * </para></listitem>
 * <listitem><para>
 * User record hasn't been found ( MGD_ERR_INTERNAL )
 * </para></listitem>
 * <listitem><para>
 * Failed to update storage record ( MGD_ERR_INTERNAL )
 * </para></listitem>
 * <listitem><para>
 * 'authtype' property value is invalid ( MGD_ERR_INTERNAL )
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * 
 * Since: 9.09
 */
gboolean 
midgard_user_update (MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);

	MidgardUserClass *klass = MIDGARD_USER_GET_CLASS (self);
	return klass->update (self);
}

gboolean
__midgard_user_update (MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (MIDGARD_IS_USER (self), FALSE);
	
	MidgardConnection *mgd = MGD_OBJECT_CNC (self);
	const gchar *guid = MGD_OBJECT_GUID (self);

	g_return_val_if_fail (mgd != NULL, FALSE);

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	/* Validate guid */
	if (!guid
		|| (*guid && *guid == '\0')
 		|| (guid && !midgard_is_guid (guid))) {

		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INVALID_PROPERTY_VALUE, "Invalid guid value");
		return FALSE;
	}

	/* Validate auth type */	
	gchar *auth_type;
	GValue aval = {0, };
	g_value_init (&aval, G_TYPE_STRING);

	g_object_get_property (G_OBJECT (self), "authtype", &aval);
	auth_type = (gchar *) g_value_get_string (&aval);

	if (auth_type == NULL || (auth_type && *auth_type == '\0')) {
	
		g_value_unset (&aval);
		MIDGARD_ERRNO_SET (mgd, MGD_ERR_INVALID_PROPERTY_VALUE);
		return FALSE;
	}

	guint id = midgard_core_auth_type_id_from_name (mgd, auth_type);

	if (id == 0) {
		
		g_value_unset (&aval);
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INTERNAL, "Unknown authentication type");
		return FALSE;
	}

	self->priv->auth_type_id = id;

	/* Create parameters to get user */
	GValue lval = {0, };
	g_value_init (&lval, G_TYPE_STRING);
	g_object_get_property (G_OBJECT (self), MIDGARD_USER_PROP_LOGIN, &lval);

	guint np = 2;
	GParameter *parameters = g_new0 (GParameter, np);
	parameters[0].name = MIDGARD_USER_PROP_LOGIN;
	parameters[0].value = lval;
	parameters[1].name = "authtype";
	parameters[1].value = aval;	

	/* Check if user with given guid and authtype already exists */
	MidgardUser *user = midgard_user_get (mgd, np, (const GParameter *) parameters);

	/* Reset error, it might be changed by midgard_user_get */
	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	g_free (parameters);
	g_value_unset (&aval);
	g_value_unset (&lval);

	if (user) {
	       
		if (!g_str_equal (MGD_OBJECT_GUID (user), MGD_OBJECT_GUID (self))) {
		
			MIDGARD_ERRNO_SET (mgd, MGD_ERR_DUPLICATE);
			g_object_unref (user);
			return FALSE;
		}
	}

	if (midgard_core_query_update_dbobject_record (MIDGARD_DBOBJECT (self), NULL) )
		return TRUE;

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_INTERNAL);
	return FALSE;
}

/**
 * midgard_user_delete:
 * @self: #MidgardUser instance
 *
 * Delete user's storage record.
 * 
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * User's guid is not set ( MGD_ERR_INVALID_PROPERTY_VALUE )
 * </para></listitem>
 * <listitem><para>
 * Failed to delete storage record ( MGD_ERR_INTERNAL )
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: @TRUE on success, @FALSE otherwise
 *
 * Since: 9.09.2
 */ 
gboolean 
midgard_user_delete (MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);

	MidgardUserClass *klass = MIDGARD_USER_GET_CLASS (self);
	return klass->delete_record (self);
}

gboolean 
__midgard_user_delete (MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (MIDGARD_IS_USER (self), FALSE);
	
	MidgardConnection *mgd = MGD_OBJECT_CNC (self);
	const gchar *guid = MGD_OBJECT_GUID (self);

	g_return_val_if_fail (mgd != NULL, FALSE);

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	/* Validate guid */
	if (!guid
		|| (*guid && *guid == '\0')
 		|| (guid && !midgard_is_guid (guid))) {

		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INVALID_PROPERTY_VALUE, "Invalid guid value");
		return FALSE;
	}

	GString *del_query = g_string_new ("DELETE FROM midgard_user WHERE ");
	g_string_append_printf (del_query, "guid='%s'", guid);

      	g_debug ("%s", del_query->str);

	GError *error = NULL;
	GdaConnection *connection = mgd->priv->connection;

#ifdef HAVE_LIBGDA_4
	
	gda_execute_non_select_command (connection, del_query->str, &error);

#else 
	GdaCommand *command = gda_command_new (del_query->str, GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	gda_connection_execute_non_select_command (connection, command, NULL, &error);
	gda_command_free (command);

#endif /* HAVE_LIBGDA_4 */
	
	g_string_free (del_query, TRUE);

	if(error){
		g_clear_error(&error);
		return FALSE;
	}

	/* Set empty guid */
	g_free ((gchar *)MIDGARD_DBOBJECT (self)->dbpriv->guid);
	MIDGARD_DBOBJECT (self)->dbpriv->guid = NULL;

	return TRUE;
}

/** 
 * midgard_user_log_in:
 * @self: #MidgardUser instance
 * 
 * Logs in user instance, if given one is valid. 
 * A valid user object must have (at least) guid set. 
 * Which means, #MidgardObject must be fetched from database.
 * Either with midgard_user_get() or with midgard_user_query().
 *
 * This method silently returns with success when given user is already logged in.
 *
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * User's guid is not set ( MGD_ERR_INVALID_PROPERTY_VALUE )
 * </para></listitem>
 * </itemizedlist>
 * 
 * Returns: %TRUE if user has been logged in, %FALSE otherwise
 *
 * Since: 9.09
 */
gboolean 
midgard_user_log_in (MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);

	MidgardUserClass *klass = MIDGARD_USER_GET_CLASS (self);
	return klass->log_in (self);
}

gboolean
__midgard_user_login (MidgardUser *self)
{
	g_return_val_if_fail(self != NULL, FALSE);
	g_return_val_if_fail(MIDGARD_IS_USER (self), FALSE);

	MidgardConnection *mgd = MGD_OBJECT_CNC(self);
	g_return_val_if_fail(mgd != NULL, FALSE);

	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	/* Check if given user instance is already logged in */
	if (mgd->priv->user && mgd->priv->user == self)
		return TRUE;

	const gchar *guid = MGD_OBJECT_GUID(self);
	if (!guid
		|| (guid && *guid == '\0')
		|| (guid && !midgard_is_guid(guid)))
	{
		MIDGARD_ERRNO_SET_STRING(mgd, MGD_ERR_INVALID_PROPERTY_VALUE, "'guid' property doesn't hold guid value");
		return FALSE;
	}

	g_object_ref(self); // storing as priv->user
	mgd->priv->user = self;

	g_object_ref(self); // storing in priv->authstack
	mgd->priv->authstack = g_slist_append(mgd->priv->authstack, self);

	self->priv->is_logged = TRUE;
	g_signal_emit_by_name(mgd, "auth_changed");

	return TRUE;
}

/**
 * midgard_user_log_out:
 * @self: #MidgardUser instance
 *
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * There's no user logged in (MGD_ERR_INTERNAL)
 * </para></listitem>
 * <listitem><para>
 * User is not recently logged in (MGD_ERR_INTERNAL)
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE if user successfully logs out, %FALSE otherwise
 *
 * Since: 9.09
 */
gboolean 
midgard_user_log_out (MidgardUser *self)
{
	g_return_val_if_fail(self != NULL, FALSE);

	MidgardUserClass *klass = MIDGARD_USER_GET_CLASS(self);
	return klass->log_out(self);
}

gboolean 
__midgard_user_logout (MidgardUser *self)
{
	g_return_val_if_fail(self != NULL, FALSE);
	g_return_val_if_fail(MIDGARD_IS_USER (self), FALSE);

	MidgardConnection *mgd = MGD_OBJECT_CNC (self);
	g_return_val_if_fail(mgd != NULL, FALSE);

	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	GSList *list = mgd->priv->authstack;

	if (list == NULL || (list && g_slist_length(list) < 1)) {
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);
		return FALSE;
	}

	if ((gpointer)self == (gpointer)mgd->priv->user) {
		mgd->priv->authstack = g_slist_remove(mgd->priv->authstack, (gconstpointer) self);

		self->priv->is_logged = FALSE;
		g_object_unref(self); // not storing it in priv->authstack anymore

		g_object_unref(mgd->priv->user); // will overwrite priv->user in a moment
		if (mgd->priv->authstack && mgd->priv->authstack->data) {
			g_object_ref(mgd->priv->authstack->data); // storing in priv->user
			mgd->priv->user = (gpointer) mgd->priv->authstack->data;
		} else {
			mgd->priv->user = NULL;
		}

		g_signal_emit_by_name(mgd, "auth_changed");

		return TRUE;
	}

	/* Throw this warning explicitly as it's programmer fault.
	   Some language bindings might throw an exception in such case. */
	g_warning("Can not log out user who is not currently logged in");
	MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);

	return FALSE;
}

/**
 * midgard_user_is_user:
 * @self: #MidgardUser instance
 *
 * Checks if given user is a user.
 * For example, function will return FALSE for user who is logged in as admin or root.
 * 
 * Returns: %TRUE if user is a user, %FALSE otherwise
 */
gboolean midgard_user_is_user(MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);

	MidgardUserClass *klass = MIDGARD_USER_GET_CLASS (self);
	return klass->is_user (self);
}

gboolean 
__midgard_user_is_user (MidgardUser *self)
{	
	if (!self || !self->priv)
		return FALSE;

	if (self->priv->user_type == MIDGARD_USER_TYPE_USER)
		return TRUE;

	return FALSE;
}

/**
 * midgard_user_is_admin:
 * @self: #MidgardUser instance
 *
 * Checks if given user is an admin.
 * 
 * Returns: %TRUE if user is an admin, %FALSE otherwise
 */
gboolean 
midgard_user_is_admin (MidgardUser *self)
{
	g_return_val_if_fail (self != NULL, FALSE);

	MidgardUserClass *klass = MIDGARD_USER_GET_CLASS (self);
	return klass->is_admin (self);
}


gboolean 
__midgard_user_is_admin(MidgardUser *self)
{
	if (!self || !self->priv)
		return FALSE;

	if (self->priv->user_type == MIDGARD_USER_TYPE_ADMIN)
		return TRUE;
	
	return FALSE;
}

/* Set active flag */
gboolean midgard_user_set_active(MidgardUser *self, gboolean flag)
{
	g_assert(self != NULL);

	if(!MIDGARD_DBOBJECT (self)->dbpriv->mgd) {
		g_warning("Can not set activity for user without person assigned");
		return FALSE;
	}

	MidgardConnection *mgd = MIDGARD_DBOBJECT (self)->dbpriv->mgd;
	MidgardUser *user = MIDGARD_USER(mgd->priv->user);

	MIDGARD_ERRNO_SET (mgd, MGD_ERR_OK);

	if(!user) {
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_ACCESS_DENIED);
		return FALSE;
	}
	
	if(midgard_user_is_user(user)) {
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_ACCESS_DENIED);
		return FALSE;
	}

	guint active = 0;
	if(flag)
		active = 1;

	GString *sql = g_string_new("UPDATE midgard_user SET ");
	g_string_append_printf(sql, 
			"active = %d WHERE guid = '%s' ",
			active,
			MIDGARD_DBOBJECT (self)->dbpriv->guid); 
			
	
	gint rv =  midgard_core_query_execute(mgd, sql->str, FALSE);
	g_string_free(sql, TRUE);

	if(rv == -1)
		return FALSE;

	self->priv->active = flag;

	if(rv == -2) {
		g_warning("Provider didn't return number of updated rows. User record might be not updated");
		return TRUE;
	}
	
	return TRUE;
}

static void __set_from_sql(MidgardDBObject *object, 
		GdaDataModel *model, gint row)
{
	MidgardUser *self = MIDGARD_USER(object);	
	
	/* 'at_col_name' acceptable since we do not use this frequently */
	const GValue *value;
 	guint i = 0;	

	/* GUID */
	value = midgard_data_model_get_value_at_col_name (model, "guid", row);
	if (value)
		MIDGARD_DBOBJECT (self)->dbpriv->guid = g_value_dup_string (value);

	/* LOGIN */
	value = midgard_data_model_get_value_at_col_name (model, "login", row);
	if (value)
		self->priv->login = g_value_dup_string (value);

	/* PASSWORD */
	value = midgard_data_model_get_value_at_col_name (model, "password", row);
	if (value && G_VALUE_HOLDS_STRING (value))
		self->priv->password = g_value_dup_string (value);
	else 
		g_warning ("Invalid value type for user's password. Expected string, got %s", value ? G_VALUE_TYPE_NAME (value) : "NULL");

	/* PERSON GUID */
	value = midgard_data_model_get_value_at_col_name (model, "person", row);
	if (value && G_IS_VALUE (value) && G_VALUE_HOLDS_STRING (value)) {
		g_free (self->priv->person_guid);
		self->priv->person_guid = g_value_dup_string (value);
	
	} else {
		g_warning ("Invalid value type for user's person guid. Expected string, got %s", value ? G_VALUE_TYPE_NAME (value) : "NULL");
	}

	/* ACTIVE */
	value = midgard_data_model_get_value_at_col_name (model, "active", row);
	if (value) {
		self->priv->active = g_value_get_boolean (value);
	}

	/* AUTH TYPE */
	value = midgard_data_model_get_value_at_col_name (model, "authtype", row);
	if (value)
 		self->priv->auth_type = g_value_dup_string (value);
	

	/* AUTH TYPE ID */
	value = midgard_data_model_get_value_at_col_name (model, "authtypeid", row);
	if (value) {
		MIDGARD_GET_UINT_FROM_VALUE (i, value);
		self->priv->auth_type_id = i;
	}

	/* USER TYPE */
	value = midgard_data_model_get_value_at_col_name (model, "usertype", row);
	if (value) {
		MIDGARD_GET_UINT_FROM_VALUE (i, value);
		self->priv->user_type = i;
	}
}

/**
 * midgard_user_set_person:
 * @self: #MidgardUser instance
 * @person: #MidgardObject instance 
 *
 * Associates given #MidgardObject person with @self #MidgardUser.
 * Sets person property and updates user storage record.
 *
 * #MidgardUser @self takes ownership of the given #MidgardObject ('midgard_person' type) reference, 
 * and increases person's object reference count. 
 * 
 * See midgard_user_update() for returned error details.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 */
gboolean
midgard_user_set_person (MidgardUser *self, MidgardObject *person)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (person != NULL, FALSE);

	GType person_type = g_type_from_name ("midgard_person");
	g_return_val_if_fail (MIDGARD_IS_OBJECT (person), FALSE);
	g_return_val_if_fail (person_type == G_OBJECT_TYPE (person), FALSE);

	g_return_val_if_fail (MGD_OBJECT_GUID (person) != NULL, FALSE);	

	if (G_IS_OBJECT (self->priv->person))
		g_object_unref (self->priv->person);
	self->priv->person = g_object_ref (person);
	g_free (self->priv->person_guid);
	self->priv->person_guid = g_strdup (MGD_OBJECT_GUID (person));

	return midgard_user_update (self);
}

/**
 * midgard_user_get_person:
 * @self: #MidgardUser instance
 *
 * Returned object should not be unref.
 *
 * Returns: (transfer none): #MidgardObject of "midgard_person" type, or %NULL if none associated.
 */
MidgardObject *
midgard_user_get_person (MidgardUser *self)
{
	MidgardUserClass *klass = MIDGARD_USER_GET_CLASS (self);
	return klass->get_person (self);	
}

MidgardObject *
__midgard_user_get_person(MidgardUser *self)
{
	g_assert(self != NULL);

	if (self->priv->person == NULL) {

		gchar *person_guid = self->priv->person_guid;

		/* Load person from storage if user holds its guid reference */
		if (person_guid && *person_guid != '\0') {
		
			GValue gval = {0, };
			g_value_init (&gval, G_TYPE_STRING);
			g_value_set_string (&gval, self->priv->person_guid);
			self->priv->person = midgard_object_new (MGD_OBJECT_CNC (self), "midgard_person", &gval);
			g_value_unset (&gval);
		}
	}

	return self->priv->person;
}

MidgardUser *
midgard_user_quick_login (MidgardConnection *mgd, const gchar *login, const gchar *password)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (login != NULL, NULL);
	g_return_val_if_fail (password != NULL, NULL);

	guint np = 3;
	GParameter *parameters = g_new0 (GParameter, np);
	parameters[0].name = "login";
	GValue lval = {0, };
	g_value_init (&lval, G_TYPE_STRING);
	g_value_set_string (&lval, login);
	parameters[0].value = lval;

	parameters[1].name = "password";
	GValue pval = {0, };
	g_value_init (&pval, G_TYPE_STRING);
	g_value_set_string (&pval, password);
	parameters[1].value = pval;

	parameters[2].name = "authtype";
	GValue aval = {0, };
	g_value_init (&aval, G_TYPE_STRING);
	g_value_set_string (&aval, "Plaintext");
	parameters[2].value = aval;

	MidgardUser *user = midgard_user_new (mgd, np, parameters);

	g_value_unset (&lval);
	g_value_unset (&pval);
	g_value_unset (&aval);
	g_free (parameters);

	if (!user)
		return NULL;

	if (midgard_user_log_in (user))
		return user;

	g_object_unref (user);
	return NULL;
}

/* GOBJECT ROUTINES */

static void _midgard_user_finalize(GObject *object)
{
	g_assert (object != NULL);
	
	MidgardUser *self = (MidgardUser *) object;
	
	if (self && (MIDGARD_DBOBJECT (self)->dbpriv && self->priv->is_logged))
		(void) midgard_user_log_out (self);

	g_free (self->priv->auth_type);
	self->priv->auth_type = NULL;

	self->priv->auth_type_id = 0;

	self->priv->user_type = MIDGARD_USER_TYPE_NONE;

	g_free (self->priv->login);
	self->priv->login = NULL;

	g_free (self->priv->password);
	self->priv->password = NULL;

	g_free (self->priv->person_guid);
	self->priv->person_guid = NULL;

	g_free(self->priv);
	self->priv = NULL;	

	__parent_class->finalize (object);
}

static void
_midgard_user_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardUser *self = MIDGARD_USER(object);

	switch (property_id) {
	     
		case MIDGARD_USER_LOGIN:
			g_free (self->priv->login);
			self->priv->login = g_value_dup_string (value);
			break;

		case MIDGARD_USER_PASS:
			g_free (self->priv->password);
			self->priv->password = g_value_dup_string (value);
			break;

		case MIDGARD_USER_ACTIVE:
			self->priv->active = g_value_get_boolean (value);
			break;

		case MIDGARD_USER_AUTH_TYPE:
			g_free (self->priv->auth_type);
			self->priv->auth_type = g_value_dup_string (value);
			/* TODO, set auth type id via core API */
			break;

		case MIDGARD_USER_TYPE:
			if (g_value_get_uint (value) > MIDGARD_USER_TYPE_ADMIN
					|| g_value_get_uint (value) < MIDGARD_USER_TYPE_NONE) {
	
				g_warning ("Invalid user type (%d).", g_value_get_uint (value));

			} else {

				self->priv->user_type = g_value_get_uint (value);	
			}
			break;
	
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
			break;

	}
}


static void
_midgard_user_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardUser *self = (MidgardUser *) object;
	
	switch (property_id) {
		
		case MIDGARD_USER_GUID:
			g_value_set_string(value, MIDGARD_DBOBJECT (self)->dbpriv->guid);
			break;

		case MIDGARD_USER_LOGIN:
			g_value_set_string(value, self->priv->login);
			break;

		case MIDGARD_USER_PASS:
			g_value_set_string(value, self->priv->password);
			break;

		case MIDGARD_USER_ACTIVE:
			g_value_set_boolean(value, self->priv->active);	
			break;

		case MIDGARD_USER_TYPE:
			g_value_set_uint (value, self->priv->user_type);
			break;

		case MIDGARD_USER_AUTH_TYPE:
			g_value_set_string (value, self->priv->auth_type);
			break;

		case MIDGARD_USER_AUTH_TYPE_ID:
			g_value_set_uint (value, self->priv->auth_type_id);
			break;

		case MIDGARD_USER_PERSON_GUID:
			g_value_set_string (value, self->priv->person_guid);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(self, property_id, pspec);
			break;
	}
}

static GObject *
__midgard_user_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (__parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	MIDGARD_USER (object)->priv = g_new(MidgardUserPrivate, 1);
	MIDGARD_USER (object)->priv->person = NULL;
	MIDGARD_USER (object)->priv->person_guid = g_strdup ("");
	MIDGARD_USER (object)->priv->user_type = MIDGARD_USER_TYPE_NONE;
	MIDGARD_USER (object)->priv->fetched = FALSE;
	MIDGARD_USER (object)->priv->login = NULL;
	MIDGARD_USER (object)->priv->password = NULL;
	MIDGARD_USER (object)->priv->active = FALSE;
	MIDGARD_USER (object)->priv->auth_type = NULL;
	MIDGARD_USER (object)->priv->auth_type_id = 0;	
	MIDGARD_USER (object)->priv->is_logged = FALSE;

	return G_OBJECT(object);
}

static void
__midgard_user_dispose (GObject *object)
{
	MidgardUser *self = MIDGARD_USER (object);	
	if (self->priv->person && G_IS_OBJECT (self->priv->person))
		g_object_unref (self->priv->person);
	__parent_class->dispose (object);
}

static gboolean 
_user_storage_exists (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(klass != NULL, FALSE);

	return midgard_core_table_exists(mgd, MIDGARD_USER_TABLE);
}

static gboolean 
_user_storage_delete (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (klass != NULL, FALSE);

	g_warning ("midgard_user storage can not be deleted");
	return FALSE;
}

static void _midgard_user_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardUserClass *klass = MIDGARD_USER_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	 
	gobject_class->constructor = __midgard_user_constructor;
	gobject_class->dispose = __midgard_user_dispose;
	gobject_class->finalize = _midgard_user_finalize;
	gobject_class->get_property = _midgard_user_get_property;
	gobject_class->set_property = _midgard_user_set_property;

	/* Virtual methods */
	klass->get_person = __midgard_user_get_person;
	klass->log_in = __midgard_user_login;
	klass->log_out = __midgard_user_logout;
	klass->get = __midgard_user_get;
	klass->query = __midgard_user_query;
	klass->create = __midgard_user_create;
	klass->update = __midgard_user_update;
	klass->delete_record = __midgard_user_delete;
	klass->is_user = __midgard_user_is_user;
	klass->is_admin = __midgard_user_is_admin;

	MgdSchemaPropertyAttr *prop_attr;
	MgdSchemaTypeAttr *type_attr = midgard_core_schema_type_attr_new();

	GParamSpec *pspec;
	gchar *property_name;

	 /* GUID */
	property_name = "guid";
	pspec = g_param_spec_string (property_name,
			"Guid which identifies user object.",
			"",
			"",
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_USER_GUID,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_GUID;
	prop_attr->field = g_strdup(property_name);
	prop_attr->table = g_strdup(MIDGARD_USER_TABLE);
	prop_attr->tablefield = g_strjoin(".", MIDGARD_USER_TABLE, property_name, NULL);
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

	/* LOGIN */
	property_name = "login";
	pspec = g_param_spec_string (property_name,
			"midgard_user's login",
			"",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_USER_LOGIN,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_STRING;
	prop_attr->field = g_strdup(property_name);
	prop_attr->table = g_strdup(MIDGARD_USER_TABLE);
	prop_attr->tablefield = g_strjoin(".", MIDGARD_USER_TABLE, property_name, NULL);
	prop_attr->dbindex = TRUE;
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

	 /* PASSWORD */
	property_name = "password";
	pspec = g_param_spec_string ("password",
			"midgard_user's password",
			"",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_USER_PASS,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_STRING;
	prop_attr->field = g_strdup(property_name);
	prop_attr->table = g_strdup(MIDGARD_USER_TABLE);
	prop_attr->tablefield = g_strjoin(".", MIDGARD_USER_TABLE, property_name, NULL);
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

       	/* PERSON */
	property_name = "person";
	pspec = g_param_spec_string (property_name,
			"Guid which identifies person object associated with user.",
			"",
			"",
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_USER_PERSON_GUID,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_GUID;
	prop_attr->field = g_strdup("person_guid");
	prop_attr->table = g_strdup(MIDGARD_USER_TABLE);
	prop_attr->tablefield = g_strjoin(".", MIDGARD_USER_TABLE, "person_guid", NULL);
	prop_attr->is_link = TRUE;
	prop_attr->link = g_strdup ("midgard_person");
	prop_attr->link_target = g_strdup ("guid");
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

	/* ACTIVE */
	property_name = "active";
	pspec = g_param_spec_boolean (property_name,
			"midgard_user's active info",
			"",
			FALSE, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_USER_ACTIVE,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_BOOLEAN;
	prop_attr->field = g_strdup(property_name);
	prop_attr->table = g_strdup(MIDGARD_USER_TABLE);
	prop_attr->tablefield = g_strjoin(".", MIDGARD_USER_TABLE, property_name, NULL);
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

	 /* AUTH TYPE  */
	property_name = "authtype";
	pspec = g_param_spec_string (property_name,
			"midgard_user's authentication type",
			"",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_USER_AUTH_TYPE,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_STRING;
	prop_attr->field = g_strdup("auth_type");
	prop_attr->table = g_strdup(MIDGARD_USER_TABLE);
	prop_attr->tablefield = g_strjoin(".", MIDGARD_USER_TABLE, "auth_type", NULL);
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

	/* AUTH TYPE ID */
	property_name = "authtypeid";
	pspec = g_param_spec_uint (property_name,
			"midgard_user's authentication type id",
			"",
			0, G_MAXUINT32, 0, G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_USER_AUTH_TYPE_ID,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup("auth_type_id");
	prop_attr->table = g_strdup(MIDGARD_USER_TABLE);
	prop_attr->tablefield = g_strjoin(".", MIDGARD_USER_TABLE, "auth_type_id", NULL);
	prop_attr->dbindex = TRUE;
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

	/* USER TYPE */
	property_name = "usertype";
	pspec = g_param_spec_uint (property_name,
			"midgard_user's type",
			"",
			0, G_MAXUINT32, 0, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_USER_TYPE,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup("user_type");
	prop_attr->table = g_strdup(MIDGARD_USER_TABLE);
	prop_attr->tablefield = g_strjoin(".", MIDGARD_USER_TABLE, "user_type", NULL);
	prop_attr->dbindex = TRUE;
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv = g_new(MidgardDBObjectPrivate, 1);
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data = type_attr;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data->table = g_strdup(MIDGARD_USER_TABLE);
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data->tables = g_strdup(MIDGARD_USER_TABLE);
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->has_metadata = FALSE;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->uses_workspace = FALSE;

	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->__set_from_sql = __set_from_sql;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->set_from_sql = NULL;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->create_storage = midgard_core_query_create_class_storage;	
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->update_storage = midgard_core_query_update_class_storage;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_exists = _user_storage_exists; 
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->delete_storage = _user_storage_delete;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->set_static_sql_select = NULL;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_insert = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_statement_insert;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_insert_params = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_statement_insert_params;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_update = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_statement_update;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_update_params = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_statement_update_params;

	type_attr->params = midgard_core_dbobject_class_list_properties (MIDGARD_DBOBJECT_CLASS (klass), &type_attr->num_properties);	
	guint i;
	const gchar *field_name;
	GString *sql_full = g_string_new ("");

	for (i = 0; i < type_attr->num_properties; i++) {
		field_name = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (klass), type_attr->params[i]->name);
		if (i > 0)
			g_string_append (sql_full, ", ");
		g_string_append_printf (sql_full, "%s AS %s", field_name, type_attr->params[i]->name);
	}
	type_attr->sql_select_full = g_strdup (sql_full->str);
	g_string_free (sql_full, TRUE);
}

static void _midgard_user_instance_init(
		GTypeInstance *instance, gpointer g_class)
{
	return;	
}

/* Register midgard_user type */
GType midgard_user_get_type(void) 
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardUserClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_user_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardUser),
			0,              /* n_preallocs */
			(GInstanceInitFunc) _midgard_user_instance_init/* instance_init */
		};
		type = g_type_register_static (MIDGARD_TYPE_DBOBJECT, "MidgardUser", &info, 0);
	}
	
	return type;
}
