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

#include "midgard_cr_core_query_holder.h"
#include "midgard_cr_core_query_property.h"
#include "midgard_cr_core_query_storage.h"
#include "midgard_cr_core_query_private.h"

/**
 * midgard_cr_core_query_property_new:
 * @property: name of the property
 * @storage: (allow-none): optional storage for given property
 *
 * Returns: new #MidgardCRCoreQueryProperty instance or NULL on failure
 * Since: 10.05
 */ 
MidgardCRCoreQueryProperty *
midgard_cr_core_query_property_new (const gchar *property, MidgardCRCoreQueryStorage *storage)
{
	g_return_val_if_fail (property != NULL, NULL);
	MidgardCRCoreQueryProperty *self = g_object_new (MIDGARD_CR_CORE_TYPE_QUERY_PROPERTY, "property", property, NULL);

	if (storage != NULL)
		self->priv->storage = g_object_ref (storage);

	return self;
}

static void
__get_value (MidgardCRCoreQueryHolder *self, GValue *value)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (self != NULL);

	MidgardCRCoreQueryProperty *mqp = (MidgardCRCoreQueryProperty *) self;

	if (!G_VALUE_HOLDS_STRING (value))
		g_value_init (value, G_VALUE_TYPE (&mqp->priv->value));
	g_value_copy ((const GValue *) &mqp->priv->value, value);

	return;
}

static gboolean
__set_value (MidgardCRCoreQueryHolder *self, const GValue *value)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);
	g_return_val_if_fail (G_VALUE_HOLDS_STRING (value), FALSE);

	MidgardCRCoreQueryProperty *mqp = (MidgardCRCoreQueryProperty *) self;

	if (G_IS_VALUE (&mqp->priv->value))
		g_value_unset (&mqp->priv->value);

	g_value_init (&mqp->priv->value, G_TYPE_STRING);
	g_value_copy (value, &mqp->priv->value);

	return TRUE;
}

/* GOBJECT ROUTINES */

enum {
	MIDGARD_CR_CORE_QUERY_PROPERTY_PROPNAME = 1,
	MIDGARD_CR_CORE_QUERY_PROPERTY_STORAGE
};

static GObjectClass *parent_class= NULL;

static void
_midgard_cr_core_query_property_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardCRCoreQueryProperty *self = (MidgardCRCoreQueryProperty *)instance;

	self->priv = g_new (MidgardCRCoreQueryPropertyPrivate, 1);	
	self->priv->storage = NULL;
	self->priv->klass = NULL;
	GValue value = {0, };
	self->priv->value = value;
	g_value_init (&self->priv->value, G_TYPE_STRING);
}

static GObject *
_midgard_cr_core_query_property_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties, construct_properties);

	return G_OBJECT(object);
}

static void
_midgard_cr_core_query_property_dispose (GObject *object)
{	
	MidgardCRCoreQueryProperty *self = (MidgardCRCoreQueryProperty *) object;

	if (self->priv->storage) {
		g_object_unref (self->priv->storage);
		self->priv->storage = NULL;
	}

	parent_class->dispose (object);
}

static void
_midgard_cr_core_query_property_finalize (GObject *object)
{
	MidgardCRCoreQueryProperty *self = MIDGARD_CR_CORE_QUERY_PROPERTY (object);

	if (self->priv && G_IS_VALUE (&self->priv->value))
		g_value_unset (&self->priv->value);

	g_free (self->priv);
	self->priv = NULL;

        parent_class->finalize (object);
}

static void
__midgard_cr_core_query_property_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardCRCoreQueryProperty *self = (MidgardCRCoreQueryProperty *) object;

	switch (property_id) {
		
		case MIDGARD_CR_CORE_QUERY_PROPERTY_PROPNAME:
			__get_value (MIDGARD_CR_CORE_QUERY_HOLDER (object), value);
			break;

		case MIDGARD_CR_CORE_QUERY_PROPERTY_STORAGE:
			g_value_set_object (value, self->priv->storage);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
__midgard_cr_core_query_property_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardCRCoreQueryProperty *self = (MidgardCRCoreQueryProperty *) (object);

	switch (property_id) {

		case MIDGARD_CR_CORE_QUERY_PROPERTY_PROPNAME:
			__set_value (MIDGARD_CR_CORE_QUERY_HOLDER (object), (GValue *)value);
			break;

		case MIDGARD_CR_CORE_QUERY_PROPERTY_STORAGE:
			if (self->priv->storage)
				g_object_unref (self->priv->storage);
			self->priv->storage = g_value_dup_object (value);
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}


static void
_midgard_cr_core_query_property_class_init (MidgardCRCoreQueryPropertyClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_cr_core_query_property_constructor;
	object_class->dispose = _midgard_cr_core_query_property_dispose;
	object_class->finalize = _midgard_cr_core_query_property_finalize;

	object_class->set_property = __midgard_cr_core_query_property_set_property;
	object_class->get_property = __midgard_cr_core_query_property_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_string ("property",
			"",
			"",
			"",
			G_PARAM_READWRITE);
	/**
	 * MidgardCRCoreQueryProperty:property:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_CR_CORE_QUERY_PROPERTY_PROPNAME, pspec);

	pspec = g_param_spec_object ("storage",
			"",
			"",
			MIDGARD_CR_CORE_TYPE_QUERY_STORAGE,
			G_PARAM_READWRITE);
	/**
	 * MidgardCRCoreQueryProperty:property:
	 * 
	 */  
	g_object_class_install_property (object_class, MIDGARD_CR_CORE_QUERY_PROPERTY_STORAGE, pspec);
}

static void
midgard_cr_core_query_property_iface_init (MidgardCRCoreQueryHolderIFace *iface)
{
       iface->get_value = __get_value;
       iface->set_value = __set_value;
}

GType
midgard_cr_core_query_property_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardCRCoreQueryPropertyClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			(GClassInitFunc) _midgard_cr_core_query_property_class_init,
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardCRCoreQueryProperty),
			0,      /* n_preallocs */
			_midgard_cr_core_query_property_instance_init    /* instance_init */
		};
      
		static const GInterfaceInfo property_info = {
			(GInterfaceInitFunc) midgard_cr_core_query_property_iface_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardCRCoreQueryProperty", &info, 0);
		g_type_add_interface_static (type, MIDGARD_CR_CORE_TYPE_QUERY_HOLDER, &property_info);
    	}
    	return type;
}

