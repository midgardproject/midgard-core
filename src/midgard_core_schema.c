/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_core_object.h"
#include "schema.h"

typedef struct {
	GString *string;
	guint elts;
	MgdSchemaTypeAttr *type;
}_str_cont;

static GHashTable *__copy_hash_table_with_strings(GHashTable *src);

MgdSchemaPropertyAttr *  
midgard_core_schema_type_property_attr_new_empty (void)
{
	MgdSchemaPropertyAttr *prop = g_new(MgdSchemaPropertyAttr, 1); 

	/* Ensure, default type is string */
	prop->gtype = G_TYPE_NONE;
	prop->type = NULL;

	prop->name = NULL;	
	prop->default_value = NULL;
	prop->dbtype = NULL;
	prop->field = NULL;
	prop->dbindex = FALSE;
	prop->table = NULL;
	prop->tablefield = NULL;
	prop->upfield = NULL;
	prop->parentfield = NULL;
	prop->primaryfield = NULL;
	prop->link = NULL;
	prop->link_target = NULL;
	prop->is_primary = FALSE;
	prop->is_reversed = FALSE;
	prop->is_link = FALSE;
	prop->is_linked = FALSE;
	prop->is_private = FALSE;
	prop->is_unique = FALSE;
	prop->description = NULL;
	prop->user_values = NULL;
	prop->derived = NULL;

	return prop;
}

MgdSchemaPropertyAttr * 
midgard_core_schema_type_property_attr_new (void)
{
	MgdSchemaPropertyAttr *prop = midgard_core_schema_type_property_attr_new_empty();

	/* Ensure, default type is string */
	prop->gtype = G_TYPE_STRING;
	prop->type = g_strdup ("string");

	prop->description = g_strdup ("");
	prop->user_values = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

	return prop;
}

void
midgard_core_schema_type_property_copy (MgdSchemaPropertyAttr *src_prop, MgdSchemaTypeAttr *dest_type)
{
	g_assert (src_prop != NULL);
	g_assert (dest_type != NULL);

	MgdSchemaPropertyAttr *prop = midgard_core_schema_type_property_attr_new ();

	prop->gtype = src_prop->gtype;
	g_free ((gchar *)prop->type); /* Explicitly free, it's initialized as new empty string by default */
	prop->type = g_strdup(src_prop->type);
	prop->name = src_prop->name ? g_strdup(src_prop->name) : NULL;

	if (src_prop->default_value) {
		
		prop->default_value = g_new0(GValue, 1);
		g_value_init(prop->default_value, G_VALUE_TYPE(src_prop->default_value));
		g_value_copy(src_prop->default_value, prop->default_value);
	}

	prop->dbtype = src_prop->dbtype ? g_strdup(src_prop->dbtype) : NULL;
	prop->field = src_prop->field ? g_strdup(src_prop->field) : NULL;
	prop->dbindex = src_prop->dbindex;
	prop->table = src_prop->table ? g_strdup(src_prop->table) : NULL;
	prop->tablefield = src_prop->tablefield ? g_strdup(src_prop->tablefield) : NULL;
	prop->upfield = src_prop->upfield ? g_strdup(src_prop->upfield) : NULL;
	prop->parentfield = src_prop->parentfield ? g_strdup(src_prop->parentfield) : NULL;
	prop->primaryfield = src_prop->primaryfield ? g_strdup(src_prop->primaryfield) : NULL;
	prop->link = src_prop->link ? g_strdup(src_prop->link) : NULL;
	prop->link_target = src_prop->link_target ? g_strdup(src_prop->link_target) : NULL;
	prop->is_primary = src_prop->is_primary;
	prop->is_reversed = src_prop->is_reversed;
	prop->is_link = src_prop->is_link;
	prop->is_linked = src_prop->is_linked;
	g_free (prop->description); /* Explicitly free, it's initialized with new empty string by default */
	prop->description = g_strdup(src_prop->description);
	g_hash_table_destroy (prop->user_values);
	prop->user_values = __copy_hash_table_with_strings(src_prop->user_values);

	g_hash_table_insert(dest_type->prophash, g_strdup(prop->name), prop);
}

void
midgard_core_schema_type_property_attr_free (MgdSchemaPropertyAttr *prop)
{
	g_assert (prop != NULL);
	
	g_free ((gchar *)prop->type);
	prop->type = NULL;

	g_free ((gchar *)prop->dbtype);
	prop->dbtype = NULL;

	g_free ((gchar *)prop->name);
	prop->name = NULL;

	g_free ((gchar *)prop->field);
	prop->field = NULL;

	g_free ((gchar *)prop->table);
	prop->table = NULL;

	g_free ((gchar *)prop->tablefield);
	prop->tablefield = NULL;

	g_free ((gchar *)prop->upfield);
	prop->upfield = NULL;

	g_free ((gchar *)prop->parentfield);
	prop->parentfield = NULL;

	g_free ((gchar *)prop->primaryfield);
	prop->primaryfield = NULL;

	g_free ((gchar *)prop->link);
	prop->link = NULL;

	g_free ((gchar *)prop->link_target);
	prop->link_target = NULL;

	g_free ((gchar *)prop->description);
	prop->description = NULL;

	g_hash_table_destroy(prop->user_values);
	prop->user_values = NULL;

	prop->derived = NULL;

	g_free (prop);

	prop = NULL;
}

MgdSchemaTypeAttr *
midgard_core_schema_type_attr_new (void)
{
	MgdSchemaTypeAttr *type = g_new (MgdSchemaTypeAttr, 1);
	type->name = NULL;
	type->base_index = 0;
	type->num_properties = 0;
	type->class_nprop = 0;
	type->params = NULL;
	type->properties = NULL;
	type->table = NULL;
	type->view_table = NULL;
	type->parentfield = NULL;
	type->upfield = NULL;
	type->primaryfield = NULL;
	type->parent = NULL;
	type->primary = NULL;
	type->property_up = NULL;
	type->property_parent = NULL;
	type->tables = NULL;
	type->tableshash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	type->prophash = g_hash_table_new (g_str_hash, g_str_equal);
	type->_properties_list = NULL;
	type->children = NULL;
	type->unique_name = NULL;
	type->sql_select_full = NULL;
	type->copy_from = NULL;
	type->extends = NULL;
	type->joins = NULL;
	type->constraints = NULL;
	type->is_view = FALSE;
	type->sql_create_view = NULL;
	type->metadata_class_name = NULL;
	type->metadata_class_ptr = NULL;
	type->user_values = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	type->is_abstract = FALSE;
	type->is_iface = FALSE;
	type->is_mixin = FALSE;

	return type;
}

void
midgard_core_schema_type_attr_extend (MgdSchemaTypeAttr *src, MgdSchemaTypeAttr *dst)
{
     	g_return_if_fail (src != NULL);
	g_return_if_fail (dst != NULL);

	g_free (dst->parentfield);
	dst->parentfield = src->parentfield ? g_strdup (src->parentfield) : NULL;
	g_free (dst->upfield);
	dst->upfield = src->upfield ? g_strdup (src->upfield) : NULL;
	g_free ((gchar *)dst->parent);
	dst->parent = src->parent ? g_strdup (src->parent) : NULL;
	g_free ((gchar *)dst->primary);
	dst->primary = src->primary ? g_strdup (src->primary) : NULL;
	g_free ((gchar *)dst->primaryfield);
	dst->primaryfield = src->primaryfield ? g_strdup (src->primaryfield) : NULL;
	g_free ((gchar *)dst->property_up);
	dst->property_up = src->property_up ? g_strdup (src->property_up) : NULL;
	g_free ((gchar *)dst->property_parent);
	dst->property_parent = src->property_parent ? g_strdup (src->property_parent) : NULL;
	
	if (src->children) {
		GSList *children;
		for (children = src->children; children != NULL; children = children->next) {
			dst->children = g_slist_append (dst->children, g_strdup ((gchar *) children->data));
		}
	}

	g_free ((gchar *)dst->unique_name);
	dst->unique_name = src->unique_name ? g_strdup (src->unique_name) : NULL;
	g_free ((gchar *)dst->metadata_class_name);
	dst->metadata_class_name = src->metadata_class_name ? g_strdup (src->metadata_class_name) : NULL;
	dst->metadata_class_ptr = src->metadata_class_ptr ? src->metadata_class_ptr : NULL;
}

void 
_destroy_property_hash (gpointer key, gpointer value, gpointer userdata)
{
	MgdSchemaPropertyAttr *prop = (MgdSchemaPropertyAttr *) value;
	gchar *name = (gchar *) key;

	if(prop)
		midgard_core_schema_type_property_attr_free (prop);

	if(name) {

		g_free (name);
		name = NULL;
	}
}

void 
midgard_core_schema_type_attr_free (MgdSchemaTypeAttr *type)
{
	g_assert (type != NULL);

	g_free (type->name);
	type->name = NULL;

	g_free ((gchar *)type->table);
	type->table = NULL;

	g_free ((gchar *)type->view_table);
	type->view_table = NULL;

	g_free ((gchar *)type->tables);
	type->tables = NULL;

	g_free ((gchar *)type->parent);
	type->parent = NULL;

	g_free ((gchar *)type->primary);
	type->primary = NULL;

	g_free ((gchar *)type->property_up);
	type->property_up = NULL;

	g_free ((gchar *)type->property_parent);
	type->property_parent = NULL;

	g_hash_table_destroy(type->tableshash);
	type->tableshash = NULL;

	g_hash_table_foreach(type->prophash, _destroy_property_hash, NULL);
	g_hash_table_destroy(type->prophash);
	type->prophash = NULL;

	if (type->_properties_list)
		g_slist_free (type->_properties_list);
	type->_properties_list = NULL;

       	g_free (type->sql_select_full);
	g_free (type->parentfield);
	g_free (type->upfield);
	g_free (type->primaryfield);
	g_free (type->params);
	g_free (type->properties);
	g_free ((gchar *)type->unique_name);

	if (type->joins != NULL) {
		g_slist_free(type->joins);
		type->joins = NULL;
	}

	if (type->constraints != NULL) {
		g_slist_free(type->constraints);
		type->constraints = NULL;
	}

	g_free (type->sql_create_view);
	type->sql_create_view = NULL;
	g_free (type->metadata_class_name);
	type->metadata_class_name = NULL;

	g_hash_table_destroy (type->user_values);
        type->user_values = NULL;

	g_free (type);

	type = NULL;
}

static void 
__copy_hash_table_keys (gpointer key, gpointer value, gpointer userdata)
{
	gchar *k = (gchar *)key;
	gchar *v = (gchar *)value;
	GHashTable *table = (GHashTable *)userdata;

	g_hash_table_insert (table, g_strdup(k), g_strdup(v));
}

static GHashTable*
__copy_hash_table_with_strings (GHashTable *src)
{
	GHashTable *dest = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	g_hash_table_foreach (src, __copy_hash_table_keys, dest);

	return dest;
}

void			
midgard_core_schema_type_set_table (MgdSchemaTypeAttr *type, const gchar *table)
{
	g_assert (type != NULL);
	g_assert (table != NULL);

	if (type->table != NULL)
		return;

	type->table = g_strdup ((gchar *)table);
	midgard_core_schema_type_add_table (type, table);
}

static void 
__build_tables_static_string (gpointer key, gpointer val, gpointer userdata)
{
	_str_cont *_cont  = (_str_cont *) userdata;
	gchar *table = (gchar *) key;

	if(_cont->elts == 0)
		g_string_append_printf (_cont->string, "%s", table);
	else
		g_string_append_printf (_cont->string, ", %s", table);

	_cont->elts++;		
}

void			
midgard_core_schema_type_add_table (MgdSchemaTypeAttr *type, const gchar *table)
{
	g_assert (type != NULL);
	g_assert (table != NULL);

	if (type->tableshash == NULL)
		return;

	g_hash_table_insert (type->tableshash, g_strdup(table), NULL);

	GString *_sql = g_string_new ("");
	_str_cont *cont = g_new (_str_cont, 1);
	cont->string = _sql;
	cont->elts = 0;

	g_hash_table_foreach (type->tableshash, __build_tables_static_string, cont);
	g_free ((gchar *)type->tables);
	type->tables = g_strdup (_sql->str);
	g_string_free (_sql, TRUE);
	g_free (cont);
}

void 
midgard_core_schema_type_initialize_paramspec (MgdSchemaTypeAttr *type)
{
	g_assert (type != NULL);

	GHashTable *hash = type->prophash;

	if (hash == NULL)
		return;

	guint n = g_hash_table_size(hash);
	guint m = n+1;

	if (n < 1) {	
		g_debug ("Initializing '%s' type without properties", type->name);
		return;
	}

	type->params = g_new(GParamSpec*, m);		
	GSList *list = NULL;

	gchar *property = NULL;
	MgdSchemaPropertyAttr *prop_attr = NULL;		

	/* FIXME, This is pure workaround. Valid properties with inheritance chain is needed */
	/* We use _properties_list because GHashTable iterator or g_hash_table_foreach
	 * create paramspec not in the same order we declared them in MgdSchema file. */
	for (list = type->_properties_list; list != NULL; list = list->next) {

		property = (gchar *) list->data;
		prop_attr = g_hash_table_lookup (hash, (gpointer) property);

		if (prop_attr == NULL) {

			g_warning("Missed property information (property: %s.%s)", type->name, property);
			return;
		}

		guint n = ++type->num_properties;
		GParamSpec **params = type->params;
		GType ptype = prop_attr->gtype;
		gchar *nick = "";

		if (ptype == MGD_TYPE_STRING) {

			params[n-1] = g_param_spec_string(
					property, nick, prop_attr->description,
					"",  G_PARAM_READWRITE);
		} else if (ptype == MGD_TYPE_TIMESTAMP) {
		
			params[n-1] = g_param_spec_boxed(
					property, nick, prop_attr->description,
					MGD_TYPE_TIMESTAMP, G_PARAM_READWRITE);
		
		} else if (ptype == MGD_TYPE_UINT) {
			
			params[n-1] = g_param_spec_uint(
					property, nick, prop_attr->description,
					0, G_MAXUINT32, 0, G_PARAM_READWRITE);
		
		} else if (ptype == MGD_TYPE_INT) {
		
			params[n-1] = g_param_spec_int(
					property, nick, prop_attr->description,
					G_MININT32, G_MAXINT32, 0, G_PARAM_READWRITE);
		
		} else if (ptype == MGD_TYPE_FLOAT) {
		
			params[n-1] = g_param_spec_float(
					property, nick, prop_attr->description,
					-G_MAXFLOAT, G_MAXFLOAT, 0, G_PARAM_READWRITE);
		
		} else if (ptype == MGD_TYPE_BOOLEAN) {
			
			params[n-1] = g_param_spec_boolean(
					property, nick, prop_attr->description,
					FALSE, G_PARAM_READWRITE);
		
		} else {
			
			params[n-1] = g_param_spec_string(
					property, nick, prop_attr->description,
					"", G_PARAM_READWRITE);
		}
	}
}

void			
midgard_core_schema_type_property_set_table (MgdSchemaPropertyAttr *prop, const gchar *table)
{
	g_assert (prop != NULL);
	g_assert (table != NULL);

	if (prop->table != NULL)
		return;

	prop->table = g_strdup((gchar *)table);
}

void 
midgard_core_schema_type_property_set_tablefield (MgdSchemaPropertyAttr *prop, const gchar *table, const gchar *field)
{
	g_assert (prop != NULL);
	g_assert (table != NULL);
	g_assert (field != NULL);

	/* in case there are two attributes of which one takes precedence */
	/* FIXME, we shouldn't allow this, but it's kept for safety reason */
	if (prop->tablefield != NULL)
		g_free ((gchar *)prop->tablefield);

	gchar *_table = g_strdup (table);
	gchar *_field = g_ascii_strdown (field, -1);

	if (prop->table) g_free ( (gchar *)prop->table);
	prop->table = g_strdup(_table);

	/* Set table implicitly */
	midgard_core_schema_type_property_set_table(prop, _table);

	/* Set field implicitly */
	if (prop->field) g_free ( (gchar *)prop->field); 
	prop->field = g_strdup(_field);

	prop->tablefield = g_strjoin(".", _table, _field, NULL);

	g_free (_table);
	g_free (_field);
}

GType
midgard_core_schema_gtype_from_string(const gchar *type)
{
	if (!type)
		return G_TYPE_NONE;

	if(g_str_equal(type, "string"))
		return MGD_TYPE_STRING;

	if(g_str_equal(type, "integer"))
		return MGD_TYPE_INT;

	if(g_str_equal(type, "unsigned integer"))
		return MGD_TYPE_UINT;

	if(g_str_equal(type, "float"))
		return MGD_TYPE_FLOAT;

       	/* FIXME, change to MGD_TYPE_DOUBLE once mgdschema supports it */
	if(g_str_equal(type, "double"))
		return MGD_TYPE_FLOAT;

	if(g_str_equal(type, "boolean"))
		return MGD_TYPE_BOOLEAN;

	if(g_str_equal(type, "bool"))
		return MGD_TYPE_BOOLEAN;

	if(g_str_equal(type, "datetime"))
		return MGD_TYPE_TIMESTAMP;

	if(g_str_equal(type, "longtext"))
		return MGD_TYPE_LONGTEXT;

	if(g_str_equal(type, "text"))
		return MGD_TYPE_LONGTEXT;

	if(g_str_equal(type, "guid"))
		return MGD_TYPE_GUID;

	return G_TYPE_NONE;
}

void
midgard_core_schema_type_property_set_gtype (MgdSchemaPropertyAttr *prop, const gchar *type)
{
	g_assert (prop != NULL);
	g_assert (type != NULL);

	if (prop->type != NULL)
		g_free ((gchar *)prop->type);

	prop->type = g_strdup((gchar *)type);
	prop->gtype = midgard_core_schema_gtype_from_string(type);
}

static void 
__build_static_sql(gpointer key, gpointer val, gpointer userdata)
{
	_str_cont *_cont  = (_str_cont *) userdata;
	gchar *property = (gchar *) key;
	MgdSchemaPropertyAttr *prop_attr = (MgdSchemaPropertyAttr *)val;
	MgdSchemaTypeAttr *type = _cont->type;

	/* Check if property attributes are correct */
	/* Set missed table and tablefield */
	if (prop_attr->table == NULL) {
		midgard_core_schema_type_property_set_tablefield(prop_attr, type->table, prop_attr->field);
	}

	/* upfield defined, set tablefield */
	if (prop_attr->upfield != NULL) 
		midgard_core_schema_type_property_set_tablefield(prop_attr, prop_attr->table, prop_attr->upfield);
	
	/* parentfield defined, set tablefield */
	if (prop_attr->parentfield != NULL) 
		midgard_core_schema_type_property_set_tablefield(prop_attr, prop_attr->table, prop_attr->parentfield);

	if(_cont->elts == 0)
		g_string_append_printf(_cont->string, "%s AS %s", (gchar *)prop_attr->tablefield, property);
	else
		g_string_append_printf(_cont->string, ", %s AS %s", (gchar *)prop_attr->tablefield, property);

	_cont->elts++;		
}

void                    
midgard_core_schema_type_build_static_sql (MgdSchemaTypeAttr *type_attr)
{
	g_assert (type_attr != NULL);

	if (type_attr->prophash == NULL || type_attr->table == NULL)
		return;

	GString *_sql = g_string_new(" ");
	_str_cont cont = {NULL, 0, NULL};
	cont.string = _sql;
	cont.elts = 0;
	cont.type = type_attr;

	g_hash_table_foreach (type_attr->prophash, __build_static_sql, &cont);

	if (_sql->str == NULL)
		return;

	g_string_free (_sql, TRUE);
	/* type_attr->sql_select_full = g_string_free (_sql, FALSE); */
}

static void 
__field_is_equal(gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaPropertyAttr *prop_attr = (MgdSchemaPropertyAttr *)val;
	MgdSchemaPropertyAttr *src_prop = (MgdSchemaPropertyAttr *)userdata;

	if (!g_str_equal (prop_attr->name, src_prop->name)) {

		if (prop_attr->tablefield == NULL
				|| src_prop->tablefield == NULL)
			return;

		if (g_str_equal (prop_attr->tablefield, src_prop->tablefield)) {

			g_warning ("Field %s redefined for %s and %s", 
					prop_attr->tablefield, prop_attr->name, src_prop->name);
			g_error ("Table columns collision");
		}
	}
}

static void 
__check_field_duplicates(gpointer key, gpointer val, gpointer userdata)
{
	MgdSchemaPropertyAttr *prop_attr = (MgdSchemaPropertyAttr *)val;
	MgdSchemaTypeAttr *type = (MgdSchemaTypeAttr *)userdata;

	g_hash_table_foreach (type->prophash, __field_is_equal, prop_attr);
}

void
midgard_core_schema_type_validate_fields (MgdSchemaTypeAttr *type)
{
	g_assert (type != NULL);

	if (type->table == NULL)
		return;

	g_hash_table_foreach (type->prophash, __check_field_duplicates, type);
}

MgdSchemaPropertyAttr*
midgard_core_schema_type_property_lookup (MgdSchemaTypeAttr *type, const gchar *name)
{
	g_return_val_if_fail (type != NULL, NULL);
	g_return_val_if_fail (name != NULL, NULL);

	if (!type->prophash)
		return NULL;

	return g_hash_table_lookup (type->prophash, name);
}
