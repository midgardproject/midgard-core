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

#include "midgard_query_holder.h"
#include "midgard_query_property.h"
#include "midgard_dbobject.h"
#include "midgard_query_storage.h"
#include "midgard_core_query.h"

/**
 * midgard_query_property_new:
 * @property: name of the property
 * @storage: optional storage for given property
 *
 * Returns: new #MidgardQueryProperty instance or NULL on failure
 * Since: 10.05
 */ 
MidgardQueryProperty *
midgard_query_property_new (const gchar *property, MidgardQueryStorage *storage)
{
	g_return_val_if_fail (property != NULL, NULL);
	MidgardQueryProperty *self = g_object_new (MIDGARD_TYPE_QUERY_PROPERTY, "property", property, NULL);

	if (storage != NULL)
		self->priv->storage = storage;

	return self;
}

static void
__get_value (MidgardQueryHolder *self, GValue *value)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (self != NULL);

	MidgardQueryProperty *mqp = (MidgardQueryProperty *) self;

	if (!G_VALUE_HOLDS_STRING (value))
		g_value_init (value, G_VALUE_TYPE (&mqp->priv->value));
	g_value_copy ((const GValue *) &mqp->priv->value, value);

	return;
}

static gboolean
__set_value (MidgardQueryHolder *self, const GValue *value)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);
	g_return_val_if_fail (G_VALUE_HOLDS_STRING (value), FALSE);

	MidgardQueryProperty *mqp = (MidgardQueryProperty *) self;

	if (G_IS_VALUE (&mqp->priv->value))
		g_value_unset (&mqp->priv->value);

	g_value_init (&mqp->priv->value, G_TYPE_STRING);
	g_value_copy (value, &mqp->priv->value);

	return TRUE;
}

/* GOBJECT ROUTINES */

enum {
	MIDGARD_QUERY_PROPERTY_PROPNAME = 1,
	MIDGARD_QUERY_PROPERTY_STORAGE
};

static GObjectClass *parent_class= NULL;

static GObject *
_midgard_query_property_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties, construct_properties);

	MidgardQueryProperty *self = (MidgardQueryProperty *) object;
	self->priv = g_new (MidgardQueryPropertyPrivate, 1);	
	self->priv->storage = NULL;
	self->priv->klass = NULL;
	GValue value = {0, };
	self->priv->value = value;
	g_value_init (&self->priv->value, G_TYPE_STRING);

	return G_OBJECT(object);
}

static void
_midgard_query_property_dispose (GObject *object)
{	
	parent_class->dispose (object);
}

static void
_midgard_query_property_finalize (GObject *object)
{
	MidgardQueryProperty *self = MIDGARD_QUERY_PROPERTY (object);

	if (self->priv && G_IS_VALUE (&self->priv->value))
		g_value_unset (&self->priv->value);

	g_free (self->priv);
	self->priv = NULL;

        parent_class->finalize (object);
}

static void
__midgard_query_property_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardQueryProperty *self = (MidgardQueryProperty *) object;

	switch (property_id) {
		
		case MIDGARD_QUERY_PROPERTY_PROPNAME:
			__get_value (MIDGARD_QUERY_HOLDER (object), value);
			break;

		case MIDGARD_QUERY_PROPERTY_STORAGE:
			g_value_set_object (value, self->priv->storage);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
__midgard_query_property_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardQueryProperty *self = (MidgardQueryProperty *) (object);

	switch (property_id) {

		case MIDGARD_QUERY_PROPERTY_PROPNAME:
			__set_value (MIDGARD_QUERY_HOLDER (object), (GValue *)value);
			break;

		case MIDGARD_QUERY_PROPERTY_STORAGE:
			self->priv->storage = g_value_get_object (value);
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}


static void
_midgard_query_property_class_init (MidgardQueryPropertyClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_query_property_constructor;
	object_class->dispose = _midgard_query_property_dispose;
	object_class->finalize = _midgard_query_property_finalize;

	object_class->set_property = __midgard_query_property_set_property;
	object_class->get_property = __midgard_query_property_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_string ("property",
			"",
			"",
			"",
			G_PARAM_READWRITE);
	/**
	 * MidgardQueryProperty:property:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_QUERY_PROPERTY_PROPNAME, pspec);

	pspec = g_param_spec_object ("storage",
			"",
			"",
			MIDGARD_TYPE_QUERY_STORAGE,
			G_PARAM_READWRITE);
	/**
	 * MidgardQueryProperty:property:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_QUERY_PROPERTY_STORAGE, pspec);
}

static void
midgard_query_property_iface_init (MidgardQueryHolderIFace *iface)
{
       iface->get_value = __get_value;
       iface->set_value = __set_value;
}

GType
midgard_query_property_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryPropertyClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			(GClassInitFunc) _midgard_query_property_class_init,
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardQueryProperty),
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
      
		static const GInterfaceInfo property_info = {
			(GInterfaceInitFunc) midgard_query_property_iface_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryProperty", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_HOLDER, &property_info);
    	}
    	return type;
}

