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

#ifndef MIDGARD_REFLECTOR_PROPERTY_H
#define MIDGARD_REFLECTOR_PROPERTY_H

#include "midgard_type.h"
#include "midgard_object.h"

/* convention macros */
#define MIDGARD_TYPE_REFLECTOR_PROPERTY (midgard_reflector_property_get_type())
#define MIDGARD_REFLECTOR_PROPERTY(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_REFLECTOR_PROPERTY, MidgardReflectorProperty))
#define MIDGARD_REFLECTOR_PROPERTY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_REFLECTOR_PROPERTY, MidgardReflectorPropertyClass))
#define MIDGARD_IS_REFLECTOR_PROPERTY(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_REFLECTOR_PROPERTY))
#define MIDGARD_IS_REFLECTOR_PROPERTY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_REFLECTOR_OBJECT_PROPERTY))
#define MIDGARD_REFLECTOR_PROPERTY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_REFLECTOR_PROPERTY, MidgardReflectorPropertyClass))

typedef struct _MidgardReflectorProperty MidgardReflectorProperty;
typedef struct _MidgardReflectorPropertyClass MidgardReflectorPropertyClass;

struct _MidgardReflectorPropertyClass{
	GObjectClass parent;
};

GType midgard_reflector_property_get_type(void);

MidgardReflectorProperty	*midgard_reflector_property_new 		(const gchar *classname);
GType				midgard_reflector_property_get_midgard_type 	(MidgardReflectorProperty *self, const gchar *propname);
gboolean 			midgard_reflector_property_is_link 		(MidgardReflectorProperty *self, const gchar *propname);
gboolean			midgard_reflector_property_is_linked 		(MidgardReflectorProperty *self, const gchar *propname);
const MidgardDBObjectClass	*midgard_reflector_property_get_link_class 	(MidgardReflectorProperty *self, const gchar *propname);
const gchar			*midgard_reflector_property_get_link_name 	(MidgardReflectorProperty *self, const gchar *propname);
const gchar 			*midgard_reflector_property_get_link_target 	(MidgardReflectorProperty *self, const gchar *propname);
const gchar 			*midgard_reflector_property_description 	(MidgardReflectorProperty *self, const gchar *propname);
const gchar 			*midgard_reflector_property_get_user_value 	(MidgardReflectorProperty *self, const gchar *property, const gchar *name);
gboolean			midgard_reflector_property_is_private 		(MidgardReflectorProperty *self, const gchar *property);

#endif /* MIDGARD_REFLECTOR_PROPERTY_H */
