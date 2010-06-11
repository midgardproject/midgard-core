/*  
  Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
  
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
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "midgard_datatypes.h"
#include "midgard_type.h"
#include "schema.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include <libxml/parserInternals.h>
#include "guid.h"
#include "midgard_core_xml.h"
#include "midgard_core_query.h"
#include "query_constraint.h"
#include "midgard_core_views.h"
#include "midgard_metadata.h"

#define MGD_VIEW_RW_VIEW "view"
#define MGD_VIEW_RW_NAME "name"

static gchar *parsed_view = NULL;

static void __view_error(xmlNode *node, const gchar *msg, ...)
{
	g_assert(node != NULL);

	va_list args;
	va_start (args, msg);
	g_logv(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, msg, args);
	va_end(args);

	g_error("Failed to parse %s view (line %ld)", parsed_view, xmlGetLineNo(node));
}

static void __get_property_attribute (xmlNode *node, gchar **property_string, 
		gchar **target_name, MgdSchemaPropertyAttr **prop_attr, MidgardDBObjectClass *klass)
{
	*target_name = property_string[1];

	if (property_string[2] != NULL && !g_str_equal (*target_name, "metadata")) {
		__view_error (node, "%s not allowed in view configuration", target_name);
		return;	
	}

	/* Check metadata */
	if (property_string[2] != NULL && g_str_equal (*target_name, "metadata")) {

		if (!MGD_DBCLASS_METADATA_CLASS (klass)) {
			__view_error (node, "No metadata registered for %s class", G_OBJECT_CLASS_NAME (klass));
			return;
		}

		*target_name = property_string[2];
		*prop_attr = g_hash_table_lookup (MIDGARD_DBOBJECT_CLASS (MGD_DBCLASS_METADATA_CLASS (klass))->dbpriv->storage_data->prophash, *target_name);
	
		if (!*prop_attr)
			__view_error (node, "%s not found. Not registered for %s.metadata ?", *target_name, property_string[0]);
	}

	/* Fallback to custom properties, metadata class doesn't exist in MgdSchema scope */	
	if (!*prop_attr) {
		*prop_attr = g_hash_table_lookup (klass->dbpriv->storage_data->prophash, *target_name);
		if (!*prop_attr)
			__view_error(node, "%s not found. Not registered for %s ?", *target_name, property_string[0]);

		if ((*prop_attr)->is_private)
			__view_error (node, "Private property %s.%s can not be added to view.",
					G_OBJECT_CLASS_NAME (G_OBJECT_CLASS (klass)), (*prop_attr)->name);
	}
}

static void __get_view_properties(xmlNode *node, MgdSchemaTypeAttr *type)
{
	xmlNode *cur;
	MgdSchemaPropertyAttr *rprop_attr = NULL;
	gchar *property_name = NULL;

	for (cur = node->children; cur; cur = cur->next) {

		if (cur->type == XML_ELEMENT_NODE 
				&& g_str_equal(cur->name, "property")) { /* FIXME, add property to reserved words constants */

			property_name = NULL;
			rprop_attr = NULL;

			xmlChar *name = xmlGetProp(cur, (const xmlChar *)TYPE_RW_NAME);

			if (!name || (name && *name == '\0'))
				__view_error(cur, "Can not register view with empty property", NULL);

			xmlChar *use_prop = xmlGetProp(cur, (const xmlChar *) "use"); /* FIXME, use reserved word */
			
			if (!use_prop || (use_prop && *use_prop == '\0'))
				__view_error(cur, "Referenced class:property can not be empty", NULL);

			gchar **rprop = g_strsplit_set ((const gchar *)use_prop, ":.", -1);
			xmlFree (use_prop);
			if (!rprop || rprop[0] == NULL || rprop[1] == NULL) {
				__view_error(cur, "Referenced property can not be empty", NULL);
				return;
			}

			MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(rprop[0]);		
			if (!klass)
				__view_error (cur, "Defined '%s' class is not registered as midagrd_object derived one", rprop[0]);

			const gchar *table = midgard_core_class_get_table (MIDGARD_DBOBJECT_CLASS (klass));
			if (table == NULL)
				__view_error (cur, "Can not create proper view. Defined '%s' class has NULL storage", rprop[0]);

			__get_property_attribute (cur, rprop, &property_name, &rprop_attr, MIDGARD_DBOBJECT_CLASS (klass));

			midgard_core_schema_type_property_copy(rprop_attr, type);
			
			/* TODO, refactor with some usable TypeAttr related API */
			/* Create property attributes copy using original property.
			   Then change name. */
			MgdSchemaPropertyAttr *prop_attr = g_hash_table_lookup(type->prophash, property_name);
	
			if (!prop_attr)
				g_warning("Can not find  %s.%s in newly registered view", type->name, name); 

			if (prop_attr) {

				g_free((gchar *)prop_attr->name);
				prop_attr->name = g_strdup((gchar *)name);

				/* Free old key and remove new one in hash table */
				g_hash_table_remove(type->prophash, (gconstpointer) rprop[1]);

				gchar *property_name = g_strdup ((gchar *)name);
				g_hash_table_insert(type->prophash, property_name, prop_attr);

				/* FIXME
				 * Workaround for properties not being registered in the same order as defined 
				 * in view xml file */	
				type->_properties_list = g_slist_append (type->_properties_list, property_name);	
			}

			g_strfreev(rprop);
			xmlFree(name);	
		}
	}
}

static const gchar *__allowed_joins[] = {"left", "right", "inner", "inner left", "inner right", "outer", NULL };

static void __get_view_joins(xmlNode *node, MgdSchemaTypeAttr *type)
{
	xmlNode *cur;
	gchar *property_name = NULL;	
	MgdSchemaPropertyAttr *propright = NULL;
	MgdSchemaPropertyAttr *propleft = NULL;

	for (cur = node->children; cur; cur = cur->next) {
	
		if (cur->type == XML_ELEMENT_NODE 
				&& g_str_equal(cur->name, "join")) { /* FIXME, add join to reserved words constants */

			property_name = NULL;
			propright = NULL;
			propleft = NULL;

			xmlChar *jointype = xmlGetProp(cur, (const xmlChar *)"type");

			if (!jointype || (jointype && *jointype == '\0'))
				__view_error(cur, "Can not create join with empty type", NULL);

			if (!midgard_core_xml_attribute_is_allowed(__allowed_joins, jointype))
				__view_error(cur, "%s join type is not allowed", jointype);

			xmlChar *classname = xmlGetProp(cur, (const xmlChar *)"class");
			xmlChar *table = NULL;

			if (!classname || (classname && *classname == '\0')) {

				table = xmlGetProp(cur, (const xmlChar *) TYPE_RW_TABLE);

				if (!table)
					__view_error(cur, "Can not create join. Empty, not defined class or table", NULL);
			}

			MidgardDBObjectClass *klass = NULL;
			MidgardDBObjectClass *joinklass = NULL;

			if (classname) 
				joinklass = MIDGARD_DBOBJECT_CLASS(MIDGARD_OBJECT_GET_CLASS_BY_NAME((const gchar *)classname));

			if (!joinklass) {
				g_warning ("%s is not registered in GType system", classname);
				__view_error (cur, "Invalid classname for defined join");
				return;
			}

			gchar *left = midgard_core_xml_get_child_attribute_content_by_name(cur, "condition", "left");
			if (!left)
				__view_error(cur, "Condition left is missing", NULL);

			gchar *right = midgard_core_xml_get_child_attribute_content_by_name(cur, "condition", "right");
			if (!right)
				__view_error(cur, "Condition right is missing", NULL);

			/* Get left property attribute */
			gchar **classprop = g_strsplit_set(left, ":.", -1);
			if (!classprop || classprop[0] == NULL || classprop[1] == NULL) {
				__view_error(cur, "Condition left problem", NULL);
				return;
			}

			klass = MIDGARD_DBOBJECT_CLASS(MIDGARD_OBJECT_GET_CLASS_BY_NAME((const gchar *)classprop[0]));
			if (!klass) {
				__view_error(cur, "Class %s not registered", classprop[0]);
				return;
			}

			MgdSchemaPropertyAttr *propleft = 
				g_hash_table_lookup(klass->dbpriv->storage_data->prophash, classprop[1]);
			if (!propleft)
				__view_error(cur, "Property %s not registered for %s", classprop[1], classprop[0]); 
			/* __get_property_attribute (cur, classprop, &property_name, &propleft, klass); */ 
			g_strfreev(classprop);

			/* Get right property attribute */
			classprop = g_strsplit_set(right, ":.", -1);
			if (!classprop || classprop[0] == NULL || classprop[1] == NULL) {
				__view_error(cur, "Condition right problem", NULL);
				return;
			}

			klass = MIDGARD_DBOBJECT_CLASS(MIDGARD_OBJECT_GET_CLASS_BY_NAME((const gchar *)classprop[0]));
			if (!klass) {
				__view_error(cur, "Class %s not registered", classprop[0]);
				return;
			}

			MgdSchemaPropertyAttr *propright = 
				g_hash_table_lookup(klass->dbpriv->storage_data->prophash, classprop[1]);
			if (!propright)
				__view_error(cur, "Property %s not registered for %s", classprop[1], classprop[0]);
			/* __get_property_attribute (cur, classprop, &property_name, &propright, klass); */
			g_strfreev(classprop);

			MidgardDBJoin *mdbj = midgard_core_dbjoin_new();
			mdbj->type = g_utf8_strup((const gchar *)jointype, strlen((gchar *)jointype));

			/* configure table, if there's class, get it's table, in other case, get table directly */
			if (classname)
				mdbj->table = g_strdup(midgard_core_class_get_table(joinklass));
			else 
				mdbj->table = g_strdup((gchar *)table);
	
			mdbj->left = propleft;
			mdbj->right = propright;

			g_free(left);
			g_free(right);

			type->joins = g_slist_append(type->joins, (gpointer) mdbj);

			xmlFree(jointype);
			xmlFree(classname);
			if (table)
				xmlFree(table);
		}
	}
}

static void __get_view_constraints(xmlNode *node, MgdSchemaTypeAttr *type)
{
	xmlNode *cur;

	for (cur = node->children; cur; cur = cur->next) {
	
		if (cur->type == XML_ELEMENT_NODE 
				&& g_str_equal(cur->name, "constraint")) { /* FIXME, add constraint to reserved words constants */
			
			xmlChar *property = xmlGetProp(cur, (const xmlChar *)"property");

			if (!property || (property && *property == '\0'))
				__view_error(cur, "Can not add constraint with empty property", NULL);

			gchar **classprop = g_strsplit((gchar *)property, ":", 2);
			if (!classprop || classprop[0] == NULL || classprop[1] == NULL) {
				__view_error(cur, "Constraint property misconfigured", NULL);
				return;
			}

			MidgardDBObjectClass *klass = 
				MIDGARD_DBOBJECT_CLASS(MIDGARD_OBJECT_GET_CLASS_BY_NAME((const gchar *)classprop[0]));
			if (!klass)
				__view_error(cur, "Class %s not registered", classprop[0]);

			MidgardCoreQueryConstraint *constraint = midgard_core_query_constraint_new();
			if (!midgard_core_query_constraint_parse_property(&constraint, klass, classprop[1]))
				__view_error(cur, "Can not parse %s constraint property", classprop[1]);

			g_strfreev(classprop);
			xmlFree(property);
			
			/* Add operator */
			xmlChar *operator = xmlGetProp(cur, (const xmlChar *)"operator");
			if (!operator || (operator && *operator == '\0'))
				__view_error(cur, "Can not add constraint with empty operator", NULL);
			if (!midgard_core_query_constraint_add_operator(constraint, (const gchar *)operator))
				__view_error(cur, "Invalid operator", NULL);
			xmlFree(operator);

			/* Add value */
			xmlChar *valtype = xmlGetProp(cur, (const xmlChar *)"value_type");
			if (!valtype || (valtype && *valtype == '\0'))
				__view_error(cur, "Can not add constraint with empty value_type", NULL);

			GType vtype = midgard_core_schema_gtype_from_string((const gchar *)valtype);
			if (!vtype)
				__view_error(cur, "Invalid %s value_type", valtype);

			xmlFree(valtype);

			xmlChar *value = xmlGetProp(cur, (const xmlChar *)"value");
			if (!value || (value && *value == '\0'))
				__view_error(cur, "Can not add constraint with empty value", NULL);

			/* Get value as string and convert to declared type */
			GValue strval = {0, };
			g_value_init(&strval, G_TYPE_STRING);
			g_value_set_string(&strval, (const gchar *)value);

			GValue val = {0, };
			g_value_init(&val, vtype);
			g_value_transform((const GValue *) &strval, &val);
		
			midgard_core_query_constraint_add_value(constraint, &val);
			midgard_core_query_constraint_build_condition(constraint);

			type->constraints = g_slist_append(type->constraints, (gpointer) constraint);
			
			g_value_unset(&val);
			g_value_unset(&strval);
			xmlFree(value);
		}
	}
}

static void __mgdschematype_from_node(xmlNode *node, GSList **list)
{
	g_assert(node != NULL);
	
	xmlChar *viewname = xmlGetProp(node, (const xmlChar *)MGD_VIEW_RW_NAME);
	if (!viewname) 	
		__view_error(node, "Empty view name. Can not define any type.", NULL);

	MgdSchemaTypeAttr *type = midgard_core_schema_type_attr_new();
	type->name = g_strdup((gchar *)viewname);
	type->is_view = TRUE;

	/* Determine base view table */
	xmlChar *table = xmlGetProp(node, (const xmlChar *) TYPE_RW_TABLE);

	/* Table not defined, try classtable */	
	if (!table) {

		xmlChar *classname = xmlGetProp(node, (const xmlChar *) "class");

		if (!classname)
			__view_error(node, "'table' or 'class' attribute not found", NULL);

		MidgardDBObjectClass *dbklass = 
			MIDGARD_DBOBJECT_CLASS(g_type_class_peek(g_type_from_name((const gchar *)classname)));
		table = (xmlChar *)g_strdup(midgard_core_class_get_table(dbklass));

		if (!table)
			__view_error(node, "NULL table for %s class", classname);

		xmlFree(classname);
	}
	
	midgard_core_schema_type_set_table(type, (const gchar *)table);		

	xmlFree(table);

	__get_view_properties(node, type);
	__get_view_joins(node, type);
	__get_view_constraints(node, type);

	midgard_core_schema_type_build_static_sql(type);

	*list = g_slist_append(*list, type);

	xmlFree(viewname);
}

gchar *__build_static_create_view(MgdSchemaTypeAttr *type)
{
	GString *query = g_string_new("CREATE VIEW ");	

	g_string_append_printf(query, "%s AS SELECT %s FROM %s ", 
			type->name, (gchar *)type->sql_select_full, type->table);

	GSList *list = NULL;

	for (list = type->joins; list != NULL; list = list->next) {
	
		MidgardDBJoin *join = (MidgardDBJoin *) list->data;
		g_string_append_printf(query, "%s JOIN %s ON %s = %s ", 
				join->type, 
				join->table, 
				join->left->tablefield, join->right->tablefield);		
	}	

	/* Select only those records which are not deleted. #1437 */
	list = NULL;
	guint i = 0;
	g_string_append(query, " WHERE ");

	if (!type->joins)
		g_string_append_printf (query, " %s.metadata_deleted = 0 ", type->table);

	for (list = type->joins; list != NULL; list = list->next) {
		
		if(i > 0)
			g_string_append(query, " AND ");

		MidgardDBJoin *join = (MidgardDBJoin *) list->data;
		g_string_append_printf(query, " %s.metadata_deleted = 0 AND %s.metadata_deleted = 0 ", 
				join->left->table, join->right->table);

		i++;
	}


	list = NULL;
	i = 0;

	for (list = type->constraints; list != NULL; list = list->next) {
		
		g_string_append(query, " AND ");
		g_string_append(query,
				MIDGARD_CORE_QUERY_CONSTRAINT(list->data)->priv->condition);
	}

	return g_string_free(query, FALSE);
}

static void __register_view_type (MgdSchemaTypeAttr *type)
{

	if (type == NULL) 
		return;

	if (type->params == NULL) {

		g_warning("No parameters found for %s schema type. Not registering.", type->name);
		return;
	}

	GType new_type;
	new_type = midgard_core_view_type_register(type, MIDGARD_TYPE_OBJECT);

	if (new_type) {

		GObject *foo = g_object_new(new_type, NULL);
		/* Set number of properties.
		 * This way we gain performance for instance_init call */
		GParamSpec **pspecs =
			g_object_class_list_properties(
					G_OBJECT_GET_CLASS(G_OBJECT(foo)),
					&type->class_nprop);

		/* Replace storage name */
		g_hash_table_destroy(type->tableshash);
		g_free((gchar *)type->table);
		type->table = NULL;
		type->tableshash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
		midgard_core_schema_type_set_table(type, (const gchar *)type->name);

		/* Recreate static sql used by QB */
		g_free(type->sql_select_full);
		guint i;
		GString *ssf = g_string_new("");
		gboolean add_coma = FALSE;

		for (i = 0; i < type->class_nprop; i++) {
		
			if (G_TYPE_FUNDAMENTAL (pspecs[i]->value_type) == G_TYPE_OBJECT) {
				if (i == 1)
					add_coma = FALSE;
				continue;
			}

			if (!add_coma)
				g_string_append(ssf, pspecs[i]->name);
			else 
				g_string_append_printf(ssf, ", %s", pspecs[i]->name);

			add_coma = TRUE;

			/* Replace tablefield for every property */
			MgdSchemaPropertyAttr *prop_attr = g_hash_table_lookup(type->prophash, pspecs[i]->name);
			if (!prop_attr) {

				g_warning ("%s property not registered for %s", pspecs[i]->name, type->name);
				g_error ("Failed to register view class");
			}
			midgard_core_schema_type_property_set_tablefield(prop_attr, type->name, pspecs[i]->name);
		}

		type->sql_select_full = ssf->str;
		g_string_free(ssf, FALSE);

		g_free(pspecs);
		g_object_unref(foo);
	}
}

void midgard_core_views_read_file(const gchar *path)
{
	g_assert(path != NULL);

	xmlNode *node;
	xmlDocPtr doc = xmlParseFile(path);

	if (doc == NULL) {

		g_warning("Skipping malformed midgard_view schema file %s", path);
		return;
	}

	xmlNodePtr root = xmlDocGetRootElement(doc);

	if (root == NULL
			|| root->ns == NULL
			|| !g_str_equal(root->ns->href, MGD_SCHEMA_XML_NAMESPACE)
			|| !g_str_equal(root->name, MGD_SCHEMA_XML_ROOT_NAME)) {

		g_warning("Skipping invalid midgard_view schema file %s", path);
		xmlFreeDoc(doc);
		
		return;
	}

	guint views_found = 0;
	GSList *list = NULL;

	for (node = root->children; node; node = node->next) {

		if (node->type == XML_ELEMENT_NODE 
				&& g_str_equal(node->name, MGD_VIEW_RW_VIEW)) {

			views_found++;
			__mgdschematype_from_node(node, &list);
		}
	}

	if (list != NULL) {

		GSList *clist = NULL;
		for (clist = list; clist != NULL; clist = clist->next) {
			
			MgdSchemaTypeAttr *type = (MgdSchemaTypeAttr *) clist->data;
			type->sql_create_view = __build_static_create_view(type);
			midgard_core_schema_type_initialize_paramspec (type);
			midgard_core_schema_type_validate_fields(type);
			__register_view_type(type);	
		}
	}

	if (views_found == 0)
		g_warning("Can not find any view defined in %s xml view schema file.", path);

	xmlFreeDoc(doc);

	return;
}

void midgard_core_views_read_dir(const gchar *path)
{
	g_assert(path != NULL);

	const gchar *filename;
	GError *error;
	GDir *dir = g_dir_open(path, 0, &error);
	gchar *fullpath = NULL;

	if (!dir) {

		g_warning("Failed to read '%s' directory. %s", 
				path, error && error->message ? error->message : "Error message not available"); 

		if (error) 
			g_error_free(error);

		return;
	}

	do {
		filename = g_dir_read_name(dir);
		fullpath = g_build_path(G_DIR_SEPARATOR_S, path, filename, NULL);

		if (!g_file_test (fullpath, G_FILE_TEST_IS_DIR) 
				&& g_file_test (fullpath, G_FILE_TEST_IS_REGULAR) 
				&& g_str_has_suffix(filename, ".xml")){

			/* Hide hidden, recovery, and similiar files */
			if (g_str_has_prefix(filename, ".")
					|| g_str_has_prefix(filename, "#") 
					|| g_str_has_suffix(filename, "~")) {
				
				g_warning("File %s ignored!", filename);

			} else {	
				
				parsed_view = fullpath;
				midgard_core_views_read_file(fullpath);
				parsed_view = NULL;
			}
		}

		g_free (fullpath);

	} while (filename != NULL);

	g_dir_close(dir);
}



