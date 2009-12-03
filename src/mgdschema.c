/* Midgard schema , records and objects definition.
   
  Copyright (C) 2004,2005,2006,2007,2008, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
  
  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <config.h>
#include "midgard_schema.h"
#include "midgard_object.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "midgard_datatypes.h"
#include "midgard_type.h"
#include "schema.h"
#include "midgard_object_class.h"
#include "midgard_core_config.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include "midgard_core_views.h"
#include "midgard_reflection_property.h"
#include <libxml/parserInternals.h>
#include "guid.h"
#include "midgard_core_xml.h"

/* TODO tune header files , no need to include string.h while we need to include midgard.h in almost every case */

/* Global schema_trash used when MgdSchema is destroyed 
 * Trash must be global as we share pointers between different schemas.
 * Especially Midgard core object's MgdSchemaTypes are shared.
 * */
static GSList *schema_trash = NULL;

/* Global schema file path, it's a pointer to filename being parsed.
 * Only for warning purposes */
static const gchar *parsed_schema = NULL;

void _destroy_query_hash(gpointer key, gpointer value, gpointer userdata)
{
	gchar *name = (gchar *) key;	
	if(name) g_free(name);
}

/* return type's struct  or NULL if type is not in schema */
MgdSchemaTypeAttr *
midgard_schema_lookup_type (MidgardSchema *schema, gchar *name)
{
	g_assert(schema != NULL);
	g_assert(name != NULL);

	return (MgdSchemaTypeAttr *) g_hash_table_lookup(schema->types, name);	
}

/**
 * \ingroup mgdschema
 * \internal
 * Lookup for property attributes 
 *
 * @param schema MgdSchema used by application
 * @param type Name of type registered in schema
 * @param propname Name of property registered for type
 *
 * @return hash table which contains property's attributes
 */ 
GHashTable *
midgard_schema_lookup_type_member(MidgardSchema *schema, const gchar *typename, gchar *propname)
{
	g_assert(schema != NULL);
	g_assert(typename != NULL);
	g_assert(propname != NULL);
	
	MgdSchemaTypeAttr *sts;
	
	sts = midgard_schema_lookup_type(schema, (gchar *)typename);
	
	if (sts != NULL) 
		return g_hash_table_lookup(sts->prophash, propname);

	return NULL;
}

static void __warn_msg(xmlNode * node, const gchar *msg)
{
	gchar *prop = (gchar *)xmlGetProp(node, (const xmlChar *)"name");
	g_warning("%s ( %s:%s, %s on line %ld )", msg, node->name, prop,
			parsed_schema, xmlGetLineNo(node));
	g_free(prop);
}

/* Define which element names should be addedd to schema. 
 * We change them only here , and in xml file itself.
 */
static const gchar *mgd_complextype[] = { 
	"type", 
	"property", 
	"Schema", 
	"description", 
	"include", 
	"copy", 
	"extends", 
	TYPE_RW_USERVALUES,
	NULL 
};

static const gchar *mgd_attribute[] = { 
	TYPE_RW_TYPE,
	TYPE_RW_NAME, 
	TYPE_RW_TABLE, 
	TYPE_RW_EXTENDS,
	TYPE_RW_COPY,
	TYPE_RW_METADATA,
	"parent", 
	"parentfield", 
	"link", 
	"upfield", 
	"field", 
	"reverse", 
	"primaryfield", 
	"dbtype", 
	"index", 
	"default", 
	"unique",
	"multilang",
	PROP_RW_PRIVATE,
	NULL 
};

static const gchar *schema_allowed_types[] = {
	"integer",
	"unsigned integer",
	"float",
	"double",
	"string",
	"text",
	"guid",
	"longtext",
	"bool",
	"boolean",
	"datetime",
	NULL
};

/* RESERVED type names */
static const gchar *rtypes[] = { 
	"midgard_view",
	"midgard_user",
	NULL
};

/* RESERVED table names */
static const gchar *rtables[] = {
	"repligard",
	"midgard_user",
	NULL
};

/* RESERVED column names */
static const gchar *rcolumns[] = {
	"metadata_",	
	"group", /* This is MySQL reserved word */
	NULL
};

/* RESERVED property subnodes' names */
static const gchar *subnnames[] = {
	"description",
	NULL
};

static gboolean strv_contains(const char **strv, const xmlChar *str) {
        g_assert(strv != NULL);
        g_assert(str != NULL);
        while (*strv != NULL) {
                if (g_str_equal(*strv++, str)) {
                        return TRUE;
                }
        }
        return FALSE;
}

static void check_metadata_column(const xmlChar *column){
	
	if(g_str_has_prefix((const gchar *)column, "metadata_"))
		g_critical("Column names prefixed with 'metadata_' are reserved ones");
}

static void check_property_name(const xmlChar *name, const gchar *typename){
	
	g_assert(name != NULL);

	if(g_strrstr_len((const gchar *)name, strlen((const gchar *)name), "_") != NULL)
		g_critical("Type '%s', property '%s'. Underscore not allowed in property names!",
				typename, name);

	if(g_str_equal(name, "guid"))
		g_critical("Type '%s'.Property name 'guid' is reserved!",
				typename);		
}

/* Get all type attributes */
static void
_get_type_attributes(xmlNode * node, MgdSchemaTypeAttr *type_attr, MidgardSchema *schema)
{
	xmlAttr *attr;
	xmlChar *attrval;

	if (node != NULL){

		attr = node->properties;
		attr = attr->next; /* avoid getting already added */

		while (attr != NULL){

			if (!strv_contains(mgd_attribute, attr->name)){
				g_warning("Wrong attribute '%s' in '%s' on line %ld",
						attr->name, parsed_schema, xmlGetLineNo(node));	
			}	

			attr = attr->next;
		}	

		/* Get table name */
		attrval = xmlGetProp(node, (const xmlChar *)TYPE_RW_TABLE);
		if (attrval) {

			if (strv_contains(rtables, attrval)) 
				g_error("'%s' is reserved table name", attrval);					

			midgard_core_schema_type_set_table (type_attr, (const gchar *)attrval);				
			xmlFree(attrval);	
		
		}

		/* parent */
		attrval = xmlGetProp(node, (const xmlChar *)TYPE_RW_PARENT);
		if (attrval) {
		
			if (type_attr->parent) g_free((gchar *)type_attr->parent);	
			type_attr->parent = g_strdup((gchar *)attrval);
			xmlFree(attrval);
		}

		/* extends */
		attrval = xmlGetProp(node, (const xmlChar *)TYPE_RW_EXTENDS);
		if (attrval) { 

			type_attr->extends = g_strdup((gchar *)attrval);
			xmlChar *table_name = (xmlChar *)xmlGetProp(node, (const xmlChar *)"table");	

			if (table_name != NULL) {

				__warn_msg(node, "Can not define 'table' and 'extends' attributes together");
				xmlFree(table_name);

				/* This might be undefined result, so we fallback to NULL default */
				g_free(type_attr->extends);
				type_attr->extends = NULL;
			}
	
			xmlFree(table_name);
		}

		/* copy */
		attrval = xmlGetProp(node, (const xmlChar *)TYPE_RW_COPY);
		if (attrval) {

			type_attr->copy_from = g_strdup((gchar *)attrval);
			xmlChar *table_name = (xmlChar *)xmlGetProp(node, (const xmlChar *)"table");	

			if (table_name == NULL) {

				__warn_msg(node, "Can not copy MgdSchema type without storage defined");
				g_error("Table definition missed");
			}
	
			xmlFree(table_name);
			xmlFree(attrval);
		}

		/* metadata */
		attrval = xmlGetProp(node, (const xmlChar *)TYPE_RW_METADATA);

		if (attrval) {

			if (g_str_equal (attrval, "false")) {

				type_attr->metadata_class = NULL;
			
			} else {

				type_attr->metadata_class = g_strdup((gchar *)attrval);
			}

			xmlFree(attrval);
		
		} else {

			type_attr->metadata_class = g_strdup ("midgard_metadata");
		}
	}
}

static void midgard_core_schema_get_property_type(xmlNode *node, 
		MgdSchemaTypeAttr *type_attr, MgdSchemaPropertyAttr *prop_attr)
{
	xmlAttr *attr;
	xmlChar *attrval;

	if (node != NULL) {
		
		attr = node->properties;
		attr = attr->next;
		
		while (attr != NULL) {

			if(g_str_equal(attr->name, "type")) {

				attrval = xmlNodeListGetString (node->doc, attr->children, 1);

				if (strv_contains(schema_allowed_types, attrval)) {					
	
					midgard_core_schema_type_property_set_gtype(prop_attr, (const gchar *)attrval);								
				} else {

					__warn_msg(node, "Unknown type");
				}

				xmlFree(attrval);
			}

			attr = attr->next;
		}
	}
	
	return;
}

static void midgard_core_schema_get_default_value(xmlNode *node, 
		MgdSchemaTypeAttr *type_attr, MgdSchemaPropertyAttr *prop_attr)
{	
	xmlAttr *attr;
	xmlChar *attrval;

	if (node != NULL) {
		
		attr = node->properties;
		attr = attr->next;
		
		while (attr != NULL) {

			if(g_str_equal(attr->name, "default")) {

				attrval = xmlNodeListGetString (node->doc, attr->children, 1);
				prop_attr->default_value = g_new0(GValue, 1);

				if (prop_attr->gtype == MGD_TYPE_STRING
						|| prop_attr->gtype == MGD_TYPE_LONGTEXT) {

					g_value_init(prop_attr->default_value, G_TYPE_STRING);
					g_value_set_string(prop_attr->default_value, (const gchar *)attrval);

				} else if (prop_attr->gtype == MGD_TYPE_INT) {

					if (attrval == NULL || *attrval == '\0') {
						__warn_msg(node, "Can not set default integer value from empty string");
						goto STEP_FORWARD;
					}

					g_value_init(prop_attr->default_value, G_TYPE_INT);
					g_value_set_int(prop_attr->default_value, (gint) atoi((const gchar *)attrval));

				} else if (prop_attr->gtype == MGD_TYPE_UINT) {

					if (attrval == NULL || *attrval == '\0') {
						__warn_msg(node, "Can not set default integer value from empty string");
						goto STEP_FORWARD;
					}

					g_value_init(prop_attr->default_value, G_TYPE_UINT);
					g_value_set_uint(prop_attr->default_value, (guint) atoi((const gchar *)attrval));


				} else if (prop_attr->gtype == MGD_TYPE_FLOAT) {

					if (attrval == NULL || *attrval == '\0') {
						__warn_msg(node, "Can not set default float value from empty string");
						goto STEP_FORWARD;
					}

					g_value_init(prop_attr->default_value, G_TYPE_FLOAT);
					g_value_set_float(prop_attr->default_value, (gfloat) g_ascii_strtod((const gchar *)attrval, NULL));

				} else if (prop_attr->gtype == MGD_TYPE_TIMESTAMP) {

					if (attrval == NULL || *attrval == '\0') {
						__warn_msg(node, "Can not set default timestamp value from empty string");
						goto STEP_FORWARD;
					}

					GValue strval = {0, };
					g_value_init(&strval, G_TYPE_STRING);
					g_value_set_string(&strval, (const gchar *)attrval); 

					g_value_init(prop_attr->default_value, MIDGARD_TYPE_TIMESTAMP);
					g_value_transform(&strval, prop_attr->default_value);
					g_value_unset(&strval);

				} else if (prop_attr->gtype == MGD_TYPE_BOOLEAN) {

					if (attrval == NULL || *attrval == '\0') {
						__warn_msg(node, "Can not set default boolean value from empty string");
						goto STEP_FORWARD;
					}

					g_value_init(prop_attr->default_value, G_TYPE_BOOLEAN);
					g_value_set_boolean(prop_attr->default_value, (gint) atoi((const gchar *)attrval));
	

				} else if (prop_attr->gtype == MGD_TYPE_GUID) {

					if (attrval == NULL || *attrval == '\0') {
						__warn_msg(node, "Can not set default guid value from empty string");
						goto STEP_FORWARD;
					}

					if (!midgard_is_guid((const gchar *)attrval)) {

						__warn_msg(node, "Given string (default value) is not guid");
						goto STEP_FORWARD;
					}

					g_value_init(prop_attr->default_value, G_TYPE_STRING);
					g_value_set_string(prop_attr->default_value, (const gchar *)attrval);

				} else {

					__warn_msg(node, "Do not know how to set default value for unknown type");
				}
					
				xmlFree(attrval);
			}
STEP_FORWARD:
			attr = attr->next;
		}
	}
	
	return;
}

/* Check if property is unique_name */
static void
midgard_core_schema_get_unique_name(xmlNode * node, MgdSchemaTypeAttr *type_attr, MgdSchemaPropertyAttr *prop_attr)
{
	xmlAttr *attr;
	xmlChar *attrval;

	if (node != NULL){

		attr = node->properties;
		attr = attr->next; /* avoid getting already added */

		while (attr != NULL){

			if (!strv_contains(mgd_attribute, attr->name)){
				g_warning("Wrong attribute '%s' in '%s' on line %ld",
						attr->name, parsed_schema, xmlGetLineNo(node));	
			}	
					
			if (g_str_equal(attr->name, "unique")) {

				attrval = xmlGetProp(node, (const xmlChar*)"unique");

				if (attrval && g_str_equal(attrval, "yes")) {
					
					if (prop_attr->gtype != MGD_TYPE_STRING) 
						__warn_msg(node, "Unique name must be declared as string type");

					gchar *propname = (gchar *)xmlGetProp(node, (const xmlChar *)"name");
					type_attr->unique_name = g_strdup(propname);
					g_free(propname);
				}

				if (attrval != NULL)
					xmlFree(attrval);
			}
	
			attr = attr->next;			
		}
	}
}

/* Get property attributes */
static void 
_get_property_attributes(xmlNode * node, 
		MgdSchemaTypeAttr *type_attr, MgdSchemaPropertyAttr *prop_attr)
{
	g_return_if_fail (node != NULL);
	g_return_if_fail (type_attr != NULL);
	g_return_if_fail (prop_attr != NULL);

	xmlAttr *attr;
	xmlChar *attrval = NULL;
	
	/* Validate attributes. Throw warning if there's invalid one. */
	attr = node->properties;
	attr = attr->next;

	while (attr != NULL) {
		attrval = xmlNodeListGetString (node->doc, attr->children, 1);
		if (!strv_contains(mgd_attribute, attr->name)) {
			g_warning ("Wrong attribute '%s' in '%s' on line %ld",
					attr->name, parsed_schema, xmlGetLineNo(node));
		}
		attr = attr->next;
	}
	
	/* dbtype */	
	attrval = xmlGetProp (node, (const xmlChar *)"dbtype");
	if (attrval) {
		prop_attr->dbtype = g_strdup((gchar *)attrval);
		xmlFree (attrval);
	}

	/* field */
	attrval = xmlGetProp (node, (const xmlChar *)PROP_RW_FIELD);
	if (attrval) {

		/* Check if column name is reserved one */
		check_metadata_column (attrval);
		if (strv_contains (rcolumns, attrval)) {
			g_critical("'%s' is reserved column name", attrval);
		} 
		prop_attr->field = g_strdup ((gchar *)attrval);
		xmlFree (attrval);
	}
		
	/* index */
	attrval = xmlGetProp (node, (const xmlChar *)PROP_RW_INDEX);
	if (attrval) {
		if (g_str_equal(attrval, "yes") || g_str_equal (attrval, "true"))
			prop_attr->dbindex = TRUE;
		xmlFree (attrval);
	}
			
	/* table, backward compatibility */
	attrval = xmlGetProp (node, (const xmlChar *)TYPE_RW_TABLE);
	if (attrval) {
		/* Disable warning, we can not use debug level */
		/* __warn_msg (node, "'table' attribute not supported"); */
		xmlFree (attrval);
	}

	/* upfield */
	attrval = xmlGetProp (node, (const xmlChar *)PROP_RW_UPFIELD);
	if (attrval) {
		check_metadata_column (attrval);
		/* Check if column name is reserved one */
		if (strv_contains (rcolumns, attrval)) {
			g_critical("'%s' is reserved column name", attrval);
		}				

		if(!type_attr->property_up) {
			xmlChar *tmpattr = xmlGetProp (node, (const xmlChar *)"name");
			type_attr->property_up = g_strdup((gchar *)tmpattr);
			type_attr->upfield = g_strdup((gchar *)attrval);
			if (!midgard_core_object_prop_up_is_valid(prop_attr->gtype)) {
				__warn_msg(node, "Invalid type for up property.");
				g_error("Wrong schema attribute");
			}
			xmlFree(tmpattr);

		} else {
			__warn_msg(node, "Upfield redefined!");
		}

		prop_attr->upfield = g_strdup((gchar *)attrval);		
		xmlFree (attrval);
	}

	/* parentfield */
	attrval = xmlGetProp (node, (const xmlChar *)PROP_RW_PARENTFIELD);
	if (attrval) {
		check_metadata_column (attrval);
		/* Check if column name is reserved one */
		if (strv_contains (rcolumns, attrval)) {
			g_critical ("'%s' is reserved column name", attrval);
		}

		if (!type_attr->property_parent) {
			xmlChar *tmpattr = xmlGetProp (node, (const xmlChar *)"name");
			type_attr->property_parent = g_strdup ((gchar *)tmpattr);
			type_attr->parentfield = g_strdup ((gchar *)attrval);
			if (!midgard_core_object_prop_parent_is_valid (prop_attr->gtype)) {
				__warn_msg (node, "Invalid type for parent property.");
				g_error ("Wrong schema attribute");
			}
			xmlFree(tmpattr);
		} else {                         
			__warn_msg(node, "Parentfield redefined!");
		}

		if (g_str_equal (type_attr->upfield, type_attr->parentfield)) {
			__warn_msg (node, "Parentfield and upfield is the same property");
			g_error ("Wrong schema attribute");
		}

		prop_attr->parentfield = g_strdup ((gchar *)attrval);
		xmlFree (attrval);
	}

	/* multilang, backward compatibility */
	attrval = xmlGetProp (node, (const xmlChar *)"multilang");
	if (attrval) {
		/* Disable warning, we can not use debug level */
		/* __warn_msg (node, "'multilang' attribute not supported"); */
		xmlFree (attrval);
	}

	/* primaryfield */
	attrval = xmlGetProp (node, (const xmlChar *)PROP_RW_PRIMARY);
	if (attrval) {
		check_metadata_column (attrval);
		
		/* Check if column name is reserved one */
		if (strv_contains (rcolumns, attrval)) 
			g_critical ("'%s' is reserved column name",attrval);

		prop_attr->primaryfield = g_strdup ((gchar *)attrval);
		prop_attr->is_primary = TRUE;
		xmlChar *tmpattr = xmlGetProp (node, (const xmlChar *)"name");
		type_attr->primary = g_strdup ((gchar *)tmpattr);
		type_attr->primaryfield = g_strdup ((gchar *)attrval);
		xmlFree (tmpattr);
		xmlFree (attrval);
	}

	/* reverse */
	attrval = xmlGetProp (node, (const xmlChar *)PROP_RW_REVERSE);
	if (attrval ) {
		if (g_str_equal (attrval, "yes") || g_str_equal (attrval, "true"))
			prop_attr->is_reversed = TRUE;	
			
		xmlFree (attrval);
	}
		
	/* link */	
	attrval = xmlGetProp (node, (const xmlChar *)PROP_RW_LINK);
	if (attrval) {
		if(!midgard_core_object_prop_link_is_valid(prop_attr->gtype)) {
			__warn_msg(node, "Invalid type for link property.");
			g_error("Wrong schema attribute");
		}

		gchar **link = g_strsplit((gchar *)attrval, ":", -1);
		prop_attr->link = g_strdup((gchar *)link[0]);
		prop_attr->is_link = TRUE;
		
		if(link[1])
			prop_attr->link_target = g_strdup((gchar *)link[1]);
		else 
			prop_attr->link_target = g_strdup("guid");
		
		g_strfreev(link);
		xmlFree (attrval);
	}

	/* private */
	attrval = xmlGetProp (node, (const xmlChar *)PROP_RW_PRIVATE);
	if (attrval) {
		if (g_str_equal (attrval, "yes") || g_str_equal (attrval, "true"))
			prop_attr->is_private = TRUE;
		xmlFree (attrval);
	}
}

static void
_get_user_fields (xmlNode *node, MgdSchemaPropertyAttr *prop_attr)
{
	xmlNode *snode = NULL;	

	for (snode = node->children; snode != NULL; snode = snode->next) {
		
		if (snode->type == XML_ELEMENT_NODE){

			xmlParserCtxtPtr parser = xmlNewParserCtxt();
			gchar *value = (gchar *)xmlNodeGetContent(snode);

			xmlChar *decoded =
				xmlStringDecodeEntities(parser,	(const xmlChar *) value,
							XML_SUBSTITUTE_REF, 0, 0, 0);
		
			if (g_str_equal (snode->name, "description")) {

				g_free (prop_attr->description); /* Explicitly free, it's initialized with new empty string by default */
				prop_attr->description = g_strdup((gchar *)decoded);

			} else {	
				
				g_hash_table_insert(prop_attr->user_values, g_strdup((gchar *)snode->name), g_strdup((gchar *)decoded));
			}

			g_free(decoded);
			xmlFreeParserCtxt(parser);
			g_free(value);
		}
	}
}

static void
__get_type_user_values (xmlNode *node, MgdSchemaTypeAttr *type_attr)
{
	xmlNode *snode = NULL;	
	xmlNode *user_value = NULL;

	for (snode = node; snode != NULL; snode = snode->next) {
		
		if (snode->type != XML_ELEMENT_NODE)
			continue;

		if (g_str_equal (snode->name, TYPE_RW_USERVALUES)) {

			for (user_value = snode->children; user_value != NULL; user_value = user_value->next) {

				if (user_value->type != XML_ELEMENT_NODE)
					continue;
	
				xmlParserCtxtPtr parser = xmlNewParserCtxt();
				gchar *value = (gchar *)xmlNodeGetContent(user_value);	
				xmlChar *decoded =
					xmlStringDecodeEntities (parser,	(const xmlChar *) value,
							XML_SUBSTITUTE_REF, 0, 0, 0);

				g_hash_table_insert (type_attr->user_values, 
						g_strdup ((gchar *)user_value->name), g_strdup ((gchar *)decoded));

				g_free (decoded);
				xmlFreeParserCtxt (parser);
				g_free (value);
			}
		}
	}
}

static void 
__get_properties (xmlNode *curn, MgdSchemaTypeAttr *type_attr, MidgardSchema *schema)
{
	xmlNode *node = NULL;
	xmlChar *nv = NULL;	
	MgdSchemaPropertyAttr *prop_attr;
	gchar *tmpstr;

	for (node = curn; node != NULL; node = node->next){
		
		if (node->type != XML_ELEMENT_NODE)
			continue;

		if (!strv_contains (mgd_complextype, node->name)) {
			g_warning ("Wrong node name '%s' in '%s' on line %ld",
					node->name, parsed_schema, xmlGetLineNo(node));
		}

		if (g_str_equal (node->name, TYPE_RW_PROPERTY)) {
				
			nv = xmlGetProp (node, (const xmlChar *) TYPE_RW_NAME);
			check_property_name (nv, type_attr->name);
			
			prop_attr = g_hash_table_lookup (type_attr->prophash, (gchar *)nv);
				
			if (prop_attr != NULL) {

				__warn_msg (node, "Invalid property");
				g_error ("Property '%s' already added to %s", nv, type_attr->name);
			}

			prop_attr = midgard_core_schema_type_property_attr_new();
			prop_attr->name = g_strdup ((gchar *)nv);
			midgard_core_schema_get_property_type (node, type_attr, prop_attr);
			midgard_core_schema_get_default_value (node, type_attr, prop_attr);
			midgard_core_schema_get_unique_name (node, type_attr, prop_attr);
			_get_user_fields (node, prop_attr);
			_get_property_attributes (node, type_attr, prop_attr);

			if(prop_attr->is_primary 
					&& prop_attr->gtype != MGD_TYPE_UINT) {

				tmpstr = (gchar *)xmlGetProp(node->parent, (const xmlChar *)"name");
				g_message(" %s - type for primaryfield not defined" 
						" as 'unsigned integer'. Forcing uint type", tmpstr);
				g_free(tmpstr);
			}

			gchar *property_name = g_strdup ((gchar *)nv);
			g_hash_table_insert (type_attr->prophash, property_name, prop_attr);
			/* Workaround.
			 * _properties_list is only properties' holder.
			 * It just holds properties in the same order we declared them in MgdSchema file */ 
			type_attr->_properties_list = g_slist_append (type_attr->_properties_list, property_name);

			xmlFree (nv);
		}				
	}
}

static void
_get_element_names (xmlNode *curn , MgdSchemaTypeAttr *type_attr, MidgardSchema *schema)
{
	xmlNode *obj = NULL;
	xmlChar *nv = NULL;
	MgdSchemaTypeAttr *duplicate;

	for (obj = curn->children; obj != NULL; obj = obj->next) {

		if (obj->type != XML_ELEMENT_NODE)
			continue;

		if (!strv_contains(mgd_complextype, obj->name)) {

			g_warning("Wrong node name '%s' in '%s' on line %ld",
					obj->name, parsed_schema, xmlGetLineNo(obj));
		}

		if (!g_str_equal(obj->name, TYPE_RW_TYPE))
			continue;

		/* Get type name */
		nv = xmlGetProp (obj, (const xmlChar *) TYPE_RW_NAME); 

		/* Check if type name is reserved one */
		if (strv_contains(rtypes, nv)) {

			__warn_msg(obj, "Invalid type name");
			g_error("'%s' is reserved type name", nv);
		}
				
		/* Check for duplicates */
		duplicate = midgard_schema_lookup_type(schema, (gchar *)nv);
					
		if (duplicate != NULL) {

			__warn_msg(obj, "Invalid type name");
			g_error("%s:%s already added to schema!", obj->name, nv);
		}

		type_attr = midgard_core_schema_type_attr_new();
		type_attr->name = g_strdup((gchar *)nv);
		g_hash_table_insert(schema->types, g_strdup((gchar *)nv), type_attr);
		_get_type_attributes (obj, type_attr, schema);

		if (obj->children != NULL) {

			__get_properties(obj->children, type_attr, schema);
			__get_type_user_values (obj->children, type_attr);
		}

		xmlFree(nv);
	}
}

/* Start processing type's properties data. 
 * This function is called from __get_tdata_foreach 
 */
void __get_pdata_foreach(gpointer key, gpointer value, gpointer user_data)
{
	MgdSchemaTypeAttr *type_attr  = (MgdSchemaTypeAttr *) user_data;  
	MgdSchemaPropertyAttr *prop_attr = (MgdSchemaPropertyAttr *) value;
	const gchar *primary;
	const gchar *pname = (gchar *) key;
	const gchar *upfield = NULL, *parentfield = NULL;
	const gchar *fname = NULL;
	gchar *table = NULL;
	
	table = g_strdup (prop_attr->table);

	upfield = prop_attr->upfield;
	parentfield = prop_attr->parentfield;
	primary = prop_attr->primaryfield;
	
	if (table != NULL){
		
		fname = prop_attr->field; 
		midgard_core_schema_type_property_set_tablefield (prop_attr, table, fname ? fname : pname);
		midgard_core_schema_type_add_table(type_attr, table);
	}

	/* FIXME , this needs to be refactored for real property2field mapping */
	if(!type_attr->primaryfield)
		type_attr->primaryfield = g_strdup("guid");

	if(!type_attr->primary){
		type_attr->primary = g_strdup("guid");
	}	

	if(primary != NULL && table != NULL) 	
		midgard_core_schema_type_property_set_tablefield(prop_attr, table, primary);

	if(upfield != NULL && table != NULL)
		midgard_core_schema_type_property_set_tablefield(prop_attr, table, upfield);
	
	if(parentfield != NULL && table != NULL) 
		midgard_core_schema_type_property_set_tablefield(prop_attr, table, parentfield);

	if(prop_attr->field == NULL)
		prop_attr->field = g_strdup(pname);

	/* Force G_TYPE_UINT type for primaryfield */
	if((prop_attr->is_primary) && (prop_attr->gtype != G_TYPE_UINT))
		prop_attr->gtype = G_TYPE_UINT;

	g_free (table);
}

/* Get types' data. Type's name and its values.
 * We can not start creating GParamSpec while we parse xml file.
 * This is internally used with _register_types function. At this 
 * moment we can count all properties of type and call sizeof
 * with correct values
 */ 
void __get_tdata_foreach(gpointer key, gpointer value, gpointer user_data)
{	
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *) value;
	guint np;		
	
	if(g_type_from_name(key))
		return;
	
	np = g_hash_table_size(type_attr->prophash);

	if (np > 0) {

		if (type_attr->table)
			g_hash_table_foreach(type_attr->prophash, __get_pdata_foreach, type_attr);
		
	} else {

		g_warning("Type %s has less than 1 property!", (gchar *)key);
	}
}

/* Copy source schema types ( class names ) and its MgdSchemaType structures
 * to destination schema */
void _copy_schemas(gpointer key, gpointer value, gpointer userdata)
{
	gchar *typename = (gchar *)key;
	MgdSchemaTypeAttr *type = (MgdSchemaTypeAttr *)value;
	MgdSchema *ts = (MgdSchema *) userdata;

	if(!g_hash_table_lookup(ts->types, typename)) {
		
		if(type != NULL)
			g_hash_table_insert(ts->types, g_strdup(typename), type); 
	}
}

/* We CAN NOT define some data during __get_tdata_foreach.
 * parent and children relation ( for example ) must be done AFTER
 * all types are registered and all types internal structures are
 * already initialized and defined
 */ 
void __postconfig_schema_foreach(gpointer key, gpointer value, gpointer userdata)
{
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *) value;
	MgdSchemaTypeAttr *parenttype = NULL;
	MidgardSchema *schema = (MidgardSchema *) userdata;
	gchar *typename = (gchar *) key;
	const gchar *parentname;

	parentname = type_attr->parent;
	
	if (parentname  != NULL ){
	
		/* WARNING!! , all types which has parenttype defined will be appended to 
		 * this list. It doesn't matter if they are "registered" in such schema.
		 * Every GObjectClass has only one corresponding C structure , so it is impossible
		 * to initialize new MgdSchemaTypeAttr per every classname 
		 * in every schema and keep them  separated for every GObjectClass , 
		 * as we define only one GType.
		 * It may be resolved by expensive hash lookups for every new object instance.
		 */

		if ((parenttype = midgard_schema_lookup_type(schema, (gchar *)parentname)) != NULL){
			// g_debug("type %s, parent %s", typename, parentname); 
			if (!g_slist_find(parenttype->children, (gpointer) g_strdup(typename))) {
				parenttype->children = 
					g_slist_append(parenttype->children, (gpointer) g_strdup(typename));
			}			
		} 
	}		
}

static const char *NAMESPACE = "http://www.midgard-project.org/repligard/1.4";

/* Forward declarations used by read_schema_path() */
static void read_schema_directory(GHashTable *files, const char *directory);
static void read_schema_file(GHashTable *files, const char *file);

static void read_schema_path(GHashTable *files, const char *path) {
        g_assert(files != NULL);
        g_assert(path != NULL);
        if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
                read_schema_directory(files, path);
        } else if (g_file_test(path, G_FILE_TEST_EXISTS)) {
                read_schema_file(files, path);
        } else {
                g_warning("Schema path %s not found", path);
        }
}

/*
 * \internal
 * Reads the schema file from the given path to the given schema hash table.
 * The file is parsed as an XML file and the resulting xmlDocPtr is inserted
 * to the schema hash table with the file path as the key. Both the XML
 * document and the file path are newly allocated and must be freed when
 * the hash table is destroyed. Logs a warning and returns if the file
 * has already been included in the hash table or if it cannot be parsed.
 * Any includes within the parsed file are processed recursively.
 */
static void read_schema_file(GHashTable *files, const char *file) {
        g_assert(files != NULL);
        g_assert(file != NULL);

        /* Guard against duplicate loading of schema files */
        if (g_hash_table_lookup(files, file) != NULL) {
                g_warning("Skipping already seen schema file %s", file);
                return;
        }

        /* Parse this schema file */
        /* g_debug("Reading schema file %s", file); */
        xmlDocPtr doc = xmlParseFile(file);
        if (doc == NULL) {
                g_warning("Skipping malformed schema file %s", file);
                return;
        }
        xmlNodePtr root = xmlDocGetRootElement(doc);
        if (root == NULL
            || root->ns == NULL
            || !g_str_equal(root->ns->href, NAMESPACE)
            || !g_str_equal(root->name, "Schema")) {
                g_warning("Skipping invalid schema file %s", file);
                xmlFreeDoc(doc);
                return;
        }

        /* Add the schema file to the hash table */
        g_hash_table_insert(files, g_strdup(file), doc);

        /* Read all included schema files */
        xmlNodePtr node = root->children;
        while (node != NULL) {
                xmlChar *attr = xmlGetNoNsProp(node, (xmlChar *) "name");
                if (node->type == XML_ELEMENT_NODE
                    && node->ns != NULL
                    && g_str_equal(node->ns->href, NAMESPACE)
                    && g_str_equal(node->name, "include")
                    && attr != NULL) {
                        GError *error = NULL;
                        gchar *name = g_filename_from_utf8(
                                (const gchar *) attr, -1, NULL, NULL, &error);
                        if (name == NULL) {
                                g_warning("g_filename_from_utf8: %s", error->message);
                                g_error_free(error);
                        } else if  (g_path_is_absolute(name)) {
                                read_schema_path(files, name);
                                g_free(name);
                        } else {
                                gchar *dir = g_path_get_dirname(file);
                                gchar *path = g_build_filename(dir, name, NULL);
                                read_schema_path(files, path);
                                g_free(path);
                                g_free(dir);
                                g_free(name);
                        }
                }
                if (attr != NULL) {
                        xmlFree(attr);
                }
                node = node->next;
        }
}

/*
 * \internal
 * Reads the schema directory from the given path to the given schema
 * hash table. Ignores all hidden files and subdirectories, and recurses
 * into normal subdirectories. All files with the suffix ".xml" are read
 * and inserted into the schema hash table.
 */
static void read_schema_directory(GHashTable *files, const char *directory) {
        g_assert(files != NULL);
        g_assert(directory != NULL);

        GError *error = NULL;
        GDir *dir = g_dir_open(directory, 0, &error);
        if (dir != NULL) {
                const gchar *file = g_dir_read_name(dir);
                while (file != NULL) {
                        gchar *path = g_build_filename(directory, file, NULL);
                        if (g_str_has_prefix(file, ".")) {
                                /* skip hidden files and directories */
                        } else if (g_str_has_prefix(file, "#")) {
                                /* skip backup files and directories */
                        } else if (!g_file_test(path, G_FILE_TEST_IS_DIR)) {
                                /* recurse into subdirectories */
                                read_schema_directory(files, path);
                        } else if (g_str_has_suffix(file, ".xml")) {
                                /* read xml file, guaranteed to exist */
                                read_schema_file(files, path);
                        }
                        g_free(path);
                        file = g_dir_read_name(dir);
                }
                g_dir_close(dir);
        } else {
                g_warning("g_dir_open: %s", error->message);
                g_error_free(error);
        }
}

/*
 * \internal
 * Reads all the schema files identified by the given path and any includes
 * within the schema files. Returns the fiels as a newly allocated hash
 * table with the schema file paths as keys and the parsed XML documents
 * (xmlDocPtr) as values. The returned hash table contains content destroy
 * functions so the caller can just use g_hash_table_destroy() to release
 * all memory associated with the returned hash table.
 */
static GHashTable *read_schema_files(const char *path) {
        g_assert(path != NULL);
        GHashTable *files = g_hash_table_new_full(
                g_str_hash, g_str_equal, g_free, (GDestroyNotify) xmlFreeDoc);
        read_schema_path(files, path);
        return files;
}

static void parse_schema(gpointer key, gpointer value, gpointer user_data) {
        g_assert(key != NULL);
        g_assert(value != NULL);
        g_assert(user_data != NULL);
	parsed_schema = (const gchar *)key;

        MidgardSchema *schema = (MidgardSchema *) user_data;
        xmlDocPtr doc = (xmlDocPtr) value;
        xmlNodePtr root = xmlDocGetRootElement(doc);
        _get_element_names(root, NULL, schema);
}

/* API functions */

/* Initializes basic Midgard classes */
/**
 * midgard_schema_init:
 * @self: #MidgardSchema instance
 * @path: full path to a xml file with common classes
 *
 * Reads xml file which defines very basic and common classes.
 * By default it's `/usr/local/share/midgard/MgdObjects.xml` file.
 */
void midgard_schema_init(MidgardSchema *self, const gchar *path)
{	
	g_assert(self != NULL);

	const gchar *lpath = path;

	if (path == NULL) {
		
		lpath = MIDGARD_GLOBAL_SCHEMA;
		const gchar *env_schema = g_getenv("MIDGARD_ENV_GLOBAL_SCHEMA");
		if (env_schema != NULL)
			lpath = env_schema;
	}

	if (g_file_test(lpath, G_FILE_TEST_IS_DIR)) {

		g_warning("Common MgdObjects.xml path is a directory");
		return;
	}
	
	if (!g_file_test(lpath, G_FILE_TEST_EXISTS)) {

		g_warning("Common MgdObjects.xml file (%s) doesn't exist.", lpath);
		return;
	}

	midgard_schema_read_file(self, lpath);
}

/* Checks if classname is registsred for schema. */
/**
 * midgard_schema_type_exists:
 * @self: #MidgardSchema instance
 * @classname: #GObjectClass derived class name
 *
 * Returns: %TRUE if class is registered as #MidgardObjectClass derived, %FALSE otherwise
 */
gboolean midgard_schema_type_exists(MidgardSchema *self, const gchar *classname)
{
	g_assert(self != NULL);
	g_assert(classname != NULL);
	
	if(g_hash_table_lookup(self->types, classname))
		return TRUE;

	return FALSE;
}

/**
 * midgard_schema_read_file:
 * @self: #MidgardSchema instance
 * @filename: full path to a file
 *
 * Reads file at given path and initialize all #MidgardObjectClass derived classes defined
 * in this file. Also reads all files which are included in this file.
 * Such files are read and parsed when given file is already parsed and all classes
 * defined in given file are already registered in GType system.
 */
void midgard_schema_read_file(
		MidgardSchema *self, const gchar *filename) 
{
	MidgardSchema *schema = self;

	g_assert(schema != NULL);
        g_assert(filename != NULL);

        GHashTable *files = read_schema_files(filename);
        g_hash_table_foreach(files, parse_schema, schema);
        g_hash_table_destroy(files);

        /* register types */
        g_hash_table_foreach(schema->types, __get_tdata_foreach, schema);
    
        /* Do postconfig for every initialized schema */
        g_hash_table_foreach(schema->types, __postconfig_schema_foreach, schema);
}

static void __is_linked_valid(MidgardDBObjectClass *klass, const gchar *prop)
{
	MidgardReflectionProperty *smrp = NULL;
	MidgardReflectionProperty *dmrp = NULL;
	smrp = midgard_reflection_property_new(klass);
	const gchar *typename = G_OBJECT_CLASS_NAME(klass);

	if(!midgard_reflection_property_is_link(smrp, prop)) 
		return;

	const MidgardDBObjectClass *pklass = midgard_reflection_property_get_link_class(smrp, prop);
	
	if(!pklass)		
		g_error("Can not get link class for %s.%s", typename, prop);
				
	const gchar *prop_link = midgard_reflection_property_get_link_target(smrp, prop);
	
	if(!prop_link) 
		g_error("Can not get link for %s.%s", typename, prop);

	/* We need to set is_linked for linked property. It can not be done when schema is parsed because
	 * linked class might be not registered in that phase */
	MgdSchemaPropertyAttr *prop_attr =
		g_hash_table_lookup(pklass->dbpriv->storage_data->prophash, prop_link);

	if (prop_attr == NULL) {
		
		g_warning("Couldn't find property attribute for %s.%s",
				G_OBJECT_CLASS_NAME(pklass), prop_link);
	} else {
		
		prop_attr->is_linked = TRUE;
	}

	GType src_type = midgard_reflection_property_get_midgard_type(smrp, prop);
	dmrp = midgard_reflection_property_new(MIDGARD_DBOBJECT_CLASS(pklass));
	GType dst_type = midgard_reflection_property_get_midgard_type(dmrp, prop_link);
	
	if(src_type != dst_type)
		g_error("Mismatched references' types: %s.%s type != %s.%s type",
				typename, prop, G_OBJECT_CLASS_NAME(pklass), prop_link);

	return;
}

static void __midgard_schema_validate()
{
	guint n_types, i;
	guint n_prop = 0;
	guint j = 0;
	const gchar *typename, *parentname;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT, &n_types);
	MidgardObjectClass *klass = NULL;
	MidgardObjectClass *pklass = NULL;
	
	for (i = 0; i < n_types; i++) {

		typename = g_type_name(all_types[i]);
		klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(typename);

		GParamSpec **pspecs = g_object_class_list_properties(G_OBJECT_CLASS(klass), &n_prop);

		if(n_prop == 0)
			continue;

		for(j = 0; j < n_prop; j++) {

			__is_linked_valid(MIDGARD_DBOBJECT_CLASS(klass), pspecs[j]->name);
		}

		g_free(pspecs);

		parentname = klass->dbpriv->storage_data->parent;

		if(parentname != NULL) {
			
			/* validate tree parent class */
			pklass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(parentname);
		
			if(pklass == NULL) {
				
				g_error("Parent %s for %s class is not registered in GType system",
						parentname, typename);
			}
		
			/* validate parent property */
			const gchar *parent_property = midgard_object_class_get_property_parent(klass);
			
			if(parent_property == NULL) {
				
				g_error("Parent property missed for %s class. %s declared as tree parent class",
						typename, parentname);
			}

			/* validate parent property if exists */
			const gchar *prop = midgard_object_class_get_property_parent(klass);	
			MidgardReflectionProperty *smrp = NULL;
	
			if(prop) {
		
				smrp = midgard_reflection_property_new(MIDGARD_DBOBJECT_CLASS(klass));
				if(!midgard_reflection_property_is_link(smrp, prop)) 
					g_error("Parent property %s of class %s is not defined as link", prop, typename);
			
				__is_linked_valid(MIDGARD_DBOBJECT_CLASS(klass), prop);

				g_object_unref(smrp);	
			}
			
			/* validate up property link */
			prop = midgard_object_class_get_property_up(klass);
			
			if(prop) {
				
				smrp = midgard_reflection_property_new(MIDGARD_DBOBJECT_CLASS(klass));
				if(!midgard_reflection_property_is_link(smrp, prop)) 
					g_error("Up property %s of class %s is not defined as link", prop, typename);
			
				__is_linked_valid(MIDGARD_DBOBJECT_CLASS(klass), prop);

				g_object_unref(smrp);
			}
		}
	}

	g_free(all_types);
}

static void __extend_type_attr(gpointer key, gpointer val, gpointer userdata)
{	
	MgdSchemaPropertyAttr *parent_attr = (MgdSchemaPropertyAttr *) val;
	MgdSchemaTypeAttr *type = (MgdSchemaTypeAttr *) userdata;

	/* Look for property registered for child type. If it is registered we silently
	   return. Child type may use different field for inherited property name */
	MgdSchemaPropertyAttr *prop_attr = g_hash_table_lookup(type->prophash, key);

	if (prop_attr) 
		return;
	
	midgard_core_schema_type_property_copy(parent_attr, type);
}

static void __extend_type_foreach(gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *) val;
	MidgardSchema *schema = (MidgardSchema*) userdata;

	if (val == NULL) return;
	if (type_attr->extends == NULL) return;

	MgdSchemaTypeAttr *parent_type_attr = 
		midgard_schema_lookup_type(schema, type_attr->extends);

	if (!parent_type_attr)
		g_error("Type information for %s (%s's parent) not found", 
				type_attr->extends, type_attr->name);

	/* Use parent's storage */
	type_attr->table = g_strdup(parent_type_attr->table);
	type_attr->tables = g_strdup(parent_type_attr->tables);

	g_hash_table_foreach(parent_type_attr->prophash, __extend_type_attr, type_attr);	
}

static void __copy_type_attr(gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaPropertyAttr *parent_attr = (MgdSchemaPropertyAttr *) val;
	MgdSchemaTypeAttr *type = (MgdSchemaTypeAttr *) userdata;

	/* Look for property registered for child type. If it is registered we silently
	   return. Child type may use different field for inherited property name */
	MgdSchemaPropertyAttr *prop_attr = g_hash_table_lookup(type->prophash, key);

	if (prop_attr) 
		return;
	
	midgard_core_schema_type_property_copy(parent_attr, type);

	prop_attr = g_hash_table_lookup(type->prophash, key);

	/* redefine tablefield */
	if (type->table)
		midgard_core_schema_type_property_set_tablefield(prop_attr, type->table, prop_attr->field);
}

static void __copy_type_foreach(gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *) val;
	MidgardSchema *schema = (MidgardSchema*) userdata;

	gchar *copied = type_attr->copy_from;

	if (val == NULL) return;
	if (copied == NULL) return;

	MgdSchemaTypeAttr *parent_type_attr = 
		midgard_schema_lookup_type(schema, copied);

	if (!parent_type_attr)
		g_error("Type information for %s (%s's parent) not found", 
				copied, type_attr->name);

	g_hash_table_foreach(parent_type_attr->prophash, __copy_type_attr, type_attr);	
}

static void __register_schema_type (gpointer key, gpointer val, gpointer user_data)
{
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *) val;

	if (val == NULL) return;
	if (type_attr->params == NULL) {

		g_warning("No parameters found for %s schema type. Not registering.", type_attr->name);
		return;
	}

	GType new_type;
	new_type = midgard_type_register(type_attr, MIDGARD_TYPE_OBJECT);

	if (new_type) {

		GObject *foo = g_object_new(new_type, NULL);
		/* Set number of properties.
		 * This way we gain performance for instance_init call */
		GParamSpec **pspecs = 
			g_object_class_list_properties(
					G_OBJECT_GET_CLASS(G_OBJECT(foo)), 
					&type_attr->class_nprop);
		g_free(pspecs);
		g_object_unref(foo); 
	}
}

static void __build_static_sql (gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *)val;
	midgard_core_schema_type_build_static_sql (type_attr);
}

static void __initialize_paramspec (gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *)val;
	midgard_core_schema_type_initialize_paramspec (type_attr);
}

static void __validate_fields (gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaTypeAttr *type_attr = (MgdSchemaTypeAttr *)val;
	midgard_core_schema_type_validate_fields (type_attr);
}

static void __midgard_schema_postconfig(MidgardSchema *self)
{
	g_assert(self != NULL);

	/* TODO, validate metadata class, once we support user defined one */
	g_hash_table_foreach (self->types, __extend_type_foreach, self);
	g_hash_table_foreach (self->types, __copy_type_foreach, self);
	g_hash_table_foreach (self->types, __initialize_paramspec, NULL);
	g_hash_table_foreach (self->types, __build_static_sql, NULL);
	g_hash_table_foreach (self->types, __validate_fields, NULL);
	g_hash_table_foreach (self->types, __register_schema_type, NULL);
}

/**
 * midgard_schema_read_dir:
 * @self: #MidgardSchema instance
 * @dirname: a directory with 'schema' and 'views' subdirectories. 
 *
 * This function expects 'schema' subdirectory at least.
 * 'views' subdirectory is optional, and if missed, won't be read.
 * midgard_schema_read_file() is invoked for every valid xml MgdSchema file 
 * found in 'schema' subdirectory found in  given directory.
 * 
 * You can also use explicit NULL instead of directory path.
 * In such case, MIDGARD_ENV_GLOBAL_SHAREDIR environment variable is checked.
 * If it's not set, directory path is determined using prefix set for compile 
 * and build time.
 *
 * Returns: %TRUE if files has been read, %FALSE otherwise
 */
gboolean midgard_schema_read_dir(
		MidgardSchema *self, const gchar *dirname)
{
	const gchar *fname = NULL;
	gchar *fpfname = NULL ;
	GDir *dir;
	gint visible = 0;
	gchar *lschema_dir = midgard_core_config_build_schemadir_path(dirname);
	gchar *lview_dir = midgard_core_config_build_viewdir_path(dirname);

	if (!lschema_dir)
		return FALSE;

	dir = g_dir_open(lschema_dir, 0, NULL);
	
	if (dir != NULL) {
		
		while ((fname = g_dir_read_name(dir)) != NULL) {

			visible = 1;
			fpfname = g_strconcat(lschema_dir, "/", fname, NULL);
			
			/* Get files only with xml extension, 
			 * swap files with '~' suffix will be ignored */
			if (!g_file_test (fpfname, G_FILE_TEST_IS_DIR) 
					&& g_str_has_suffix(fname, ".xml")){
				
				/* Hide hidden files */
				if(g_str_has_prefix(fname, "."))
					visible = 0;
				
				/* Hide recovery files if such exist */
				if(g_str_has_prefix(fname, "#"))
					visible = 0;
				
				if ( visible == 0)
					g_warning("File %s ignored!", fpfname);
				
				if(visible == 1){  					
					/* FIXME, use fpath here */
					midgard_schema_read_file(self, fpfname);
				}
			}
			g_free(fpfname);
			
			/* Do not free ( or change ) fname. 
			 * Glib itself is responsible for data returned from g_dir_read_name */
		}

		/* post parsing routines */
		__midgard_schema_postconfig(self);

		/* validate */
		__midgard_schema_validate();

		g_dir_close(dir);
		
		if (lschema_dir)
			g_free(lschema_dir);

		/* Optionally read view directory and initialize every midgard_view derived class */
		if (lview_dir != NULL) {

			midgard_core_views_read_dir(lview_dir);
			g_free(lview_dir);
		}

		return TRUE;
	}

	if (lschema_dir)
		g_free(lschema_dir);

	if (lview_dir)
		g_free(lview_dir);

	return FALSE;
}

/* SCHEMA DESTRUCTORS */

/* Free type names, collect all MgdSchemaTypeAttr pointers */
static void _get_schema_trash(gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaTypeAttr *stype = (MgdSchemaTypeAttr *) val;
	
	/* Collect types data pointers*/
	if(!g_slist_find(schema_trash, stype)) {
		/* g_debug("Adding %s type to trash", (gchar *)key); */ 
		schema_trash = g_slist_append(schema_trash, stype);
	}
}

/* GOBJECT AND CLASS */

/* Create new object instance */
static void 
_schema_instance_init 
(GTypeInstance *instance, gpointer g_class)
{
	MidgardSchema *self = (MidgardSchema *) instance;
	self->types = g_hash_table_new (g_str_hash, g_str_equal);
}

/* Finalize  */
static void 
_midgard_schema_finalize (GObject *object)
{
	g_assert (object != NULL); /* just in case */
	MidgardSchema *self = (MidgardSchema *) object;	

	g_hash_table_foreach (self->types, _get_schema_trash, NULL);
	g_hash_table_destroy (self->types);
	
	for ( ; schema_trash ; schema_trash = schema_trash->next){
		midgard_core_schema_type_attr_free ((MgdSchemaTypeAttr *) schema_trash->data);
	}

	g_slist_free (schema_trash);					
}

/* Initialize class */
static void 
_midgard_schema_class_init (gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardSchemaClass *klass = MIDGARD_SCHEMA_CLASS (g_class);
	
	gobject_class->set_property = NULL;
	gobject_class->get_property = NULL;
	gobject_class->finalize = _midgard_schema_finalize;
	
	klass->init = midgard_schema_init;
	klass->read_dir = midgard_schema_read_dir;
	klass->read_file = midgard_schema_read_file;
	klass->type_exists = midgard_schema_type_exists;
}

/* Register MidgardSchema type */
GType
midgard_schema_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardSchemaClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_schema_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSchema),
			0,              /* n_preallocs */
			_schema_instance_init    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT,
				"midgard_schema",
				&info, 0);
	}
	return type;
}
