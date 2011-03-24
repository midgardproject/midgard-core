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

#include "midgard_workspace_context.h"
#include "midgard_core_workspace.h"
#include "midgard_workspace_storage.h"
#include "midgard_workspace.h"
#include "midgard_workspace_manager.h"
#include "midgard_core_object.h"

/**
 * midgard_workspace_context_new:
 *
 * Returns: #MidgardWorkspaceContext instance
 * Since: 10.05.5
 */
MidgardWorkspaceContext *
midgard_workspace_context_new ()
{
	MidgardWorkspaceContext *self = g_object_new (MIDGARD_TYPE_WORKSPACE_CONTEXT, NULL);
	return self;
}

static gboolean 
_midgard_workspace_context_create (const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *wss, const gchar *path, GError **error)
{
	g_return_val_if_fail (manager != NULL, FALSE);
	g_return_val_if_fail (wss != NULL, FALSE);

	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (wss);
	MidgardConnection *mgd = manager->priv->mgd;
	g_return_val_if_fail (mgd != NULL, FALSE);	

	GError *err = NULL;
	guint row_id;
	gint id = midgard_core_workspace_get_id_by_path (mgd, path, &row_id, &err);

	/* Valid path and workspace exists */
	if (id > 0) {
		g_propagate_error (error, err);
		return FALSE;
	}

	/* Invalid path */
	if (id == -1 
			&& (err->domain == MIDGARD_WORKSPACE_STORAGE_ERROR 
				&& err->code == MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_PATH)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	/* Workspace doesn't exist. Create it */
	if (id == -1 
			&& (err->domain == MIDGARD_WORKSPACE_STORAGE_ERROR 
				&& err->code == MIDGARD_WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS)) 
		g_clear_error (&err);
	
	gchar **tokens = g_strsplit (path, "/", 0);
        guint i = 0;
	/* If path begins with slash, first element is an empty string. Ignore it. */
	if (*tokens[0] == '\0')
		i++;

	guint j = i;
	guint up = 0;
	const gchar *name = NULL;
	MidgardWorkspace *ws = NULL;
	guint name_row_id;
	GString *dpath = g_string_new ("");

	gboolean rv = TRUE;
	while (tokens[j] != NULL) {
		name = tokens[j];
		if (j > 1) /* Ignore empty string */
			g_string_append_printf (dpath, "/%s", tokens[j-1]);

		/* Try to create workspace */
		ws = midgard_workspace_new ();
		g_object_set (ws, "name", name, NULL);	
		if (!midgard_workspace_manager_create (manager, MIDGARD_WORKSPACE_STORAGE (ws), dpath->str, &err)) {
			if (err && (err->code == MIDGARD_WORKSPACE_STORAGE_ERROR_PATH_EXISTS
						|| err->code == MIDGARD_WORKSPACE_STORAGE_ERROR_NAME_EXISTS)) {
				rv = FALSE;		
			} else {
				g_propagate_error (error, err);
				g_object_unref (ws);
				g_string_free (dpath, TRUE);
				return FALSE;
			}

			if (err)
				g_clear_error (&err);
		}

		id = ws->priv->id;
		g_object_unref (ws);

		j++;
	}

	g_string_free (dpath, TRUE);

	if (ws && G_IS_OBJECT (ws))
		g_object_unref (ws);

	self->priv->path = g_strdup (path);
	self->priv->id = id;

	/* FIXME, connect to manager create signal */

	return rv;
}

static gboolean 
_midgard_workspace_context_update (const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *self, GError **error)
{
	g_return_val_if_fail (manager != NULL, FALSE);
	g_return_val_if_fail (self != NULL, FALSE);

	g_set_error (error, MIDGARD_WORKSPACE_STORAGE_ERROR, 
			MIDGARD_WORKSPACE_STORAGE_ERROR_CONTEXT_VIOLATION, 
			"Can not update context");

	return FALSE;
}

static gboolean 
_midgard_workspace_context_purge (const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *self, GError **error)
{
	g_return_val_if_fail (manager != NULL, FALSE);
	g_return_val_if_fail (self != NULL, FALSE);

	g_set_error (error, MIDGARD_WORKSPACE_STORAGE_ERROR, 
			MIDGARD_WORKSPACE_STORAGE_ERROR_CONTEXT_VIOLATION, 
			"Can not purge context");

	return FALSE;
}

static gboolean
_midgard_workspace_context_get_by_path (const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *wss, const gchar *path, GError **error)
{
	g_return_val_if_fail (manager != NULL, FALSE);
	g_return_val_if_fail (wss != NULL, FALSE);

	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (wss);
	MidgardConnection *mgd = manager->priv->mgd;
	g_return_val_if_fail (mgd != NULL, FALSE);	

	GError *err = NULL;
	guint row_id;
	gint id = midgard_core_workspace_get_id_by_path (mgd, path, &row_id, &err);

	/* Valid path and workspace exists */
	if (id > 0) {
		self->priv->path = g_strdup (path);
		self->priv->id = id;	
		return TRUE;
	}

	if (id == -1 || err) { 
		g_propagate_error (error, err);
		return FALSE;
	}

	return FALSE;
}

static gchar **
_midgard_workspace_context_list_workspace_names (MidgardWorkspaceStorage *wss, guint *elements)
{
	g_return_val_if_fail (wss != NULL, NULL);

	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (wss);
	gchar *path = self->priv->path;
	if (elements)
		*elements = 0;

	if (!path)
		return NULL;

	gchar **tokens = g_strsplit (path, "/", 0);
	guint i = 0;
	guint j = 0;

	/* compute number of elements and ignore empty element */	
	while (tokens[i] != NULL) {
		if (*tokens[i] != '\0')
			j++;
		i++;
	}

	/* Initialize NULL terminated array */
	gchar **names = g_new (gchar *, j);
	if (elements)
		*elements = j;
	i = 0;
	j = 0;
	while (tokens[i] != NULL) {
		if (*tokens[i] != '\0') {
			/* Do not duplicate names. Let's reuse local strings anyway */
			names[j] = tokens[i];
			j++;
		}
		i++;
	}
	
	g_free (tokens);

	return names;
}

static MidgardWorkspaceStorage*
_midgard_workspace_context_get_workspace_by_name (MidgardWorkspaceStorage *wss, const gchar *name)
{
	g_return_val_if_fail (wss != NULL, NULL);
	g_return_val_if_fail (name != NULL, NULL);

	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (wss);
	const MidgardWorkspaceManager *manager = self->priv->manager;
	gchar *path = self->priv->path;

	if (!path)
		return NULL;

	gchar **tokens = g_strsplit (path, "/", 0);
	guint i = 0;
	guint j = 0;
	gboolean exists = FALSE;

	while (tokens[i] != NULL) {
		if (g_str_equal (tokens[i], name)) {
			exists = TRUE;
			i++;
			break;
		}
		i++;
	}

	if (!exists) {
		g_strfreev (tokens);
		return NULL;
	}

	GString *new_path = g_string_new ("");
	for (j = 0; j < i; j++) {
		if (*tokens[j] != '\0')
			g_string_append_printf (new_path, "/%s", tokens[j]);
	}

	MidgardWorkspace *ws = midgard_workspace_new ();
	if (!ws)
		return NULL; /* FIXME, fatal error */
	gboolean rv = midgard_workspace_manager_get_workspace_by_path (manager, MIDGARD_WORKSPACE_STORAGE (ws), (const gchar *)new_path->str, NULL);
 	g_string_free (new_path, TRUE);
	g_strfreev (tokens);

	if (rv)
		return MIDGARD_WORKSPACE_STORAGE (ws);

	g_object_unref (ws);
	return NULL;	
}

static MidgardWorkspaceStorage **
_midgard_workspace_context_list_children (MidgardWorkspaceStorage *wss, guint *n_objects)
{
	g_return_val_if_fail (wss != NULL, NULL);
	if (n_objects)
		*n_objects = 0;

	guint n_names;
	gchar **names = midgard_workspace_storage_list_workspace_names (wss, &n_names);
	if (names == NULL)
		return NULL;

	guint i = 0;
	if (n_objects) 
		*n_objects = n_names;
	MidgardWorkspace **children = g_new (MidgardWorkspace*, n_names);
	for (i = 0; i < n_names; i++) {
		children[i] = MIDGARD_WORKSPACE (midgard_workspace_storage_get_workspace_by_name (wss, names[i]));		
	}

	g_free (names);

	return (MidgardWorkspaceStorage **) children;
}

/**
 * midgard_workspace_context_has_workspace:
 * @self: #MidgardWorkspaceContext instance
 * @workspace: #MidgardWorkspace to check
 *
 * Check, whether given @workspace is in given context
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_workspace_context_has_workspace (MidgardWorkspaceContext *self, MidgardWorkspace *workspace)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (workspace != NULL, FALSE);

	gboolean rv = FALSE;
	GSList *ids = MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (self)->priv->list_ids (MIDGARD_WORKSPACE_STORAGE (self));
	if (!ids)
		return rv;

	GSList *l;
	guint ws_id = 0;
	for (l = ids; l != NULL; l = l->next) {
		GValue *val = (GValue *) l->data;
		if (G_VALUE_HOLDS_UINT (val))
			ws_id = g_value_get_uint (val);
		else 
			ws_id = g_value_get_int (val);

		if (ws_id == workspace->priv->id) {
			rv = TRUE;
			break;
		}
	}
	
	g_slist_free (ids);
	return rv;
}

static GSList*
_midgard_workspace_context_iface_list_ids (MidgardWorkspaceStorage *self)
{
	MidgardWorkspaceContext *ctx = MIDGARD_WORKSPACE_CONTEXT (self);
	const MidgardWorkspaceManager *manager = ctx->priv->manager;
	MidgardConnection *mgd = manager->priv->mgd;
	return midgard_core_workspace_get_context_ids (mgd, ctx->priv->id);
}

static guint
_midgard_workspace_context_iface_get_id (MidgardWorkspaceStorage *self)
{
	MidgardWorkspaceContext *ctx = MIDGARD_WORKSPACE_CONTEXT (self);
	return ctx->priv->id;
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class = NULL;

const gchar *
_midgard_workspace_context_get_path (MidgardWorkspaceStorage *wss)
{
	g_return_val_if_fail (wss != NULL, NULL);
	
	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (wss);
	if (!self->priv->manager)
		return NULL;

	MidgardConnection *mgd = self->priv->manager->priv->mgd;
	if (!mgd)
		return NULL;

	guint ws_id = self->priv->id;	

	if (ws_id == 0)
		return NULL;

	const GValue *up_val = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_UP, ws_id, NULL);
	if (!up_val)
		return NULL;

	guint up_id = 0;
	if (G_VALUE_HOLDS_UINT (up_val))
		up_id = g_value_get_uint (up_val);
	else
		up_id = (guint) g_value_get_int (up_val);

	GSList *list = midgard_core_workspace_get_parent_names (mgd, up_id);
	GSList *l;
	GString *str = g_string_new ("");
	
	for (l = list; l != NULL; l = l->next) {
		gchar *name = (gchar *) l->data;
		g_string_append_printf (str, "/%s", name);
	}
	
	if (list)
		g_slist_free (list);

	const GValue *name_value = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_NAME, ws_id, NULL);

	g_string_append_printf (str, "/%s", g_value_get_string (name_value));
	
	g_free (self->priv->path);
	self->priv->path = g_string_free (str, FALSE);
	
	return (const gchar *) self->priv->path;
}

static void
_midgard_workspace_context_iface_init (MidgardWorkspaceStorageIFace *iface)
{
	/* Interface implementation */
	iface->get_path = _midgard_workspace_context_get_path;
	iface->list_children = _midgard_workspace_context_list_children;
	iface->list_workspace_names = _midgard_workspace_context_list_workspace_names;
	iface->get_workspace_by_name = _midgard_workspace_context_get_workspace_by_name;

	/* Private implementation, WorkspaceManager helpers */ 
	iface->priv = g_new (MidgardWorkspaceStorageIFacePrivate, 1);
	iface->priv->list_ids = _midgard_workspace_context_iface_list_ids;
	iface->priv->get_id = _midgard_workspace_context_iface_get_id;
	iface->priv->create = _midgard_workspace_context_create;
	iface->priv->update = _midgard_workspace_context_update;
	iface->priv->purge = _midgard_workspace_context_purge;
	iface->priv->get_by_path = _midgard_workspace_context_get_by_path;
	return;
}

static void
_midgard_workspace_context_iface_finalize (MidgardWorkspaceStorageIFace *iface)
{
	if (!iface->priv)
		return;

	g_free (iface->priv);
	iface->priv = NULL;

	return;
}

static void
_midgard_workspace_context_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (instance);
	self->priv = midgard_core_workspace_private_new ();
}

static GObject *
_midgard_workspace_context_constructor (GType type,
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
_midgard_workspace_context_dispose (GObject *object)
{	
	MidgardWorkspaceContext *self = (MidgardWorkspaceContext *) object;
	__parent_class->dispose (object);
}

static void
_midgard_workspace_context_finalize (GObject *object)
{
	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (object);
	midgard_core_workspace_private_free (self->priv);

	__parent_class->finalize (object);
}

static void
_midgard_workspace_context_class_init (MidgardWorkspaceContextClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	__parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_workspace_context_constructor;
	object_class->dispose = _midgard_workspace_context_dispose;
	object_class->finalize = _midgard_workspace_context_finalize;
}

GType
midgard_workspace_context_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardWorkspaceContextClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			(GClassInitFunc)_midgard_workspace_context_class_init,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardWorkspaceContext),
			0,      /* n_preallocs */
			_midgard_workspace_context_instance_init    /* instance_init */
		};
      
		static const GInterfaceInfo property_info = {
			(GInterfaceInitFunc) _midgard_workspace_context_iface_init,   
			(GInterfaceFinalizeFunc) _midgard_workspace_context_iface_finalize,
			NULL	/* interface_data */
		};

  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardWorkspaceContext", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_WORKSPACE_STORAGE, &property_info);
    	}
    	return type;
}

