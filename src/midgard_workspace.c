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

#include "midgard_workspace_storage.h"
#include "midgard_workspace.h"
#include "midgard_workspace_context.h"
#include "midgard_core_workspace.h"
#include "midgard_core_object.h"
#include "midgard_core_query.h"
#include "guid.h"
#include "midgard_query_constraint_simple.h"
#include "midgard_query_holder.h"
#include "midgard_query_value.h"
#include "midgard_query_property.h"
#include "midgard_query_executor.h"
#include "midgard_query_select.h"
#include "midgard_query_storage.h"
#include "midgard_query_constraint.h"
#include "midgard_query_constraint_group.h"
#include "midgard_error.h"

/**
 * midgard_workspace_new:
 *
 * Simplified constructor.
 *
 * Returns: #MidgardWorkspace instance
 * Since: 10.05.5
 */ 
MidgardWorkspace *
midgard_workspace_new ()
{
	MidgardWorkspace *self = g_object_new (MIDGARD_TYPE_WORKSPACE, NULL);
	return self;
}


gboolean
_midgard_workspace_get_by_path (const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *wss, const gchar *path, GError **error)
{
	g_return_val_if_fail (wss != NULL, FALSE);
	g_return_val_if_fail (path != NULL, FALSE);	

	MidgardWorkspace *self = MIDGARD_WORKSPACE (wss);
	MidgardConnection *mgd = manager->priv->mgd;
	g_return_val_if_fail (mgd != NULL, FALSE);

	GError *err = NULL;
	gint id = 0;
	guint row_id;
	id = midgard_core_workspace_get_id_by_path (mgd, path, &row_id, &err);

	if (id == -1) {
		g_propagate_error (error, err);
		return FALSE;
	}

	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_GET_CLASS (self);
	dbklass->dbpriv->set_from_data_model (MIDGARD_DBOBJECT (self), mgd->priv->workspace_model, row_id, 0);

	return TRUE;
}

static gboolean 
_midgard_workspace_create (const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *ws, const gchar *_path, GError **error)
{
	g_return_val_if_fail (manager != NULL, FALSE);
	g_return_val_if_fail (ws != NULL, FALSE);

	const gchar *path = _path;
	if (path == NULL)
		path = "";

	MidgardWorkspace *self = MIDGARD_WORKSPACE (ws);
	MidgardConnection *mgd = manager->priv->mgd;
	MGD_OBJECT_CNC (self) = mgd;
	g_return_val_if_fail (mgd != NULL, FALSE);

	gchar *workspace_name = self->priv->name;
	if (workspace_name == NULL 
			|| (workspace_name != NULL && *workspace_name == '\0')) {
		g_set_error (error, MIDGARD_WORKSPACE_STORAGE_ERROR, MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_VALUE,
					"Invalid (empty or null) workspace's name"); 
		return FALSE;
	}

	GString *ws_path = g_string_new ("");
	g_string_append_printf (ws_path, "%s/%s", path, workspace_name);

	if (midgard_workspace_manager_path_exists (manager, ws_path->str)) {
		g_set_error (error, MIDGARD_WORKSPACE_STORAGE_ERROR, MIDGARD_WORKSPACE_STORAGE_ERROR_NAME_EXISTS, 
				"WorkspaceStorage at path '%s/%s' already exists", 
				path, workspace_name);
		g_string_free (ws_path, TRUE);
		return FALSE;
	}
	
	GError *err = NULL;
	gint up_id = 0;

	/* Ignore empty path, we're going to create root workspace */
	if (*path != '\0') {
		up_id = midgard_core_workspace_get_id_by_path (mgd, path, NULL, &err);	
		if (err && err->code) {
			if (err->code == MIDGARD_WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS) {
				if (up_id == -1) 
					up_id = 0;
				g_clear_error (&err);	
			} else if (err->code == MIDGARD_WORKSPACE_STORAGE_ERROR_NAME_EXISTS
					|| err->code == MIDGARD_WORKSPACE_STORAGE_ERROR_PATH_EXISTS) {
				g_clear_error (&err);
			} else {
				g_propagate_error (error, err);
				g_string_free (ws_path, TRUE);
				return FALSE;
			}
		}
	}

	/* TODO, set parent workspace, if needed 
	 * MidgardWorkspaceContext *context = midgard_workspace_context_new (); */

	/* Create, set guid and up id */
	MIDGARD_DBOBJECT(self)->dbpriv->guid = (const gchar *) midgard_guid_new (mgd);
	self->priv->up_id = up_id;
	if (midgard_core_query_create_dbobject_record (MIDGARD_DBOBJECT (self))) {
		/* TODO ?, emit created signal */
		/* Refresh available workspaces model */
		midgard_core_workspace_list_all (mgd);

		/* Get id of newly created object */
		guint row_id;
		gint id = midgard_core_workspace_get_col_id_by_name (mgd, self->priv->name, MGD_WORKSPACE_FIELD_IDX_ID, self->priv->up_id, &row_id);
		if (id < 1) {
			g_warning ("Newly created workspace (%s) id is not unique (%d)", ws_path->str, id);
			/* TODO, set error and delete workspace from database */
			g_string_free (ws_path, TRUE);
			return FALSE;
		}
		self->priv->id = id;
		g_string_free (ws_path, TRUE);
		return TRUE;
	}

	g_string_free (ws_path, TRUE);

	/* Create failed, reset values */
	g_free ((gchar *)MGD_OBJECT_GUID (self));
	MGD_OBJECT_GUID (self) = NULL;

	self->priv->up_id = 0;
	self->priv->id = 0;

	/* FIXME, Set internal error */
	return FALSE;
}

static gboolean 
_midgard_workspace_update (const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *ws, GError **error)
{
	g_return_val_if_fail (manager != NULL, FALSE);
	g_return_val_if_fail (ws != NULL, FALSE);

	MidgardWorkspace *self = MIDGARD_WORKSPACE (ws);
	MidgardConnection *mgd = manager->priv->mgd;
	MGD_OBJECT_CNC (self) = mgd;
	g_return_val_if_fail (mgd != NULL, FALSE);

	if (self->priv->id == 0) {
		g_set_error (error, MIDGARD_WORKSPACE_STORAGE_ERROR, MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_VALUE,
					"Invalid value ID for workspace"); 
		return FALSE;
	}

	gchar *workspace_name = self->priv->name;
	if (workspace_name == NULL 
			|| (workspace_name != NULL && *workspace_name == '\0')) {
		g_set_error (error, MIDGARD_WORKSPACE_STORAGE_ERROR, MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_VALUE,
					"Invalid (empty or null) workspace's name"); 
		return FALSE;
	}

	/* Check duplicate */
	const MidgardWorkspaceContext *context = midgard_workspace_get_context (self);
	if (context) {
		MidgardWorkspace *ws_dup = (MidgardWorkspace *) midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (context), workspace_name);	
		if (ws_dup 
				&& g_str_equal (ws_dup->priv->name, workspace_name)
				&& ws_dup->priv->id != self->priv->id) {
			g_object_unref (ws_dup);
			g_set_error (error, MIDGARD_WORKSPACE_STORAGE_ERROR, MIDGARD_WORKSPACE_STORAGE_ERROR_NAME_EXISTS,
					"Workspace with given '%s' name already exists in this context", workspace_name); 
			return FALSE;
		}

		if (g_str_equal (ws_dup->priv->name, workspace_name)) {
			g_object_unref (ws_dup);
			return TRUE;
		}		

		if (ws_dup)
			g_object_unref (ws_dup);
	}

	if (midgard_core_query_update_dbobject_record (MIDGARD_DBOBJECT (self), NULL)) {
		midgard_core_workspace_list_all (mgd);
		return TRUE;
	}

	g_set_error (error, MIDGARD_GENERIC_ERROR, MGD_ERR_INTERNAL, "%s", midgard_connection_get_error_string (mgd));

	return FALSE;
}

static gboolean 
_midgard_workspace_purge (const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *ws, GError **error)
{
	return FALSE;
}

/**
 * midgard_workspace_get_context:
 * @self: #MidgardWorkspace instance
 *
 * Returns: #MidgardWorkspaceContext @self is in or %NULL
 * Since: 10.05.5
 */
const MidgardWorkspaceContext*
midgard_workspace_get_context (MidgardWorkspace *self)
{
	g_return_val_if_fail (self != NULL, NULL);

	const MidgardWorkspaceManager *manager = self->priv->manager;
	const MidgardWorkspaceContext *context = self->priv->context;	

	if (context)
		return context;

	context = midgard_workspace_context_new ();
	const gchar *path = midgard_workspace_storage_get_path (MIDGARD_WORKSPACE_STORAGE (self));
	gboolean rv = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (context), path, NULL);

	if (rv) {
		self->priv->context = context;
		return context;
	}
	
	g_object_unref ((gpointer) context);
	return NULL;
}

/**
 * midgard_workspace_is_in_context:
 * @self: #MidgardWorkspace instance
 * @context: #MidgardWorkspaceContext to check
 *
 * Check whether @self is in gven #MidgardWorkspaceContext @context
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05.5
 */
gboolean
midgard_workspace_is_in_context (MidgardWorkspace *self, MidgardWorkspaceContext *context)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (context != NULL, FALSE);

	if (!self->priv->name)
		return FALSE;

	guint elements;
	gchar **names = midgard_workspace_storage_list_workspace_names (MIDGARD_WORKSPACE_STORAGE (context), &elements);

	/* context has no single workspace */
	if (elements < 1) {
		if (names)
			g_strfreev (names);
		return FALSE;
	}

	/* Find workspace by name */
	guint i = 0;
	gchar *name = NULL;
	while (names[i] != NULL) {
		if (g_str_equal (names[i], self->priv->name)) {
			name = names[i];
			break;
		}
		i++;
	}

	g_free (names);

	/* There's no named workspace */
	if (!name) 
		return FALSE;

	/* Get named workspace from context and check if id equals*/
	MidgardWorkspace *tmp = (MidgardWorkspace *)midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (context), name);
	if (!tmp)
		return FALSE;

	guint id = tmp->priv->id;
	g_object_unref (tmp);

	if (id == self->priv->id)
		return TRUE;

	return FALSE;
}

const gchar*
_midgard_workspace_get_path (MidgardWorkspaceStorage *ws)
{
	g_return_val_if_fail (ws != NULL, NULL);

	MidgardWorkspace *self = MIDGARD_WORKSPACE (ws);
	const MidgardWorkspaceManager *manager = self->priv->manager;
	if (!manager)
		return NULL;

	MidgardConnection *mgd = manager->priv->mgd;
	g_return_val_if_fail (mgd != NULL, NULL);

	if (self->priv->name == NULL)
		return NULL;

	GSList *list = midgard_core_workspace_get_parent_names (mgd, self->priv->up_id);
	GSList *l;
	GString *str = g_string_new ("");

	for (l = list; l != NULL; l = l->next) {
		gchar *name = (gchar *) l->data;
		g_string_append_printf (str, "/%s", name);
	}

	g_string_append_printf (str, "/%s", self->priv->name);

	g_free (self->priv->path);
	self->priv->path = g_string_free (str, FALSE);

	return (const gchar *) self->priv->path;
}

static gchar **
_midgard_workspace_list_workspace_names (MidgardWorkspaceStorage *wss, guint *elements)
{
	g_return_val_if_fail (wss != NULL, NULL);

	if (elements)
		*elements = 0;

	MidgardWorkspace *self = MIDGARD_WORKSPACE (wss);
	const gchar *path = midgard_workspace_storage_get_path (wss);
	if (path == NULL)
		return NULL;

	gchar **tokens = g_strsplit (path, "/", 0);
	guint i = 0;

	GSList *slist = NULL;

	/* compute number of elements, ignore empty element and self's name */       
	while (tokens[i] != NULL) {
		if (*tokens[i] != '\0' && !g_str_equal (self->priv->name, tokens[i]))
			slist = g_slist_prepend (slist, tokens[i]);
		i++;
	}

	if (slist == NULL)
		return NULL;

	gchar **names = g_new (gchar*, g_slist_length (slist));
	slist = g_slist_reverse (slist);
	GSList *l;
	for (l = slist, i = 0; l != NULL; l = l->next, i++) {
		names[i] = (gchar*) l->data;
	}

	if (elements)
		*elements = g_slist_length (slist);
	g_slist_free (slist);

	return names;
}

static MidgardWorkspaceStorage*
_midgard_workspace_get_workspace_by_name (MidgardWorkspaceStorage *wss, const gchar *name)
{
	g_return_val_if_fail (wss != NULL, NULL);
	g_return_val_if_fail (name != NULL, NULL);

	MidgardWorkspace *self = MIDGARD_WORKSPACE (wss);
	const MidgardWorkspaceManager *manager = self->priv->manager;
	const gchar *path = midgard_workspace_storage_get_path (wss);
	
	if (!path)
		return NULL;

	const MidgardWorkspaceContext *context = midgard_workspace_get_context (self);
	if (!context)
		return NULL;

	MidgardWorkspaceStorage *ws = midgard_workspace_storage_get_workspace_by_name (MIDGARD_WORKSPACE_STORAGE (context), name);	

	if (!ws)
		return NULL;

	if (ws && MIDGARD_WORKSPACE (ws)->priv->id == self->priv->id) {
		g_object_unref (ws);
		return NULL;
	}

	return ws;
}

static MidgardWorkspaceStorage ** 
_midgard_workspace_list_children (MidgardWorkspaceStorage *wss, guint *n_objects) 
{ 
	g_return_val_if_fail (wss != NULL, NULL); 
	if (n_objects) 
		*n_objects = 0; 

	guint n_names; 
	gchar **names = midgard_workspace_storage_list_workspace_names (wss, &n_names); 
	if (names == NULL) 
		return NULL;

	guint i;
	MidgardWorkspaceStorage **children = g_new (MidgardWorkspaceStorage*, n_names);
	for (i = 0; i < n_names; i++) {
		children[i] = midgard_workspace_storage_get_workspace_by_name (wss, names[i]);
	}

	if (n_objects)
		*n_objects = n_names;

	return children;
}

static GSList*
_midgard_workspace_iface_list_ids (MidgardWorkspaceStorage *self)
{
	MidgardWorkspace *ws = MIDGARD_WORKSPACE (self);
	const MidgardWorkspaceManager *manager = ws->priv->manager;
	MidgardConnection *mgd = manager->priv->mgd;
	guint id = ws->priv->id;
	if (id == 0)
		return NULL;

	guint row_id;
	const GValue *id_val = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_ID, id, &row_id);
	GSList *list = g_slist_append (list, (gpointer) id_val);

	return list;
}

static guint
_midgard_workspace_iface_get_id (MidgardWorkspaceStorage *self)
{
	MidgardWorkspace *ws = MIDGARD_WORKSPACE (self);
	return ws->priv->id;
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
_midgard_workspace_iface_init (MidgardWorkspaceStorageIFace *iface)
{
	iface->get_path = _midgard_workspace_get_path;
	iface->list_children = _midgard_workspace_list_children;
	iface->list_workspace_names = _midgard_workspace_list_workspace_names;
	iface->get_workspace_by_name = _midgard_workspace_get_workspace_by_name;

	iface->priv = g_new (MidgardWorkspaceStorageIFacePrivate, 1);
        iface->priv->list_ids = _midgard_workspace_iface_list_ids;
	iface->priv->get_id = _midgard_workspace_iface_get_id;
	iface->priv->create = _midgard_workspace_create;
	iface->priv->update = _midgard_workspace_update;
	iface->priv->purge = _midgard_workspace_purge;
	iface->priv->get_by_path = _midgard_workspace_get_by_path;
	return;
}

static void
_midgard_workspace_iface_finalize (MidgardWorkspaceStorageIFace *iface)
{
	if (!iface->priv)
		return;
	
	g_free (iface->priv);
	iface->priv = NULL;

	return;
}


static void 
_midgard_workspace_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardWorkspace *self = (MidgardWorkspace *) instance;
	self->priv = midgard_core_workspace_private_new ();

	return;
}

static GObject *
_midgard_workspace_constructor (GType type,
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
_midgard_workspace_dispose (GObject *object)
{
	MidgardWorkspace *self = MIDGARD_WORKSPACE (object);
	if (self->priv->parent_ws) {
		g_object_unref (self->priv->parent_ws);
		self->priv->parent_ws = NULL;
	}

	if (self->priv->context) 
		g_object_unref ((gpointer)self->priv->context);
	self->priv->context = NULL;

	/* Nullify MidgardConnection pointer, it's not set explicitly for this class. */
	MGD_OBJECT_CNC (self) = NULL;

	__parent_class->dispose (object);
}

static void 
_midgard_workspace_finalize (GObject *object)
{
	MidgardWorkspace *self = MIDGARD_WORKSPACE (object);
	midgard_core_workspace_private_free (self->priv);

	__parent_class->finalize (object);
}

static void
_midgard_workspace_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
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
_midgard_workspace_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
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
			g_value_set_string (value, midgard_workspace_storage_get_path (MIDGARD_WORKSPACE_STORAGE (self)));
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

static void
_set_from_data_model (MidgardDBObject *self, GdaDataModel *model, gint row, guint column_id)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (model != NULL);
	g_return_if_fail (row > -1);

	GError *error = NULL;
	const GValue *value;
	MidgardWorkspace *ws = MIDGARD_WORKSPACE (self);

	/* guid */
	value = gda_data_model_get_value_at (model, MGD_WORKSPACE_FIELD_IDX_GUID, row, &error);
	if (!value) {
		g_warning ("Failed to get workspace guid field: %s", error && error->message ? error->message : "Unknown reason");
		if (error)
			g_clear_error (&error);
	} else {
		g_free ((gchar *)MGD_OBJECT_GUID (ws));
		if (G_VALUE_HOLDS_STRING (value))
			MGD_OBJECT_GUID (ws) = g_value_dup_string (value);
		else {
			GValue strval = {0, };
			g_value_init (&strval, G_TYPE_STRING);
			g_value_transform (value, &strval);
			MGD_OBJECT_GUID (ws) = g_value_dup_string (&strval);
			g_value_unset (&strval);
		}
	}
	if (error) g_clear_error (&error);

	/* id */
	value = gda_data_model_get_value_at (model, MGD_WORKSPACE_FIELD_IDX_ID, row, &error);
	if (!value) {
		g_warning ("Failed to get workspace id field: %s", error && error->message ? error->message : "Unknown reason");
		if (error)
			g_clear_error (&error);
	} else {
		if (G_VALUE_HOLDS_UINT (value))
			ws->priv->id = g_value_get_uint (value);
		else { 
			GValue intval = {0, };
			g_value_init (&intval, G_TYPE_UINT);
			g_value_transform (value, &intval);	
			ws->priv->id = (guint) g_value_get_uint (&intval);
			g_value_unset (&intval);
		}
	}
	if (error) g_clear_error (&error);

	/* up */
	value = gda_data_model_get_value_at (model, MGD_WORKSPACE_FIELD_IDX_UP, row, &error);
	if (!value) {
		g_warning ("Failed to get workspace up field: %s", error && error->message ? error->message : "Unknown reason");
		if (error)
			g_clear_error (&error);
	} else {
		if (G_VALUE_HOLDS_UINT (value))
			ws->priv->up_id = g_value_get_uint (value);
		else 
			ws->priv->up_id = (guint) g_value_get_int (value);
	}
	if (error) g_clear_error (&error);

	/* name */
	value = gda_data_model_get_value_at (model, MGD_WORKSPACE_FIELD_IDX_NAME, row, &error);
	if (!value) {
		g_warning ("Failed to get workspace name field: %s", error && error->message ? error->message : "Unknown reason");
		if (error)
			g_clear_error (&error);
	} else {
		g_free (ws->priv->name);
		ws->priv->name = g_value_dup_string (value);
	}
	if (error) g_clear_error (&error);

	return;
}

static void _midgard_workspace_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardWorkspaceClass *klass = MIDGARD_WORKSPACE_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_workspace_constructor;
	gobject_class->dispose = _midgard_workspace_dispose;
	gobject_class->finalize = _midgard_workspace_finalize;
	gobject_class->set_property = _midgard_workspace_set_property;
	gobject_class->get_property = _midgard_workspace_get_property;

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
	g_object_class_install_property (gobject_class,	PROPERTY_UP, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup(property_name);
	prop_attr->table = g_strdup(MGD_WORKSPACE_TABLE);
	prop_attr->tablefield = g_strjoin(".", MGD_WORKSPACE_TABLE, property_name, NULL);
	g_hash_table_insert(type_attr->prophash, g_strdup((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer) property_name);

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
			"WorkspaceStorage name",
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
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_insert = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_statement_insert;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_insert_params = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_statement_insert_params;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_update = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_statement_update;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_update_params = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_statement_update_params;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->add_fields_to_select_statement = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->add_fields_to_select_statement;	
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->set_from_data_model = _set_from_data_model;

	/* Set sql for select queries */
#warning SET STATIC SQL
	//midgard_core_dbobject_class_set_full_select (MIDGARD_DBOBJECT_CLASS (klass));
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
			(GClassInitFunc) _midgard_workspace_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardWorkspace),
			0,              /* n_preallocs */
			_midgard_workspace_instance_init /* instance_init */
		};

		static const GInterfaceInfo property_info = {
			(GInterfaceInitFunc) _midgard_workspace_iface_init,
			(GInterfaceFinalizeFunc) _midgard_workspace_iface_finalize,	
			NULL    /* interface_data */
		};

		type = g_type_register_static (MIDGARD_TYPE_DBOBJECT, "MidgardWorkspace", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_WORKSPACE_STORAGE, &property_info);
	}
	return type;
}

