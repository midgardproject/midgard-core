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

#ifndef MIDGARD_SCHEMA_OBJECT_FACTORY_H
#define MIDGARD_SCHEMA_OBJECT_FACTORY_H

#include "midgard_type.h"
#include "midgard_object.h"

/* convention macros */
#define MIDGARD_TYPE_SCHEMA_OBJECT_FACTORY (midgard_schema_object_factory_get_type())
#define MIDGARD_SCHEMA_OBJECT_FACTORY(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), MIDGARD_TYPE_SCHEMA_OBJECT_FACTORY, MidgardSchemaObjectFactory))
#define MIDGARD_SCHEMA_OBJECT_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_SCHEMA_OBJECT_FACTORY, MidgardSchemaObjectFactoryClass))
#define MIDGARD_IS_SCHEMA_OBJECT_FACTORY(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SCHEMA_OBJECT_FACTORY))
#define MIDGARD_IS_SCHEMA_OBJECT_FACTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_REFLECTOR_OBJECT_PROPERTY))
#define MIDGARD_SCHEMA_OBJECT_FACTORY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_SCHEMA_OBJECT_FACTORY, MidgardSchemaObjectFactoryClass))

typedef struct _MidgardSchemaObjectFactory MidgardSchemaObjectFactory;
typedef struct _MidgardSchemaObjectFactoryPrivate MidgardSchemaObjectFactoryPrivate;
typedef struct _MidgardSchemaObjectFactoryClass MidgardSchemaObjectFactoryClass;

struct _MidgardSchemaObjectFactory {
	GObject parent;
};

struct _MidgardSchemaObjectFactoryClass{
	GObjectClass parent;
};

GType midgard_schema_object_factory_get_type(void);

MidgardObject	*midgard_schema_object_factory_get_object_by_guid 	(MidgardConnection *mgd, const gchar *guid);
MidgardObject	*midgard_schema_object_factory_get_object_by_path 	(MidgardConnection *mgd, const gchar *classname, const gchar *path);
gboolean	midgard_schema_object_factory_object_undelete		(MidgardConnection *mgd, const gchar *guid);
MidgardObject	*midgard_schema_object_factory_create_object		(MidgardConnection *mgd, const gchar *classname, GValue *value);

#endif /* MIDGARD_SCHEMA_OBJECT_FACTORY_H */
