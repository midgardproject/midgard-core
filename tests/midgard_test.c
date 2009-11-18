/* 
 * Copyright (C) 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

void midgard_test_setup_foo(MgdObjectTest *mot, gconstpointer data)
{
	/* Do nothing */
}

void midgard_test_teardown_foo(MgdObjectTest *mot, gconstpointer data)
{
	/* Do nothing */
}

void midgard_test_run_foo(MgdObjectTest *mot, gconstpointer data)
{
	/* Do nothing */
}

void midgard_test_unref_object(MgdObjectTest *mot, gconstpointer data)
{
	g_assert(data != NULL);
	GObject *object = G_OBJECT(data);
	g_object_unref(object);
}

void midgard_test_lock_root_objects(MidgardConnection *mgd, MidgardUser *user)
{
	g_assert(user != NULL);
	/* Lock root user */

	MgdObject *person = midgard_user_get_person(user);
	g_assert(person != NULL);

	if(!midgard_object_is_locked((MIDGARD_OBJECT(person)))) {
		gboolean person_locked = midgard_object_lock(person);
		if(!person_locked)
			g_print("Can not lock person because %s \n", midgard_connection_get_error_string(mgd));
		g_assert(person_locked != FALSE);
	}
}

gchar *midgard_test_get_current_person_guid(MidgardConnection *mgd)
{
	g_assert(mgd != NULL);

	MidgardUser *user = midgard_connection_get_user(mgd);
	g_assert(user != NULL);

	MgdObject *person = midgard_user_get_person(user);
	g_assert(person != NULL);
	
	gchar *pguid = NULL;
	g_object_get(person, "guid", &pguid, NULL);
	g_assert_cmpstr(pguid, !=, "");
	g_assert_cmpstr(pguid, !=, NULL);
	
	return pguid;
}

