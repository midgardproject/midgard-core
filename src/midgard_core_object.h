/* 
 * Copyright (C) 2006, 2008 Piotr Pokora <piotr.pokora@infoglob.pl>
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

#ifndef MIDGARD_CORE_OBJECT_H
#define MIDGARD_CORE_OBJECT_H

#include <libxml/parser.h> 
#include <libxml/tree.h>
#include "midgard_schema.h"
#include <libgda/libgda.h>
#include "midgard_object.h"
#include "schema.h"
#include "midgard_timestamp.h"
#include "midgard_connection.h"

struct _MidgardDBObjectPrivate {

	const gchar *guid;	
	MgdSchemaTypeAttr *storage_data;
	MidgardConnection *mgd;
	gboolean has_metadata;
	MidgardMetadata *metadata;
	GdaDataModel *datamodel;
	gint row;	

	/* GdaSql virtual helpers */
	void			(*add_fields_to_select_statement)	(MidgardDBObjectClass *klass, 
			GdaSqlStatementSelect *select, const gchar *table_name);

	GSList 			*(*set_from_sql)	(MidgardConnection *mgd, GType type, const gchar *sql);	
	void 			(*__set_from_sql)	(MidgardDBObject *self, GdaDataModel *model, gint row);
	void 			(*set_from_data_model)	(MidgardDBObject *self, GdaDataModel *model, gint row);
	void 			(*set_from_xml_node)	(MidgardDBObject *self, xmlNode *node);
	MidgardConnection 	*(*get_connection) 	(MidgardDBObject *self);
	gboolean		(*create_storage)	(MidgardConnection *mgd, MidgardDBObjectClass *klass);
	gboolean 		(*update_storage)	(MidgardConnection *mgd, MidgardDBObjectClass *klass);
	gboolean		(*storage_exists)	(MidgardConnection *mgd, MidgardDBObjectClass *klass);
	gboolean		(*delete_storage)	(MidgardConnection *mgd, MidgardDBObjectClass *klass);
	gboolean		(*get_property)		(MidgardDBObject *self, const gchar *name, GValue *value);
	gboolean		(*set_property)		(MidgardDBObject *self, const gchar *name, GValue *value);
};

#define MGD_OBJECT_GUID(___obj) MIDGARD_DBOBJECT(___obj)->dbpriv->guid
#define MGD_OBJECT_CNC(___obj) MIDGARD_DBOBJECT(___obj)->dbpriv->mgd

#define MGD_DBOBJECT_DBPRIV (__obj) MIDGARD_DBOBJECT(__obj)->dbpriv
#define MGD_DBOBJECT_HAS_METADATA (__obj) MGD_TYPE_ATTR_METADATA_CLASS(MGD_DBOBJECT_DBPRIV(__obj)->storage_data) ? TRUE : FALSE; 
#define MGD_DBOBJECT_METADATA(___obj) MIDGARD_DBOBJECT(___obj)->dbpriv->metadata
#define MGD_DBCLASS_METADATA_CLASS(__klass) MIDGARD_DBOBJECT_CLASS(__klass)->dbpriv->storage_data->metadata_class_ptr
#define MGD_DBCLASS_PROPERTY_UP(__klass) MIDGARD_DBOBJECT_CLASS(__klass)->dbpriv->storage_data->property_up
#define MGD_DBCLASS_PROPERTY_PARENT(__klass) MIDGARD_DBOBJECT_CLASS(__klass)->dbpriv->storage_data->property_parent
#define MGD_DBCLASS_PROPERTY_UNIQUE(__klass) MIDGARD_DBOBJECT_CLASS(__klass)->dbpriv->storage_data->unique_name

/* Private structure for private data of MgdSchema objects */
struct _MidgardObjectPrivate{
	const gchar *action;
	gchar *exported;
	gchar *imported;
	GSList *parameters;
	GHashTable *_params;
};

struct _MidgardObjectClassPrivate {
	MgdSchemaTypeAttr *storage_data;
};

//typedef struct _MidgardTimestamp MidgardTimestamp;

/* Private structure for private data of MgdSchema object's metadata */
struct _MidgardMetadataPrivate {

        /* Object properties */
	gchar  *creator;
	MidgardTimestamp *created;
	gchar *revisor;
	MidgardTimestamp *revised;
	guint revision;
	gchar *locker;
	MidgardTimestamp *locked;
	gchar *approver;
	MidgardTimestamp *approved;
	gchar *authors;
	gchar *owner;
	MidgardTimestamp *schedule_start;
	MidgardTimestamp *schedule_end;
	gboolean hidden;
	gboolean nav_noentry;
	guint32 size;
	MidgardTimestamp *published;
	MidgardTimestamp *exported;
	MidgardTimestamp *imported;
	gboolean deleted;
	gint32 score;

	gboolean is_locked;
	gboolean lock_is_set;

	gboolean is_approved;
	gboolean approve_is_set;
	
	/* Other sruct members */
	MidgardDBObject *object;
};

/* Private structure for Midgard Config object */
struct _MidgardConfigPrivate{
	
	GKeyFile *keyfile;
	guint dbtype;
	GIOChannel *log_channel;
	gchar *configname;
	
	gchar *host;
	gchar *database;
	guint port;
	gchar *dbuser;
	gchar *dbpass;
	gchar *blobdir;
	gchar *logfilename;
	gchar *schemafile;
	gchar *loglevel;
	gboolean tablecreate;
	gboolean tableupdate;
	FILE  *logfile;
	gchar *mgdusername;
	gchar *mgdpassword;
	gboolean testunit;
	guint loghandler;
	guint authtype;
	gchar *pamfile;
};

struct _MidgardConnectionPrivate {
	MidgardConnection *pattern; /* a pointer to connection we made copy from */
        MidgardConfig *config;
	gboolean connected;
	gboolean free_config;
	guint loghandler;
	guint loglevel;
	GData *sg_datalist;
	MidgardSchema *schema;
#ifdef HAVE_LIBGDA_4
	GdaSqlParser *parser;
#else
	GdaClient *client;
#endif
	GdaConnection *connection;

	const gchar *cnc_str;
	const gchar *auth_str;
	
	MidgardUser *user;
	gboolean inherited;
	GTimeVal app_time;
	GTimer *timer;
	gchar *configname;

	gboolean error_clbk_connected;

	/* Auth stack & types */
	GSList *authtypes;
	GSList *authstack;

	gboolean replication;
	gboolean quota;
	gboolean debug;
};

#define MGD_CNC_PERSON(_cnc) _cnc->priv->user ? midgard_user_get_person (_cnc->priv->user) : NULL;

struct _MidgardBlobPrivate {
	MidgardObject *attachment;
	gchar *location;
	MidgardConnection *mgd;
	gchar *blobdir;
	GIOChannel *channel;
	gchar *filepath;				
	gchar *parentguid;
	gchar *content;
	gchar *encoding;
};

typedef enum {
	OBJECT_UPDATE_NONE = 0,
	OBJECT_UPDATE_EXPORTED,
	OBJECT_UPDATE_IMPORTED
} _ObjectActionUpdate;


/* Object's xml */
xmlDoc *midgard_core_object_create_xml_doc(void);
void midgard_core_object_get_xml_doc(  MidgardConnection *mgd,
					const gchar *xml,
					xmlDoc **doc,
					xmlNode **root_node);
gchar *midgard_core_object_to_xml(GObject *object);
gboolean _nodes2object(GObject *object, xmlNode *node, gboolean force);
xmlNode *_get_type_node(xmlNode *node);
GObject **midgard_core_object_from_xml(MidgardConnection *mgd, const gchar *xml, gboolean force);

/* Object's routines */
gboolean _midgard_object_update(MidgardObject *object, _ObjectActionUpdate replicate);
gboolean _midgard_object_create(MidgardObject *object, const gchar *create_guid, _ObjectActionUpdate replicate);
void _object_copy_properties(GObject *src, GObject *dest);

/* Links */
gboolean midgard_core_object_prop_link_is_valid(GType ltype);

/* Tree related routines */
gboolean midgard_core_object_prop_type_is_valid(GType src_type, GType dst_type);
gboolean midgard_core_object_prop_parent_is_valid(GType ptype);
gboolean midgard_core_object_prop_up_is_valid(GType ptype);
GType midgard_core_object_get_property_parent_type(MidgardObjectClass *klass);
GType midgard_core_object_get_property_up_type(MidgardObjectClass *klass);
gboolean midgard_core_object_prop_parent_is_set(MidgardObject *object);
gboolean midgard_core_object_prop_up_is_set(MidgardObject *object);

gboolean midgard_core_object_has_dependents(MidgardObject *self, const gchar *classname);

/* ACL */
gboolean midgard_core_object_property_refuse_private (MidgardConnection *self, MgdSchemaTypeAttr *attr, MidgardDBObject *object, const gchar *property);

/* D-Bus */
void midgard_core_dbus_send_serialized_object(MidgardObject *object, const gchar *path);

#endif /* MIDGARD_CORE_OBJECT_H */
