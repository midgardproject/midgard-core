/* 
 * Copyright (C) 2006, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef _PRIVATE_SCHEMA_H
#define _PRIVATE_SCHEMA_H

#include "midgard_schema.h"
#include "midgard_defs.h"

typedef struct _MgdSchemaTypeAttr MgdSchemaTypeAttr;
typedef struct _MgdSchemaPropertyAttr MgdSchemaPropertyAttr;

struct _MgdSchemaPropertyAttr{
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

#define MGD_TYPE_ATTR_METADATA_CLASS(__typeattr) __typeattr->metadata_class_name

/* MgdSchema storage utilities */
MgdSchemaTypeAttr 	*midgard_schema_lookup_type		(MidgardSchema *schema, gchar *type);
GType			midgard_type_register			(MgdSchemaTypeAttr *data, GType parent_type);

MgdSchemaTypeAttr	*midgard_core_schema_type_attr_new	(void);
void 			midgard_core_schema_type_attr_free	(MgdSchemaTypeAttr *prop);
void                    midgard_core_schema_type_attr_extend	(MgdSchemaTypeAttr *src, MgdSchemaTypeAttr *dst);

void			midgard_core_schema_type_set_table	 	(MgdSchemaTypeAttr *prop, const gchar *table);
void 			midgard_core_schema_type_add_table 		(MgdSchemaTypeAttr *type, const gchar *table);
void			midgard_core_schema_type_build_static_sql	(MgdSchemaTypeAttr *type_attr);
void			midgard_core_schema_type_initialize_paramspec 	(MgdSchemaTypeAttr *type);
void 			midgard_core_schema_type_validate_fields	(MgdSchemaTypeAttr *type);

MgdSchemaPropertyAttr	*midgard_core_schema_type_property_attr_new_empty	(void);
MgdSchemaPropertyAttr	*midgard_core_schema_type_property_attr_new		(void);
void 			midgard_core_schema_type_property_attr_free		(MgdSchemaPropertyAttr *prop);
void 			midgard_core_schema_type_property_copy			(MgdSchemaPropertyAttr *src_prop, MgdSchemaTypeAttr *dest_type);
void			midgard_core_schema_type_property_set_gtype		(MgdSchemaPropertyAttr *prop, const gchar *type);
void 			midgard_core_schema_type_property_set_tablefield	(MgdSchemaPropertyAttr *prop, const gchar *table, const gchar *field);
void			midgard_core_schema_type_property_set_table 		(MgdSchemaPropertyAttr *prop, const gchar *table);
MgdSchemaPropertyAttr	*midgard_core_schema_type_property_lookup		(MgdSchemaTypeAttr *type, const gchar *name);

GType 			midgard_core_schema_gtype_from_string		(const gchar *type);

#define MGD_SCHEMA_XML_NAMESPACE "http:/""/www.midgard-project.org/repligard/1.4"
#define MGD_SCHEMA_XML_ROOT_NAME "Schema"

/* RESERVED WORDS */
#define TYPE_RW_TYPE		"type"
#define TYPE_RW_NAME		"name"
#define TYPE_RW_PARENT		"parent"
#define TYPE_RW_TABLE		"table"
#define TYPE_RW_COPY		"copy"
#define TYPE_RW_EXTENDS		"extends"
#define TYPE_RW_METADATA 	"metadata"
#define TYPE_RW_USERVALUES	"user_values"
#define TYPE_RW_PROPERTY	"property"

#define PROP_RW_PRIVATE		"private"
#define PROP_RW_FIELD		"field"
#define PROP_RW_INDEX		"index"
#define PROP_RW_UPFIELD		"upfield"
#define PROP_RW_PARENTFIELD	"parentfield"
#define PROP_RW_PRIMARY		"primaryfield"
#define PROP_RW_REVERSE		"reverse"
#define PROP_RW_LINK		"link"

#endif /* _PRIVATE_SCHEMA_H */
