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

#ifndef MIDGARD_WORKSPACE_MANAGER_H
#define MIDGARD_WORKSPACE_MANAGER_H

#include "midgard_dbobject.h"
#include "midgard_defs.h"
#include "midgard_workspace_context.h"
#include "midgard_workspace_storage.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_WORKSPACE_MANAGER (midgard_workspace_manager_get_type())
#define MIDGARD_WORKSPACE_MANAGER(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_WORKSPACE_MANAGER, MidgardWorkspaceManager))
#define MIDGARD_WORKSPACE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_WORKSPACE_MANAGER, MidgardWorkspaceManagerClass))
#define MIDGARD_IS_WORKSPACE_MANAGER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_WORKSPACE_MANAGER))
#define MIDGARD_IS_WORKSPACE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_WORKSPACE_MANAGER))
#define MIDGARD_WORKSPACE_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_WORKSPACE_MANAGER, MidgardWorkspaceManagerClass))

typedef struct _MidgardWorkspaceManager MidgardWorkspaceManager;
typedef struct _MidgardWorkspaceManagerClass MidgardWorkspaceManagerClass;
typedef struct _MidgardWorkspaceManagerPrivate MidgardWorkspaceManagerPrivate; 

struct _MidgardWorkspaceManagerClass{
	GObjectClass parent;
};

struct _MidgardWorkspaceManager{
	GObject parent;

	/* < private > */
	MidgardWorkspaceManagerPrivate *priv;
};

GType 			midgard_workspace_manager_get_type		(void);
MidgardWorkspaceManager	*midgard_workspace_manager_new			(MidgardConnection *mgd);
gboolean 		midgard_workspace_manager_create		(const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, const gchar *path, GError **error);
gboolean 		midgard_workspace_manager_update		(const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, GError **error);
gboolean 		midgard_workspace_manager_purge			(const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, GError **error);
gboolean 		midgard_workspace_manager_path_exists		(const MidgardWorkspaceManager *self, const gchar *path);
gboolean		midgard_workspace_manager_get_workspace_by_path (const MidgardWorkspaceManager *self, MidgardWorkspaceStorage *ws, const gchar *path, GError **error);

G_END_DECLS

#endif /* MIDGARD_WORKSPACE_MANAGER_H */
