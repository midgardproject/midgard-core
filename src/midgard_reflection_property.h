/* 
 * Copyright (C) 2006 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_REFLECTION_PROPERTY_H
#define MIDGARD_REFLECTION_PROPERTY_H

#include "midgard_type.h"
#include "midgard_object.h"

/* convention macros */
#define MIDGARD_TYPE_REFLECTION_PROPERTY (midgard_reflection_property_get_type())
#define MIDGARD_REFLECTION_PROPERTY(object)  \
	(G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_REFLECTION_PROPERTY, MidgardReflectionProperty))
#define MIDGARD_REFLECTION_PROPERTY_CLASS(klass)  \
	(G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_REFLECTION_PROPERTY, MidgardReflectionPropertyClass))
#define MIDGARD_IS_REFLECTION_PROPERTY(object)   \
	(G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_REFLECTION_PROPERTY))
#define MIDGARD_IS_REFLECTION_PROPERTY_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_REFLECTION_OBJECT_PROPERTY))
#define MIDGARD_REFLECTION_PROPERTY_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_REFLECTION_PROPERTY, MidgardReflectionPropertyClass))

typedef struct _MidgardReflectionProperty MidgardReflectionProperty;
typedef struct _MidgardReflectionPropertyClass MidgardReflectionPropertyClass;

struct _MidgardReflectionPropertyClass{
	GObjectClass parent;

	/* public class members */
	gboolean (*is_link) (MidgardReflectionProperty *object, const gchar *propname);
};

GType midgard_reflection_property_get_type(void);

MidgardReflectionProperty	*midgard_reflection_property_new (MidgardDBObjectClass *klass);
GType				midgard_reflection_property_get_midgard_type (MidgardReflectionProperty *self, const gchar *propname);
gboolean 			midgard_reflection_property_is_link (MidgardReflectionProperty *self, const gchar *propname);
gboolean			midgard_reflection_property_is_linked (MidgardReflectionProperty *self, const gchar *propname);
const MidgardDBObjectClass	*midgard_reflection_property_get_link_class (MidgardReflectionProperty *self, const gchar *propname);
const gchar			*midgard_reflection_property_get_link_name (MidgardReflectionProperty *self, const gchar *propname);
const gchar 			*midgard_reflection_property_get_link_target (MidgardReflectionProperty *self, const gchar *propname);
const gchar 			*midgard_reflection_property_description (MidgardReflectionProperty *self, const gchar *propname);
const gchar 			*midgard_reflection_property_get_user_value (MidgardReflectionProperty *self, const gchar *property, const gchar *name);
gboolean			midgard_reflection_property_is_private (MidgardReflectionProperty *self, const gchar *property);

#endif /* MIDGARD_REFLECTION_PROPERTY_H */
