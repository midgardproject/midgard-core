/* 
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_base_abstract.h"
#include "schema.h"
#include "midgard_core_object.h"

static void
__midgard_base_abstract_derived_class_set_property (GObject *object, guint prop_id,
		const GValue *value, GParamSpec   *pspec)
{
	gint prop_id_local = 0;
	GType current_type = G_TYPE_FROM_INSTANCE(object);
	MgdSchemaTypeAttr *priv = G_TYPE_INSTANCE_GET_PRIVATE (object, current_type, MgdSchemaTypeAttr);
	MidgardBaseAbstract *self = (MidgardBaseAbstract *) object;
	MgdSchemaPropertyAttr *prop;

	prop_id_local = prop_id - priv->base_index - 1;
	if ((prop_id_local >= 0) && (prop_id_local < priv->num_properties)) {
		if (priv->num_properties) {
			prop = priv->properties[prop_id_local];
			if (!G_IS_VALUE(&prop->value)) {
				g_value_init(&prop->value, G_VALUE_TYPE(value));
				/* g_debug(" Set property %s", pspec->name );  */
			}
			g_value_copy(value, &prop->value);
		}
		return;
	}
}

static void
__midgard_base_abstract_derived_class_get_property (GObject *object, guint prop_id,
		GValue *value, GParamSpec   *pspec)
{
	gint prop_id_local = 0;
	GType current_type = G_TYPE_FROM_INSTANCE(object);
	MgdSchemaTypeAttr *priv = G_TYPE_INSTANCE_GET_PRIVATE (object, current_type, MgdSchemaTypeAttr);
	MidgardBaseAbstract *self = (MidgardBaseAbstract *) object;
	GValue *pval;

	prop_id_local = prop_id - priv->base_index - 1;
	if ((prop_id_local >= 0) 
			&& (prop_id_local < priv->num_properties)) {
		if (priv->num_properties) {
			if (priv->properties) {
				pval = &priv->properties[prop_id_local]->value;
				if (G_IS_VALUE (pval)) {
					g_value_copy(pval, value);
				} 
			}
		}
	}	
}

static void
__midgard_base_abstract_derived_class_init(gpointer g_class, gpointer class_data)
{
	MgdSchemaTypeAttr *data = (MgdSchemaTypeAttr *) class_data;
	GObjectClass *gobject_class = G_OBJECT_CLASS(g_class);
	MidgardBaseAbstractClass *mklass = (MidgardBaseAbstractClass *) g_class;

	guint idx;	
	g_type_class_add_private (g_class, sizeof(MgdSchemaTypeAttr));
	
	gobject_class->set_property = __midgard_base_abstract_derived_class_set_property;
	gobject_class->get_property = __midgard_base_abstract_derived_class_get_property;

	for (idx = 1; idx <= data->num_properties; idx++) {
		/* g_print ("Installing property id %d :: %s \n", idx, data->params[idx-1]->name);  */
		g_object_class_install_property (gobject_class,
				data->base_index + idx ,
				data->params[idx-1]);
	}
}

GType
midgard_type_register_abstract (MgdSchemaTypeAttr *type_data, GType parent_type)
{
	gchar *classname = type_data->name;

	GType class_type = g_type_from_name (classname);

        if (class_type) 
                return class_type;

        {
                GTypeInfo *midgard_type_info = g_new0 (GTypeInfo, 1);

                /* our own class size is 0 but it should include space for a parent, therefore add it */
                midgard_type_info->class_size = sizeof (MidgardBaseAbstractClass);
                midgard_type_info->base_init = NULL;
                midgard_type_info->base_finalize = NULL;
                midgard_type_info->class_init  = __midgard_base_abstract_derived_class_init;
                midgard_type_info->class_finalize  = NULL;
                midgard_type_info->class_data = type_data;
                /* our own instance size is 0 but it should include space for a parent,
                 * therefore add it */
                midgard_type_info->instance_size = sizeof (MidgardBaseAbstract);
                midgard_type_info->n_preallocs = 0;
                midgard_type_info->instance_init = NULL;
                midgard_type_info->value_table = NULL;

 		GType tmp_type;		
		guint n_types;
		guint i;
		gchar **extends = midgard_core_schema_type_list_extends (type_data, &n_types);
		GType real_parent_type = MIDGARD_TYPE_BASE_ABSTRACT;

		for (i = 0; i < n_types; i++) {
			tmp_type = g_type_from_name (extends[i]);
			if (!G_TYPE_IS_INTERFACE (tmp_type) && tmp_type != G_TYPE_INVALID) { 
				real_parent_type = tmp_type;
				continue;
			}
		}

		GType type = g_type_register_static (real_parent_type, classname, midgard_type_info, G_TYPE_FLAG_ABSTRACT);
		
		static const GInterfaceInfo iface_info = {
			NULL,	/* interface initialize */
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		for (i = 0; i < n_types; i++) {
			tmp_type = g_type_from_name (extends[i]);
			/* Implicitly register interface, if such is not registered in GType system.
			 * If it's defined in schema we collected all rquired metadata.
			 * If not, we have to throw error */
			if (g_type_from_name (extends[i]) == G_TYPE_INVALID) {
				MgdSchemaTypeAttr *iface_attr = midgard_schema_lookup_type (type_data->schema, extends[i]);
				if (!iface_attr)
					g_error ("Can not find '%s' definition", extends[i]);
				if (iface_attr->is_iface)
					tmp_type = midgard_core_type_register_interface (iface_attr);
			}

			midgard_core_interface_add_prerequisites (type, tmp_type);				
			g_type_add_interface_static (type, tmp_type, &iface_info);
		}

		if (extends)
			g_strfreev (extends);
                g_free (midgard_type_info);

                return type;   
        }                      
}

/* MIDGARD_BASE_ABSTRACT ABSTRACT CLASS ROUTINES */

static GObjectClass *__parent_class= NULL;

static void 
_midgard_base_abstract_class_init (gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardBaseAbstractClass *klass = MIDGARD_BASE_ABSTRACT_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->dispose = __parent_class->dispose;
	gobject_class->finalize = __parent_class->finalize;
	gobject_class->set_property = NULL;
	gobject_class->get_property = NULL;
}

GType 
midgard_base_abstract_get_type (void)
{
       	static GType type = 0;
 
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardBaseAbstractClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_base_abstract_class_init,	
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardBaseAbstract),
			0,              /* n_preallocs */
			NULL		/* instance_init */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardBaseAbstract", &info, G_TYPE_FLAG_ABSTRACT);
	}

	return type;
}

