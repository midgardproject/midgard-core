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

#ifndef MIDGARD_CORE_WORKSPACE_H
#define MIDGARD_CORE_WORKSPACE_H

#include "midgard_dbobject.h"
#include "midgard_workspace.h"
#include "midgard_workspace_context.h"
#include "midgard_workspace_storage.h"
#include "midgard_workspace_manager.h"

G_BEGIN_DECLS

#define MGD_WORKSPACE_TABLE "midgard_workspace"

#define MGD_WORKSPACE_OID_FIELD "midgard_ws_oid_id"
#define MGD_WORKSPACE_ID_FIELD "midgard_ws_id"

#define MGD_WORKSPACE_FIELD_ID          "id"
#define MGD_WORKSPACE_FIELD_IDX_ID      0
#define MGD_WORKSPACE_FIELD_UP          "up"
#define MGD_WORKSPACE_FIELD_IDX_UP      1
#define MGD_WORKSPACE_FIELD_GUID        "guid"
#define MGD_WORKSPACE_FIELD_IDX_GUID    2
#define MGD_WORKSPACE_FIELD_NAME        "name"
#define MGD_WORKSPACE_FIELD_IDX_NAME    3

struct _MidgardWorkspacePrivate {
	MidgardWorkspace *parent_ws;
	gchar *path;
	gchar *name;
	guint id;
	guint up_id;
	const MidgardWorkspaceManager *manager;
	const MidgardWorkspaceContext *context;
};

MidgardWorkspacePrivate *midgard_core_workspace_private_new ();
void 			midgard_core_workspace_private_free (MidgardWorkspacePrivate *ws_priv);

struct _MidgardWorkspaceManagerPrivate {
	        MidgardConnection *mgd;
};

struct _MidgardWorkspaceStorageIFacePrivate {
	GSList		*(*list_ids)	(MidgardWorkspaceStorage *self);
	guint		(*get_id)	(MidgardWorkspaceStorage *self);
	gboolean	(*create)	(const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *self, const gchar *path, GError **error);
	gboolean	(*update)	(const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *self, GError **error);
	gboolean	(*purge)	(const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *self, GError **error);
	gboolean	(*path_exists)	(const MidgardWorkspaceManager *manager, const gchar *path);
	gboolean	(*get_by_path)	(const MidgardWorkspaceManager *manager, MidgardWorkspaceStorage *self, const gchar *path, GError **error);
};

void		midgard_core_workspace_list_all			(MidgardConnection *mgd);
gboolean	midgard_core_workspace_name_exists		(MidgardWorkspace *workspace, MidgardWorkspace *parent);
GSList 		*midgard_core_workspace_get_parent_names	(MidgardConnection *mgd, guint up);
const GValue 	*midgard_core_workspace_get_value_by_id 	(MidgardConnection *mgd, guint field_id, guint id, guint *row_id);
gint		midgard_core_workspace_get_col_id_by_name 	(MidgardConnection *mgd, const gchar *name, gint col_idx, gint up_id_check, guint *row_id);
GSList 		*midgard_core_workspace_get_context_ids 	(MidgardConnection *mgd, guint id);
gint		midgard_core_workspace_get_id_by_path		(MidgardConnection *mgd, const gchar *path, guint *row_id, GError **error);

G_END_DECLS

#endif /* MIDGARD_CORE_WORKSPACE_H */
