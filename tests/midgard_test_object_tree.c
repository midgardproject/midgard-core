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

#include "midgard_test_object_tree.h"
#include "midgard_test_object_basic.h"

void midgard_test_object_tree_basic(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);
	MidgardObject *object = MIDGARD_OBJECT(mot->object);
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS(object);

	/* Get parent (in tree) class and check if parent has the same child declared */
	const gchar *pname = midgard_schema_object_tree_get_parent_name (object);
	
	if (!pname)
		return;

	MidgardObjectClass *pklass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(pname);
	g_assert(pklass != NULL);

	/* There is tree parent declared so parent property can not be NULL */
	const gchar *parent_property = midgard_reflector_object_get_property_parent (G_OBJECT_CLASS_NAME (klass));
	g_assert(parent_property != NULL);

	guint n_child;
	gchar **children = midgard_reflector_object_list_children (G_OBJECT_CLASS_NAME (pklass), &n_child);
	g_assert(children != NULL);

	guint i = 0;
	gboolean has_child_class = FALSE;
	
	for (i = 0; i < n_child; i++) {
		if (g_type_from_name (children[i]) == G_OBJECT_CLASS_TYPE(klass))
			has_child_class = TRUE;
	}

	g_free(children);
	g_assert(has_child_class != FALSE);
}

void midgard_test_object_tree_create(MidgardObjectTest *mot, gconstpointer data)
{
	g_assert(mot != NULL);
	MidgardObject *_object = MIDGARD_OBJECT(mot->object);
	MidgardConnection *mgd = MIDGARD_CONNECTION(midgard_object_get_connection(_object));
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS(_object);

	/* Get parent (in tree) class and check if parent has the same child declared */
	const gchar *pname = midgard_schema_object_tree_get_parent_name(_object);
	
	if(!pname)
		return;

	MidgardObjectClass *pklass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(pname);
	g_assert(pklass != NULL);

	/* Check if class has unique property */
	const gchar *unique_name = midgard_reflector_object_get_property_unique (G_OBJECT_CLASS_NAME (klass));

	if (!unique_name)
		return;

	/* Check if class has 'name' property registered */
	GParamSpec *unique_name_pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), unique_name);
	g_assert (unique_name_pspec != NULL);

	/* Empty name */
	MidgardObject *object = midgard_object_new(mgd, G_OBJECT_TYPE_NAME(_object), NULL);
	g_object_set(object, unique_name, "", NULL);

	/* Workaround */
	const gchar *parent_property = midgard_reflector_object_get_property_parent (G_OBJECT_CLASS_NAME (klass));
	
	if (parent_property)
		g_object_set(object, parent_property, 1, NULL);

	gboolean created = midgard_object_create(object);
	MIDGARD_TEST_ERROR_OK(mgd);
	g_assert(created != FALSE);

	gboolean purged = midgard_object_purge(object, TRUE);
	g_assert(purged != FALSE);
	g_object_unref(object);

	object = midgard_object_new(mgd, G_OBJECT_TYPE_NAME(_object), NULL);
	g_object_set(object, "name", "", NULL);

	/* Workaround */
	parent_property = midgard_reflector_object_get_property_parent (G_OBJECT_CLASS_NAME (klass));
	
	if (parent_property)
		g_object_set(object, parent_property, 1, NULL);

	created = midgard_object_create(object);
	MIDGARD_TEST_ERROR_OK(mgd);
	g_assert(created != FALSE);

	purged = midgard_object_purge(object, TRUE);
	g_assert(purged != FALSE);
	g_object_unref(object);

	/* Unique name */
	object = midgard_object_new(mgd, G_OBJECT_TYPE_NAME(_object), NULL);
	g_object_set(object, unique_name, "Unique", NULL);

	/* Workaround */
	parent_property = midgard_reflector_object_get_property_parent (G_OBJECT_CLASS_NAME (klass));
	
	if (parent_property)
		g_object_set(object, parent_property, 1, NULL);

	created = midgard_object_create(object);
	MIDGARD_TEST_ERROR_OK(mgd);
	g_assert(created != FALSE);

	MidgardObject *dupobject = midgard_object_new(mgd, G_OBJECT_TYPE_NAME(_object), NULL);
	g_object_set(dupobject, unique_name, "Unique", NULL);
	
	/* Workaround */
	parent_property = midgard_reflector_object_get_property_parent (G_OBJECT_CLASS_NAME (klass));
	
	if (parent_property)
		g_object_set(dupobject, parent_property, 1, NULL);

	gboolean dupcreated = midgard_object_create(dupobject);	
	MIDGARD_TEST_ERROR_ASSERT(mgd, MGD_ERR_DUPLICATE);
	g_assert(dupcreated != TRUE);	
	g_object_unref(dupobject);

	purged = midgard_object_purge(object, TRUE);
	g_assert(purged != FALSE);
	g_object_unref(object);
}

void midgard_test_object_tree_get_parent(MidgardObjectTest *mot, gconstpointer data)
{
	/* FIXME, we can not invalidate only tree "identifiers" */
	return;
	g_assert(mot != NULL);
	MidgardObject *_object = MIDGARD_OBJECT(mot->object);
	//MidgardConnection *mgd = MIDGARD_CONNECTION(midgard_object_get_connection(_object));
	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS(_object);
	
	g_test_bug ("#1594");
	/* This bug has been caused by parent (in tree) class name not being set to any default value.
	 * We just validate tree logic here. Real example with object fetching should be covered by 
	 * particular, tree optimized tests. Which should be done in fresh&clean database. */

	const gchar *up_property = midgard_reflector_object_get_property_up (G_OBJECT_CLASS_NAME (klass));
	const gchar *parent_property = midgard_reflector_object_get_property_parent (G_OBJECT_CLASS_NAME (klass));
	const gchar *parent_name = midgard_schema_object_tree_get_parent_name (_object);

	/* Check if there's parent defined */
	if (!up_property && !parent_property) {
		
		g_assert_cmpstr (parent_name, ==, NULL);
		return;
	}
	
	if (up_property || parent_property)
		g_assert_cmpstr (parent_name, !=, NULL);

	return;
}
