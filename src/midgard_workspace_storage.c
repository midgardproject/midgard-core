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
#include "midgard_core_object.h"

GQuark
midgard_workspace_storage_error_quark (void)
{
	return g_quark_from_static_string ("midgard_workspace_storage_error-quark");
}

/**
 * midgard_workspace_storage_get_path:
 * @self: #MidgardWorkspaceStorage instance
 *
 * Returned path may be %NULL, if given object doesn't exists in storage
 *
 * Returns: (transfer none): a path, given #MidgardWorkspaceStorage is at.
 * Since: 10.05.5
 */
const gchar *
midgard_workspace_storage_get_path (MidgardWorkspaceStorage *self)
{
	g_return_val_if_fail (self != NULL, NULL);

	return MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (self)->get_path (self);
}

/**
 * midgard_workspace_storage_get_workspace_by_name:
 * @self: #MidgardWorkspaceStorage instance
 * @name: a name of #MidgardWorkspaceStorage object to find
 *
 * Returns: newly allocated #MidgardWorkspaceStorage object if found, %NULL otherwise
 * Since: 10.05.5
 */ 
MidgardWorkspaceStorage *
midgard_workspace_storage_get_workspace_by_name (MidgardWorkspaceStorage *self, const gchar *name)
{
	g_return_val_if_fail (self != NULL, NULL);

	return MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (self)->get_workspace_by_name (self, name);
}

/**
 * midgard_workspace_storage_list_children:
 * @self: #MidgardWorkspaceStorage instance
 * @n_objects: a pointer to store number of returned objects
 *
 * In case of #MidgardWorkspaceContext, all #MidgardWorkspace objects from context are 
 * returned. 
 *
 * Returns: (array length=n_objects): newly allocated array of #MidgardWorkspaceStorage objects
 * Since: 10.05.5
 */ 
MidgardWorkspaceStorage **
midgard_workspace_storage_list_children (MidgardWorkspaceStorage *self, guint *n_objects)
{
	g_return_val_if_fail (self != NULL, NULL);

	return MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (self)->list_children (self, n_objects);
}

/**
 * midgard_workspace_storage_list_workspace_names:
 * @self: #MidgardWorkspaceStorage instance
 * @n_names: a pointer to store number of returned names
 * 
 * Returns: (transfer container) (array length=n_names): array of names
 * Since: 10.05.5
 */ 
gchar **
midgard_workspace_storage_list_workspace_names (MidgardWorkspaceStorage *self, guint *n_names)
{
	g_return_val_if_fail (self != NULL, NULL);

	return MIDGARD_WORKSPACE_STORAGE_GET_INTERFACE (self)->list_workspace_names (self, n_names);
}

GType
midgard_workspace_storage_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardWorkspaceStorageIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardWorkspaceStorage", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
	}
	return type;
}
