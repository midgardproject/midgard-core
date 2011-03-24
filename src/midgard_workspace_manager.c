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

/* This is not nice. Can be done as iface's private virtual method. */
#define __SET_MANAGER(__obj, __mngr) \
	if (MIDGARD_IS_WORKSPACE (__obj)) MIDGARD_WORKSPACE (__obj)->priv->manager = __mngr; \
	if (MIDGARD_IS_WORKSPACE_CONTEXT (__obj)) MIDGARD_WORKSPACE_CONTEXT (__obj)->priv->manager = __mngr; 

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

	MidgardWorkspaceManager *self = g_object_new (MIDGARD_TYPE_WORKSPACE_MANAGER, NULL);
	self->priv->mgd = mgd;

	return self;
}

/**
 * midgard_workspace_manager_create:
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
midgard_workspace_manager_create (const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, const gchar *path, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (ws != NULL, FALSE);

	gboolean rv = MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (ws)->priv->create (self, ws, path, error);
	if (rv) {
		__SET_MANAGER (ws, self);
		midgard_core_workspace_list_all (self->priv->mgd);
		/* TODO, emit signal */
	}
	return rv;
}

/**
 * midgard_workspace_manager_update:
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
midgard_workspace_manager_update (const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, GError **error)
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
 * midgard_workspace_manager_purge:
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
midgard_workspace_manager_purge (const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, GError **error)
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

/* GOBJECT ROUTINES */

static GObjectClass *parent_class = NULL;

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
	return;
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
_midgard_workspace_manager_class_init (MidgardWorkspaceManagerClass *klass, gpointer class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_workspace_manager_constructor;
	object_class->dispose = _midgard_workspace_manager_dispose;
	object_class->finalize = _midgard_workspace_manager_finalize;
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

