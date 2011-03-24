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
#define MIDGARD_OBJECT(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object), MIDGARD_TYPE_OBJECT, MidgardObject))
#define MIDGARD_OBJECT_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_OBJECT, MidgardObjectClass))
#define MIDGARD_IS_OBJECT(object)   (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_OBJECT))
#define MIDGARD_IS_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_OBJECT))
#define MIDGARD_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_OBJECT, MidgardObjectClass))
#define MIDGARD_OBJECT_GET_CLASS_BY_NAME(name) ((MidgardObjectClass*) g_type_class_peek(g_type_from_name(name)))

typedef struct _MidgardObject MidgardObject;
typedef struct _MidgardObjectPrivate MidgardObjectPrivate;
typedef struct _MidgardObjectClassPrivate MidgardObjectClassPrivate;

struct _MidgardObject {
	MidgardDBObject parent;

	/* <private> */
	MidgardObjectPrivate *priv;	
};

struct _MidgardObjectClass {
	MidgardDBObjectClass parent;
	
	/* private */
	MidgardObjectClassPrivate *priv;

	/* methods */
	const MidgardConnection *(*get_connection) (MidgardDBObject *);	

	/* signals */
	void (*action_create)		(MidgardObject *object);
	void (*action_create_hook)   	(MidgardObject *object);
	void (*action_created)		(MidgardObject *object);
	void (*action_update)		(MidgardObject *object);
	void (*action_update_hook)   	(MidgardObject *object);
	void (*action_updated)		(MidgardObject *object);
	void (*action_delete)		(MidgardObject *object);
	void (*action_delete_hook)   	(MidgardObject *object);
	void (*action_deleted)		(MidgardObject *object);
	void (*action_purge)		(MidgardObject *object);
	void (*action_purge_hook)   	(MidgardObject *object);
	void (*action_purged)		(MidgardObject *object);
	void (*action_import)   	(MidgardObject *object);
	void (*action_import_hook)   	(MidgardObject *object);
	void (*action_imported) 	(MidgardObject *object);
	void (*action_export)		(MidgardObject *object);
	void (*action_export_hook)   	(MidgardObject *object);
	void (*action_exported) 	(MidgardObject *object);
	void (*action_loaded)		(MidgardObject *object);
	void (*action_loaded_hook)   	(MidgardObject *object);
	void (*action_approve)		(MidgardObject *object);
	void (*action_approve_hook)	(MidgardObject *object);
	void (*action_approved)		(MidgardObject *object);
	void (*action_unapprove)	(MidgardObject *object);
	void (*action_unapprove_hook)	(MidgardObject *object);
	void (*action_unapproved)	(MidgardObject *object);
	void (*action_lock)		(MidgardObject *object);
	void (*action_lock_hook)	(MidgardObject *object);
	void (*action_locked)		(MidgardObject *object);
	void (*action_unlock)		(MidgardObject *object);
	void (*action_unlock_hook)	(MidgardObject *object);
	void (*action_unlocked)		(MidgardObject *object);

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

MidgardObject *midgard_object_new (MidgardConnection *mgd, const gchar *name, GValue *value);
MidgardObject *midgard_object_factory (MidgardConnection *mgd, const gchar *name, GValue *value);

gboolean midgard_object_get_by_id(MidgardObject *object, guint id);
gboolean midgard_object_get_by_guid(MidgardObject *object, const gchar *guid);
gboolean midgard_object_get_by_path(MidgardObject *self, const gchar *path);

gboolean midgard_object_update(MidgardObject *self);
gboolean midgard_object_create(MidgardObject *object);
gboolean midgard_object_delete(MidgardObject *object, gboolean check_dependents);
gboolean midgard_object_purge(MidgardObject *object, gboolean check_dependents);

gchar * midgard_object_build_path(MidgardObject *mobj);

gboolean midgard_object_has_dependents(MidgardObject *self);
gboolean midgard_object_set_guid(MidgardObject *self, const gchar *guid);
void midgard_object_set_connection(MidgardObject *self, MidgardConnection *mgd);
const MidgardConnection *midgard_object_get_connection(MidgardObject *self);

gboolean midgard_object_approve(MidgardObject *self);
gboolean midgard_object_is_approved(MidgardObject *self);
gboolean midgard_object_unapprove(MidgardObject *self);

gboolean midgard_object_lock(MidgardObject *self);
gboolean midgard_object_is_locked(MidgardObject *self);
gboolean midgard_object_unlock(MidgardObject *self);

MidgardWorkspace	*midgard_object_get_workspace (MidgardObject *self);

GValue *midgard_object_get_schema_property (MidgardObject *self, const gchar *property);
void midgard_object_set_schema_property (MidgardObject *self, const gchar *property, GValue *value);

#endif /* MIDGARD_OBJECT_H */
