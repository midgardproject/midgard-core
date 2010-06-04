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
#include "midgard_query_value.h"
#include "midgard_dbobject.h"

struct _MidgardQueryValuePrivate {
	GObject parent;
	GValue value;
};

/**
 * midgard_query_value_new:
 * @value: a #GValue value
 *
 * Returns: new #MidgardQueryValue or %NULL on failure
 * Since: 10.05
 */ 
MidgardQueryValue *
midgard_query_value_new (const GValue *value)
{
	g_return_val_if_fail (value != NULL, NULL);
	MidgardQueryValue *self = g_object_new (MIDGARD_TYPE_QUERY_VALUE, NULL);

	GValue pval = {0, };
	g_value_init (&pval, G_VALUE_TYPE (value));
	g_value_copy (value, &pval);

	self->priv = g_new (MidgardQueryValuePrivate, 1);
	self->priv->value = pval;

	return self;
}

static void
__get_value (MidgardQueryHolder *self, GValue *value)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (self != NULL);

	MidgardQueryValue *mqp = (MidgardQueryValue *) self;

	g_value_init (value, G_VALUE_TYPE (&mqp->priv->value));
	g_value_copy ((const GValue *) &mqp->priv->value, value);

	return;
}

static gboolean
__set_value (MidgardQueryHolder *self, const GValue *value)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	MidgardQueryValue *mqp = (MidgardQueryValue *) self;

	if (!mqp->priv)
		return;

	if (G_IS_VALUE (&mqp->priv->value))
		g_value_unset (&mqp->priv->value);

	g_value_init (&mqp->priv->value, G_VALUE_TYPE (value));
	g_value_copy (value, &mqp->priv->value);

	return TRUE;
}

/* GOBJECT ROUTINES */

enum {
	PROPERTY_VALUE = 1
};

static void
midgard_query_value_init (MidgardQueryHolderIFace *iface)
{
       iface->get_value = __get_value;
       iface->set_value = __set_value;
}

static GObjectClass *parent_class= NULL;

static void
__midgard_query_value_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardQueryValue *self = (MidgardQueryValue *) instance;
	self->priv = g_new (MidgardQueryValuePrivate, 1);

}

static GObject *
_midgard_query_value_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties, construct_properties);
	
	return G_OBJECT(object);
}

static void
__midgard_query_value_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	switch (property_id) {
		
		case PROPERTY_VALUE:
			__get_value (MIDGARD_QUERY_HOLDER (object), value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
__midgard_query_value_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	switch (property_id) {

		case PROPERTY_VALUE:
			__set_value (MIDGARD_QUERY_HOLDER (object), value);
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}


static void
_midgard_query_value_dispose (GObject *object)
{
	parent_class->dispose (object);
}

static void
_midgard_query_value_finalize (GObject *object)
{
	MidgardQueryValue *self = MIDGARD_QUERY_VALUE (object);

	if (G_IS_VALUE (&self->priv->value))
		g_value_unset (&self->priv->value);

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
_midgard_query_value_class_init (MidgardQueryValueClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_query_value_constructor;
	object_class->dispose = _midgard_query_value_dispose;
	object_class->finalize = _midgard_query_value_finalize;

	object_class->set_property = __midgard_query_value_set_property;
	object_class->get_property = __midgard_query_value_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_gtype ("value",
			"",
			"",
			G_TYPE_VALUE,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	/**
	 * MidgardQueryValue:value:
	 * 
	 */  
	g_object_class_install_property (object_class, PROPERTY_VALUE, pspec);
}

GType
midgard_query_value_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardQueryValueClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			(GClassInitFunc) _midgard_query_value_class_init, /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardQueryValue),
			0,      /* n_preallocs */
			__midgard_query_value_instance_init    /* instance_init */
		};
      
		static const GInterfaceInfo value_info = {
			(GInterfaceInitFunc) midgard_query_value_init,   
			NULL,	/* interface_finalize */
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryValue", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_HOLDER, &value_info);
    	}
    	return type;
}

