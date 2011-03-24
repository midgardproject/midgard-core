/* 
 * Copyright (C) 2006, 2007, 2008, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
 * Copyright (C) 2006 Jukka Zitting <jukka.zitting@gmail.com>
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

#ifndef MIDGARD_CONNECTION_H
#define MIDGARD_CONNECTION_H

#define GETTEXT_PACKAGE "midgard"
#include <glib/gi18n-lib.h>

#include "midgard_defs.h"
#include "midgard_config.h"
#include "midgard_workspace_storage.h"
#include "midgard_workspace_manager.h"

/* convention macros */
#define MIDGARD_TYPE_CONNECTION (midgard_connection_get_type())
#define MIDGARD_CONNECTION(object) (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_CONNECTION, MidgardConnection))
#define MIDGARD_CONNECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_CONNECTION, MidgardConnectionClass))
#define MIDGARD_IS_CONNECTION(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_CONNECTION))
#define MIDGARD_IS_CONNECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_CONNECTION))
#define MIDGARD_CONNECTION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_CONNECTION, MidgardConnectionClass))

typedef struct MidgardConnectionClass MidgardConnectionClass;
typedef struct _MidgardConnectionPrivate MidgardConnectionPrivate; 

struct MidgardConnectionClass {
	GObjectClass parent;

	/* signals */
	void	(*error) 		(GObject *object);
	void	(*auth_changed) 	(GObject *object);
	void 	(*lost_provider)	(GObject *object);
	void	(*connected)		(GObject *object);
	void 	(*disconnected)		(GObject *object);

	/* signals IDs */
	guint signal_id_error;
	guint signal_id_auth_changed;
	guint signal_id_lost_provider;
	guint signal_id_connected;
	guint signal_id_disconnected;
};

struct _MidgardConnection {
	GObject parent;

	/* <public> */
	gint errnum;
	gchar *errstr;
	GError *err;
	
	/* < private > */
	MidgardConnectionPrivate *priv;	
};

GType			 midgard_connection_get_type			(void);
MidgardConnection 	*midgard_connection_new				(void);
gboolean 		 midgard_connection_open			(MidgardConnection *self, const gchar *name, GError **error);
GHashTable 		*midgard_connection_open_all			(gboolean userdir); 
gboolean 		 midgard_connection_open_config			(MidgardConnection *self, MidgardConfig *config);
gboolean 		 midgard_connection_open_from_file		(MidgardConnection *self, const gchar *filepath, GError **error);
gboolean		 midgard_connection_is_connected		(MidgardConnection *self);
gboolean 		 midgard_connection_set_loglevel		(MidgardConnection *self, const gchar *level, GLogFunc log_func);
guint 			 midgard_connection_get_loglevel		(MidgardConnection *self);
void 			 midgard_connection_set_loghandler		(MidgardConnection *self, guint loghandler);
guint 			 midgard_connection_get_loghandler		(MidgardConnection *self);
gint 			 midgard_connection_get_error			(MidgardConnection *self);
void 			 midgard_connection_set_error			(MidgardConnection *self, gint errcode);
const gchar 		*midgard_connection_get_error_string		(MidgardConnection *self);
MidgardUser 		*midgard_connection_get_user			(MidgardConnection *self);
MidgardConnection 	*midgard_connection_copy			(MidgardConnection *self);
gboolean		midgard_connection_reopen			(MidgardConnection *self);
gchar 			**midgard_connection_list_auth_types		(MidgardConnection *self, guint *n_types);
void			midgard_connection_enable_quota			(MidgardConnection *self, gboolean toggle);
void			midgard_connection_enable_replication		(MidgardConnection *self, gboolean toggle);
void			midgard_connection_enable_dbus			(MidgardConnection *self, gboolean toggle);
void			midgard_connection_enable_workspace		(MidgardConnection *self, gboolean toggle);
gboolean 		midgard_connection_is_enabled_quota		(MidgardConnection *self);
gboolean 		midgard_connection_is_enabled_replication	(MidgardConnection *self);
gboolean 		midgard_connection_is_enabled_dbus		(MidgardConnection *self);
gboolean 		midgard_connection_is_enabled_workspace		(MidgardConnection *self);
void			midgard_connection_close			(MidgardConnection *self);
gboolean                midgard_connection_set_workspace                (MidgardConnection *self, MidgardWorkspaceStorage *workspace);
const MidgardWorkspaceStorage   *midgard_connection_get_workspace       (MidgardConnection *self);
const MidgardWorkspaceManager	*midgard_connection_get_workspace_manager (MidgardConnection *self);

#endif /* MIDGARD_CONNNECTION_H */
