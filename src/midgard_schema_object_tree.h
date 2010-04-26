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

#ifndef MIDGARD_SCHEMA_OBJECT_TREE_H
#define MIDGARD_SCHEMA_OBJECT_TREE_H

#include "midgard_type.h"
#include "midgard_object.h"

/* convention macros */
#define MIDGARD_TYPE_SCHEMA_OBJECT_TREE (midgard_schema_object_tree_get_type())
#define MIDGARD_SCHEMA_OBJECT_TREE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object), MIDGARD_TYPE_SCHEMA_OBJECT_TREE, MidgardSchemaObjectTree))
#define MIDGARD_SCHEMA_OBJECT_TREE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_SCHEMA_OBJECT_TREE, MidgardSchemaObjectTreeClass))
#define MIDGARD_IS_SCHEMA_OBJECT_TREE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_SCHEMA_OBJECT_TREE))
#define MIDGARD_IS_SCHEMA_OBJECT_TREE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_REFLECTOR_OBJECT_PROPERTY))
#define MIDGARD_SCHEMA_OBJECT_TREE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_SCHEMA_OBJECT_TREE, MidgardSchemaObjectTreeClass))

typedef struct _MidgardSchemaObjectTree MidgardSchemaObjectTree;
typedef struct _MidgardSchemaObjectTreePrivate MidgardSchemaObjectTreePrivate;
typedef struct _MidgardSchemaObjectTreeClass MidgardSchemaObjectTreeClass;

struct _MidgardSchemaObjectTree {
	GObject parent;
};

struct _MidgardSchemaObjectTreeClass{
	GObjectClass parent;
};

GType		midgard_schema_object_tree_get_type			(void);

gboolean	midgard_schema_object_tree_is_in_tree			(MidgardObject *object, MidgardObject *parent_object);
const gchar 	*midgard_schema_object_tree_get_parent_name		(MidgardObject *object);
MidgardObject	*midgard_schema_object_tree_get_parent_object		(MidgardObject *object);
MidgardObject   **midgard_schema_object_tree_list_objects		(MidgardObject *object, guint *n_objects);
MidgardObject	**midgard_schema_object_tree_list_children_objects	(MidgardObject *object, const gchar *classname, guint *n_objects);

#endif /* MIDGARD_SCHEMA_OBJECT_TREE_H */
