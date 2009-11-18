/* 
Copyright (C) 2004,2005,2006,2007,2008, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
Copyright (C) 2004 Alexander Bokovoy <ab@samba.org>

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MIDGARD_OBJECT_H
#define MIDGARD_OBJECT_H

#include <glib-object.h>
#include "query_builder.h"
#include "midgard_dbobject.h"

#define MIDGARD_TYPE_OBJECT midgard_object_get_type()
#define MIDGARD_OBJECT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object), MIDGARD_TYPE_OBJECT, MgdObject))
#define MIDGARD_OBJECT_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_OBJECT, MidgardObjectClass))
#define MIDGARD_IS_OBJECT(object)   (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_OBJECT))
#define MIDGARD_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_OBJECT))
#define MIDGARD_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_OBJECT, MidgardObjectClass))
#define MIDGARD_OBJECT_GET_CLASS_BY_NAME(name) ((MidgardObjectClass*) g_type_class_peek(g_type_from_name(name)))

typedef struct _MgdObject MgdObject;
typedef struct _MgdObject MidgardObject;
typedef struct _MgdObject midgard_object;
typedef struct _MidgardObjectPrivate MidgardObjectPrivate;
typedef struct _MidgardObjectClassPrivate MidgardObjectClassPrivate;

struct _MgdObject {
	GObject parent;
	MidgardDBObjectPrivate *dbpriv;
	MidgardObjectPrivate *priv;	
	MidgardMetadata *metadata;	
};

struct _MidgardObjectClass {
	GObjectClass parent;
	
	/* private */
	MidgardDBObjectPrivate *dbpriv;
	MidgardObjectClassPrivate *priv;

	/* methods */
	const MidgardConnection *(*get_connection) (MidgardDBObject *);	

	/* signals */
	void (*action_create)		(MgdObject *object);
	void (*action_create_hook)   	(MgdObject *object);
	void (*action_created)		(MgdObject *object);
	void (*action_update)		(MgdObject *object);
	void (*action_update_hook)   	(MgdObject *object);
	void (*action_updated)		(MgdObject *object);
	void (*action_delete)		(MgdObject *object);
	void (*action_delete_hook)   	(MgdObject *object);
	void (*action_deleted)		(MgdObject *object);
	void (*action_purge)		(MgdObject *object);
	void (*action_purge_hook)   	(MgdObject *object);
	void (*action_purged)		(MgdObject *object);
	void (*action_import)   	(MgdObject *object);
	void (*action_import_hook)   	(MgdObject *object);
	void (*action_imported) 	(MgdObject *object);
	void (*action_export)		(MgdObject *object);
	void (*action_export_hook)   	(MgdObject *object);
	void (*action_exported) 	(MgdObject *object);
	void (*action_loaded)		(MgdObject *object);
	void (*action_loaded_hook)   	(MgdObject *object);
	void (*action_approve)		(MgdObject *object);
	void (*action_approve_hook)	(MgdObject *object);
	void (*action_approved)		(MgdObject *object);
	void (*action_unapprove)	(MgdObject *object);
	void (*action_unapprove_hook)	(MgdObject *object);
	void (*action_unapproved)	(MgdObject *object);
	void (*action_lock)		(MgdObject *object);
	void (*action_lock_hook)	(MgdObject *object);
	void (*action_locked)		(MgdObject *object);
	void (*action_unlock)		(MgdObject *object);
	void (*action_unlock_hook)	(MgdObject *object);
	void (*action_unlocked)		(MgdObject *object);

	/* signals id */
	guint signal_action_loaded;
	guint signal_action_loaded_hook;
	guint signal_action_update;
	guint signal_action_update_hook;
	guint signal_action_updated;
	guint signal_action_create;
	guint signal_action_create_hook;
	guint signal_action_created;
	guint signal_action_delete;
	guint signal_action_delete_hook;
	guint signal_action_deleted;
	guint signal_action_purge;
	guint signal_action_purge_hook;
	guint signal_action_purged;
	guint signal_action_import;
	guint signal_action_import_hook;
	guint signal_action_imported;
	guint signal_action_export;
	guint signal_action_export_hook;
	guint signal_action_exported;
	guint signal_action_approve;
	guint signal_action_approve_hook;
	guint signal_action_approved;
	guint signal_action_unapprove;
	guint signal_action_unapprove_hook;
	guint signal_action_unapproved;
	guint signal_action_lock;
	guint signal_action_lock_hook;
	guint signal_action_locked;
	guint signal_action_unlock;
	guint signal_action_unlock_hook;
	guint signal_action_unlocked;
};

GType midgard_object_get_type(void);

enum MidgardObjectAction
{
	MGD_OBJECT_ACTION_NONE = 0,
	MGD_OBJECT_ACTION_DELETE, /**< Object has been deleted and can be undeleted >*/
	MGD_OBJECT_ACTION_PURGE, /**< Object has been purged ( no recovery or undelete ) >*/
	MGD_OBJECT_ACTION_CREATE, /**< Object has been created >*/
	MGD_OBJECT_ACTION_UPDATE /**< Object has been updated >*/
};

MgdObject *midgard_object_new(MidgardConnection *mgd, const gchar *name, GValue *value);

gboolean midgard_object_get_by_id(MgdObject *object, guint id);
gboolean midgard_object_get_by_guid(MgdObject *object, const gchar *guid);
gboolean midgard_object_get_by_path(MgdObject *self, const gchar *path);

gboolean midgard_object_update(MgdObject *self);
gboolean midgard_object_create(MgdObject *object);
gboolean midgard_object_delete(MgdObject *object);
gboolean midgard_object_undelete(MidgardConnection *mgd, const gchar *guid);
gboolean midgard_object_purge(MgdObject *object);

gchar * midgard_object_build_path(MgdObject *mobj);

gboolean midgard_object_is_in_parent_tree(MgdObject *self, guint rootid, guint id);
gboolean midgard_object_is_in_tree(MgdObject *self, guint rootid, guint id);
gchar * midgard_object_get_tree(MgdObject *object, GSList *tnodes);
const gchar *midgard_object_parent(MgdObject *self);
MgdObject *midgard_object_get_parent(MgdObject *self);

GObject **midgard_object_list(MgdObject *self, guint *n_objects);
GObject **midgard_object_list_children(MgdObject *object, const gchar *childname, guint *n_objects);

gboolean midgard_object_has_dependents(MgdObject *self);
gboolean midgard_object_set_guid(MgdObject *self, const gchar *guid);
void midgard_object_set_connection(MgdObject *self, MidgardConnection *mgd);
const MidgardConnection *midgard_object_get_connection(MgdObject *self);

gboolean midgard_object_approve(MgdObject *self);
gboolean midgard_object_is_approved(MgdObject *self);
gboolean midgard_object_unapprove(MgdObject *self);

gboolean midgard_object_lock(MgdObject *self);
gboolean midgard_object_is_locked(MgdObject *self);
gboolean midgard_object_unlock(MgdObject *self);

#endif /* MIDGARD_OBJECT_H */
