/* 
 * Copyright (C) 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_DBUS_H
#define MIDGARD_DBUS_H

#include <glib-object.h>
#include "midgard_connection.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_DBUS (midgard_dbus_get_type()) 
#define MIDGARD_DBUS(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_DBUS, MidgardDbus))
#define MIDGARD_DBUS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_DBUS, MidgardDbusClass))
#define MIDGARD_IS_DBUS(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_DBUS))
#define MIDGARD_IS_DBUS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_DBUS))
#define MIDGARD_DBUS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_DBUS, MidgardDbusClass))

typedef struct _MidgardDbus MidgardDbus;
typedef struct _MidgardDbusClass MidgardDbusClass;
typedef struct _MidgardDbusPrivate MidgardDbusPrivate; 

struct _MidgardDbusClass{
	GObjectClass parent;

	/* signals */
	void	(*notified)	(GObject *self, const gchar *message);
	
	/* signals id */
	guint signal_notified;
};

struct _MidgardDbus{
	GObject parent;

	/* < private > */
	MidgardDbusPrivate *priv;
};

GType midgard_dbus_get_type(void);

MidgardDbus	*midgard_dbus_new		(MidgardConnection *mgd, const gchar *path, gboolean use_session);
void 		midgard_dbus_send		(MidgardConnection *mgd, const gchar *path, const gchar *message, gboolean use_session);
const gchar 	*midgard_dbus_get_message	(MidgardDbus *self);
const gchar 	*midgard_dbus_get_name		(void);

G_END_DECLS

#endif /* MIDGARD_DBUS_H */
