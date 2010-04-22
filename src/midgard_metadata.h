/* 
 * Copyright (C) 2005 Piotr Pokora <piotr.pokora@infoglob.pl>
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
 * */

#ifndef MIDGARD_METADATA_H
#define MIDGARD_METADATA_H

#include <glib-object.h>
#include "midgard_object.h"
#include "midgard_dbobject.h"

/* convention macros */
#define MIDGARD_TYPE_METADATA        		(midgard_metadata_get_type ())
#define MIDGARD_METADATA(obj)        		(G_TYPE_CHECK_INSTANCE_CAST ((obj), MIDGARD_TYPE_METADATA, MidgardMetadata))
#define MIDGARD_METADATA_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_METADATA, MidgardMetadataClass))
#define MIDGARD_IS_METADATA(obj)     		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), MIDGARD_TYPE_METADATA))
#define MIDGARD_IS_METADATA_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_METADATA))
#define MIDGARD_METADATA_GET_CLASS(obj)  	(G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_METADATA, MidgardMetadataClass))

typedef struct _MidgardMetadataPrivate MidgardMetadataPrivate;

struct _MidgardMetadataClass {
	MidgardDBObjectClass parent;
};

struct _MidgardMetadata {
	MidgardDBObject parent;	
	MidgardMetadataPrivate *priv;
};

GType 			midgard_metadata_get_type 	(void);
MidgardMetadata		*midgard_metadata_new 		(MidgardDBObject *object);

#endif /* MIDGARD_METADATA_H */
