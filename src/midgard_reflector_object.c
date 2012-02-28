/* 
 * Copyright (C) 2010, 2011 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_reflector_object.h"
#include "schema.h"
#include "midgard_object.h"
#include "midgard_core_object_class.h"
#include "midgard_core_object.h"
#include "midgard_base_abstract.h"
#include "midgard_base_interface.h"
#include "midgard_base_mixin.h"

#define _GET_CLASS_BY_NAME(__name, __retval) 						\
	gpointer *klass = NULL;								\
	gboolean is_iface = FALSE;							\
	GType __class_type = g_type_from_name (__name);					\
	if (g_type_is_a (__class_type, MIDGARD_TYPE_DBOBJECT)				\
			|| g_type_is_a (__class_type, MIDGARD_TYPE_BASE_ABSTRACT)) {	\
		klass = g_type_class_peek (__class_type);				\
	} else if (g_type_is_a (__class_type, MIDGARD_TYPE_BASE_INTERFACE)) {		\
			klass = g_type_default_interface_ref (__class_type);		\
			is_iface = TRUE;						\
	} else {									\
		g_warning ("Invalid '%s' class name ", __name);				\
		return __retval;							\
	}

#define _GET_TYPE_ATTR(__klass) 									\
	MgdSchemaTypeAttr *type_attr = NULL;								\
	if (is_iface) {											\
		gpointer *ifaceptr = g_type_default_interface_ref (__class_type);			\
		type_attr = ((MidgardBaseInterfaceIFace *)ifaceptr)->priv->type_data;			\
	} else {											\
		type_attr = midgard_core_class_get_type_attr(MIDGARD_DBOBJECT_CLASS(__klass)); 		\
	} 												\
	if (type_attr == NULL) {									\
		g_warning ("Failed to find attributes for '%s' type", g_type_name (__class_type));	\
	}
	

/**
 * midgard_reflector_object_get_property_primary:
 * @classname: Name of the class
 *
 * Returns: (transfer none): Name of property which is defined as primary for given class or %NULL.
 * Since: 10.05
 */  
const gchar* 
midgard_reflector_object_get_property_primary (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, NULL);

	_GET_CLASS_BY_NAME (classname, NULL);
	_GET_TYPE_ATTR (klass);

	return type_attr->primary;
}

/**
 * midgard_reflector_object_get_property_up:
 * @classname: Name of the class
 *
 * Returns: (transfer none): Name of property which is defined as 'up' for given class or %NULL.
 * Since: 10.05
 */  
const gchar*
midgard_reflector_object_get_property_up (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, NULL);

	_GET_CLASS_BY_NAME (classname, NULL);
	_GET_TYPE_ATTR (klass);

	return type_attr->property_up;
}

/**
 * midgard_reflector_object_get_property_unique:
 * @classname: Name of the class
 *
 * Returns: (transfer none): Name of property which is defined unique for given class, or %NULL.
 * Since: 10.05
 */  
const gchar*
midgard_reflector_object_get_property_unique (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, NULL);

	_GET_CLASS_BY_NAME (classname, NULL);
	_GET_TYPE_ATTR (klass);

	return type_attr->unique_name;
}

/**
 * midgard_reflector_object_get_property_parent:
 * @classname: Name of the class
 *
 * Returns: (transfer none): Name of property which is defined as 'parent' for given class or %NULL.
 * Since: 10.05
 */  
const gchar*
midgard_reflector_object_get_property_parent (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, NULL);

	_GET_CLASS_BY_NAME (classname, NULL);
	_GET_TYPE_ATTR (klass);

	return type_attr->property_parent;
}

/**
 * midgard_reflector_object_list_children:
 * @classname: Name of the class
 * @n_children: pointer to store number of children classes
 *
 * Returns newly allocated, children ( in midgard tree ) classes' names. 
 * Returned array should be freed if no longer needed without freeing array's elements. 
 *
 * Returns: (array length=n_children) (transfer container): array of strings or %NULL.
 * Since: 10.05
 */  
gchar**
midgard_reflector_object_list_children (const gchar *classname, guint *n_children)
{
	g_return_val_if_fail (classname != NULL, NULL);
	g_return_val_if_fail (n_children != NULL, NULL);

	_GET_CLASS_BY_NAME (classname, NULL);
	_GET_TYPE_ATTR (klass);

	if (!type_attr->children)
		return NULL;

	 GSList *list; 
	 guint i = 0;
	 guint length = g_slist_length (type_attr->children);
	 *n_children = length;

	 /* possible? */
	 if (length == 0)
		 return NULL;

	 gchar **children_class = g_new (gchar*, length + 1);

	 for (list = type_attr->children; list != NULL; list = list->next, i++) 
		 children_class[i] = (gchar *) list->data;

	 *n_children = length;
	 children_class[length] = NULL;
	 
	 return children_class;
}

/**
 * midgard_reflector_object_has_metadata:
 * @classname: Name of the class
 *
 * Checks whether metadata is defined for given @classname.
 *
 * Returns: %TRUE or %FALSE
 * Since: 10.05
 */  
gboolean
midgard_reflector_object_has_metadata_class (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, FALSE);

	_GET_CLASS_BY_NAME (classname, FALSE);
	_GET_TYPE_ATTR (klass);

	if (type_attr->metadata_class_name)
		return TRUE;

	return FALSE;
}

/**
 * midgard_reflector_object_get_metadata_class:
 * @classname: Name of the class
 *
 * Returns: (transfer none): Name of the metadata class of the given one or %NULL.
 * Since: 10.05
 */  
const gchar*
midgard_reflector_object_get_metadata_class (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, NULL);

	_GET_CLASS_BY_NAME (classname, NULL);
	_GET_TYPE_ATTR (klass);

	return (const gchar *)type_attr->metadata_class_name;
}

/**
 * midgard_reflector_object_get_schema_value:
 * @classname: Name of the class
 * @name: node's name declared for given @klass
 *
 *
 * Returns: (transfer none): value of given node's name or %NULL.
 * Since: 10.05
 */  
const gchar*
midgard_reflector_object_get_schema_value (const gchar *classname, const gchar *name)
{
	g_return_val_if_fail (classname != NULL, NULL);

	_GET_CLASS_BY_NAME (classname, NULL);
	_GET_TYPE_ATTR (klass);

	return (const gchar *) g_hash_table_lookup (type_attr->user_values, (gpointer) name);
}

/**
 * midgard_reflector_object_is_mixin:
 * @classname: Name of the class
 *
 * Returns: %TRUE if given type name is a mixin, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_reflector_object_is_mixin (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, FALSE);

	GType class_type  = g_type_from_name (classname);
	if (class_type == G_TYPE_INVALID) 
		return FALSE;

	return g_type_is_a (class_type, MIDGARD_TYPE_BASE_MIXIN);
}

/**
 * midgard_reflector_object_is_interface:
 * @classname: Name of the class
 *
 * Returns: %TRUE if given type name is an interface, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_reflector_object_is_interface (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, FALSE);

	GType class_type  = g_type_from_name (classname);
	if (class_type == G_TYPE_INVALID) 
		return FALSE;

	return G_TYPE_IS_INTERFACE (class_type);
}

/**
 * midgard_reflector_object_is_abstract:
 * @classname: Name of the class
 *
 * Returns: %TRUE if given type name is an abstract one, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_reflector_object_is_abstract (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, FALSE);

	GType class_type  = g_type_from_name (classname);
	if (class_type == G_TYPE_INVALID) 
		return FALSE;

	return G_TYPE_IS_ABSTRACT (class_type);
}

/**
 * midgard_reflector_object_list_defined_properties:
 * @classname: Name of the class
 * @n_prop: a pointer to store number of returned properties
 * 
 * Returns newly allocated, array of properties. 
 * Returned array should be freed if no longer needed without freeing array's elements. 
 *
 * Returns: (transfer container): array of strings or %NULL.
 * 
 * Since: 10.05.5
 */ 
gchar **
midgard_reflector_object_list_defined_properties (const gchar *classname, guint *n_prop)
{
	g_return_val_if_fail (classname != NULL, FALSE);

	GType class_type  = g_type_from_name (classname);
	if (class_type == G_TYPE_INVALID) {
		return NULL;
	}

	GParamSpec **pspecs = NULL;
	if (G_TYPE_IS_INTERFACE (class_type)) {
		pspecs = g_object_interface_list_properties (g_type_default_interface_peek (class_type), n_prop);
	} else if (G_TYPE_IS_CLASSED (class_type)) {
		pspecs = g_object_class_list_properties (g_type_class_peek (class_type), n_prop);
	}

	if (*n_prop == 0)
		return NULL;

	gchar **names = g_new (gchar *, *n_prop + 1);
	guint i;
	for (i = 0; i < *n_prop; i++)
	{
		names[i] = (gchar*) pspecs[i]->name;
	}

	g_free(pspecs);
	names[i] = NULL;
	return names;
}

/* GOBJECT ROUTINES */

GType 
midgard_reflector_object_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardReflectorObjectClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			NULL,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardReflectorObject),
			0,              /* n_preallocs */
			NULL /* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardReflectorObject", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
