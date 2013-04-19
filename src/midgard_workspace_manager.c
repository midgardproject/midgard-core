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

#include "midgard_workspace_manager.h"
#include "midgard_core_workspace.h"
#include "midgard_workspace_storage.h"
#include "midgard_workspace.h"
#include "midgard_core_object.h"
#include "midgard_error.h"
#include "midgard_core_object_class.h"
#include "midgard_core_query.h"

/* This is not nice. Can be done as iface's private virtual method. */
#define __SET_MANAGER(__obj, __mngr) \
	if (MIDGARD_IS_WORKSPACE (__obj)) MIDGARD_WORKSPACE (__obj)->priv->manager = g_object_ref((GObject*)__mngr); \
	if (MIDGARD_IS_WORKSPACE_CONTEXT (__obj)) MIDGARD_WORKSPACE_CONTEXT (__obj)->priv->manager = g_object_ref((GObject*)__mngr); 

/**
 * midgard_workspace_manager_new:
 * @mgd: #MidgardConnection instance
 *
 * Returns: new #MidgardWorkspaceManager object
 *
 * Since: 10.05.5
 */ 
MidgardWorkspaceManager *
midgard_workspace_manager_new (MidgardConnection *mgd) 
{
	g_return_val_if_fail (mgd != NULL, NULL);

	MidgardWorkspaceManager *self = g_object_new (MIDGARD_TYPE_WORKSPACE_MANAGER, "connection", mgd, NULL);
	return self;
}

/**
 * midgard_workspace_manager_create_workspace:
 * @self: #MidgardWorkspaceManager instance
 * @ws: #MidgardWorkspaceStorage instance to create
 * @path: path at which workspace object should be created
 * @error: (error-domains MIDGARD_WORKSPACE_STORAGE_ERROR): pointer to store error
 *
 * Create given #MidgardWorkspaceStorage instance in underlying storage. 
 * If given #MidgardWorkspaceStorage is #MidgardWorkspaceContext, then it's created 
 * at given path and context's path is set. 
 * If it's #MidgardWorkspace, it's created at given path with its name, which also creates
 * new context. (e.g. if the path is '/Organization/Branch' and workspace name is 'Private', 
 * then its accessible with '/Organization/Branch/Private' path.)
 *
 * Cases to return %FALSE:
 *
 * <itemizedlist>
 * <listitem><para>
 * Invalid path given ( MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_PATH ) 
 * </para></listitem>
 * <listitem><para>
 * Workspace at path already exists ( MIDGARD_WORKSPACE_STORAGE_ERROR_NAME_EXISTS ) 
 * </para></listitem>
 * <listitem><para>
 * Workspace at path already exists ( MIDGARD_WORKSPACE_STORAGE_ERROR_PATH_EXISTS ) 
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_workspace_manager_create_workspace (const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, const gchar *path, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (ws != NULL, FALSE);

	gboolean rv = MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (ws)->priv->create (self, ws, path, error);
	if (rv) {
		__SET_MANAGER (ws, self);
		midgard_core_workspace_list_all (self->priv->mgd, FALSE);
		/* TODO, emit signal */
	}
	return rv;
}

/**
 * midgard_workspace_manager_update_workspace:
 * @self: #MidgardWorkspaceManager instance
 * @ws: #MidgardWorkspaceStorage instance 
 * @error: (error-domains MIDGARD_WORKSPACE_STORAGE_ERROR): pointer to store error
 *
 * Update #MidgardWorkspaceStorage instance in underlying storage. 
 * A valid path should be set for given @ws workspace object, and @ws should be fetched from database.
 *
 * Cases to return %FALSE:
 *
 * <itemizedlist>
 * <listitem><para>
 * Invalid path given ( MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_PATH ) 
 * </para></listitem>
 * <listitem><para>
 * Workspace at path already exists ( MIDGARD_WORKSPACE_STORAGE_ERROR_PATH_EXISTS ) 
 * </para></listitem>
 * <listitem><para>
 * Given workspace object doesn't exist in storage ( MIDGARD_WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS ) 
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_workspace_manager_update_workspace (const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (ws != NULL, FALSE);

	gboolean rv = MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (ws)->priv->update (self, ws, error);

	/* TODO, 
	 * midgard_core_workspace_list_all (mgd);
	 * if (rv) emit signal */

	return rv;
}

/**
 * midgard_workspace_manager_purge_workspace:
 * @self: #MidgardWorkspaceManager instance
 * @ws: #MidgardWorkspaceStorage instance 
 * @error: (error-domains MIDGARD_WORKSPACE_STORAGE_ERROR): pointer to store error
 *
 * Purge #MidgardWorkspaceStorage instance from underlying storage. 
 * Given @ws workspace object should be fetched from database.
 *
 * Cases to return %FALSE:
 *
 * <itemizedlist>
 * <listitem><para>
 * Given workspace object doesn't exist in storage ( MIDGARD_WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS ) 
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_workspace_manager_purge_workspace (const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (ws != NULL, FALSE);

	gboolean rv = MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (ws)->priv->purge (self, ws, error);

	/* TODO, 
	 * midgard_core_workspace_list_all (mgd);
	 * if (rv) emit signal */

	return rv;
}

/**
 * midgard_workspace_manager_path_exists:
 * @self: #MidgardWorkspaceManager instance
 * @path: A path to check 
 *
 * Check if given @path exists. 
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_workspace_manager_path_exists (const MidgardWorkspaceManager *self, const gchar *path)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (path != NULL, FALSE);

	gint path_id = midgard_core_workspace_get_id_by_path (self->priv->mgd, path, NULL, NULL);
	if (path_id > 0)
		return TRUE;

	return FALSE;
}

/**
 * midgard_workspace_manager_get_workspace_by_path:
 * @self: #MidgardWorkspaceManager instance
 * @ws: #MidgardWorkspaceStorage instance 
 * @path: workspace path
 * @error: (error-domains MIDGARD_WORKSPACE_STORAGE_ERROR): pointer to store error
 *
 * Get given workspace by given path. 
 *
 * Cases to return %FALSE:
 *
 * <itemizedlist>
 * <listitem><para>
 * Invalid path given ( MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_PATH ) 
 * </para></listitem>
 * <listitem><para>
 * Given workspace object doesn't exist in storage ( MIDGARD_WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS ) 
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 * Since: 10.05.5
 */ 
gboolean
midgard_workspace_manager_get_workspace_by_path (const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, const gchar *path, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (ws != NULL, FALSE);

	gboolean rv = MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (ws)->priv->get_by_path (self, ws, path, error);

	if (rv) {
		__SET_MANAGER (ws, self);
	}
	return rv;
}

/** 
 * midgard_workspace_manager_purge_content:
 * @self: #MidgardWorkspaceManager instance
 * @type: the type name
 * @ws: #MidgardWorkspace instance
 * @error: pointer to store returned error
 *
 * From given @ws workspace, removes entire content of given @type name.
 * There's no undelete available for this operation.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05.5
 */ 
gboolean                
midgard_workspace_manager_purge_content (const MidgardWorkspaceManager *self, const gchar *type, MidgardWorkspace *ws, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (type != NULL, FALSE);
	g_return_val_if_fail (ws != NULL, FALSE);

	MidgardDBObjectClass *dbklass = g_type_class_peek (g_type_from_name (type));
	if (!dbklass) {
		g_set_error (error, MIDGARD_GENERIC_ERROR,
				MIDGARD_GENERIC_ERROR_INVALID_NAME,
				"'%s' is not registered in GType system", type);
		return FALSE;
	}

	const gchar *table = midgard_core_class_get_table (dbklass);
	if (!table) {
		g_set_error (error, MIDGARD_GENERIC_ERROR,
				MIDGARD_GENERIC_ERROR_OBJECT_NO_STORAGE,
				"No table defined for '%s'", type);
		return FALSE;
	}
	
	GString *query = g_string_new ("DELETE FROM ");
	g_string_append_printf (query, "%s WHERE midgard_ws_id = %d", 
			table, ws->priv->id);

	gint rv = midgard_core_query_execute (self->priv->mgd, query->str, FALSE);
	g_string_free (query, TRUE);

	if (rv == 0) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL, " ");
		return FALSE;
	}
	
	return TRUE;
}

/**
 * midgard_workspace_manager_move_content:
 * @self: #MidgardWorkspaceManager instance
 * @type: the type name
 * @src: source #MidgardWorkspace 
 * @dest: destination #MidgardWorkspace
 * @error: pointer to store returned error
 *
 * Moves the content of given #type name from @src, source #MidgardWorkspace to 
 * @dest, destination one.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05.5
 */ 
gboolean
midgard_workspace_manager_move_content (const MidgardWorkspaceManager *self, const gchar *type, MidgardWorkspace *src, MidgardWorkspace *dest, GError **error) 
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (type != NULL, FALSE);
	g_return_val_if_fail (src != NULL, FALSE);
	g_return_val_if_fail (dest != NULL, FALSE);

	MidgardDBObjectClass *dbklass = g_type_class_peek (g_type_from_name (type));
	if (!dbklass) {
		g_set_error (error, MIDGARD_GENERIC_ERROR,
				MIDGARD_GENERIC_ERROR_INVALID_NAME,
				"'%s' is not registered in GType system", type);
		return FALSE;
	}

	const gchar *table = midgard_core_class_get_table (dbklass);
	if (!table) {
		g_set_error (error, MIDGARD_GENERIC_ERROR,
				MIDGARD_GENERIC_ERROR_OBJECT_NO_STORAGE,
				"No table defined for '%s'", type);
		return FALSE;
	}
	
	GString *query = g_string_new ("UPDATE ");
	g_string_append_printf (query, "%s SET midgard_ws_id = %d WHERE midgard_ws_id = %d", 
			table, dest->priv->id, src->priv->id);

	gint rv = midgard_core_query_execute (self->priv->mgd, query->str, FALSE);
	g_string_free (query, TRUE);

	if (rv == 0) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL, " ");
		return FALSE;
	}
	
	return TRUE;
}

/**
 * midgard_workspace_manager_get_object_workspace:
 * @self: #MidgardWorkspaceManager instance
 * @object: #GObject instance
 * @error: pointer to store returned error
 *
 * Get the workspace associated with given object. Valid #MidgardObject or #MidgardDBObject is recommended.
 * In other case, object instance is ignored and %NULL is returned unconditionaly.
 *
 * Returns: (transfer full): #MidgardWorkspace or %NULL
 *
 * Since: 12.09.2
 */ 
MidgardWorkspace*
midgard_workspace_manager_get_object_workspace (const MidgardWorkspaceManager *self, GObject *object, GError **error)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (object != NULL, NULL);

	if (!MIDGARD_IS_OBJECT(object))
		return;
	if (!MIDGARD_IS_DBOBJECT(object))
		return;

	guint ws_id = MGD_OBJECT_WS_ID (object);
	if (ws_id == 0)
		return NULL;

	guint row_id;
	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	const GValue *val = midgard_core_workspace_get_value_by_id (mgd, MGD_WORKSPACE_FIELD_IDX_ID, ws_id, &row_id);
	if (!val) {
		g_set_error (error, MIDGARD_WORKSPACE_STORAGE_ERROR,
				MIDGARD_WORKSPACE_STORAGE_ERROR_INVALID_VALUE,
				"Can not find workspace with invalid identifier");
		return NULL;
	}
	
	MidgardWorkspace *ws = midgard_workspace_new ();
	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_GET_CLASS (ws);
	dbklass->dbpriv->set_from_data_model (MIDGARD_DBOBJECT (ws), mgd->priv->workspace_model, row_id, 0);
	
	return ws;
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class = NULL;

enum {
	PROPERTY_CONNECTION = 1
};

static void
_midgard_workspace_manager_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardWorkspaceManager *self = MIDGARD_WORKSPACE_MANAGER (instance);
	self->priv = g_new (MidgardWorkspaceManagerPrivate, 1);
	self->priv->mgd = NULL;
}

static GObject *
_midgard_workspace_manager_constructor (GType type,
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
_midgard_workspace_manager_dispose (GObject *object)
{
	MidgardWorkspaceManager *self = (MidgardWorkspaceManager *)object;	
	if (self->priv->mgd)
		g_object_ref (self->priv->mgd);
	self->priv->mgd = NULL;

	parent_class->dispose(object);
}

static void
_midgard_workspace_manager_finalize (GObject *object)
{
	MidgardWorkspaceManager *self = MIDGARD_WORKSPACE_MANAGER (object);

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
_midgard_workspace_manager_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardWorkspaceManager *self = (MidgardWorkspaceManager *) object;
	
	switch (property_id) {

		case PROPERTY_CONNECTION:
			/* write and constructor only */
			break;
		
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
_midgard_workspace_manager_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardWorkspaceManager *self = (MidgardWorkspaceManager *) (object);
	GObject *mgd;

 	switch (property_id) {

		case PROPERTY_CONNECTION:
			if (!G_VALUE_HOLDS_OBJECT (value))
				return;
			self->priv->mgd = g_value_dup_object (value);
			break;
		
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
    			break;
	}
}

static void
_midgard_workspace_manager_class_init (MidgardWorkspaceManagerClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_workspace_manager_constructor;
	object_class->dispose = _midgard_workspace_manager_dispose;
	object_class->finalize = _midgard_workspace_manager_finalize;

	object_class->set_property = _midgard_workspace_manager_set_property;
	object_class->get_property = _midgard_workspace_manager_get_property;

	 /* Properties */
	GParamSpec *pspec = g_param_spec_object ("connection",
			"MidgardConnection", 
			"Pointer to a connection, WorkspaceManager has been initialized for",
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardWorkspaceManager:connection:
	 * @manager: #MidgardWorkspaceManager instance
	 *
	 * Pointer to a connection, @manager has been initialized for
	 */
	g_object_class_install_property (object_class, PROPERTY_CONNECTION, pspec);
}

GType
midgard_workspace_manager_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardWorkspaceManagerClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			(GClassInitFunc)_midgard_workspace_manager_class_init,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardWorkspaceManager),
			0,      /* n_preallocs */
			_midgard_workspace_manager_instance_init    /* instance_init */
		};
      	
  		type = g_type_register_static (G_TYPE_OBJECT, "MidgardWorkspaceManager", &info, 0);
		
    	}
    	return type;
}

