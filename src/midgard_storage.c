/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include <glib.h>
#include <glib-object.h>
#include "midgard_defs.h"
#include "midgard_storage.h"
#include "midgard_core_object.h"
#include "midgard_core_query.h"
#include "midgard_user.h"
#include "midgard_core_connection.h"
#include "midgard_core_metadata.h"
#include "midgard_replicator.h"
#include "midgard_core_workspace.h"

#define ADMIN_USER_GUID "f4edf43cc32a11deb15c11e64583ae10ae10"
#define ADMIN_USER_LOGIN "admin"
#define ADMIN_USER_PASSWORD "password"

#define ADMIN_PERSON_GUID "f6b665f1984503790ed91f39b11b5392"
#define ADMIN_PERSON_CREATED "1999-05-18 14:40:01"
#define ADMIN_PERSON_FIRSTNAME "Midgard"
#define ADMIN_PERSON_LASTNAME "Administrator"

#define DEFAULT_AUTH_TYPE "Plaintext"

struct _MidgardStorage {
	GObject parent;
};

struct _MidgardStorageClass {
	GObjectClass parent;
};

/**
 * midgard_storage_create_base_storage:
 * @mgd: #MidgardConnection instance 
 *
 * Creates storage for base Midgard classes.
 * Tables created by this method: repligard, midgard_user and midgard_person.
 * 
 * Returns: %TRUE if tables has been created, %FALSE otherwise.
 *
 * Since: 9.09
 */
gboolean
midgard_storage_create_base_storage(MidgardConnection *mgd)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(mgd->priv->connection != NULL, FALSE);

	gboolean ret =  midgard_core_query_create_basic_db(mgd);

	if(!ret)
		return FALSE;
	
	/* Create midgard_user storage */	
	midgard_storage_create (mgd, g_type_name (MIDGARD_TYPE_USER));

	GString *query = g_string_new ("SELECT id from midgard_user where guid = ");
	g_string_append_printf (query, "'%s'", ADMIN_USER_GUID);
	const gchar *sql = query->str;

	GdaDataModel *model = midgard_core_query_get_model(mgd, sql);
	guint ret_rows = 0;

	if(model)
		ret_rows = gda_data_model_get_n_rows(model);

        if(!model || ret_rows == 0) {

		/* Enable admin login */
		GString *query = g_string_new ("INSERT INTO midgard_user (login, password, user_type, auth_type, auth_type_id, guid, person_guid, active)");
		g_string_append_printf (query, "VALUES ('%s', '%s', %d, '%s', %d, '%s', '%s', ##active::boolean)",
				ADMIN_USER_LOGIN, ADMIN_USER_PASSWORD,
				MIDGARD_USER_TYPE_ADMIN, DEFAULT_AUTH_TYPE, 
				midgard_core_auth_type_id_from_name (mgd, DEFAULT_AUTH_TYPE), 
				ADMIN_USER_GUID, ADMIN_PERSON_GUID);
		
		sql = query->str;
		GValue *bval = g_new0 (GValue, 1);
		g_value_init (bval, G_TYPE_BOOLEAN);
		g_value_set_boolean (bval, TRUE);
		midgard_core_query_execute_statement (mgd, sql, TRUE, "active", bval, NULL);
		g_string_free (query, TRUE);
	}

	/* quota table */
	midgard_storage_create (mgd, "midgard_quota");

	/* person table */	
	midgard_storage_create (mgd, "midgard_person");
	sql = "SELECT lastname, firstname FROM person WHERE guid='f6b665f1984503790ed91f39b11b5392'";
	model = midgard_core_query_get_model(mgd, sql);

	if(model)
		ret_rows = gda_data_model_get_n_rows(model);

	if(!model || ret_rows == 0) {
		
		MidgardObject *person = midgard_object_new (mgd, "midgard_person", NULL);
		/* guid */
		g_free ((gchar *)MGD_OBJECT_GUID (person));
		MGD_OBJECT_GUID (person) = g_strdup (ADMIN_PERSON_GUID);
		/* lastname and firstname */
		g_object_set (G_OBJECT (person), 
			"lastname", ADMIN_PERSON_LASTNAME,
			"firstname", ADMIN_PERSON_FIRSTNAME, NULL);
		/* metadata creator */
		GValue val = {0, };
		g_value_init (&val, G_TYPE_STRING);
		g_value_set_string (&val, ADMIN_PERSON_GUID);
		midgard_core_metadata_set_creator (MGD_DBOBJECT_METADATA (person), &val);
		g_value_unset (&val);
		//midgard_object_create (person);

		/* metadata created */
		g_value_init (&val, G_TYPE_STRING);
		g_value_set_string (&val, ADMIN_PERSON_CREATED);
		midgard_core_metadata_set_created (MGD_DBOBJECT_METADATA (person), &val);
		g_value_unset (&val);

		midgard_replicator_import_object (MIDGARD_DBOBJECT (person), TRUE);
	}

	if(model)
		g_object_unref(model);

	/* workspace table */
	midgard_storage_create (mgd, "MidgardWorkspace");

	return TRUE;
}

/**
 * midgard_storage_create:
 * @mgd: #MidgardConnection instance
 * @name: name of #MidgardDBObject derived class
 * 
 * Creates underlying storage (e.g. table in database) for class which is identified by given @name.
 * It may be class which represents any underlying storage type (database table or view, for example).
 *
 * If underlying storage already exists, this method silently ignore creation
 * and returns %TRUE. Such case is not considered an error.
 *
 * This method also creates metadata storage if given class uses such.
 * 
 * Indexes are created if:
 * <itemizedlist>
 * <listitem><para>
 * property is a link type
 * </para></listitem>
 * <listitem><para>
 * property is linked to another property
 * </para></listitem>
 * <listitem><para>
 * property is either parent or up
 * </para></listitem>
 * <listitem><para>
 * property holds guid value
 * </para></listitem>
 * </itemizedlist>    
 *    
 * Auto increment (and primary key ) field is created if property is defined 
 * as primaryproperty, and it's integer ( or unsigned one ) type
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */
gboolean 
midgard_storage_create (MidgardConnection *mgd, const gchar *name)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	MidgardDBObjectClass *klass = g_type_class_peek (g_type_from_name (name));
	if (!MIDGARD_IS_DBOBJECT_CLASS (klass)) {
		g_warning ("Expected DBObject derived instance. '%s' given", name);
		return FALSE;
	}

	if (klass->dbpriv->create_storage == NULL)
		return FALSE;

	return klass->dbpriv->create_storage(mgd, klass);
}

/**
 * midgard_storage_update:
 * @mgd: #MidgardConnection instance
 * @name: Name of #MidgardDBObjectClass derived class.
 *
 * Update underlying storage.
 * 
 * This method doesn't create storage. It creates new columns if are defined
 * for class properties and do not exist in storage yet.
 *
 * See midgard_storage_create() if you need more info about indexes.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */
gboolean 
midgard_storage_update (MidgardConnection *mgd, const gchar *name)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	MidgardDBObjectClass *klass = g_type_class_peek (g_type_from_name (name));
	if (!MIDGARD_IS_DBOBJECT_CLASS (klass)) {
		g_warning ("Expected DBObject derived instance. '%s' given", name);
		return FALSE;
	}

	if (klass->dbpriv->update_storage == NULL)
		return FALSE;

	return klass->dbpriv->update_storage(mgd, klass);
}

/**
 * midgard_storage_exists:
 * @mgd: #MidgardConnection instance
 * @name: Name of #MidgardDBObjectClass derived class
 *
 * Checks whether storage for given class exists.
 *
 * Returns: %TRUE if storage exists, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean 
midgard_storage_exists (MidgardConnection *mgd, const gchar *name)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	MidgardDBObjectClass *klass = g_type_class_peek (g_type_from_name (name));
	if (!MIDGARD_IS_DBOBJECT_CLASS (klass)) {
		g_warning ("Expected DBObject derived instance. '%s' given", name);
		return FALSE;
	}

	if (klass->dbpriv->storage_exists == NULL)
		return FALSE;

	return klass->dbpriv->storage_exists(mgd, klass);
}

/**
 * midgard_storage_delete:
 * @mgd: #MidgardConnection instance
 * @name: name of #MidgardDBObjectClass derived class.
 * 
 * Delete storage for given class.
 * 
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean midgard_storage_delete (MidgardConnection *mgd, const gchar *name)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	MidgardDBObjectClass *klass = g_type_class_peek (g_type_from_name (name));
	if (!MIDGARD_IS_DBOBJECT_CLASS (klass)) {
		g_warning ("Expected DBObject derived instance. '%s' given", name);
		return FALSE;
	}

	if (klass->dbpriv->delete_storage == NULL)
		return FALSE;

	return klass->dbpriv->delete_storage(mgd, klass);
}

/* OBJECT ROUTINES */

GType	
midgard_storage_get_type(void)
{
	static GType type = 0;

	if (type == 0) {

		static const GTypeInfo info = {

			sizeof (MidgardStorageClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			NULL,		/* class init */ 
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardStorage),
			0,              /* n_preallocs */  
			NULL
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardStorage", &info, 0);
	}
	return type;
}
