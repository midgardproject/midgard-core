/* MidgardCore SchemaBuilder routines routines
 *    
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

#ifndef _MIDGARD_CR_CORE_SCHEMA_BUILDER_H_
#define _MIDGARD_CR_CORE_SCHEMA_BUILDER_H_

#include <glib-object.h>
#include "midgard3.h"
#include "midgard_local.h"

typedef struct _MgdSchemaTypeAttr MgdSchemaTypeAttr;
typedef struct _MgdSchemaPropertyAttr MgdSchemaPropertyAttr;

struct _MgdSchemaTypeAttr {
	gchar *name;
	guint base_index;
	guint num_properties;	
	GParamSpec **params;
	MgdSchemaPropertyAttr **properties;
	GHashTable *prophash;	
	GSList *_properties_list;
	GSList *children;
	gchar *copy_from;
	gchar *extends;
	gchar *metadata_class_name;
	GHashTable *user_values;
};

struct _MgdSchemaPropertyAttr {
	GType gtype;
	GValue value;
	guint8 access_flags;
	GValue *default_value;
	const gchar *type;
	const gchar *name;
	const gchar *link;
	const gchar *link_target;
	gboolean is_link;
	gboolean is_linked;
	gboolean is_private;
	gboolean is_unique;
	gchar *description;
	GHashTable *user_values;
};

void midgard_cr_core_schema_builder_register_type (MidgardCRSchemaBuilder *builder, GError **error);

#endif /* _MIDGARD_CR_CORE_SCHEMA_BUILDER_H_ */
