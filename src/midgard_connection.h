/* 
 * Copyright (C) 2006, 2007, 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

/* convention macros */
#define MIDGARD_TYPE_CONNECTION (midgard_connection_get_type())
#define MIDGARD_CONNECTION(object)  \
	        (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_CONNECTION, MidgardConnection))
#define MIDGARD_CONNECTION_CLASS(klass)  \
	        (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_CONNECTION, MidgardConnectionClass))
#define MIDGARD_IS_CONNECTION(object)   \
	        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_CONNECTION))
#define MIDGARD_IS_CONNECTION_CLASS(klass) \
	        (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_CONNECTION))
#define MIDGARD_CONNECTION_GET_CLASS(obj) \
	        (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_CONNECTION, MidgardConnectionClass))

typedef struct MidgardConnectionClass MidgardConnectionClass;
typedef struct _MidgardConnectionPrivate MidgardConnectionPrivate; 
typedef struct _MidgardConnection midgard_connection;

struct MidgardConnectionClass {
	GObjectClass parent;

	/* class members */
	gboolean	(*open)			(MidgardConnection *mgd, const char *name, GError **error);
	gboolean	(*open_config) 		(MidgardConnection *mgd, MidgardConfig *config);
	gboolean 	(*set_loglevel) 	(MidgardConnection *mgd, const gchar *levelstring, GLogFunc log_func);
	guint 		(*get_loglevel) 	(MidgardConnection *mgd);
	void 		(*set_loghandler) 	(MidgardConnection *mgd, guint loghandler);
	guint 		(*get_loghandler) 	(MidgardConnection *mgd);

	/* signals */
	void	(*error) 		(GObject *object);
	void	(*auth_changed) 	(GObject *object);
	void 	(*lost_provider)	(GObject *object);

	/* signals IDs */
	guint signal_id_error;
	guint signal_id_auth_changed;
	guint signal_id_lost_provider;
};

struct _MidgardConnection {

	/* < public > */
	GObject parent;
	gint errnum;
	gchar *errstr;
	GError *err;
	
	/* < private > */
	MidgardConnectionPrivate *priv;
	
	/* Legacy, FIXME, remove */	
	gpointer person;	
	gboolean quota;	
};

GType			 midgard_connection_get_type			(void);
MidgardConnection 	*midgard_connection_new				(void);
gboolean 		 midgard_connection_open			(MidgardConnection *self, const gchar *name, GError **error);
GHashTable 		*midgard_connection_open_all			(gboolean userdir); 
gboolean 		 midgard_connection_open_config			(MidgardConnection *self, MidgardConfig *config);
gboolean 		 midgard_connection_open_from_file		(MidgardConnection *self, const gchar *filepath, GError **error);
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

/*
 * This is a part of public API , however it shouldn't be used in any
 * application unless you really know what you are doing.
 */
MidgardConnection *midgard_connection_struct_new(void);

MidgardConnection *midgard_connection_struct2gobject(MidgardConnection *conn);

gboolean midgard_connection_struct_open_config(
	MidgardConnection *mgd, MidgardConfig *config);

void midgard_connection_struct_free(MidgardConnection *self);

#endif /* MIDGARD_CONNNECTION_H */
