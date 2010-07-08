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

#include "midgard_workspace.h"
#include "midgard_core_workspace.h"
#include "midgard_core_object.h"
#include "midgard_core_query.h"
#include "guid.h"

GQuark 
midgard_workspace_error_quark (void)
{
	return g_quark_from_static_string ("midgard_workspace_error-quark");
}

/**
 * midgard_wrokspace_new:
 * @mgd: #MidgardConnection instance
 * @parent_workspace: (allow none): parent #MidgardWorkspace for given one
 *
 * If created, new workspace will be created under @parent_workspace if given not %NULL.
 * In other case, new instance is assumed top most workspace.
 *
 * Returns: #MidgardWorkspace object or %NULL
 * Since: 10.11
 */ 
MidgardWorkspace *
midgard_workspace_new (MidgardConnection *mgd, MidgardWorkspace *parent_workspace)
{
	g_return_val_if_fail (mgd != NULL, NULL);

	MidgardWorkspace *self = NULL;
	if (!parent_workspace)
		self = g_object_new (MIDGARD_TYPE_WORKSPACE, "connection", mgd, NULL);
	else
		self = g_object_new (MIDGARD_TYPE_WORKSPACE, "connection", mgd, "parent_workspace", parent_workspace, NULL);

	return self;
}

/**
 * midgard_workspace_create:
 * @self: #MidgardWorkspace instance
 * @error: pointer to store returned error 
 *
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * workspace with such name (either root or child one) already exists ( WORKSPACE_ERROR_NAME_EXISTS )
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.11
 */
gboolean 
midgard_workspace_create (MidgardWorkspace *self, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	MidgardConnection *mgd = MGD_OBJECT_CNC (self);
	g_return_val_if_fail (MIDGARD_IS_CONNECTION (mgd), FALSE);

	/* TODO, emit create signal */

	GError *er = NULL;
	if (midgard_core_workspace_name_exists (self, self->priv->parent_ws)) {
		er = g_error_new (MIDGARD_WORKSPACE_ERROR, WORKSPACE_ERROR_NAME_EXISTS, 
				"Workspace at path '%s' already exists", midgard_workspace_get_path (self));
		g_propagate_error (error, er);
		return FALSE;
	}

	MidgardWorkspace *parent_ws = self->priv->parent_ws;
	if (parent_ws) {
		self->priv->up_id = parent_ws->priv->id;
	}

	/* Create */
	MIDGARD_DBOBJECT(self)->dbpriv->guid = (const gchar *) midgard_guid_new (mgd);
	if (midgard_core_query_create_dbobject_record (MIDGARD_DBOBJECT (self))) {
		/* TODO ?, emit created signal */
		midgard_core_workspace_list_all (mgd);
		return TRUE;
	}

	/* Create failed, reset values */
	g_free ((gchar *)MGD_OBJECT_GUID (self));
	MGD_OBJECT_GUID (self) = NULL;

	self->priv->up_id = 0;

	/* FIXME, Set internal error */
	return FALSE;
}

/**
 * midgard_workspace_get_path:
 * @self: #MidgardWorkspace instance
 *
 * Returns: (transfer full): newly allocated string which holds a path, given workspace is at. 
 * Returned string should be freed when no longer needed.
 * Since: 10.11
 */
gchar*
midgard_workspace_get_path (MidgardWorkspace *self)
{
	g_return_val_if_fail (self != NULL, NULL);

	MidgardConnection *mgd = MGD_OBJECT_CNC (self);
	g_return_val_if_fail (mgd != NULL, NULL);

	/* Do not cache path to avoid callbacks overhead */
	GSList *list = midgard_core_workspace_get_parent_names (mgd, self->priv->up_id);
	GSList *l;
	GString *str = g_string_new ("");

	for (l = list; l != NULL; l = l->next) {
		gchar *name = (gchar *) l->data;
		g_string_append_printf (str, "/%s", name);
	}

	g_string_append_printf (str, "/%s", self->priv->name);

	return g_string_free (str, FALSE);
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_PARENT_WS = 1,
	PROPERTY_GUID,
	PROPERTY_ID,
	PROPERTY_UP,
	PROPERTY_PATH, 
	PROPERTY_NAME
};

static void 
__midgard_workspace_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardWorkspace *self = (MidgardWorkspace *) instance;
	self->priv = g_new (MidgardWorkspacePrivate, 1);
	self->priv->parent_ws = NULL;
	self->priv->path = NULL;
	self->priv->name = NULL;
	self->priv->id = 0;
	self->priv->up_id = 0;

	return;
}

static GObject *
__midgard_workspace_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (__parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);
	return G_OBJECT(object);
}

static void
__midgard_workspace_dispose (GObject *object)
{
	MidgardWorkspace *self = MIDGARD_WORKSPACE (object);
	if (self->priv->parent_ws) {
		g_object_unref (self->priv->parent_ws);
		self->priv->parent_ws = NULL;
	}

	__parent_class->dispose (object);
}

static void 
__midgard_workspace_finalize (GObject *object)
{
	MidgardWorkspace *self = MIDGARD_WORKSPACE (object);

	g_free (self->priv->path);
	self->priv->path = NULL;

	g_free (self->priv->name);
	self->priv->name = NULL;
}

static void
__midgard_workspace_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	MidgardWorkspace *self = MIDGARD_WORKSPACE (object);

	switch (property_id) {
		
		case PROPERTY_PARENT_WS:
			if (!G_VALUE_HOLDS_OBJECT (value))
				return;
			GObject *obj = g_value_get_object (value);
			if (!MIDGARD_IS_WORKSPACE (obj))
				return;
			MidgardWorkspace *parent_ws = g_value_dup_object (value);
			self->priv->parent_ws = parent_ws;
			self->priv->up_id = parent_ws->priv->id;
			break;

		case PROPERTY_NAME:
			g_free (self->priv->name);
			self->priv->name = g_value_dup_string (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;		
	}
}

static void
__midgard_workspace_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	MidgardWorkspace *self = (MidgardWorkspace *) object;
	
	switch (property_id) {
		
		case PROPERTY_PARENT_WS:
			/* do not return object via property, instead call API so we can annotate returned object's ownership */
			break;

		case PROPERTY_GUID:
			g_value_set_string (value, MGD_OBJECT_GUID (self));
			break;

		case PROPERTY_ID:
			g_value_set_uint (value, self->priv->id);
			break;

		case PROPERTY_UP:
			g_value_set_uint (value, self->priv->up_id);
			break;

		case PROPERTY_PATH:
			g_value_set_string (value, midgard_workspace_get_path (self));
			break;

		case PROPERTY_NAME:
			g_value_set_string (value, self->priv->name);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static gboolean
_workspace_storage_exists (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(klass != NULL, FALSE);

	return midgard_core_table_exists(mgd, MGD_WORKSPACE_TABLE);
}

static gboolean
_workspace_storage_delete (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (klass != NULL, FALSE);

	g_warning ("MidgardWorkspace storage can not be deleted");
	return FALSE;
}


static void __midgard_workspace_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardWorkspaceClass *klass = MIDGARD_WORKSPACE_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = __midgard_workspace_constructor;
	gobject_class->dispose = __midgard_workspace_dispose;
	gobject_class->finalize = __midgard_workspace_finalize;
	gobject_class->set_property = __midgard_workspace_set_property;
	gobject_class->get_property = __midgard_workspace_get_property;

	/* PROPERTIES */
	MgdSchemaPropertyAttr *prop_attr;
	MgdSchemaTypeAttr *type_attr = midgard_core_schema_type_attr_new();

	GParamSpec *pspec;
	const gchar *property_name;

	/* parent workspace */
	property_name = "parent_workspace";
	pspec = g_param_spec_object (property_name,
			"Parent workspace",
			"",
			MIDGARD_TYPE_WORKSPACE,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROPERTY_PARENT_WS, pspec);
	 
	/* guid */
	property_name = "guid";
	pspec = g_param_spec_string (property_name,
			"Guid which identifies workspace object.",
			"",
			"",
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROPERTY_GUID, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_GUID;
	prop_attr->field = g_strdup (property_name);
	prop_attr->table = g_strdup (MGD_WORKSPACE_TABLE);
	prop_attr->tablefield = g_strjoin (".", MGD_WORKSPACE_TABLE, property_name, NULL);
	g_hash_table_insert (type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer) property_name);

	/* id */
	property_name = "id";
	pspec = g_param_spec_uint (property_name,
			"Local storage id which identifies workspace",
			"",
			0, G_MAXUINT32, 0, G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,	PROPERTY_ID, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup(property_name);
	prop_attr->table = g_strdup(MGD_WORKSPACE_TABLE);
	prop_attr->tablefield = g_strjoin(".", MGD_WORKSPACE_TABLE, property_name, NULL);
	/* Set id property and field as Primary Key */
	prop_attr->primaryfield = g_strdup (property_name);
	prop_attr->is_primary = TRUE;
	type_attr->primary = g_strdup (property_name);
	type_attr->primaryfield = g_strdup (property_name);
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer) property_name);

	/* up */
	property_name = "up";
	pspec = g_param_spec_uint (property_name,
			"Local storage id which identifies up workspace",
			"",
			0, G_MAXUINT32, 0, G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,	PROPERTY_ID, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup(property_name);
	prop_attr->table = g_strdup(MGD_WORKSPACE_TABLE);
	prop_attr->tablefield = g_strjoin(".", MGD_WORKSPACE_TABLE, property_name, NULL);
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer) property_name);

	/* path */
	property_name = "path";
	pspec = g_param_spec_string (property_name,
			"A path workspace is at",
			"",
			"",
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class, PROPERTY_PATH, pspec);

	/* name */
	property_name = "name";
	pspec = g_param_spec_string (property_name,
			"Workspace name",
			"",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class, PROPERTY_NAME, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_STRING;
	prop_attr->field = g_strdup (property_name);
	prop_attr->table = g_strdup (MGD_WORKSPACE_TABLE);
	prop_attr->tablefield = g_strjoin (".", MGD_WORKSPACE_TABLE, property_name, NULL);
	g_hash_table_insert (type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer )property_name);

	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv = g_new(MidgardDBObjectPrivate, 1);
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data = type_attr;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data->table = g_strdup(MGD_WORKSPACE_TABLE);
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data->tables = g_strdup(MGD_WORKSPACE_TABLE);
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->has_metadata = FALSE;
	
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->create_storage = midgard_core_query_create_class_storage;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->update_storage = midgard_core_query_update_class_storage;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_exists = _workspace_storage_exists;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->delete_storage = _workspace_storage_delete;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->set_statement_insert = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->set_statement_insert;

	/* Initialize persistent statement */
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->set_statement_insert (MIDGARD_DBOBJECT_CLASS (klass));
}

GType 
midgard_workspace_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardWorkspaceClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) __midgard_workspace_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardWorkspace),
			0,              /* n_preallocs */
			__midgard_workspace_instance_init /* instance_init */
		};
		type = g_type_register_static (MIDGARD_TYPE_DBOBJECT, "MidgardWorkspace", &info, 0);
	}
	return type;
}

