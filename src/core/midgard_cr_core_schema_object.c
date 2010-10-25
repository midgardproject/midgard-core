/* 
Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Piotr Pokora <piotrek.pokora@gmail.com>
Copyright (C) 2004 Alexander Bokovoy <ab@samba.org>

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

#include "midgard_cr_core_schema_object.h"

static GObjectClass *__core_schema_object_parent_class = NULL;
static GObject *__core_schema_object_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties);
static void __core_schema_object_dispose (GObject *object);
               
/* AB: This is shortcut macro for traversing through class hierarchy (from child to ancestor)
 * until we reach GObject. On each iteration we fetch private data storage for the object
 * according to the current class type and run supplied code.
 * This allows us to implement dynamic subclassing cast which is done usually via compiled 
 * time structure casts in appropriate class-specific hooks for each class in a hierarchy.
 * As our object classes have the same structure at each level of inheritance, we are able
 * to use the same triple of functions for get_property(), set_property(), and finalize()
 * for each class.
 */
#define G_MIDGARD_LOOP_HIERARCHY_START	\
	do {	\
		if (current_type == MIDGARD_CR_TYPE_REPOSITORY_OBJECT) break; \
		priv = G_TYPE_INSTANCE_GET_PRIVATE (object, current_type, MgdSchemaTypeAttr);  \

#define G_MIDGARD_LOOP_HIERARCHY_STOP	\
		current_type = g_type_parent(current_type);	\
	} while (current_type != MIDGARD_CR_TYPE_REPOSITORY_OBJECT);

/* AB: Handle property assignments. Despite its simplicity, this is *very* important function */
static void
__core_schema_object_set_property (GObject *object, guint prop_id, 
    const GValue *value, GParamSpec   *pspec)
{	
	gint prop_id_local = 0;
	GType current_type = G_TYPE_FROM_INSTANCE(object);
	MgdSchemaTypeAttr *priv = G_TYPE_INSTANCE_GET_PRIVATE (object, current_type, MgdSchemaTypeAttr);
	MgdSchemaPropertyAttr *prop;

	switch (prop_id) {						
		default:
			G_MIDGARD_LOOP_HIERARCHY_START	
			//prop_id_local = prop_id - klass_priv->base_index - 1;
			prop_id_local = prop_id;
			if ((prop_id_local >= 0) && (prop_id_local < priv->num_properties)) {
				if (priv->num_properties) {
					prop = priv->properties[prop_id_local];
					if (!G_IS_VALUE(&prop->value)) {
						g_value_init(&prop->value, G_VALUE_TYPE(value));	
					}
					g_value_copy(value, &prop->value);
				}
				return;
			}
			G_MIDGARD_LOOP_HIERARCHY_STOP
	}
}

/* Get object's property */
static void
__core_schema_object_get_property (GObject *object, guint prop_id,
		GValue *value, GParamSpec   *pspec)
{
	gint prop_id_local = 0;
	GType current_type = G_TYPE_FROM_INSTANCE(object);
	MgdSchemaTypeAttr *priv = G_TYPE_INSTANCE_GET_PRIVATE (object, current_type, MgdSchemaTypeAttr);
	GValue *pval;

	switch (prop_id) {	
		default:
			G_MIDGARD_LOOP_HIERARCHY_START
			//prop_id_local = prop_id - priv->base_index - 1;
			prop_id_local = prop_id;
			if ((prop_id_local >= 0) && (
						prop_id_local < priv->num_properties)) {
				if (priv->num_properties) {
					if (priv->properties) {
						pval = &priv->properties[prop_id_local]->value;
						if (G_IS_VALUE (pval)) {	
							g_value_copy(pval, value);			
						} 
					}
				}
				return;
			}
			G_MIDGARD_LOOP_HIERARCHY_STOP
	}
}

static GObject *
__core_schema_object_constructor (GType type, guint n_construct_properties, GObjectConstructParam *construct_properties)
{
	GObject *object =
		G_OBJECT_CLASS (__core_schema_object_parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	MgdSchemaTypeAttr *klass_priv = G_TYPE_CLASS_GET_PRIVATE (G_OBJECT_GET_CLASS (object), type, MgdSchemaTypeAttr);
	MgdSchemaTypeAttr *priv = G_TYPE_INSTANCE_GET_PRIVATE ((GTypeInstance*)object, type, MgdSchemaTypeAttr);

	priv->base_index = 1;
	priv->num_properties = klass_priv->num_properties;

	/* Allocate properties storage for this instance  */
	priv->properties = priv->num_properties ? g_new0 (MgdSchemaPropertyAttr*, priv->num_properties) : NULL;

	if (priv->properties) {
		guint idx;
		for (idx = 0; idx < priv->num_properties; idx++) {
			priv->properties[idx] = g_new0 (MgdSchemaPropertyAttr, 1);
		}
	}
	return object;
}

static void 
__core_schema_object_dispose (GObject *object)
{
	__core_schema_object_parent_class->dispose (object);
}

/* 
 * Finalizer for MidgardCRRepositoryObject instance.
 * Cleans up all allocated data. As optimization, it handles all data
 * which belongs to its ancestors up to but not inluding GObject.
 * It is really makes no sense to call this function recursively
 * for each ancestor because we already know object's structure.
 * For GObject we call its finalizer directly.
 */
static void __core_schema_object_finalize (GObject *object) 
{
	guint idx;
	MgdSchemaTypeAttr *priv;

	if (object == NULL)
		return;

	MidgardCRRepositoryObject *self = (MidgardCRRepositoryObject *)object;
		
	GType current_type = G_TYPE_FROM_INSTANCE(object); 

	G_MIDGARD_LOOP_HIERARCHY_START
		for (idx = 0; idx < priv->num_properties; idx++) {
			if (priv->properties[idx]) {
				if (G_IS_VALUE(&priv->properties[idx]->value)) {
					g_value_unset(&priv->properties[idx]->value);
				}
				g_free (priv->properties[idx]);
				priv->properties[idx] = NULL;
			}
		}
	if (priv->properties) {
		g_free (priv->properties);
		priv->properties = NULL;
	}
	G_MIDGARD_LOOP_HIERARCHY_STOP 
		
	/* Call parent's finalizer if it is there */
	{
		GObjectClass *parent_class = g_type_class_ref (current_type);		
		if (parent_class->finalize) {
			parent_class->finalize (object);
		}
		g_type_class_unref (parent_class);
	}
}

/* Initialize class. 
 * Properties setting follow data in class_data.
 */ 
static void
__core_schema_object_class_init(gpointer g_class, gpointer class_data)
{
	MgdSchemaTypeAttr *data = (MgdSchemaTypeAttr *) class_data;

	GObjectClass *gobject_class = G_OBJECT_CLASS(g_class);
	MidgardCRRepositoryObjectClass *mklass = (MidgardCRRepositoryObjectClass *) g_class;
	guint idx;
	
	__core_schema_object_parent_class = g_type_class_peek_parent (g_class);

	gobject_class->set_property = __core_schema_object_set_property;
	gobject_class->get_property = __core_schema_object_get_property;
	gobject_class->finalize = __core_schema_object_finalize;
	gobject_class->constructor = __core_schema_object_constructor;
        gobject_class->dispose = __core_schema_object_dispose;	

	g_type_class_add_private (g_class, sizeof(MgdSchemaTypeAttr));
	
	/* List parent class properties so we can set current class base_index */
	guint n_prop;
	GParamSpec **pspecs =
		g_object_class_list_properties (g_type_class_peek_parent(g_class), &n_prop);
	g_free(pspecs);

	if (data)
		data->properties = g_malloc (sizeof (MgdSchemaPropertyAttr*) *(data->num_properties + 1));
	
	data->base_index = n_prop;

	/* Note, that we start numbering from 1 , not from 0. property_id must be > 0 */
	for (idx = 1; idx <= data->num_properties; idx++) {
		g_object_class_install_property ( gobject_class, data->base_index + idx , data->params[idx-1]);
	}

	/* class_data is available in class init/finalize only.
	 * Get class private structure and set valid values */
	MgdSchemaTypeAttr *type_attr = g_type_class_get_private ((GTypeClass*)mklass, G_OBJECT_CLASS_TYPE (mklass));
	type_attr->num_properties = data->num_properties + n_prop + 1;
	type_attr->base_index = data->base_index;
}

GType
midgard_cr_core_schema_object_register_type (MgdSchemaTypeAttr *type_data, GType parent_type)
{
	gchar *classname = type_data->name;
	GType class_type = g_type_from_name (classname);
        if (class_type) 
                return class_type;

        {
                GTypeInfo *midgard_type_info = g_new0 (GTypeInfo, 1);

		/* FIXME, does it make sense? */
		if (type_data == NULL)
			type_data = g_new (MgdSchemaTypeAttr, 1);

                /* our own class size is 0 but it should include space for a parent, therefore add it */
                midgard_type_info->class_size = sizeof (MidgardCRRepositoryObjectClass);
                midgard_type_info->base_init = NULL;
                midgard_type_info->base_finalize = NULL;
                midgard_type_info->class_init  = __core_schema_object_class_init;
                midgard_type_info->class_finalize  = NULL;
                midgard_type_info->class_data = type_data;
                /* our own instance size is 0 but it should include space for a parent,
                 * therefore add it */
                midgard_type_info->instance_size = sizeof (MidgardCRRepositoryObject);
                midgard_type_info->n_preallocs = 0;
                midgard_type_info->instance_init = NULL;
                midgard_type_info->value_table = NULL;
               
		static const GInterfaceInfo property_info = {
			NULL,	/* interface init */
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		GType type = g_type_register_static (parent_type, classname, midgard_type_info, 0);
		g_type_add_interface_static (type, MIDGARD_CR_TYPE_STORABLE, &property_info);

                g_free (midgard_type_info);

		g_type_add_class_private (type, sizeof(MgdSchemaTypeAttr));

                return type;   
        }                      
}
