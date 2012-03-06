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

#include "midgard_schema_object_tree.h"
#include "midgard_error.h"
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include "midgard_reflector_object.h"
#include "midgard_reflector_property.h"
#include "midgard_datatypes.h"

gboolean _midgard_tree_exists(MidgardConnection *mgd, 
		const gchar *table, const gchar *upfield, 
		gint root, gint id)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(table != NULL, FALSE);
	g_return_val_if_fail(upfield != NULL, FALSE);

	if(id == 0) {
		g_warning("tree_exists: expected id > 0");
		return FALSE;
	}

	gint _id = id;
	gint _root = root;
	gboolean _exists = FALSE;

	do {

		GString *where = g_string_new("");
		g_string_append_printf(where, "id = %d ", _id);

		GValue *value = 
			midgard_core_query_get_field_value(mgd, 
					upfield, table, where->str);
		g_string_free(where, TRUE);

		if(G_IS_VALUE(value)) {
			if(G_VALUE_HOLDS_INT(value))
				_id = g_value_get_int(value);
			if(G_VALUE_HOLDS_UINT(value))
				_id = g_value_get_uint(value);
		}

		if(_root == _id)
			return TRUE;

		if(_id == 0)
			return FALSE;

	} while (_id > 0);

	return _exists;
}

gboolean midgard_object_is_in_parent_tree(MidgardObject *self, guint rootid, guint id)
{
	g_return_val_if_fail(self != NULL, FALSE);

	if (!MGD_OBJECT_CNC (self)) {
		g_warning("Object has no connection pointer!");
		return FALSE;
	}

	MidgardConnection *mgd = MGD_OBJECT_CNC (self);

	MidgardObjectClass *parent_class = 
		MIDGARD_OBJECT_GET_CLASS_BY_NAME (midgard_schema_object_tree_get_parent_name (self));

	const gchar *parent_table, *up_field;
	const gchar *parent_field;

	MgdSchemaTypeAttr *type_attr = 
		midgard_core_class_get_type_attr(MIDGARD_DBOBJECT_GET_CLASS(self));
	parent_field = type_attr->parentfield;

	if(parent_field == NULL) {
		parent_field = type_attr->upfield;
	}

	parent_table = 
		midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(parent_class));
	up_field = MIDGARD_DBOBJECT_CLASS (parent_class)->dbpriv->storage_data->upfield;

	GString *where = g_string_new("");
	g_string_append_printf(where, "id = %d ", id);
	
	GValue *value =
		midgard_core_query_get_field_value(mgd,
				parent_field , type_attr->table, where->str);
	g_string_free(where, TRUE);
	
	guint _id = 0;

	if(G_IS_VALUE(value)) {
		if(G_VALUE_HOLDS_INT(value))
			_id = g_value_get_int(value);
		if(G_VALUE_HOLDS_UINT(value))
			_id = g_value_get_uint(value);
	}

	if(!parent_table)
		return FALSE;
	if(!up_field)
		return FALSE;

	if(_id == 0)
		return FALSE;

	if (_id == rootid)
		return TRUE;

	gboolean rv = 
		_midgard_tree_exists(MGD_OBJECT_CNC (self), parent_table, 
				up_field, rootid, _id);

	return rv;
}

gboolean midgard_object_is_in_tree(MidgardObject *self, guint rootid, guint id)
{
	g_return_val_if_fail(self != NULL, FALSE);
	
	if(!MGD_OBJECT_CNC (self)) {
		g_warning("Object has no connection pointer!");
		return FALSE;
	}

	const gchar *table = 
		midgard_core_class_get_table(MIDGARD_DBOBJECT_GET_CLASS(self));
	const gchar *upfield = MIDGARD_DBOBJECT (self)->dbpriv->storage_data->upfield;

	if(table == NULL || upfield == NULL)
		return FALSE;

	gboolean rv =
		_midgard_tree_exists(MGD_OBJECT_CNC (self), table,
				upfield, rootid, id);
	
	return rv;
}

void __midgard_tree_get_id_list(MidgardConnection *mgd, 
		const gchar *table, const gchar *field, 
		guint id, GList **idlist)
{
	GString *query = g_string_new("SELECT ");
	g_string_append_printf(query, 
			"id FROM %s WHERE %s = %d ",	
			table, field, id);
	
	GdaDataModel *model = 
		midgard_core_query_get_model(mgd, query->str);
	g_string_free(query, TRUE);

	if(!model)
		return;

	gint ret_rows = gda_data_model_get_n_rows(model);
	guint rows;
	const GValue *value;
	guint retid = 0;
	MidgardTypeHolder *holder;

	if(ret_rows == 0) {
		g_object_unref(model);
		return;
	}
	
	for(rows = 0; rows < ret_rows; rows++) {
		
		value = midgard_data_model_get_value_at(model, 0, rows);
		
		if(G_IS_VALUE(value)) {
			
			if(G_VALUE_TYPE(value) == G_TYPE_INT)
				retid = g_value_get_int(value);
			if(G_VALUE_TYPE(value) == G_TYPE_UINT)
				retid = g_value_get_uint(value);

			if(retid > 0) {
				
				holder = g_new(MidgardTypeHolder, 1);
				holder->level = (guint)retid;
			
				*idlist = g_list_append(*idlist, holder);
				__midgard_tree_get_id_list(mgd,
						table, field, retid, idlist);
			}
		}
	}

	g_object_unref(model);

	return;
}

guint *_midgard_tree_ids(MidgardConnection *mgd, 
		MidgardObjectClass *klass, guint startid)
{
	g_assert(mgd != NULL);
	g_assert(klass != NULL);

	const gchar *table = midgard_core_class_get_table (MIDGARD_DBOBJECT_CLASS(klass));
	const gchar *pcol, *pprop;
	const gchar *classname = G_OBJECT_CLASS_NAME (klass);
	pprop = midgard_reflector_object_get_property_parent(classname);

	if(pprop == NULL) {
		pprop = MGD_DBCLASS_PROPERTY_UP (klass);
	} else {
		MidgardReflectorProperty *mrp = midgard_reflector_property_new (classname);
		const MidgardDBObjectClass *dbklass = midgard_reflector_property_get_link_class (mrp, pprop);
		g_object_unref (mrp);
		return _midgard_tree_ids (mgd, MIDGARD_OBJECT_CLASS (dbklass), startid);
	}

	pcol = midgard_core_class_get_property_colname(MIDGARD_DBOBJECT_CLASS(klass), pprop);

	GList *idlist = NULL;
	MidgardTypeHolder *holder = g_new(MidgardTypeHolder, 1);
	holder->level = startid;
	idlist = g_list_append(idlist, holder);
	
	__midgard_tree_get_id_list(mgd, table, pcol, startid, &idlist);

	guint i = g_list_length(idlist);
	guint *treeid = g_new(guint, i+1);
	GList *l = NULL;
	i = 0;
	for(l = idlist; l != NULL; l = l->next) {
		MidgardTypeHolder *holder = (MidgardTypeHolder *)l->data;
		treeid[i] = holder->level;
		i++;
	}
	treeid[i] = -1;

	g_list_free(idlist);

	return treeid;
}

guint*
midgard_core_query_get_tree_ids (MidgardConnection *mgd, MidgardObjectClass *klass, guint tid)
{
	return _midgard_tree_ids (mgd, klass, tid);
}

/**
 * midgard_schema_object_tree_is_in_tree:
 * @object: #MidgardObject instance to check existance in tree
 * @parent_object: parent #MidgardObject instance 
 *
 * Returns: %TRUE, if given @object exists under @parent_object in tree. %FALSE otherwise.
 *
 * Since: 10.05
 */ 
gboolean
midgard_schema_object_tree_is_in_tree (MidgardObject *object, MidgardObject *parent_object)
{
	g_return_val_if_fail (object != NULL, FALSE);
	g_return_val_if_fail (parent_object != NULL, FALSE);

	const gchar *parent_classname = midgard_schema_object_tree_get_parent_name (object);
	guint oid;
	g_object_get (object, "id", &oid, NULL);
	guint parent_oid;
	g_object_get (parent_object, "id", &parent_oid, NULL);

	if (oid == 0)
		return FALSE;

	if (!parent_classname) 
		return midgard_object_is_in_tree (object, parent_oid, oid);

	if (g_str_equal (parent_classname, G_OBJECT_TYPE_NAME (parent_object)))
		return midgard_object_is_in_parent_tree (object, parent_oid, oid);

	return FALSE;
}

/**
 * midgard_schema_object_tree_get_parent_name:
 * @object: #MidgardObject instance
 *
 * Returns: (transfer none): classname which, in tree is a parent one for given @object 
 * Since: 10.05
 */ 
const gchar*
midgard_schema_object_tree_get_parent_name (MidgardObject *object)
{
	g_return_val_if_fail (object != NULL, NULL);

	if (MIDGARD_DBOBJECT (object)->dbpriv->storage_data->parent)
		return MIDGARD_DBOBJECT (object)->dbpriv->storage_data->parent;

	return NULL;
}

/**
 * midgard_schema_object_tree_get_parent_object:
 * @object: #MidgardObject instance
 *
 * Get tree parent object, of the given @object.
 *
 * Returns: (transfer full): parent object or %NULL
 * Since: 10.05
 */ 
MidgardObject*
midgard_schema_object_tree_get_parent_object (MidgardObject *object)
{
	g_return_val_if_fail (object != NULL, NULL);

	MidgardObject *mobj = object;
        MidgardObject *pobj = NULL;
	MidgardObject *self = object;
        const gchar *pcstring;
        guint puint = 0;
        gint pint = 0;
        GParamSpec *fprop = NULL;
        GValue pval = {0,};
        gboolean ret_object = FALSE;
        const gchar *parent_class_name = NULL;
        MidgardConnection *mgd = MGD_OBJECT_CNC (self); 
	const gchar *classname = G_OBJECT_TYPE_NAME (self);
	
        const gchar *property_up = midgard_reflector_object_get_property_up (classname);

        if (property_up) {

		fprop = g_object_class_find_property( G_OBJECT_GET_CLASS(mobj), property_up);
		MidgardReflectorProperty *mrp = midgard_reflector_property_new (classname);
		if (midgard_reflector_property_is_link (mrp, property_up)) {

			parent_class_name = midgard_reflector_property_get_link_name (mrp, property_up);
   
			if (parent_class_name)
				pobj = midgard_object_new (mgd, parent_class_name, NULL);

			g_object_unref (mrp);
		}	

		if (!pobj)
			return NULL;
	}

	if (fprop) {
		
		g_value_init(&pval,fprop->value_type);
		g_object_get_property(G_OBJECT(mobj), 
				midgard_reflector_object_get_property_up(classname), &pval);
		
		switch(fprop->value_type) {
			
			case G_TYPE_STRING:
				
				if ((pcstring = g_value_get_string(&pval)) != NULL) {
					
					ret_object = TRUE;
					if (!midgard_object_get_by_guid(pobj, pcstring)) {
						g_object_unref(pobj);
						pobj = NULL;
					}
				}
				break;

			case G_TYPE_UINT:
				
				if ((puint = g_value_get_uint(&pval))) {

					ret_object = TRUE;
					if (!midgard_object_get_by_id(pobj, puint)) {
						g_object_unref(pobj);
						pobj = NULL;
					}
				}				
				break;

				
			case G_TYPE_INT:

				if ((pint = g_value_get_int(&pval))) {

					ret_object = TRUE;
					if (!midgard_object_get_by_id(pobj, pint)) {
						g_object_unref(pobj);
						pobj = NULL;
					}
				}
				break;
		}
		
		g_value_unset(&pval);

		if (ret_object) {
			return pobj;
		} else {
			g_object_unref(pobj);
			pobj = NULL;
		}
	}
        
        /* I do make almost the same for property_parent, because I want to 
	 * avoid plenty of warnings after G_VALUE_HOLDS which could be used 
	 * with  value returned for mobj->priv->storage_data->tree->property_up 
	 */ 

	if (midgard_reflector_object_get_property_parent(classname) == NULL)
		return NULL;

	parent_class_name = midgard_schema_object_tree_get_parent_name (self);
	if (!parent_class_name)
		return NULL;

	pobj =  midgard_object_new(MGD_OBJECT_CNC (self) , MIDGARD_DBOBJECT (mobj)->dbpriv->storage_data->parent, NULL);
	
	if (pobj == NULL)
		return NULL;
	
	fprop = g_object_class_find_property(
			G_OBJECT_GET_CLASS(mobj),
			midgard_reflector_object_get_property_parent(classname));

	if (!fprop)
		return NULL;
       
        g_value_init(&pval,fprop->value_type);
        g_object_get_property(G_OBJECT(mobj), 
			midgard_reflector_object_get_property_parent(classname) , &pval);

        switch(fprop->value_type) {
            
            case G_TYPE_STRING:
		    
		    if ((pcstring = g_value_get_string(&pval)) != NULL) {
			    
			    if (!midgard_object_get_by_guid(pobj, pcstring)) {
				    g_object_unref(pobj);
				    pobj = NULL;
			    }
		    }
		    break;

            case G_TYPE_UINT:
		    
		    if ((puint = g_value_get_uint(&pval))) {

			    if (!midgard_object_get_by_id(pobj, puint)) {
				    g_object_unref(pobj);
				    pobj = NULL;
			    }
		    }
		    break;

	    case G_TYPE_INT:
		    
		    if ((pint = g_value_get_int(&pval))) {

			    if (!midgard_object_get_by_id(pobj, pint)) {
				    g_object_unref(pobj);
				    pobj = NULL;
			    }
		    }
		    break;


        }
        
        g_value_unset(&pval);
        
        return pobj;
}

/**
 * midgard_schema_object_tree_list_objects:
 * @object: #MidgardObject instance
 * @n_objects: (out):pointer to store number of returned objects
 *
 * List tree children objects, of given @object type.
 *
 * Returns: (array length=n_objects) (transfer full): newly allocated array of #MidgardObject objects
 * Since: 10.05
 */ 
MidgardObject**
midgard_schema_object_tree_list_objects (MidgardObject *object, guint *n_objects)
{
	g_return_val_if_fail (object != NULL, NULL);

	*n_objects = 0;
	GParamSpec *fprop;
	const gchar *classname = G_OBJECT_TYPE_NAME (object);

	MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (object), MGD_ERR_OK);

	const gchar *primary_prop = midgard_reflector_object_get_property_primary(classname);
	const gchar *up_property = midgard_reflector_object_get_property_up(classname);
	if (up_property == NULL) 
		return NULL;

	fprop = g_object_class_find_property(
			G_OBJECT_GET_CLASS(G_OBJECT(object)), primary_prop);
	if (!fprop)
		return NULL;

	GValue pval = {0,};
	g_value_init(&pval,fprop->value_type);
	g_object_get_property(G_OBJECT(object), primary_prop, &pval);

	/* If primary property holds uint and its value is 0, do not try to return objects */
	if (G_VALUE_HOLDS_UINT(&pval)) {
		if (g_value_get_uint(&pval) == 0) {
			MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (object), MGD_ERR_NOT_EXISTS);
			return NULL;
		}
	}

	if (g_object_class_find_property(
				G_OBJECT_GET_CLASS(G_OBJECT(object)),
				up_property) == NULL ) {
		MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (object), MGD_ERR_NOT_EXISTS);
		return NULL;
	}
	
	GError *err = NULL;
	MidgardDBObject **objects = midgard_core_query_get_objects (MGD_OBJECT_CNC (object), 
			classname, &err, up_property, &pval, NULL);

	if (objects) {
		guint i = 0;
		while (objects[i] != NULL)
			i++;
		*n_objects = i;
		return (MidgardObject **) objects;
	}

	if (err) {
		MIDGARD_ERRNO_SET_STRING (MGD_OBJECT_CNC (object), MGD_ERR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
	}

	return NULL;
}

/**
 * midgard_schema_object_tree_list_children_objects:
 * @object: #MidgardObject instance
 * @classname: name of the tree child class
 * @n_objects: (out): pointer to store number of returned objects
 *
 * List all @classname objects, if exist and are tree children of given @object.
 *
 * Returns: (array length=n_objects) (transfer full): array of #MidgardObject objects, or %NULL.
 * Since: 10.05
 */ 
MidgardObject**
midgard_schema_object_tree_list_children_objects (MidgardObject *object, const gchar *classname, guint *n_objects)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (classname != NULL, NULL);

	*n_objects = 0;
	GParamSpec *fprop ;
	const gchar *primary_prop = MIDGARD_DBOBJECT (object)->dbpriv->storage_data->primary;

	MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (object), MGD_ERR_OK);
	if ((classname == NULL) || (MIDGARD_DBOBJECT (object)->dbpriv->storage_data->children == NULL)) {
		MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (object), MGD_ERR_NOT_EXISTS);    
		return NULL;
	}

	GSList *list;
	GSList *children = MIDGARD_DBOBJECT (object)->dbpriv->storage_data->children;
	gboolean found = FALSE;

	for (list = children; list != NULL; list = list->next) {
		if (list->data && g_str_equal(list->data, classname))
			found = TRUE;
	}

	if (!found) {
		MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (object), MGD_ERR_NOT_EXISTS);
		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
				"Child type (%s) is not a child type of (%s)", 
				classname, G_OBJECT_TYPE_NAME(object));
		return NULL;
	}

	const gchar *property_parent = midgard_reflector_object_get_property_parent(classname);
	if (property_parent == NULL) {
		MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (object), MGD_ERR_NOT_EXISTS);
		return NULL;
	}

	fprop = g_object_class_find_property(G_OBJECT_GET_CLASS(object),primary_prop);
	if (fprop == NULL)
		return NULL;

	GValue pval = {0,};
	g_value_init(&pval,fprop->value_type);
	g_object_get_property(G_OBJECT(object), primary_prop, &pval);

	GError *err = NULL;
	MidgardDBObject **objects = midgard_core_query_get_objects (MGD_OBJECT_CNC (object),
			classname, &err, property_parent, &pval, NULL);
	if (objects) {
		guint i = 0;
		while (objects[i] != NULL)
			i++;
		*n_objects = i;
		return (MidgardObject **) objects;
	}

	if (err) {
		MIDGARD_ERRNO_SET_STRING (MGD_OBJECT_CNC (object), MGD_ERR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
	}

	return NULL;
}

/* GOBJECT ROUTINES */

GType
midgard_schema_object_tree_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardSchemaObjectTreeClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			NULL,		/* class init */
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSchemaObjectTree),
			0,              /* n_preallocs */
			NULL
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSchemaObjectTree", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
