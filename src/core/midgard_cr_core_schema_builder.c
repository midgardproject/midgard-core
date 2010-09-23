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

#include "midgard_cr_core_schema_builder.h"

typedef struct _MgdSchemaTypeAttr MgdSchemaTypeAttr;
typedef struct _MgdSchemaPropertyAttr MgdSchemaPropertyAttr;

struct _MgdSchemaTypeAttr {
	gchar *name;
	guint base_index;
	guint num_properties;
	guint class_nprop;
	GParamSpec **params;
	MgdSchemaPropertyAttr **properties;
	GHashTable *prophash;
	GHashTable *tableshash;
	GSList *_properties_list;
	GParamSpec **storage_params;
	guint storage_params_count;
	const gchar *tables;
	const gchar *table;
	const gchar *parent;
	const gchar *primary;
	const gchar *property_up;
	const gchar *property_parent;
      	gchar *parentfield;
	gchar *upfield;
	gchar *primaryfield;
	GSList *children;
	gchar *sql_select_full;
	const gchar *unique_name;
	gchar *copy_from;
	gchar *extends;
	GSList *joins;
	GSList *constraints;
	gboolean is_view;
	gchar *sql_create_view;
	gchar *metadata_class_name;
	MidgardMetadataClass *metadata_class_ptr;
	GHashTable *user_values;
};

struct _MgdSchemaPropertyAttr {
	GType gtype;
	GValue value;
	guint8 access_flags;
	GValue *default_value;
	const gchar *type;
	const gchar *name;
	const gchar *dbtype;
	const gchar *field;
	gboolean dbindex;
	const gchar *table;
	const gchar *tablefield;
	const gchar *upfield;
	const gchar *parentfield;
	const gchar *primaryfield;
	const gchar *link;
	const gchar *link_target;
	gboolean is_link;
	gboolean is_linked;
	gboolean is_primary;
	gboolean is_reversed;
	gboolean is_private;
	gboolean is_unique;
	gchar *description;
	GHashTable *user_values;
};

void 
midgard_cr_core_schema_builder_register_types (MidgardCRSchemaBuilder *builder, GError **error)
{
	g_return_if_fail (builder != NULL);
	g_return_if_fail (error == NULL || *error == NULL);
}
