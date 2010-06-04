/* 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_reflector_property.h"
#include "schema.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"

struct _MidgardReflectorPropertyPrivate {
	MidgardDBObjectClass *klass;
	const gchar *classname;
};

/**
 * midgard_reflector_property_new:
 * @classname: (in): Name of #MidgardDBObject (or derived) class
 *
 * Returns: (transfer full): newly initialized #MidgardReflectorProperty instance or %NULL on failure.
 *
 * Since: 10.05
 */ 
MidgardReflectorProperty *
midgard_reflector_property_new (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, NULL);

	MidgardDBObjectClass *dbklass = g_type_class_peek (g_type_from_name (classname));

	if (!dbklass)
		return NULL;

	g_return_val_if_fail (MIDGARD_IS_DBOBJECT_CLASS (dbklass), NULL);

	MidgardReflectorProperty *self = g_object_new (MIDGARD_TYPE_REFLECTOR_PROPERTY, "dbclass", classname, NULL);

	return self;
}

/**
 * midgard_reflector_property_get_midgard_type:
 * @self: #MidgardReflectorProperty instance
 * @propname: property name which is registered for #MidgardDBObjectClass
 *
 * Returns: type (#GType) of the property or %NULL if property is not registered for given class.
 *
 * Since: 10.05
 */ 
GType 
midgard_reflector_property_get_midgard_type (MidgardReflectorProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, 0);
	g_return_val_if_fail (propname != NULL, 0);
	g_return_val_if_fail (self->priv->klass != NULL, 0);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->priv->klass), propname);
	if (prop_attr == NULL)
		return MGD_TYPE_NONE;

	return prop_attr->gtype;
}

/**
 * midgard_reflector_property_is_link:
 * @self: #MidgardReflectorProperty instance
 * @propname: property name
 *
 * Checks whether property is a link.
 *
 * Returns: %TRUE if property is registered as link, %FALSE otherwise (or in case if property is not registered for given class.
 *
 * Since: 10.05
 */ 
gboolean 
midgard_reflector_property_is_link (MidgardReflectorProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (propname != NULL, FALSE);
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->priv->klass), propname);
	if (prop_attr == NULL)
		return FALSE;
	
	return prop_attr->is_link;
}

/**
 * midgard_reflector_property_is_linked:
 * @self: #MidgardReflectorProperty instance
 * @propname: property name
 * 
 * Checks if property is linked with another type. 
 *
 * Returns: %TRUE if property is linked with another type (property of another class is defined as a link to given one).
 * %FALSE is returned if property is not linked or is not registered for given class.
 *
 * Since: 10.05
 */
gboolean 
midgard_reflector_property_is_linked (MidgardReflectorProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (propname != NULL, FALSE);
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->priv->klass), propname);
	if (prop_attr == NULL)
		return FALSE;

	return prop_attr->is_linked;
}

/**
 * midgard_reflector_property_get_link_name:
 * @self: #MidgardReflectorProperty instance
 * @propname: property name
 *
 * Returns: (transfer none): The name of the class, the given property is a link to. 
 * Or %NULL if property is not a link or given property is not registered for given class.
 *
 * Since: 10.05
 */ 
const gchar *
midgard_reflector_property_get_link_name (MidgardReflectorProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (propname != NULL, NULL);	
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->priv->klass), propname);
	if (prop_attr == NULL)
		return NULL;
	
	return prop_attr->link;					
}

/**
 * midgard_reflector_property_get_link_target:
 * @self: #MidgardReflectorProperty instance
 * @propname: property name
 *
 * Returns: (transfer none): The name of the property, the given one is a link to.
 * Or %NULL if property is not a link or it's not registered for given class.
 *
 * Since: 10.05
 */ 
const gchar *
midgard_reflector_property_get_link_target (MidgardReflectorProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (propname != NULL, NULL);
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->priv->klass), propname);

	if (prop_attr == NULL)
		return NULL;	

	return prop_attr->link_target;	
}

/**
 * midgard_reflector_property_description:
 * @self: #MidgardReflectorProperty instance
 * @propname: property name
 *
 * Returns: (transfer none): description of the given property or %NULL.
 *
 * Since: 10.05
 */ 
const gchar *
midgard_reflector_property_description (MidgardReflectorProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (propname != NULL, NULL);
	g_return_val_if_fail (self->priv->klass != NULL, NULL);

	GParamSpec *prop = g_object_class_find_property(
			G_OBJECT_CLASS(self->priv->klass), propname);

	if (prop == NULL)
		return NULL;

	return  g_param_spec_get_blurb(prop);
}

/**
 * midgard_reflector_property_get_link_class: (skip)
 * @self: #MidgardReflectorProperty instance
 * @propname: property name
 *
 * Returns: the pointer to the #MidgardDBObjectClass, a given property is a link to.
 * @see midgard_reflector_property_get_link_name()
 *
 * Since: 10.05
 */ 
const MidgardDBObjectClass *
midgard_reflector_property_get_link_class (MidgardReflectorProperty *self, const gchar *propname)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (propname != NULL, NULL);
	g_return_val_if_fail (self->priv->klass != NULL, NULL);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(self->priv->klass), propname);
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
 * midgard_reflector_property_get_user_value:
 * @self: #MidgardReflectorProperty instance
 * @property: property to look value for
 * @name: name of user defined field
 * 
 * Returns: value for user defined field, or NULL if none found
 *
 * Since: 10.05
 */ 
const gchar*
midgard_reflector_property_get_user_value (MidgardReflectorProperty *self, const gchar *property, const gchar *name)
{
	g_assert (self != NULL);
	g_return_val_if_fail (property != NULL, NULL);	
	g_return_val_if_fail (name != NULL, NULL);
	g_return_val_if_fail (self->priv->klass != NULL, NULL);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr (MIDGARD_DBOBJECT_CLASS (self->priv->klass), property);
	if (prop_attr == NULL)
		return NULL;
	
	return (gchar *) g_hash_table_lookup (prop_attr->user_values, name);
}

/**
 * midgard_reflector_property_is_private:
 * @self: #MidgardReflectorProperty instance
 * @property: property name to check 
 *
 * Returns: %TRUE, if propery is defined private, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_reflector_property_is_private (MidgardReflectorProperty *self, const gchar *property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);	
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr (MIDGARD_DBOBJECT_CLASS (self->priv->klass), property);
	if (prop_attr == NULL)
		return FALSE;
	
	return prop_attr->is_private;
}

/**
 * midgard_reflector_property_is_unique:
 * @self: #MidgardReflectorProperty instance
 * @property: property name to check
 *
 * Returns: %TRUE, if property is defined as unique, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_reflector_property_is_unique (MidgardReflectorProperty *self, const gchar *property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);	
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr (MIDGARD_DBOBJECT_CLASS (self->priv->klass), property);
	if (prop_attr == NULL)
		return FALSE;
	
	return prop_attr->is_unique;
}

/**
 * midgard_reflector_property_is_primary:
 * @self: #MidgardReflectorProperty instance
 * @property: property name to check
 *
 * Returns: %TRUE, if property is primary, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_reflector_property_is_primary (MidgardReflectorProperty *self, const gchar *property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);	
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr (MIDGARD_DBOBJECT_CLASS (self->priv->klass), property);
	if (prop_attr == NULL)
		return FALSE;
	
	return prop_attr->is_primary;
}

/**
 * midgard_reflector_property_has_default_value:
 * @self: #MidgardReflectorProperty instance
 * @property: property name to check
 *
 * Returns: %TRUE, if property has default value, %FALSE otherwise
 *
 * Since: 10.05
 */ 
gboolean
midgard_reflector_property_has_default_value (MidgardReflectorProperty *self, const gchar *property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);	
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr (MIDGARD_DBOBJECT_CLASS (self->priv->klass), property);
	if (prop_attr == NULL)
		return FALSE;
	
	return prop_attr->default_value ? TRUE : FALSE;
}

/**
 * midgard_reflector_property_get_default_value:
 * @self: #MidgardReflectorProperty instance
 * @property: property name to check
 * @value: (inout): value which stores default value defined for given property
 *
 * @value should not be initialized for particular type. It's a copy of original value, 
 * so it should be unset when no longer needed.
 *
 * Returns: %TRUE, if property has default value and its copy has been made, %FALSE otherwise
 *
 * Since: 10.05
 */
gboolean
midgard_reflector_property_get_default_value (MidgardReflectorProperty *self, const gchar *property, GValue *value)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);	
	g_return_val_if_fail (self->priv->klass != NULL, FALSE);

	MgdSchemaPropertyAttr *prop_attr = 
		midgard_core_class_get_property_attr (MIDGARD_DBOBJECT_CLASS (self->priv->klass), property);
	if (prop_attr == NULL)
		return FALSE;
	
	if (!prop_attr->default_value)
		return FALSE;

	g_value_init (value, G_VALUE_TYPE (prop_attr->default_value));
	g_value_copy (prop_attr->default_value, value);

	return TRUE;
}

/* GOBJECT ROUTINES */

enum {
	MIDGARD_REFLECTOR_PROPERTY_DBCLASS = 1
};

static GObjectClass *__parent_class = NULL;

static void 
__midgard_reflector_property_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardReflectorProperty *self = (MidgardReflectorProperty *) instance;
	self->priv = g_new (MidgardReflectorPropertyPrivate, 1);
	self->priv->klass = NULL;
	self->priv->classname = NULL;

	return;
}

static GObject *
__midgard_reflector_property_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (__parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return G_OBJECT(object);
}

static void
__midgard_reflector_property_dispose (GObject *object)
{
	__parent_class->dispose (object);
}

static void
__midgard_reflector_property_finalize (GObject *object)
{
	MidgardReflectorProperty *self = (MidgardReflectorProperty *) object;
	g_free (self->priv);
	self->priv = NULL;

	__parent_class->finalize (object);
}

static void
__midgard_reflector_property_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardReflectorProperty *self = (MidgardReflectorProperty *) object;

	switch (property_id) {
		
		case MIDGARD_REFLECTOR_PROPERTY_DBCLASS:
			g_value_set_string (value, self->priv->classname);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
__midgard_reflector_property_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardReflectorProperty *self = (MidgardReflectorProperty *) (object);
	MidgardDBObjectClass *dbklass = NULL;

	switch (property_id) {

		case MIDGARD_REFLECTOR_PROPERTY_DBCLASS:
			dbklass = g_type_class_peek (g_type_from_name (g_value_get_string (value)));	
			if (dbklass) {
				self->priv->klass = dbklass;
				self->priv->classname = G_OBJECT_CLASS_NAME (G_OBJECT_CLASS (dbklass));
			}
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}
			
static void
__midgard_reflector_property_class_init (MidgardDBObjectClass *klass, gpointer g_class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	__parent_class = g_type_class_peek_parent (klass);

	//object_class->constructor = __midgard_reflector_property_constructor;
	object_class->dispose = __midgard_reflector_property_dispose;
	object_class->finalize = __midgard_reflector_property_finalize;

	object_class->set_property = __midgard_reflector_property_set_property;
	object_class->get_property = __midgard_reflector_property_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_string ("dbclass",
			"MidgardDBObject derived class name.",
			"",
			"",
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardReflectorProperty:dbclass:
	 * 
	 * Holds the name of the class which, #MidgardReflectorProperty has been initialized for.
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_REFLECTOR_PROPERTY_DBCLASS, pspec);
}

GType
midgard_reflector_property_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardReflectorPropertyClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) __midgard_reflector_property_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardReflectorProperty),
			0,              /* n_preallocs */
			(GInstanceInitFunc) __midgard_reflector_property_instance_init
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardReflectorProperty", &info, 0);
	}
	return type;
}
