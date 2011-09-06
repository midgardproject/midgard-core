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

#ifndef MIDGARD_REFLECTOR_OBJECT_H
#define MIDGARD_REFLECTOR_OBJECT_H

#include "midgard_type.h"
#include "midgard_connection.h"

/* convention macros */
#define MIDGARD_TYPE_REFLECTOR_OBJECT (midgard_reflector_object_get_type())
#define MIDGARD_REFLECTOR_OBJECT(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), MIDGARD_TYPE_REFLECTOR_OBJECT, MidgardReflectorObject))
#define MIDGARD_REFLECTOR_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_REFLECTOR_OBJECT, MidgardReflectorObjectClass))
#define MIDGARD_IS_REFLECTOR_OBJECT(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_REFLECTOR_OBJECT))
#define MIDGARD_IS_REFLECTOR_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_REFLECTOR_OBJECT_PROPERTY))
#define MIDGARD_REFLECTOR_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_REFLECTOR_OBJECT, MidgardReflectorObjectClass))

typedef struct _MidgardReflectorObject MidgardReflectorObject;
typedef struct _MidgardReflectorObjectClass MidgardReflectorObjectClass;

struct _MidgardReflectorObject {
	GObject parent;
};

struct _MidgardReflectorObjectClass{
	GObjectClass parent;
};

GType midgard_reflector_object_get_type(void);

const gchar 		*midgard_reflector_object_get_property_primary		(const gchar *classname);
const gchar 		*midgard_reflector_object_get_property_up		(const gchar *classname);
const gchar 		*midgard_reflector_object_get_property_parent		(const gchar *classname);
const gchar 		*midgard_reflector_object_get_property_unique		(const gchar *classname);
gchar 			**midgard_reflector_object_list_children		(const gchar *classname, guint *n_children);
gboolean		midgard_reflector_object_has_metadata_class		(const gchar *classname);
const gchar 		*midgard_reflector_object_get_metadata_class		(const gchar *classname);
const gchar 		*midgard_reflector_object_get_schema_value		(const gchar *classname, const gchar *name);
gboolean		midgard_reflector_object_is_mixin			(const gchar *classname);
gboolean		midgard_reflector_object_is_interface			(const gchar *classname);
gboolean		midgard_reflector_object_is_abstract			(const gchar *classname);
gchar			**midgard_reflector_object_list_defined_properties	(const gchar *classname, guint *n_prop);

#endif /* MIDGARD_REFLECTOR_OBJECT_H */
