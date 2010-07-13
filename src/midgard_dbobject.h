/* 
 * Copyright (C) 2007 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_DBOBJECT_H
#define MIDGARD_DBOBJECT_H

#include <glib.h>
#include <glib-object.h>
#include "midgard_defs.h"

/* convention macros */
#define MIDGARD_TYPE_DBOBJECT (midgard_dbobject_get_type())
#define MIDGARD_DBOBJECT(object)  \
	        (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_DBOBJECT, MidgardDBObject))
#define MIDGARD_DBOBJECT_CLASS(klass)  \
	        (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_DBOBJECT, MidgardDBObjectClass))
#define MIDGARD_IS_DBOBJECT(object)   \
	        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_DBOBJECT))
#define MIDGARD_IS_DBOBJECT_CLASS(klass) \
	        (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_DBOBJECT))
#define MIDGARD_DBOBJECT_GET_CLASS(obj) \
	        (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_DBOBJECT, MidgardDBObjectClass))

typedef struct _MidgardDBObject MidgardDBObject;
typedef struct _MidgardDBObjectClass MidgardDBObjectClass;
typedef struct _MidgardDBObjectPrivate MidgardDBObjectPrivate;

GType midgard_dbobject_get_type(void);

struct _MidgardDBObject{
	GObject parent;

	/* <private> */
	MidgardDBObjectPrivate *dbpriv;
};

struct _MidgardDBObjectClass {
	GObjectClass parent;
	/* <private> */
	MidgardDBObjectPrivate *dbpriv;
	
	/* virtual */
	const MidgardConnection *(*get_connection) (MidgardDBObject *object);
};

#endif /* MIDGARD_DBOBJECT_H */
