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

#ifndef MIDGARD_WORKSPACE_H
#define MIDGARD_WORKSPACE_H

#include "midgard_dbobject.h"
#include "midgard_defs.h"
#include "midgard_workspace_context.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_WORKSPACE (midgard_workspace_get_type())
#define MIDGARD_WORKSPACE(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_WORKSPACE, MidgardWorkspace))
#define MIDGARD_WORKSPACE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_WORKSPACE, MidgardWorkspaceClass))
#define MIDGARD_IS_WORKSPACE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_WORKSPACE))
#define MIDGARD_IS_WORKSPACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_WORKSPACE))
#define MIDGARD_WORKSPACE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_WORKSPACE, MidgardWorkspaceClass))

typedef struct _MidgardWorkspaceClass MidgardWorkspaceClass;

struct _MidgardWorkspaceClass{
	MidgardDBObjectClass parent;
};

struct _MidgardWorkspace{
	MidgardDBObject parent;

	/* < private > */
	MidgardWorkspacePrivate *priv;
};

GType 				midgard_workspace_get_type		(void);
MidgardWorkspace		*midgard_workspace_new			();
const MidgardWorkspaceContext	*midgard_workspace_get_context		(MidgardWorkspace *self);

G_END_DECLS

#endif /* MIDGARD_WORKSPACE_H */
