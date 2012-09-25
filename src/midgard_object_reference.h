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

#ifndef MIDGARD_OBJECT_REFERENCE_H
#define MIDGARD_OBJECT_REFERENCE_H

#include <glib-object.h>
#include "midgard_workspace.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_OBJECT_REFERENCE (midgard_object_reference_get_type()) 
#define MIDGARD_OBJECT_REFERENCE(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_OBJECT_REFERENCE, MidgardObjectReference))
#define MIDGARD_OBJECT_REFERENCE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_OBJECT_REFERENCE, MidgardObjectReferenceClass))
#define MIDGARD_IS_OBJECT_REFERENCE(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_OBJECT_REFERENCE))
#define MIDGARD_IS_OBJECT_REFERENCE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_OBJECT_REFERENCE_TYPE))
#define MIDGARD_OBJECT_REFERENCE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_OBJECT_REFERENCE_TYPE, MidgardObjectReferenceClass))

typedef struct _MidgardObjectReference MidgardObjectReference;
typedef struct _MidgardObjectReferencePrivate MidgardObjectReferencePrivate;
typedef struct _MidgardObjectReferenceClass MidgardObjectReferenceClass;

struct _MidgardObjectReference {
	GObject parent;

	/* < private > */
	MidgardObjectReferencePrivate *priv;
};	

struct _MidgardObjectReferenceClass {
	GObjectClass parent;
};

GType 			midgard_object_reference_get_type	(void);
MidgardObjectReference  *midgard_object_reference_new 		(const gchar *id, const gchar *name, MidgardWorkspace *Workspace);
MidgardWorkspace 	*midgard_object_reference_get_workspace	(MidgardObjectReference *self, GError **error);

G_END_DECLS

#endif /* MIDGARD_OBJECT_REFERENCE_H */
