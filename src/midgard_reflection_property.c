/* 
 * Copyright (C) 2006, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_reflection_property.h"
#include "schema.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"

struct _MidgardReflectionProperty{
	        GObject parent;

		MidgardDBObjectClass *klass;
};

static void 
_midgard_reflection_property_finalize (GObject *object)
{
	g_assert (object != NULL);

	return;
}

/* Initialize class */
static void 
_midgard_reflection_property_class_init (gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);	
	
	gobject_class->set_property = NULL;
	gobject_class->get_property = NULL;
	gobject_class->finalize = _midgard_reflection_property_finalize;
}

/* Register MidgardObjectProperty type */
GType 
midgard_reflection_property_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardReflectionPropertyClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_reflection_property_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardReflectionProperty),
			0,              /* n_preallocs */
			NULL            /* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT,
				"midgard_reflection_property",
				&info, 0);
	}
	return type;
}

/**
 * midgard_reflection_property_new:
 * @klass: #MidgardDBObjectClass pointer
 *
 * Returns: newly initialized #MidgardReflectionProperty instance or %NULL on failure.
 *
 * Deprecated:10.05
 */ 
MidgardReflectionProperty *
midgard_reflection_property_new (MidgardDBObjectClass *klass)
{
	g_assert(klass != NULL);
	MidgardReflectionProperty *self = 
		g_object_new(MIDGARD_TYPE_REFLECTION_PROPERTY, NULL);
	self->klass = klass;
	return self;
}

/**
 * midgard_reflection_property_get_midgard_type:
 * @self: #MidgardReflectionProperty instance
 * @propname: property name which is registered for #MidgardDBObjectClass
 *
 * Returns: type (#GType) of the property or %NULL if property is not registered for given class.
 * Deprecated:10.05
 */ 
GType 
midgard_reflection_property_get_midgard_type (MidgardReflectionProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, 0);
	g_return_val_if_fail (propname != NULL, 0);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->klass), propname);
	if (prop_attr == NULL)
		return MGD_TYPE_NONE;

	return prop_attr->gtype;
}

/**
 * midgard_reflection_property_is_link:
 * @self: #MidgardReflectionProperty instance
 * @propname: property name
 *
 * Checks whether property is a link.
 *
 * Returns: %TRUE if property is registered as link, %FALSE otherwise (or in case if property is not registered for given class.
 * Deprecated:10.05
 */ 
gboolean 
midgard_reflection_property_is_link (MidgardReflectionProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (propname != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->klass), propname);
	if (prop_attr == NULL)
		return FALSE;
	
	return prop_attr->is_link;
}

/**
 * midgard_reflection_property_is_linked:
 * @self: #MidgardReflectionProperty instance
 * @propname: property name
 * 
 * Checks if property is linked with another type. 
 *
 * Returns: %TRUE if property is linked with another type (property of another class is defined as a link to given one).
 * %FALSE is returned if property is not linked or is not registered for given class.
 * Deprecated:10.05
 */
gboolean 
midgard_reflection_property_is_linked (MidgardReflectionProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (propname != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->klass), propname);
	if (prop_attr == NULL)
		return FALSE;

	return prop_attr->is_linked;
}

/**
 * midgard_reflection_property_get_link_name:
 * @self: #MidgardReflectionProperty instance
 * @propname: property name
 *
 * Returns: The name of the class, the given property is a link to. 
 * Or %NULL if property is not a link or given property is not registered for given class.
 * Deprecated:10.05
 */ 
const gchar *
midgard_reflection_property_get_link_name (MidgardReflectionProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (propname != NULL, NULL);	

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->klass), propname);
	if (prop_attr == NULL)
		return NULL;
	
	return prop_attr->link;					
}

/**
 * midgard_reflection_property_get_link_target:
 * @self: #MidgardReflectionProperty instance
 * @propname: property name
 *
 * Returns: The name of the property, the given one is a link to.
 * Or %NULL if property is not a link or it's not registered for given class.
 * Deprecated:10.05
 */ 
const gchar *
midgard_reflection_property_get_link_target (MidgardReflectionProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (propname != NULL, NULL);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->klass), propname);

	if (prop_attr == NULL)
		return NULL;	

	return prop_attr->link_target;	
}

/**
 * midgard_reflection_property_description:
 * @self: #MidgardReflectionProperty instance
 * @propname: property name
 *
 * Returns: description of the given property or %NULL.
 *
 * Deprecated:10.05
 */ 
const gchar *
midgard_reflection_property_description (MidgardReflectionProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (propname != NULL, NULL);
	
	GParamSpec *prop = g_object_class_find_property(
			G_OBJECT_CLASS(self->klass), propname);

	if (prop == NULL)
		return NULL;

	return  g_param_spec_get_blurb(prop);
}

/**
 * midgard_reflection_property_get_link_class:
 * @self: #MidgardReflectionProperty instance
 * @propname: property name
 *
 * Returns the pointer to the #MidgardDBObjectClass, a given property is a link to.
 * @see midgard_reflection_property_get_link_name()
 * Deprecated:10.05
 */ 
const MidgardDBObjectClass *
midgard_reflection_property_get_link_class (MidgardReflectionProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (propname != NULL, NULL);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->klass), propname);
	if (prop_attr == NULL)
		return NULL;
	
	if (prop_attr->is_link) {	

		MidgardDBObjectClass *link_klass = 
			g_type_class_peek(g_type_from_name(prop_attr->link)); 

		return link_klass;
	}
	return NULL;
}

/**
 * midgard_reflection_property_get_user_value:
 * @self: #MidgardReflectionProperty instance
 * @property: property to look value for
 * @name: name of user defined field
 * 
 * Returns: value for user defined field, or NULL if none found
 *
 * Since: 9.09
 * Deprecated:10.05
 */ 
const gchar*
midgard_reflection_property_get_user_value (MidgardReflectionProperty *self, const gchar *property, const gchar *name)
{
	g_assert (self != NULL);
	g_return_val_if_fail (property != NULL, NULL);	
	g_return_val_if_fail (name != NULL, NULL);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr (MIDGARD_DBOBJECT_CLASS (self->klass), property);
	if (prop_attr == NULL)
		return NULL;
	
	return (gchar *) g_hash_table_lookup (prop_attr->user_values, name);
}

/**
 * midgard_reflection_property_is_private:
 * @self: #MidgardReflectionProperty instance
 * @property: property name to check 
 *
 * Returns: %TRUE, if propery is defined private, %FALSE otherwise
 *
 * Deprecated:10.05
 */ 
gboolean
midgard_reflection_property_is_private (MidgardReflectionProperty *self, const gchar *property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);	

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr (MIDGARD_DBOBJECT_CLASS (self->klass), property);
	if (prop_attr == NULL)
		return FALSE;
	
	return prop_attr->is_private;
}
