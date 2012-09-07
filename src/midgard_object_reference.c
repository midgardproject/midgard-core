/* 
 * Copyright (C) 2012 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_object_reference.h"
#include "midgard_model.h"
#include "midgard_model_reference.h"
#include "midgard_validable.h"

struct _MidgardObjectReferencePrivate {
	const gchar *name;
	const gchar *id;
	MidgardWorkspace *workspace;
};

/**
 * midgard_object_reference_new:
 * @id: object's identifier
 * @name: (allow-none): the name of the reference
 * @workspace: (allow-none): #MidgardWorkspace instance
 * 
 * Returns: new #MidgardObjectReference
 *
 * Since: 12.09
 */ 
MidgardObjectReference *             
midgard_object_reference_new (const gchar *id, const gchar *name, MidgardWorkspace *workspace)
{
	g_return_val_if_fail (id != NULL, NULL);

	const gchar *_name = "";
	if (name)
		_name = name;
	MidgardObjectReference *self = NULL;

	if (workspace)
		self = g_object_new (MIDGARD_TYPE_OBJECT_REFERENCE, "id", id, "name", name, "workspace", workspace, NULL);
	else 
		self = g_object_new (MIDGARD_TYPE_OBJECT_REFERENCE, "id", id, "name", name, NULL);

	return self;
}

/**
 * midgard_object_reference_get_workspace:
 * @self: #MidgardObjectReference instance
 * @error: pointer to store returned error
 * 
 * Returns: (transfer full): #MidgardWorkspace instance or %NULL
 *
 * Since: 12.09
 */ 
MidgardWorkspace *             
midgard_object_reference_get_workspace (MidgardObjectReference *self, GError **error)
{
	g_return_val_if_fail (self != NULL, NULL);
	/* TODO, handle error */
	if (self->priv->workspace == NULL)
		return NULL;
	return g_object_ref (self->priv->workspace);
}

const gchar*                
_midgard_object_reference_get_name (MidgardModel *iface, GError **error)
{
	MidgardObjectReference *self = MIDGARD_OBJECT_REFERENCE(iface);
	return self->priv->name;
}

const gchar*                
_midgard_object_reference_get_id (MidgardModelReference *iface, GError **error)
{
	MidgardObjectReference *self = MIDGARD_OBJECT_REFERENCE(iface);
	return self->priv->id;
}

GValue*                
_midgard_object_reference_get_id_value (MidgardModelReference *iface, GError **error)
{
	MidgardObjectReference *self = MIDGARD_OBJECT_REFERENCE(iface);

	const gchar *id = self->priv->id;
	GValue *id_value = g_new0(GValue, 1);

	/* GUID */
	if (midgard_is_guid(id)) {
		g_value_init (id_value, G_TYPE_STRING);
		g_value_set_string (id_value, id);
	/* integer */
	} else if (isdigit(id)) {
		g_value_init (id_value, G_TYPE_UINT);
		g_value_set_uint (id_value, atoi(id));
	/* undefined TODO: UUID */	
	} else {
		g_free (id_value);
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_TYPE_INVALID, 
				"Invalid ID value type. Expected guid, uuid or integer id. Got '%s'.",
				G_VALUE_TYPE_NAME (id_value));
		return;
	}	

	return id_value;
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_NAME = 1,
	PROPERTY_ID,
	PROPERTY_WORKSPACE
};


static void
_midgard_object_reference_model_iface_init (MidgardModelIFace *iface)
{
	iface->get_name = _midgard_object_reference_get_name;
	return;
}

static void
_midgard_object_reference_model_reference_iface_init (MidgardModelReferenceIFace *iface)
{
	iface->get_id = _midgard_object_reference_get_id;
	iface->get_id_value = _midgard_object_reference_get_id_value;
	return;
}

static void 
_midgard_object_reference_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardObjectReference *self = (MidgardObjectReference*) instance;
	self->priv = g_new(MidgardObjectReferencePrivate, 1);
	self->priv->name = NULL;
	self->priv->id = NULL;
	self->priv->workspace = NULL;
	return;
}

static GObject *
_midgard_object_reference_constructor (GType type,
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
_midgard_object_reference_dispose (GObject *object)
{
	MidgardObjectReference *self = MIDGARD_OBJECT_REFERENCE (object);
	if (self->priv->workspace) 
		g_object_unref (self->priv->workspace);
	self->priv->workspace = NULL;

	__parent_class->dispose (object);
}

static void 
_midgard_object_reference_finalize (GObject *object)
{
	MidgardObjectReference *self = MIDGARD_OBJECT_REFERENCE (object);

	g_free((gchar *)self->priv->name);
	self->priv->name = NULL;
		
	g_free((gchar *)self->priv->id);
	self->priv->id = NULL;

	g_free(self->priv);
	self->priv = NULL;

	__parent_class->finalize (object);
}

static void
_midgard_object_reference_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardObjectReference *self = (MidgardObjectReference *) object;

	switch (property_id) {
		
		case PROPERTY_NAME:
			g_value_set_string (value, self->priv->name);
			break;

		case PROPERTY_ID:
			g_value_set_string (value, self->priv->id);
			break;

		case PROPERTY_WORKSPACE:
			g_value_set_object (value, self->priv->workspace);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
_midgard_object_reference_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardObjectReference *self = (MidgardObjectReference *) (object);
	
	switch (property_id) {
	
		case PROPERTY_NAME:
			self->priv->name = g_value_dup_string (value);
			break;
	
		case PROPERTY_ID:
			self->priv->id = g_value_dup_string (value);
			break;

		case PROPERTY_WORKSPACE:
			self->priv->workspace = g_value_dup_object (value);
			break;
		
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void _midgard_object_reference_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardObjectReferenceClass *klass = MIDGARD_OBJECT_REFERENCE_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_object_reference_constructor;
	gobject_class->dispose = _midgard_object_reference_dispose;
	gobject_class->finalize = _midgard_object_reference_finalize;

	gobject_class->set_property = _midgard_object_reference_set_property;
	gobject_class->get_property = _midgard_object_reference_get_property;
	
	/* Properties */
	GParamSpec *pspec = g_param_spec_string ("name",
			"",
			"",
			"",
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardObjectReference:name:
	 */
	g_object_class_install_property (gobject_class, PROPERTY_NAME, pspec);

	pspec = g_param_spec_string ("id",
			"",
			"",
			"",
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardObjectReference:id:
	 */
	g_object_class_install_property (gobject_class, PROPERTY_ID, pspec);

	pspec = g_param_spec_object ("workspace",
			"",
			"",
			MIDGARD_TYPE_WORKSPACE,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardObjectReference:name:
	 */
	g_object_class_install_property (gobject_class, PROPERTY_WORKSPACE, pspec);

}

GType 
midgard_object_reference_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardObjectReferenceClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_object_reference_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardObjectReference),
			0,              /* n_preallocs */
			_midgard_object_reference_instance_init /* instance_init */
		};

		static const GInterfaceInfo model_iface_info = {
			(GInterfaceInitFunc) _midgard_object_reference_model_iface_init,
			NULL,	/* interface finalize */	
			NULL    /* interface_data */
		};

		static const GInterfaceInfo model_reference_iface_info = {
			(GInterfaceInitFunc) _midgard_object_reference_model_reference_iface_init,
			NULL,	/* interface finalize */	
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardObjectReference", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_MODEL, &model_iface_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_MODEL_REFERENCE, &model_reference_iface_info);
	}
	return type;
}

