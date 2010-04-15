
/* WARNING ! This is temporary API ! WARNING */
#include "midgard_defs.h"
#include "midgard_datatypes.h"
#include "midgard_tree.h"
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"

gboolean _midgard_tree_exists(MidgardConnection *mgd, 
		const gchar *table, const gchar *upfield, 
		gint root, gint id)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(table != NULL, FALSE);
	g_return_val_if_fail(upfield != NULL, FALSE);

	if(root == id) {
		g_warning("tress_exists: expected root != id");
		return FALSE;
	}

	if(root == 0 || id == 0) {
		g_warning("tree_exists: root(%d), id(%d). Expected root > 0 and id > 0",
				root, id);
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

/**
 * midgard_object_is_in_parent_tree:
 * @self: #MidgardObject instance
 * @rootid: guint which identifies parent's type primary property
 * @id: guint which identify object's primary property
 *
 * Checks whether object exists in parent's type tree.
 * 
 * <ulink linkend="http://www.midgard-project.org/midcom-permalink-a4e185a08fb2d0e278ef1ba3a739f77e">Midgard tree in #MgdSchema reference</ulink>
 *
 * Parent type in midgard tree is a type which can be "container" with object nodes
 * of different types. 
 *
 * Returns: %TRUE if object exists in parent tree, %FALSE otherwise.
 */ 
gboolean midgard_object_is_in_parent_tree(MidgardObject *self, guint rootid, guint id)
{
	g_return_val_if_fail(self != NULL, FALSE);

	if (!MGD_OBJECT_CNC (self)) {
		g_warning("Object has no connection pointer!");
		return FALSE;
	}

	MidgardConnection *mgd = MGD_OBJECT_CNC (self);

	MidgardObjectClass *parent_class = MIDGARD_OBJECT_GET_CLASS_BY_NAME (midgard_object_parent(self));

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

	gboolean rv = 
		_midgard_tree_exists(MGD_OBJECT_CNC (self), parent_table, 
				up_field, rootid, _id);

	return rv;
}

/**
 * midgard_object_is_in_tree:
 * @self: #MidgardObject instance
 * @rootid: guint value which identifies type's primary property
 * @id: guint value which identifies object's primary property
 *
 * Checks whether object exists in tree ( of the same type ).
 *
 * Midgard tree in MgdSchema reference:
 * http://www.midgard-project.org/midcom-permalink-a4e185a08fb2d0e278ef1ba3a739f77e
 * 
 * This function checks only objects of the same type.
 * 
 * Returns: %TRUE if object exists in tree, %FALSE otherwise. 
 */
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

	const gchar *table = midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(klass));
	const gchar *pcol, *pprop;
	const gchar *classname = G_OBJECT_CLASS_NAME (klass);
	pprop = midgard_reflector_object_get_property_parent(classname);

	if(pprop == NULL)
		pprop = MGD_DBCLASS_PROPERTY_UP (klass);

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
