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

#ifndef MIDGARD_REPLIGARD_H
#define MIDGARD_REPLIGARD_H

#include "midgard_dbobject.h"
#include "midgard_defs.h"
#include "midgard_object.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_REPLIGARD (midgard_repligard_get_type())
#define MIDGARD_REPLIGARD(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_REPLIGARD, MidgardRepligard))
#define MIDGARD_REPLIGARD_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_REPLIGARD, MidgardRepligardClass))
#define MIDGARD_IS_REPLIGARD(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_REPLIGARD))
#define MIDGARD_IS_REPLIGARD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_REPLIGARD))
#define MIDGARD_REPLIGARD_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_REPLIGARD, MidgardRepligardClass))

typedef struct _MidgardRepligard MidgardRepligard;
typedef struct _MidgardRepligardClass MidgardRepligardClass;

struct _MidgardRepligardClass{
	MidgardDBObjectClass parent;
};

struct _MidgardRepligard{
	MidgardDBObject parent;

	gchar *object_guid;
	gchar *type_name;
	guint object_action;
	guint workspace_id;

	/* < private > */
	MidgardDBObjectPrivate *dbpriv;

};

GType 				midgard_repligard_get_type		(void);
MidgardRepligard		*midgard_repligard_new			(MidgardConnection *mgd);
gboolean			midgard_repligard_create_object_info	(MidgardRepligard *self, MidgardObject *object, GError **error);
gboolean			midgard_repligard_update_object_info	(MidgardRepligard *self, MidgardObject *object, guint action, GError **error);
gboolean			midgard_repligard_purge_object_info	(MidgardRepligard *self, MidgardObject *object, guint action, GError **error);

G_END_DECLS

#endif /* MIDGARD_REPLIGARD_H */
