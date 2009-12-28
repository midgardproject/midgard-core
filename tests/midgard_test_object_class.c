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

#include "midgard_test_object_class.h"
#include "midgard_test_object_basic.h"
#include "midgard_test_query_builder.h"

void midgard_test_object_class_undelete(MidgardObjectTest *mot, gconstpointer data)
{
	return; /* FIXME! This test fails and blocks others */
	g_assert(mot != NULL);
	
	/* We can not depend on object's guid as we add empty object to test */
	/*const gchar *guid = (const gchar *)mot->guid; 
	g_assert_cmpstr(guid, !=, NULL); */

	MidgardConnection *mgd = mot->mgd;
	const gchar *class_name = G_OBJECT_TYPE_NAME(mot->object);

	GObject **objects = midgard_test_query_builder_list_all_unlocked(mgd, class_name);
	g_assert(objects != NULL);
	
	gchar *guid;
	guint i = 0;
	while(objects[i] != NULL) {
		
		/* Fail if metadata.deleted is FALSE. We deleted all objects before. */
		gboolean deleted;
		g_object_get(MIDGARD_OBJECT(objects[i])->metadata, "deleted", &deleted, NULL);
		g_assert(deleted != FALSE);

		g_object_get(objects[i], "guid", &guid, NULL);
		g_assert_cmpstr(guid, !=, NULL);
		
		gboolean undeleted;
		undeleted = midgard_object_class_undelete(mgd, guid);
		g_free(guid);
		MIDGARD_TEST_ERROR_OK(mgd);
		/* g_print(" BOOL %d", undeleted); */
		g_assert(undeleted != FALSE);
		g_object_unref(objects[i]);
		i++;
	}

	g_free(objects);
}

void midgard_test_object_class_get_object_by_guid_deleted (MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);
	
	MidgardConnection *mgd = mot->mgd;
	MidgardObject *object = mot->object;

	g_return_if_fail (object != NULL);

	g_test_bug ("#1513");

	gchar *guid;
	g_object_get (G_OBJECT (object), "guid", &guid, NULL);

	MidgardObject *deleted_object = midgard_object_class_get_object_by_guid (mgd, (const gchar *)guid);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OBJECT_DELETED);
	g_assert (deleted_object == NULL);
}
