/* 
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_base_interface.h"
#include "schema.h"
#include "midgard_base_mixin.h"
#include "midgard_core_object.h"

void
midgard_core_interface_add_prerequisites (GType type, GType iface_type)
{
	guint n_types;
	/* Get all prerequisites of given interface type */
	GType *pre_types = g_type_interface_prerequisites (iface_type, &n_types);
	if (n_types < 1)
		return;

	static const GInterfaceInfo iface_info = {
		NULL, 	/* interface init */
		NULL,   /* interface_finalize */
		NULL    /* interface_data */
	};

	guint i;
	for (i = 0; i < n_types; i++) {
		/* This can be G_OBJECT added as prerequisite */
		if (!G_TYPE_IS_INTERFACE (pre_types[i]))
			continue;

		/* Check if given type already implements interface. */
		guint n_impl;
		guint j;
		gboolean ignore = FALSE;
		GType *implemented = g_type_interfaces (type, &n_impl);
		for (j = 0; j < n_impl; j++) {
			if (implemented[j] == pre_types[i]) {
				ignore = TRUE;
				break;
			}
		}

		if (ignore)
			continue;

		/* chain up, till we can get anything */
		midgard_core_interface_add_prerequisites (type, pre_types[i]);
		g_type_add_interface_static (type, pre_types[i], &iface_info);		
	}
}

static void 
__midgard_base_interface_derived_init (gpointer *klass) 
{

}

/* We use class_init routine, instead of base_init, which should be involved in case of reference.
 * If you have better idea how to pass MgdSchemaTypeAttr to base_init, rewrite those two routines */
static void 
__midgard_base_interface_derived_class_init (gpointer *klass, gpointer class_data)
{
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *) class_data;
	guint i;
	for (i = 0; i < type_attr->num_properties; i++) {
		GParamSpec *pspec = g_object_interface_find_property (klass, type_attr->params[i]->name);
		if (!pspec)
			g_object_interface_install_property (klass, type_attr->params[i]);
	}

	((MidgardBaseInterfaceIFace *)klass)->priv = g_new (MidgardBaseInterfacePrivate, 1);
	((MidgardBaseInterfaceIFace *)klass)->priv->type_data = class_data;
}

GType 
midgard_core_type_register_interface (MgdSchemaTypeAttr *type_attr)
{
	GType type = g_type_from_name 	(type_attr->name);
	/* FIXME, throw warning if type is registered and it's not interface */
	if (type != G_TYPE_NONE && G_TYPE_IS_INTERFACE(type))
		return type;

	/* First, chain up. If prerequisite is not yet registered, attempt to do so */
	guint i;
	guint n_types;
	gchar **extends = midgard_core_schema_type_list_extends (type_attr, &n_types);
	for (i = 0; i < n_types; i++) {
		if (g_type_from_name (extends[i]) == G_TYPE_INVALID) {
			MgdSchemaTypeAttr *parent_attr = midgard_schema_lookup_type (type_attr->schema, extends[i]);
			if (!parent_attr)
				g_error ("Can not find '%s' definition", extends[i]);
			midgard_core_type_register_interface (parent_attr);
		}
	}
	if (extends)
		g_strfreev (extends);

	GTypeInfo info = {
		sizeof (MidgardBaseInterfaceIFace),
		(GBaseInitFunc) __midgard_base_interface_derived_init,
		NULL,   /* base_finalize */
		(GClassInitFunc) __midgard_base_interface_derived_class_init,	
		NULL,   /* class_finalize */
		(const MgdSchemaTypeAttr *) type_attr,   /* class_data */
		0,			
		0,      /* n_preallocs */
		NULL    /* instance_init */
	};
	
	type = g_type_register_static (G_TYPE_INTERFACE, type_attr->name, &info, 0);
	g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);

	GType parent_type = MIDGARD_TYPE_BASE_INTERFACE;
	if (type_attr->is_mixin)
		parent_type = MIDGARD_TYPE_BASE_MIXIN;
	extends = midgard_core_schema_type_list_extends (type_attr, &n_types);
	
	if (n_types == 0) {
		g_type_interface_add_prerequisite (type, parent_type);
		return type;
	}

	for (i = 0; i < n_types; i++) {
		parent_type = g_type_from_name (extends[i]);
		if (parent_type == G_TYPE_INVALID) {
			g_warning ("Failed to get type of '%s', defined as parent of '%s'", type_attr->extends, type_attr->name);
			g_error ("Invalid parent type");
		}
		g_type_interface_add_prerequisite (type, parent_type);
	}
	
	g_strfreev (extends);

	return type;
}


static void
__midgard_base_interface_init (gpointer g_class)
{

}

GType
midgard_base_interface_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardBaseInterfaceIFace),
			(GBaseInitFunc) __midgard_base_interface_init,
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardBaseInterface", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}
	return type;
}

