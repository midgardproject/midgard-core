/* 
 * Copyright (C) 2006 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_REPLICATOR_H
#define MIDGARD_REPLICATOR_H

#include "midgard_object.h"

/* convention macros */
#define MIDGARD_TYPE_REPLICATOR (midgard_replicator_get_type())
#define MIDGARD_REPLICATOR(object)  \
	(G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_REPLICATOR, MidgardReplicator))
#define MIDGARD_REPLICATOR_CLASS(klass)  \
	(G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_REPLICATOR, MidgardReplicatorClass))
#define MIDGARD_IS_REPLICATOR(object)   \
	(G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_REPLICATOR))
#define MIDGARD_IS_REPLICATOR_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_REPLICATOR))
#define MIDGARD_REPLICATOR_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_REPLICATOR, MidgardReplicatorClass))

typedef struct _MidgardReplicatorPrivate MidgardReplicatorPrivate;

typedef struct _MidgardReplicator MidgardReplicator;
typedef struct _MidgardReplicator midgard_replicator;

struct _MidgardReplicator{
	GObject parent;
};

typedef struct MidgardReplicatorClass MidgardReplicatorClass;

struct MidgardReplicatorClass{
	GObjectClass parent;
	
	/* API methods */
	gchar 		*(*serialize)		(GObject *object);
	gboolean	(*export)		(MidgardDBObject *object);
	gchar 		*(*export_purged)	(MidgardConnection *mgd, MidgardDBObjectClass *klass, const gchar *startdate, const gchar *enddate);
	gchar 		*(*serialize_blob)	(MidgardObject *object);
	gchar 		*(*export_blob)		(MidgardObject *object);
	gboolean 	(*export_by_guid)	(const gchar *guid);
	gchar 		*(*export_media)	(MidgardObject *object);
	GObject 	*(*unserialize)		(MidgardConnection *mgd, const gchar *xml, gboolean force);
	gboolean	(*import_object)	(MidgardDBObject *object, gboolean force);
	void		(*import_from_xml)	(MidgardConnection *mgd, const gchar *xml, gboolean force);
};

GType midgard_replicator_get_type(void);

gchar 			*midgard_replicator_serialize		(GObject *object);
gboolean 		midgard_replicator_export		(MidgardDBObject *object);
gchar 			*midgard_replicator_export_purged	(MidgardConnection *mgd, MidgardObjectClass *klass, const gchar *startdate, const gchar *enddate);
gchar 			*midgard_replicator_serialize_blob	(MidgardObject *object);
gchar 			*midgard_replicator_export_blob		(MidgardObject *object);
gboolean 		midgard_replicator_export_by_guid 	(MidgardConnection *mgd, const gchar *guid);
gchar 			*midgard_replicator_export_media	(MidgardObject *object);
GObject 		**midgard_replicator_unserialize	(MidgardConnection *mgd, const gchar *xml, gboolean force);
gboolean 		midgard_replicator_import_object	(MidgardDBObject *object, gboolean force);
void 			midgard_replicator_import_from_xml	(MidgardConnection *mgd, const gchar *xml, gboolean force);

#endif /* MIDGARD_REPLICATOR_H */
