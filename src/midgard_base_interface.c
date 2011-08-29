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

static void 
__midgard_interface_base_init (MidgardBaseInterface * iface) 
{
	static gboolean initialized = FALSE;
	if (!initialized) {
		initialized = TRUE;
				
//		g_object_interface_install_property (iface, g_param_spec_object ("parent", "parent", "parent", MIDGARD_CR_TYPE_MODEL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	}
}

GType 
midgard_core_type_register_interface (MgdSchemaTypeAttr *type_attr)
{
	GType type = g_type_from_name (type_attr->name);
	/* FIXME, throw warning if type is registered and it's not interface */
	if (type != G_TYPE_NONE && G_TYPE_IS_INTERFACE(type))
		return type;

	static const GTypeInfo info = {
		sizeof (MidgardBaseInterfaceIFace),
		(GBaseInitFunc) __midgard_interface_base_init,
		NULL,   /* base_finalize */
		NULL,   /* class_init */
		NULL,   /* class_finalize */
		NULL,   /* class_data */
		0,			
		0,      /* n_preallocs */
		NULL    /* instance_init */
	};
	
	GType parent_type = MIDGARD_TYPE_BASE_INTERFACE;
	if (type_attr->extends) {
		parent_type = g_type_from_name (type_attr->extends);
		if (parent_type == G_TYPE_NONE) {
			g_warning ("Failed to get type of '%s', defined as parent of '%s'", type_attr->extends, type_attr->name);
			g_error ("Invalid parent type");
		}
	}

	g_print ("CORE REGISTER '%s' IFACE \n", type_attr->name);
	
	type = g_type_register_static (G_TYPE_INTERFACE, type_attr->name, &info, 0);
	g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	g_type_interface_add_prerequisite (type, parent_type);

	return type;
}

GType
midgard_base_interface_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardBaseInterfaceIFace),
			NULL,   /* base_init */
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

