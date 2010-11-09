/* MidgardCore ObjectBuilder routines routines
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

#include "midgard_cr_core_object_builder.h"
#include "midgard_cr_core_timestamp.h"
#include "midgard_cr_core_type.h"
#include "midgard3.h"

static void midgard_cr_core_schema_type_attr_free (MgdSchemaTypeAttr *type);

MgdSchemaTypeAttr *
midgard_cr_core_schema_type_attr_new (void)
{
	MgdSchemaTypeAttr *type = g_new (MgdSchemaTypeAttr, 1);
	type->name = NULL;
	type->base_index = 0;
	type->num_properties = 0;
	type->params = NULL;
	type->properties = NULL;
	type->prophash = g_hash_table_new (g_str_hash, g_str_equal);
	type->_properties_list = NULL;
	type->children = NULL;
	type->copy_from = NULL;
	type->extends = NULL;
	type->metadata_class_name = NULL;
	type->user_values = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, (GDestroyNotify) midgard_cr_core_schema_type_attr_free);
	type->prepared_sql_statement_insert = NULL;
	type->prepared_sql_statement_insert_params = NULL;

	return type;
}

MgdSchemaTypeAttr *
midgard_cr_core_schema_type_new_from_model (MidgardCRModel *model)
{
	g_return_val_if_fail (model != NULL, NULL);
	g_return_val_if_fail (MIDGARD_CR_IS_OBJECT_MODEL (model), NULL);

	MgdSchemaTypeAttr *type_attr = midgard_cr_core_schema_type_attr_new ();
	type_attr->name = g_strdup (midgard_cr_model_get_name (model));

	MidgardCRModel *parent = midgard_cr_model_get_parent (model);
	if (parent) 
		type_attr->extends = g_strdup (midgard_cr_model_get_name (parent));

	return type_attr;
}

void 
midgard_cr_core_schema_type_attr_free (MgdSchemaTypeAttr *type)
{
	g_assert (type != NULL);

	g_free (type->name);
	type->name = NULL;

	g_hash_table_destroy(type->prophash);
	type->prophash = NULL;

	if (type->_properties_list)
		g_slist_free (type->_properties_list);
	type->_properties_list = NULL;

	g_free (type->params);
	g_free (type->properties);	

	g_free (type->metadata_class_name);
	type->metadata_class_name = NULL;

	g_hash_table_destroy (type->user_values);
        type->user_values = NULL;
	
	if (type->prepared_sql_statement_insert != NULL)
		g_object_unref (type->prepared_sql_statement_insert);
	type->prepared_sql_statement_insert = NULL;

	if (type->prepared_sql_statement_insert_params != NULL)
		g_object_unref (type->prepared_sql_statement_insert_params);
	type->prepared_sql_statement_insert_params = NULL;

	g_free (type);

	type = NULL;
}

MgdSchemaPropertyAttr *  
midgard_cr_core_schema_type_property_attr_new (void)
{
	MgdSchemaPropertyAttr *prop = g_new(MgdSchemaPropertyAttr, 1); 

	/* Ensure, default type is string */
	prop->gtype = G_TYPE_NONE;
	prop->type = NULL;

	prop->name = NULL;	
	prop->default_value = NULL;
	prop->link = NULL;
	prop->link_target = NULL;
	prop->is_link = FALSE;
	prop->is_linked = FALSE;
	prop->is_private = FALSE;
	prop->is_unique = FALSE;
	prop->description = NULL;
	prop->user_values = NULL;

	return prop;
}

GType
midgard_cr_core_schema_builder_gtype_from_name (const gchar *name)
{
	g_return_val_if_fail (name != NULL, 0);

	if (g_str_equal (name, "string")
			|| g_str_equal (name, "text")
			|| g_str_equal (name, "guid")
			|| g_str_equal (name, "uuid"))
		return G_TYPE_STRING;

	if (g_str_equal (name, "int"))
		return G_TYPE_INT;

	if (g_str_equal (name, "uint"))
		return G_TYPE_UINT;

	if (g_str_equal (name, "bool")
			|| g_str_equal (name, "boolean"))
		return G_TYPE_BOOLEAN;

	if (g_str_equal (name, "float"))
		return G_TYPE_FLOAT;

	if (g_str_equal (name, "datetime"))
		return MGD_TYPE_TIMESTAMP;

	if (g_str_equal (name, "object"))
		return G_TYPE_OBJECT;

	return G_TYPE_NONE;
}

MgdSchemaPropertyAttr *
midgard_cr_core_schema_type_property_new_from_model (MidgardCRModelProperty *model, MgdSchemaTypeAttr *type_attr)
{
	g_return_val_if_fail (model != NULL, NULL);
	g_return_val_if_fail (MIDGARD_CR_IS_OBJECT_PROPERTY_MODEL (model), NULL);

	MgdSchemaPropertyAttr *prop_attr = midgard_cr_core_schema_type_property_attr_new ();
	prop_attr->name = g_strdup (midgard_cr_model_get_name (MIDGARD_CR_MODEL(model)));

	/* type name */
	prop_attr->type = g_strdup (midgard_cr_model_property_get_valuetypename (model));
	/* gtype */
	prop_attr->gtype = midgard_cr_model_property_get_valuegtype (model);
	/* default value */
	if (prop_attr->gtype != G_TYPE_OBJECT) {
		GValue sval = {0, };
		g_value_init (&sval, G_TYPE_STRING);
		g_value_set_string (&sval, midgard_cr_model_property_get_valuedefault (model));
		prop_attr->default_value = g_new0 (GValue, 1);	
		GType typeid = midgard_cr_core_schema_builder_gtype_from_name (prop_attr->type);
		g_value_init (prop_attr->default_value, typeid);
		g_value_transform (&sval, prop_attr->default_value);
	}
	/* description */
	prop_attr->description = g_strdup (midgard_cr_model_property_get_description (model));
	/* private */
	prop_attr->is_private = midgard_cr_model_property_get_private (model);

	/* Determine classname if property holds an object */
	/*
	gint n_models;
	MidgardCRModel **models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (model), &n_models);
	
	if (!models)
		return prop_attr;
	MidgardCRModel *parent = midgard_cr_model_get_parent (models[0]);

	if (!parent) {
		g_warning ("NULL parent model for given '%s' ObjectModelProperty's model", prop_attr->name);
		g_error ("Invalid model");
	}

	type_attr->extends = g_strdup (midgard_cr_model_get_name (MIDGARD_CR_MODEL (model)));	
	g_free (models);
	*/
	return prop_attr;
}

void
midgard_core_schema_type_property_attr_free (MgdSchemaPropertyAttr *prop)
{
	g_assert (prop != NULL);
	
	g_free ((gchar *)prop->type);
	prop->type = NULL;

	g_free ((gchar *)prop->name);
	prop->name = NULL;

	g_free ((gchar *)prop->link);
	prop->link = NULL;

	g_free ((gchar *)prop->link_target);
	prop->link_target = NULL;

	g_free ((gchar *)prop->description);
	prop->description = NULL;

	g_hash_table_destroy(prop->user_values);
	prop->user_values = NULL;

	g_free (prop);

	prop = NULL;
}

void 
midgard_cr_core_object_builder_register_types (MidgardCRObjectBuilder *builder, GError **error)
{
	g_return_if_fail (builder != NULL);
	g_return_if_fail (error == NULL || *error == NULL);

	guint i = 0;
	MidgardCRObjectModel **models = builder->_models;

	if (models == NULL || *models == NULL)
		g_error_new_literal (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL, 
				"No ObjectModel registered for execution");

	GSList *model_list = NULL;
	while (models[i] != NULL) {

		MgdSchemaTypeAttr *type_attr = midgard_cr_core_schema_type_new_from_model (MIDGARD_CR_MODEL (models[i]));
		model_list = g_slist_append (model_list, type_attr);

		/* initialize property attributes */
		guint n_props;
		guint j = 0;
		MidgardCRModel **property_models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (models[i]), &n_props);
		if (!property_models) {
			i++;
			continue;
		}
		
		type_attr->properties = g_new (MgdSchemaPropertyAttr*, n_props+1);
		type_attr->properties[n_props] = NULL;
		type_attr->num_properties = n_props;
		type_attr->params = g_new (GParamSpec *, n_props+1);

		while (property_models[j] != NULL) {
			MgdSchemaPropertyAttr *tmp_prop =
				midgard_cr_core_schema_type_property_new_from_model (MIDGARD_CR_MODEL_PROPERTY (property_models[j]), type_attr);
			GType ptype = tmp_prop->gtype;
			const gchar *property = (const gchar *)tmp_prop->name;
			const gchar *nick = "fixme";
			const gchar *descr = (const gchar *) tmp_prop->description;

			if (ptype == G_TYPE_STRING) {
				type_attr->params[j] = g_param_spec_string (
					property, nick, descr,
					"",  G_PARAM_READWRITE);
			} else if (ptype == MGD_TYPE_TIMESTAMP) {
				type_attr->params[j] = g_param_spec_boxed (
					property, nick, descr,
					MGD_TYPE_TIMESTAMP, G_PARAM_READWRITE);
			} else if (ptype == G_TYPE_UINT) {
				type_attr->params[j] = g_param_spec_uint (
					property, nick, descr,
					0, G_MAXUINT32, 0, G_PARAM_READWRITE);
			} else if (ptype == G_TYPE_INT) {
				type_attr->params[j] = g_param_spec_int (
					property, nick, descr,
					G_MININT32, G_MAXINT32, 0, G_PARAM_READWRITE);
			} else if (ptype == G_TYPE_FLOAT) {
				type_attr->params[j] = g_param_spec_float (
						property, nick, descr,
						-G_MAXFLOAT, G_MAXFLOAT, 0, G_PARAM_READWRITE);
			} else if (ptype == G_TYPE_BOOLEAN) {
				type_attr->params[j] = g_param_spec_boolean (
						property, nick, descr,
						FALSE, G_PARAM_READWRITE);
			} else if (ptype == G_TYPE_OBJECT) {
				/* Determine referenced object classname*/
				guint k;
				guint n_o_models;
				MidgardCRModel **o_models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (property_models[j]), &n_o_models);
				if (!o_models)
					g_error ("No model with ReferenceObject associated with '%s' property of Object type", property);
				GType o_type = g_type_from_name (midgard_cr_model_get_name (MIDGARD_CR_MODEL (o_models[0])));
				type_attr->params[j] = g_param_spec_object (
						property, nick, descr,
						o_type, G_PARAM_READWRITE);
			} else {
				type_attr->params[j] = g_param_spec_string (
					property, nick, descr,
					"", G_PARAM_READWRITE);
			}
			type_attr->properties[j] = tmp_prop;

			j++;
		}

		/* register classes in GType system */
		midgard_cr_core_schema_object_register_type (type_attr, MIDGARD_CR_TYPE_REPOSITORY_OBJECT);	
		i++;
	}
}
