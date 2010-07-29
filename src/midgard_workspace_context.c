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
#include "midgard_core_object.h"

struct _MidgardWorkspaceContextPrivate {
	GObject  parent;
	gchar *path;
	guint workspace_id;
	MidgardConnection *mgd;
};

/**
 * midgard_workspace_context_exists:
 * @mgd: #MidgardConnection instance
 * @path: a path to check #MidgardWorkspaceContext at
 * @error: (error-domains MIDGARD_WORKSPACE_STORAGE_ERROR): pointer to store error
 *
 * Check if #MidgardWorkspaceContext exists at given path.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.11
 */ 
gboolean
midgard_workspace_context_exists (MidgardConnection *mgd, const gchar *path, GError **error)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (path != NULL, FALSE);
	g_return_val_if_fail (*error == NULL || error == NULL, FALSE);

	GError *err = NULL;
	guint row_id;
	gint id = midgard_core_workspace_get_id_by_path (mgd, path, &row_id, &err);
	if (id == -1) {
		g_propagate_error (error, err);
		return FALSE;
	}	

	return TRUE;
}

/**
 * midgard_workspace_context_create:
 * @mgd: #MidgardConnection instance
 * @path: a path to create #MidgardWorkspaceContext at
 * @error: (error-domains MIDGARD_WORKSPACE_STORAGE_ERROR): pointer to store error
 *
 * If context exists at given path, a new #MidgardWorkspaceContext object is returned.
 * If doesn't, an attempt to create new context is made. That is, every single #MidgardWorkspace 
 * is checked for every given @path's named element.
 *
 * Returns: #MidgardWorkspaceConext object or %NULL
 * Since: 10.11
 */ 
MidgardWorkspaceContext*
midgard_workspace_context_create (MidgardConnection *mgd, const gchar *path, GError **error)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (path != NULL, FALSE);
	g_return_val_if_fail (*error == NULL || error == NULL, FALSE);

	MidgardWorkspaceContext *ws_ctx = NULL;
	GError *err = NULL;
	guint row_id;
	gint id = midgard_core_workspace_get_id_by_path (mgd, path, &row_id, &err);

	/* Valid path and workspace exists */
	if (id > 0) {
		ws_ctx = g_object_new (MIDGARD_TYPE_WORKSPACE_CONTEXT, NULL);
		ws_ctx->priv->path = g_strdup (path);
		ws_ctx->priv->workspace_id = (guint) id;
		ws_ctx->priv->mgd = mgd;
		return ws_ctx; 
	}

	/* Invalid path */
	if (id == -1 
			&& (err->domain == MIDGARD_WORKSPACE_STORAGE_ERROR 
				&& err->code == WORKSPACE_STORAGE_ERROR_INVALID_PATH)) {
		g_propagate_error (error, err);
		return ws_ctx;
	}

	/* Workspace doesn't exist. Create it */
	if (id == -1 
			&& (err->domain == MIDGARD_WORKSPACE_STORAGE_ERROR 
				&& err->code == WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS)) 
		g_clear_error (&err);
	
	gchar **tokens = g_strsplit (path, "/", 0);
        guint i = 0;
	/* If path begins with slash, first element is an empty string. Ignore it. */
	if (*tokens[0] == '\0')
		i++;

	guint j = i;
	guint up = 0;
	const gchar *name = NULL;
	MidgardWorkspace *ws_parent = NULL;
	MidgardWorkspace *ws = NULL;
	while (tokens[j] != NULL) {
		name = tokens[j];
		id = midgard_core_workspace_get_col_id_by_name (mgd, name, MGD_WORKSPACE_FIELD_IDX_ID, up, &row_id);
		/* Workspace not found, create it */
		ws = midgard_workspace_new (mgd, ws_parent);
		if (id == -1) {
			g_object_set (ws, "name", name, NULL);
			if (!midgard_workspace_create (ws, NULL)) {
				/* FIXME, handle error */
				g_error ("Failed to create workspace in path");
			}
		} else {
			MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_GET_CLASS (ws);
			dbklass->dbpriv->set_from_data_model (MIDGARD_DBOBJECT (ws), mgd->priv->workspace_model, row_id, 0);
		}

		if (ws_parent)
			g_object_unref (ws_parent);
		ws_parent = ws;

		up = ws->priv->id;
		j++;
	}

	if (ws_parent && G_IS_OBJECT (ws_parent))
		g_object_unref (ws_parent);
	if (ws && G_IS_OBJECT (ws))
		g_object_unref (ws);

       	ws_ctx = g_object_new (MIDGARD_TYPE_WORKSPACE_CONTEXT, NULL);
	ws_ctx->priv->path = g_strdup (path);
	ws_ctx->priv->workspace_id = id;
	ws_ctx->priv->mgd = mgd;

	return ws_ctx;
}

/**
 * midgard_workspace_context_get_workspace_names:
 * @self: #MidgardWorkspaceContext instance
 * @elements: location to store number or returned array's elements
 *
 * Get all workspace names which exist in given #MidgardWorkspaceContext.
 * Elements are returned at the same order as they exists in context's path.
 * First array element is root #MidgardWorkspace, the last one is current one.
 *
 * Returns: (array-length=1): newly allocated, NULL terminated array of strings
 * Since: 10.11
 */ 
gchar**
midgard_workspace_context_get_workspace_names (MidgardWorkspaceContext *self, guint *elements)
{
	g_return_val_if_fail (self != NULL, NULL);

	gchar *path = self->priv->path;
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
	*elements = j;
	gchar **names = g_new (gchar *, j+1);
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
	
	names[j] = NULL;
	g_free (tokens);

	return names;
}

/** 
 * midgard_workspace_context_get_workspace_by_name:
 * @self: #MidgardWorkspaceContext
 * @name: name of the #MidgardWorkspace
 *
 * Call midgard_workspace_context_get_workspace_names() if you need all available 
 * #MidgardWorkspace names in given context. 
 *
 * Returns: new #MidgardWorkspace object or %NULL if there is no such named workspace in context.
 * Since: 10.11
 */ 
MidgardWorkspace*
midgard_workspace_context_get_workspace_by_name (MidgardWorkspaceContext *self, const gchar *name)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (name != NULL, NULL);

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

	MidgardConnection *mgd = self->priv->mgd;
	MidgardWorkspace *ws = midgard_workspace_get_by_path (mgd, (const gchar *)new_path->str, NULL);
 	g_string_free (new_path, TRUE);
	g_strfreev (tokens);

	return ws;	
}

static GSList*
_midgard_workspace_context_iface_list_ids (MidgardWorkspaceStorage *self)
{
	MidgardWorkspaceContext *ctx = MIDGARD_WORKSPACE_CONTEXT (self);
	return midgard_core_workspace_get_context_ids (ctx->priv->mgd, ctx->priv->workspace_id);
}

static guint
_midgard_workspace_context_iface_get_id (MidgardWorkspaceStorage *self)
{
	MidgardWorkspaceContext *ctx = MIDGARD_WORKSPACE_CONTEXT (self);
	return ctx->priv->workspace_id;
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class = NULL;

const gchar *
_midgard_workspace_context_get_path (MidgardWorkspaceStorage *self)
{
	return (const gchar *) MIDGARD_WORKSPACE_CONTEXT (self)->priv->path;
}

static void
_midgard_workspace_context_iface_init (MidgardWorkspaceStorageIFace *iface)
{
	iface->get_path = _midgard_workspace_context_get_path;
	iface->priv = g_new (MidgardWorkspaceStorageIFacePrivate, 1);
	iface->priv->list_ids = _midgard_workspace_context_iface_list_ids;
	iface->priv->get_id = _midgard_workspace_context_iface_get_id;
	return;
}

static void
_midgard_workspace_context_iface_finalize (MidgardWorkspaceStorageIFace *iface)
{
	if (iface->priv)
		g_free (iface->priv);
	iface->priv = NULL;
	return;
}

static void
_midgard_workspace_context_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (instance);
	self->priv = g_new (MidgardWorkspaceContextPrivate, 1);
	self->priv->path = NULL;
}

static GObject *
_midgard_workspace_context_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return G_OBJECT(object);
}

static void
_midgard_workspace_context_dispose (GObject *object)
{	
	MidgardWorkspaceContext *self = (MidgardWorkspaceContext *) object;
	parent_class->dispose (object);
}

static void
_midgard_workspace_context_finalize (GObject *object)
{
	MidgardWorkspaceContext *self = MIDGARD_WORKSPACE_CONTEXT (object);

	g_free (self->priv->path);
	self->priv->path = NULL;

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
_midgard_workspace_context_class_init (MidgardWorkspaceContextClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

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

