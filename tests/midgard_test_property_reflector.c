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

#include "midgard_test_property_reflector.h"

MidgardReflectionProperty *midgard_test_property_reflector_create(MidgardObject *object)
{
	g_assert(object != NULL);

	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS(object);

	g_assert(klass != NULL);

	/* Do not check if it's NULL here.
	 * We need to check it for every funtion scope for better error reporting. */
	return midgard_reflection_property_new(MIDGARD_DBOBJECT_CLASS(klass));
}

#define MIDGARD_TEST_MRP_NEW(__object) \
	MidgardReflectionProperty *mrp = midgard_test_property_reflector_create(__object); \
	g_assert(mrp != NULL);

void midgard_test_property_reflector_new(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	MIDGARD_TEST_MRP_NEW(object);

	g_object_unref(mrp);
}

void midgard_test_property_reflector_check_midgard_type(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	MIDGARD_TEST_MRP_NEW(object);

	guint n_prop = 0;
	guint i = 0;
	GType mpt;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_prop);

	g_assert(pspecs != NULL);
	g_assert_cmpint(n_prop, >,  0);	

	for (i = 0; i < n_prop; i++) {

		mpt = midgard_reflection_property_get_midgard_type(mrp, pspecs[i]->name);
		g_assert_cmpint(mpt, >, 0);

		if (mpt == MGD_TYPE_STRING)
			g_assert_cmpint(pspecs[i]->value_type, ==, G_TYPE_STRING);
		
		if (mpt == MGD_TYPE_INT)
			g_assert_cmpint(pspecs[i]->value_type, ==, G_TYPE_INT);
		
		if (mpt == MGD_TYPE_UINT)
			g_assert_cmpint(pspecs[i]->value_type, ==, G_TYPE_UINT);
		
		if (mpt == MGD_TYPE_FLOAT)
			g_assert_cmpint(pspecs[i]->value_type, ==, G_TYPE_FLOAT);
			
		if (mpt == MGD_TYPE_BOOLEAN)
			g_assert_cmpint(pspecs[i]->value_type, ==, G_TYPE_BOOLEAN);
		
		if (mpt == MGD_TYPE_GUID)
			g_assert_cmpuint(pspecs[i]->value_type, ==, G_TYPE_STRING);

		if (mpt == MGD_TYPE_TIMESTAMP) {
			g_assert_cmpuint(pspecs[i]->value_type, ==, MGD_TYPE_TIMESTAMP);	
		}
	}

	g_free(pspecs);

	g_object_unref(mrp);
}

void midgard_test_property_reflector_check_is_link(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	MIDGARD_TEST_MRP_NEW(object);
	
	guint n_prop = 0;
	guint i = 0;
	gboolean islink = FALSE;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_prop);

	g_assert(pspecs != NULL);
	g_assert_cmpint(n_prop, >,  0);	

	for (i = 0; i < n_prop; i++) {

		islink = midgard_reflection_property_is_link(mrp, pspecs[i]->name);
		
		if (!islink)
			continue;

		const MidgardDBObjectClass *klass = 
			midgard_reflection_property_get_link_class (mrp, pspecs[i]->name);
		g_assert(klass != NULL);

		const gchar *cname = 
			midgard_reflection_property_get_link_name (mrp, pspecs[i]->name);
		g_assert(cname != NULL);
		g_assert_cmpstr(cname, != , "");

		const gchar *target = 
			midgard_reflection_property_get_link_target (mrp, pspecs[i]->name);
		g_assert(target != NULL);

		/* We checked all link related data. Now let's check if target property is linked */
		MidgardReflectionProperty *tmrp = midgard_reflection_property_new (MIDGARD_DBOBJECT_CLASS (klass));
		g_assert(tmrp != NULL);
	
		gboolean target_islinked = midgard_reflection_property_is_linked (tmrp, target);
		g_assert (target_islinked != FALSE);

		g_object_unref (tmrp);
	}

	g_free(pspecs);
	g_object_unref(mrp);
}

void midgard_test_property_reflector_check_is_linked(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	MIDGARD_TEST_MRP_NEW(object);

	guint n_prop = 0;
	guint i = 0;
	gboolean islinked = FALSE;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_prop);

	g_assert(pspecs != NULL);
	g_assert_cmpint(n_prop, >,  0);	

	for (i = 0; i < n_prop; i++) {

		islinked = midgard_reflection_property_is_linked(mrp, pspecs[i]->name);

		if (!islinked)
			continue;
	}

	g_free(pspecs);
	g_object_unref(mrp);
}

void midgard_test_property_reflector_check_link_class(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	MIDGARD_TEST_MRP_NEW(object);
	
	guint n_prop = 0;
	guint i = 0;
	gboolean islink = FALSE;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_prop);

	g_assert(pspecs != NULL);
	g_assert_cmpint(n_prop, >,  0);	

	for (i = 0; i < n_prop; i++) {

		const MidgardDBObjectClass *klass = 
			midgard_reflection_property_get_link_class(mrp, pspecs[i]->name);

		if (klass == NULL)
			continue;

		islink = midgard_reflection_property_is_link(mrp, pspecs[i]->name);
		g_assert(islink != FALSE);

		const gchar *cname = 
			midgard_reflection_property_get_link_name(mrp, pspecs[i]->name);
		g_assert(cname != NULL);
		g_assert_cmpstr(cname, != , "");

		const gchar *target = 
			midgard_reflection_property_get_link_target(mrp, pspecs[i]->name);
		g_assert(target != NULL);

		/* We checked all link related data. Now let's check if target property is linked */
		MidgardReflectionProperty *tmrp = midgard_reflection_property_new (MIDGARD_DBOBJECT_CLASS (klass));
		g_assert(tmrp != NULL);

		gboolean target_islinked = midgard_reflection_property_is_linked(tmrp, target);
		g_assert(target_islinked != FALSE);

		g_object_unref(tmrp);
	}

	g_free(pspecs);

	g_object_unref(mrp);
}

void midgard_test_property_reflector_check_link_name(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	MIDGARD_TEST_MRP_NEW(object);
	
	guint n_prop = 0;
	guint i = 0;
	gboolean islink = FALSE;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_prop);

	g_assert(pspecs != NULL);
	g_assert_cmpint(n_prop, >,  0);	

	for (i = 0; i < n_prop; i++) {

		const gchar *name = 
			midgard_reflection_property_get_link_name(mrp, pspecs[i]->name);

		if (name == NULL)
			continue;

		islink = midgard_reflection_property_is_link(mrp, pspecs[i]->name);
		g_assert(islink != FALSE);

		const MidgardDBObjectClass *klass = 
			midgard_reflection_property_get_link_class(mrp, pspecs[i]->name);
		g_assert(klass != NULL);
		
		const gchar *target = 
			midgard_reflection_property_get_link_target(mrp, pspecs[i]->name);
		g_assert(target != NULL);

		/* We checked all link related data. Now let's check if target property is linked */
		MidgardReflectionProperty *tmrp = midgard_reflection_property_new (MIDGARD_DBOBJECT_CLASS (klass));
		g_assert(tmrp != NULL);

		gboolean target_islinked = midgard_reflection_property_is_linked(tmrp, target);
		g_assert(target_islinked != FALSE);

		g_object_unref(tmrp);
	}

	g_free(pspecs);

	g_object_unref(mrp);
}

void midgard_test_property_reflector_check_link_target(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	MIDGARD_TEST_MRP_NEW(object);
	
	guint n_prop = 0;
	guint i = 0;
	gboolean islink = FALSE;
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_prop);

	g_assert(pspecs != NULL);
	g_assert_cmpint(n_prop, >,  0);	

	for (i = 0; i < n_prop; i++) {

		const gchar *target = 
			midgard_reflection_property_get_link_target(mrp, pspecs[i]->name);

		if (target == NULL)
			continue;

		islink = midgard_reflection_property_is_link(mrp, pspecs[i]->name);
		g_assert(islink != FALSE);

		const gchar *cname = 
			midgard_reflection_property_get_link_name(mrp, pspecs[i]->name);
		g_assert(cname != NULL);
		g_assert_cmpstr(cname, != , "");

		const MidgardDBObjectClass *klass =
			midgard_reflection_property_get_link_class(mrp, pspecs[i]->name);
		g_assert(klass != NULL);
	
		/* We checked all link related data. Now let's check if target property is linked */
		MidgardReflectionProperty *tmrp = midgard_reflection_property_new (MIDGARD_DBOBJECT_CLASS (klass));
		g_assert(tmrp != NULL);

		gboolean target_islinked = midgard_reflection_property_is_linked(tmrp, target);
		g_assert(target_islinked != FALSE);

		g_object_unref(tmrp);
	}

	g_free(pspecs);

	g_object_unref(mrp);
}

void midgard_test_property_reflector_check_description(MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT(mot);
	MIDGARD_TEST_MRP_NEW(object);
	
	guint n_prop = 0;
	guint i = 0;	
	GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_GET_CLASS(object), &n_prop);

	g_assert(pspecs != NULL);
	g_assert_cmpint(n_prop, >,  0);	

	for (i = 0; i < n_prop; i++) {

		const gchar *descr = 
			midgard_reflection_property_description(mrp, pspecs[i]->name);

		if (descr == NULL)
			continue;

		if (*descr == '\0')
			continue;
	}

	g_free(pspecs);

	g_object_unref(mrp);
}
