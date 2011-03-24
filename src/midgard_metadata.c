/* 
 * Copyright (C) 2005, 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include <config.h>
#include <stdlib.h>
#include <libxml/xmlreader.h>
#include <libxml/parserInternals.h>
#include "midgard_metadata.h"
#include "midgard_timestamp.h"
#include "midgard_core_object.h"
#include "schema.h"
#include "midgard_core_metadata.h"
#include "midgard_user.h"

void __set_from_xml_node(MidgardDBObject *object, xmlNode *node);

static void 
_action_create_callback (MidgardObject *object, gpointer ud)
{
	MidgardMetadata *mdata = (MidgardMetadata *) ud;

	gchar *person_guid = "";
	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	MidgardObject *person = MGD_CNC_PERSON (mgd);

	if (person) 
		person_guid = (gchar *)MGD_OBJECT_GUID (person);			
	
	/* set creator */
	GValue rval = {0, };
	g_value_init (&rval, G_TYPE_STRING);
	g_value_set_string (&rval, person_guid);
	/* set creator */
	midgard_core_metadata_set_creator (mdata, (const GValue *)&rval);
	/* set revisor */
	midgard_core_metadata_set_revisor (mdata, (const GValue *)&rval);
	g_value_unset (&rval);

	/* set created */
	midgard_core_timestamp_set_current_time (mdata->priv->created);

	/* set revised */	
	midgard_core_timestamp_set_current_time (mdata->priv->revised);

	/* set revision */
	GValue rev_val = {0, };
	g_value_init (&rev_val, G_TYPE_UINT);
	g_value_set_uint (&rev_val, 0);
	midgard_core_metadata_set_revision (mdata, (const GValue *)&rev_val);
	g_value_unset (&rev_val);

	/* set default values */
	if (mdata->priv->locker)
		g_free (mdata->priv->locker);
	mdata->priv->locker = g_strdup ("");
	if (mdata->priv->approver)
		g_free (mdata->priv->approver);
	mdata->priv->approver = g_strdup ("");

	if (!mdata->priv->authors)
		mdata->priv->authors = g_strdup ("");
	if (!mdata->priv->owner)
		mdata->priv->owner = g_strdup ("");
}

static void 
_action_update_callback (MidgardObject *object, gpointer ud)
{
	MidgardMetadata *mdata = (MidgardMetadata *) ud;
	
	gchar *person_guid = "";
	MidgardConnection *mgd = MGD_OBJECT_CNC(object);
	MidgardObject *person = MGD_CNC_PERSON (mgd);

	if (person) 
		person_guid = (gchar *)MGD_OBJECT_GUID(person);			

	/* set revisor */
	GValue rval = {0, };
	g_value_init (&rval, G_TYPE_STRING);
	g_value_set_string (&rval, person_guid);
	midgard_core_metadata_set_revisor (mdata, (const GValue *)&rval);
	g_value_unset (&rval);

	/* set revised */
	GValue tval = {0, };
	midgard_timestamp_new_current (&tval);
	midgard_core_metadata_set_revised (mdata, (const GValue *)&tval);	
	g_value_unset (&tval);

	/* set revision */
	midgard_core_metadata_increase_revision (mdata);
}

static void 
_action_delete_callback(MidgardObject *object, gpointer ud)
{
	return;
}

static void 
_action_import_callback(MidgardObject *object, gpointer ud)
{
	MidgardMetadata *mdata = (MidgardMetadata *) ud;	
	midgard_core_timestamp_set_current_time (mdata->priv->imported);
}

static void 
_action_export_callback(MidgardObject *object, gpointer ud)
{
	MidgardMetadata *mdata = (MidgardMetadata *) ud;	

	/* This is backward "compatible" defaults set.
	 * It's done in case when some old object has been never
	 * touched by new API */
	if (!mdata->priv->revised && !mdata->priv->created) {
		midgard_core_timestamp_set_current_time (mdata->priv->created);
		midgard_core_timestamp_set_current_time (mdata->priv->revised);
	}

	if (!mdata->priv->revised && mdata->priv->created) {
		midgard_core_timestamp_set_current_time (mdata->priv->created);
		midgard_core_metadata_increase_revision(mdata);
	}

	if (!mdata->priv->created && mdata->priv->revised) {
		midgard_core_timestamp_set_current_time (mdata->priv->created);
	}
		
	/* set exported */
	midgard_core_timestamp_set_current_time (mdata->priv->exported);
}

/**
 * midgard_metadata_new:
 * @object: #MidgardObject for which metadata is created
 * 
 * Creates new midgard_metadata instance for the given #MidgardObject instance.
 *
 * Do not use g_object_new() as metadata constructor. #MidgardObject pointer is internally 
 * assigned as a pointer to midgard object for which particular metadata object 
 * instance was created.
 * 
 * #MidgardMetadata object has two "kinds" of properties.
 * The first one is settable ( and overwritten ) only by metadata implementation.
 * The second one is freely settable by application. In this case midgard core
 * keep value of such property "as is".  
 *
 * Do not free #MidgardMetadata object's memory as it is automatically
 * freed when particular #MidgardObject object's instance memory is freed. 
 *
 * Returns: newly allocated midgard_metadata instance
 */ 
MidgardMetadata *
midgard_metadata_new (MidgardDBObject *object)
{
	g_return_val_if_fail (object != NULL, NULL);

	MidgardMetadata *self = NULL;
	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	if (!mgd)
		self = g_object_new (MIDGARD_TYPE_METADATA, NULL);
	else 
		self = g_object_new (MIDGARD_TYPE_METADATA, "connection", mgd, NULL);

	g_signal_connect (G_OBJECT (object), "action-create",
			G_CALLBACK (_action_create_callback), self);

	g_signal_connect (G_OBJECT (object), "action-update",
			G_CALLBACK (_action_update_callback), self);

	g_signal_connect (G_OBJECT (object), "action-delete", 
			G_CALLBACK (_action_delete_callback), self);

	g_signal_connect (G_OBJECT (object), "action-import", 
			G_CALLBACK (_action_import_callback), self);

	g_signal_connect (G_OBJECT (object), "action-export", 
			G_CALLBACK (_action_export_callback), self);

	self->priv->object = object;

	return self;
}

void 
_midgard_metadata_set_from_data_model (MidgardDBObject *self, GdaDataModel *model, gint row, guint column_id)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (model != NULL);
	g_return_if_fail (row > -1);

	MidgardDBObject *dbobject = MIDGARD_DBOBJECT (MIDGARD_METADATA (self)->priv->object);
	MgdSchemaTypeAttr *type_attr = MIDGARD_DBOBJECT_GET_CLASS (dbobject)->dbpriv->storage_data;
	MidgardMetadata *metadata = MIDGARD_METADATA (self);

	/* creator */
	const GValue *val = gda_data_model_get_value_at (model, column_id, row, NULL);
	midgard_core_metadata_set_creator (metadata, (GValue *) val);

	/* created */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_created (metadata, val);

	/* revisor */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_revisor (metadata, val);

	/* revised */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_revised (metadata, val);
	
	/* revision */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_revision (metadata, val);
	
	/* locker */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_locker (metadata, val);

	/* locked */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_locked (metadata, val);

	/* approver */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_approver (metadata, val);

	/* approved */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_approved (metadata, val);

	/* authors */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_authors (metadata, val);

	/* owner */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_owner (metadata, val);

	/* schedule_start */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_schedule_start (metadata, val);

	/* schedule_end */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_schedule_end (metadata, val);

	/* hidden */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_hidden (metadata, val);

	/* navnoentry */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_navnoentry (metadata, val);

	/* size */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_size (metadata, val);

	/* published */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_published (metadata, val);

	/* score */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_score (metadata, val);

	/* imported */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_imported (metadata, val);

	/* exported */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_exported (metadata, val);

	/* deleted */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_deleted (metadata, val);

	/* isapproved */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_isapproved (metadata, val);

	/* islocked */
	val = gda_data_model_get_value_at (model, ++column_id, row, NULL);
	midgard_core_metadata_set_islocked (metadata, val);
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class = NULL;

enum {
    MIDGARD_METADATA_CREATOR = 1,
    MIDGARD_METADATA_CREATED,
    MIDGARD_METADATA_REVISOR,
    MIDGARD_METADATA_REVISED,
    MIDGARD_METADATA_REVISION,
    MIDGARD_METADATA_LOCKER,
    MIDGARD_METADATA_LOCKED,
    MIDGARD_METADATA_APPROVER,
    MIDGARD_METADATA_APPROVED,
    MIDGARD_METADATA_AUTHORS,
    MIDGARD_METADATA_OWNER,
    MIDGARD_METADATA_SCHEDULE_START,
    MIDGARD_METADATA_SCHEDULE_END,
    MIDGARD_METADATA_HIDDEN,
    MIDGARD_METADATA_NAV_NOENTRY,
    MIDGARD_METADATA_SIZE,
    MIDGARD_METADATA_PUBLISHED,
    MIDGARD_METADATA_SCORE,
    MIDGARD_METADATA_IMPORTED,
    MIDGARD_METADATA_EXPORTED,
    MIDGARD_METADATA_DELETED,
    MIDGARD_METADATA_ISAPPROVED,
    MIDGARD_METADATA_ISLOCKED
};

static void
_metadata_set_property (GObject *object, guint property_id,
        const GValue *value, GParamSpec *pspec)
{
	MidgardMetadata *self = (MidgardMetadata *) object;
	
	switch (property_id) {
		
		case MIDGARD_METADATA_AUTHORS:
			g_free (self->priv->authors);
			self->priv->authors = g_value_dup_string (value);
			break;
			
		case MIDGARD_METADATA_OWNER:
			g_free (self->priv->owner);
			self->priv->owner = g_value_dup_string (value);
			break;

		case MIDGARD_METADATA_SCHEDULE_START:
			g_free (self->priv->schedule_start);
			self->priv->schedule_start = g_value_dup_boxed (value);
			break;
		
		case MIDGARD_METADATA_SCHEDULE_END:
			g_free (self->priv->schedule_end);
			self->priv->schedule_end = g_value_dup_boxed (value);
			break;
		
		case MIDGARD_METADATA_HIDDEN:
			self->priv->hidden = g_value_get_boolean (value);
			break;
			
		case MIDGARD_METADATA_NAV_NOENTRY:
			self->priv->nav_noentry = g_value_get_boolean (value);
			break;
						
		case MIDGARD_METADATA_PUBLISHED:	    
			g_free (self->priv->published);	
			self->priv->published = g_value_dup_boxed (value);
			break;

		case MIDGARD_METADATA_SCORE:
			self->priv->score = g_value_get_int (value);
			break;
			
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
			break;                  
	}
}

gboolean
_midgard_metadata_dbpriv_get_property (MidgardDBObject *self, const gchar *name, GValue *value)
{
	gchar *datamodel_property = g_strconcat ("metadata_", name, NULL);
	MidgardDBObject *dbobject = MIDGARD_METADATA(self)->priv->object;

	if (!dbobject || !dbobject->dbpriv)
		return FALSE;

	gboolean rv = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->get_property (dbobject, datamodel_property, value);
	g_free (datamodel_property);

	return rv;
}

static void
_metadata_get_property (GObject *object, guint property_id,
        GValue *value, GParamSpec *pspec)
{    
	MidgardMetadata *self =	(MidgardMetadata *) object;

	if (MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->get_property (MIDGARD_DBOBJECT (object), pspec->name, value))
		return;

	switch (property_id) {
		
		case MIDGARD_METADATA_CREATOR:
			g_value_set_string (value, self->priv->creator);	
			break;
			
		case MIDGARD_METADATA_CREATED:
			g_value_set_boxed (value, self->priv->created);
			break;
			
		case MIDGARD_METADATA_REVISOR:
			g_value_set_string (value, self->priv->revisor);
			break;
			
		case MIDGARD_METADATA_REVISED:
			g_value_set_boxed (value, self->priv->revised);
			break;
			
		case MIDGARD_METADATA_LOCKER:
			g_value_set_string (value, self->priv->locker);
			break;
			
		case MIDGARD_METADATA_LOCKED:
			g_value_set_boxed (value, self->priv->locked);
			break;
			
		case MIDGARD_METADATA_APPROVER:
			g_value_set_string (value, self->priv->approver);
			break;
			
		case MIDGARD_METADATA_APPROVED:    
			g_value_set_boxed (value, self->priv->approved);
			break;
			
		case MIDGARD_METADATA_REVISION:             
			g_value_set_uint (value, self->priv->revision);            
			break;
			
		case MIDGARD_METADATA_AUTHORS:
			g_value_set_string (value, self->priv->authors);
			break;
			
		case MIDGARD_METADATA_OWNER:
			g_value_set_string (value, self->priv->owner);
			break;
			
		case MIDGARD_METADATA_SCHEDULE_START:
			g_value_set_boxed (value, self->priv->schedule_start);
			break;
			
		case MIDGARD_METADATA_SCHEDULE_END:
			g_value_set_boxed (value, self->priv->schedule_end);
			break;
			
		case MIDGARD_METADATA_HIDDEN:
			g_value_set_boolean (value, self->priv->hidden);
			break;
			
		case MIDGARD_METADATA_NAV_NOENTRY:
			g_value_set_boolean (value, self->priv->nav_noentry);
			break;
			
		case MIDGARD_METADATA_SIZE:
			g_value_set_uint(value,  self->priv->size);
			break;
			
		case MIDGARD_METADATA_PUBLISHED:
			g_value_set_boxed (value, self->priv->published);
			break;

		case MIDGARD_METADATA_SCORE:
			g_value_set_int (value, self->priv->score);
			break;

		case MIDGARD_METADATA_EXPORTED:
			g_value_set_boxed (value, self->priv->exported);
			break;

		case MIDGARD_METADATA_IMPORTED:
			g_value_set_boxed (value, self->priv->imported);
			break;

		case MIDGARD_METADATA_DELETED:
			g_value_set_boolean (value, self->priv->deleted);
			break;
		
		case MIDGARD_METADATA_ISLOCKED:
			g_value_set_boolean (value, self->priv->is_locked);
			break;
		
		case MIDGARD_METADATA_ISAPPROVED:
			g_value_set_boolean (value, self->priv->is_approved);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
			break;
	}
}

static GObject *
_metadata_constructor (GType type, guint n_construct_properties,
			GObjectConstructParam *construct_properties)
{

	GObject *object = (GObject *)
		G_OBJECT_CLASS (__parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return G_OBJECT(object);
}

static void
_metadata_dispose (GObject *object)
{
	__parent_class->dispose (object);
}

static void 
_metadata_instance_init(GTypeInstance *instance, gpointer g_class) 
{	
	MidgardMetadata *self = (MidgardMetadata *)instance;

	/* allocate private data */
	self->priv = g_new(MidgardMetadataPrivate, 1);
	self->priv->creator = NULL;
	self->priv->created = midgard_timestamp_new();
	self->priv->revised = midgard_timestamp_new();
	self->priv->revisor = NULL;
	self->priv->locker = NULL;
	self->priv->locked = midgard_timestamp_new();
	self->priv->approved = midgard_timestamp_new();
	self->priv->approver = NULL;
	self->priv->authors = NULL;
	self->priv->owner = NULL;
	self->priv->revision = 0;
	self->priv->schedule_start = midgard_timestamp_new();
	self->priv->schedule_end = midgard_timestamp_new();
	self->priv->hidden = FALSE;
	self->priv->nav_noentry = FALSE;
	self->priv->size = 0;
	self->priv->published = midgard_timestamp_new();
	self->priv->score = 0;
	self->priv->exported = midgard_timestamp_new();
	self->priv->imported = midgard_timestamp_new();
	self->priv->deleted = FALSE;

	self->priv->lock_is_set = FALSE;
	self->priv->is_locked = FALSE;

	self->priv->approve_is_set = FALSE;
	self->priv->is_approved = FALSE;

	self->priv->object = NULL;
}

static void 
_metadata_object_finalize(GObject *object) 
{
	g_assert(object);

	MidgardMetadata *self = (MidgardMetadata *)object;	

	/* Free private data */
	g_free(self->priv->creator);
	g_free(self->priv->created);
	g_free(self->priv->revised);
	g_free(self->priv->revisor);
	g_free(self->priv->locker);
	g_free(self->priv->locked);
	g_free(self->priv->approved);
	g_free(self->priv->approver);
	g_free(self->priv->authors);
	g_free(self->priv->owner);
	g_free(self->priv->schedule_start);
	g_free(self->priv->schedule_end);
	g_free(self->priv->published);
	g_free(self->priv->exported);
	g_free(self->priv->imported);

	g_free(self->priv);

	__parent_class->finalize (object);
}

/* Initialize class */
static void
_metadata_class_init (gpointer g_class, gpointer g_class_data)
{	
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardMetadataClass *klass = MIDGARD_METADATA_CLASS (g_class); 
	GParamSpec *pspec;
	__parent_class = g_type_class_peek_parent (g_class);

	gobject_class->constructor = _metadata_constructor;	
	gobject_class->set_property = _metadata_set_property;
	gobject_class->get_property = _metadata_get_property;
	gobject_class->dispose = _metadata_dispose;
	gobject_class->finalize = _metadata_object_finalize;

	MgdSchemaPropertyAttr *prop_attr;
	MgdSchemaTypeAttr *type_attr = midgard_core_schema_type_attr_new();
	
	/* Register properties */        

	/* creator */
	pspec = g_param_spec_string ("creator",
			"metadata_creator",
			"Guid of the person who created object's record",
			NULL /* We can set midgard admin */,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_CREATOR,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_GUID;
	prop_attr->field = g_strdup("metadata_creator");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("creator");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"creator"), prop_attr);

	/* created */
	pspec = g_param_spec_boxed ("created",
			"metadata_created",
			"ISO datetime when object's record has been created",
			MGD_TYPE_TIMESTAMP,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_CREATED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_created");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("created");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"created"), prop_attr);

	/* revisor */
	pspec = g_param_spec_string ("revisor",
			"metadata_revisor",
			"Guid of the person who revised object's record",
			NULL, 
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_REVISOR,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_GUID;
	prop_attr->field = g_strdup("metadata_revisor");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("revisor");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"revisor"), prop_attr);

	/* revised */
	pspec = g_param_spec_boxed ("revised",
			"metadata_revised",
			"ISO datetime when object's record has been revised",
			MIDGARD_TYPE_TIMESTAMP,
			G_PARAM_READABLE); 
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_REVISED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_revised");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("revised");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"revised"), prop_attr);

	/* revision */
	pspec = g_param_spec_uint ("revision",
			"metadata_revision",
			"Object's record revision number",
			0, G_MAXUINT32, 0, G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_REVISION,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup("metadata_revision");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("revision");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"revision"), prop_attr);

	/* locker */
	pspec = g_param_spec_string ("locker",
			"metadata_locker",
			"Guid of the person who (un)locked object's record",
			NULL,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_LOCKER,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_GUID;
	prop_attr->field = g_strdup("metadata_locker");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("locker");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"locker"), prop_attr);

	/* locked */
	pspec = g_param_spec_boxed ("locked",
			"metadata_locked",
			"ISO datetime when object's record has been (un)locked",
			MGD_TYPE_TIMESTAMP,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_LOCKED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_locked");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("locked");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"locked"), prop_attr);

	/* approver */
	pspec = g_param_spec_string ("approver",
			"metadata_approver",
			"Guid of the person who (un)approved object's record",
			NULL,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_APPROVER,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_GUID;
	prop_attr->field = g_strdup("metadata_approver");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("approver");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"approver"), prop_attr);

	/* approved */
	pspec = g_param_spec_boxed ("approved",
			"metadata_approved",
			"ISO datetime when object's record has been (un)approved",
			MGD_TYPE_TIMESTAMP,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_APPROVED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_approved");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("approved");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"approved"), prop_attr);

	/* authors */
	pspec = g_param_spec_string ("authors",
			"metadata_authors",
			"The person who is an author.",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_AUTHORS,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_STRING;
	prop_attr->field = g_strdup("metadata_authors");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("authors");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"authors"), prop_attr);

	/* owner */
	pspec = g_param_spec_string ("owner",
			"metadata_owner",
			"Group's guid which is an owner of the object",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_OWNER,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_GUID;
	prop_attr->field = g_strdup("metadata_owner");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("owner");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"owner"), prop_attr);

	/* schedulestart */
	pspec = g_param_spec_boxed ("schedulestart",
			"metadata_schedule_start",
			"ISO datetime when publication time starts.",
			MGD_TYPE_TIMESTAMP,
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_SCHEDULE_START,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_schedule_start");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("schedulestart");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"schedulestart"), prop_attr);

	/* scheduleend */
	pspec = g_param_spec_boxed ("scheduleend",
			"metadata_schedule_end",
			"ISO datetime when publication time ends.",
			MGD_TYPE_TIMESTAMP,
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_SCHEDULE_END,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_schedule_end");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("scheduleend");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"scheduleend"), prop_attr);

	/* hidden */
	pspec = g_param_spec_boolean ("hidden",
			"metadata_hidden",
			"Whether object should be hidden or not.",
			FALSE,
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_HIDDEN,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_BOOLEAN;
	prop_attr->field = g_strdup("metadata_hidden");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("hidden");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"hidden"), prop_attr);

	/* nav no entry */
	pspec = g_param_spec_boolean ("navnoentry",
			"metadata_nav_noentry",
			"Whether object is part of navigation (e.g. menu).",
			FALSE,
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_NAV_NOENTRY,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_BOOLEAN;
	prop_attr->field = g_strdup("metadata_nav_noentry");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("navnoentry");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"navnoentry"), prop_attr);

	/* size */
	pspec = g_param_spec_uint ("size",
			"metadata_size",
			"Object's size in bytes",
			0, G_MAXUINT32, 0, G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_SIZE,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup("metadata_size");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("size");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"size"), prop_attr);

	/* published */
	pspec = g_param_spec_boxed ("published",
			"metadata_published",
			"ISO datetime when object should be published (by default it's created time)",
			MGD_TYPE_TIMESTAMP,
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_PUBLISHED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_published");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("published");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"published"), prop_attr);

	/* score */
	pspec = g_param_spec_int ("score",
			"metadata_score",
			"Object's record score",
			G_MININT32, G_MAXINT32, 0, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,      
			MIDGARD_METADATA_SCORE,        
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_INT;
	prop_attr->field = g_strdup("metadata_score");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("score");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"score"), prop_attr);

	/* exported */
	pspec = g_param_spec_boxed ("exported",
			"metadata_exported",
			"Whether object has been exported or not.",
			MGD_TYPE_TIMESTAMP,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_EXPORTED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_exported");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("exported");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"exported"), prop_attr);

	/* imported */
	pspec = g_param_spec_boxed ("imported",
			"metadata_imported",
			"Whether object has been imported or not.",
			MGD_TYPE_TIMESTAMP,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_IMPORTED,
			pspec);
    	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_TIMESTAMP;
	prop_attr->field = g_strdup("metadata_imported");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("imported");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"imported"), prop_attr);

	/* deleted */
	pspec = g_param_spec_boolean ("deleted",
			"metadata_deleted",
			"Whether object is deleted or not.",
			FALSE,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_DELETED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_BOOLEAN;
	prop_attr->field = g_strdup("metadata_deleted");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("deleted");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"deleted"), prop_attr);

	/* isapproved */
	pspec = g_param_spec_boolean ("isapproved",
			"metadata_isapproved",
			"Whether object is approved or not.",
			FALSE,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_ISAPPROVED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_BOOLEAN;
	prop_attr->field = g_strdup("metadata_isapproved");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("isapproved");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"isapproved"), prop_attr);

	/* islocked */
	pspec = g_param_spec_boolean ("islocked",
			"metadata_islocked",
			"Whether object is locked or unlocked.",
			FALSE,
			G_PARAM_READABLE);
	g_object_class_install_property (gobject_class,
			MIDGARD_METADATA_ISLOCKED,
			pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_BOOLEAN;
	prop_attr->field = g_strdup("metadata_islocked");
	prop_attr->table = NULL;
	prop_attr->tablefield = NULL;
	prop_attr->name = g_strdup ("islocked");
	g_hash_table_insert(type_attr->prophash,
			g_strdup((gchar *)"islocked"), prop_attr);

	/* Initialize private member */
	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_CLASS (klass);
	dbklass->dbpriv = g_new(MidgardDBObjectPrivate, 1);
	dbklass->dbpriv->storage_data = type_attr;
	dbklass->dbpriv->set_from_xml_node = __set_from_xml_node;
	dbklass->dbpriv->add_fields_to_select_statement = NULL;
	dbklass->dbpriv->has_metadata = FALSE;
	dbklass->dbpriv->get_property = _midgard_metadata_dbpriv_get_property;
	dbklass->dbpriv->set_property = midgard_core_metadata_dbpriv_set_property;
	dbklass->dbpriv->set_from_data_model = _midgard_metadata_set_from_data_model;
}

GType midgard_metadata_get_type (void)
{   
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardMetadataClass),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			_metadata_class_init,   /* class_init */			
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			sizeof (MidgardMetadata),
			0,      /* n_preallocs */
			_metadata_instance_init    /* instance_init */
		};
		type = g_type_register_static (MIDGARD_TYPE_DBOBJECT, "MidgardMetadata", &info, 0);
	}
	return type;
}

xmlNode *__metadata_lookup_node(xmlNode *node, const gchar *name)
{
	xmlNode *cur = NULL;

	for (cur = node; cur; cur = cur->next) {
		
		if (cur->type == XML_ELEMENT_NODE) {
			
			if (g_str_equal (cur->name, name))
					return cur;
		}
	}

	return NULL;
}

gchar *__get_node_content_string(xmlNode *node)
{
	if(!node)
		return NULL;

	gchar *content = NULL;
	content = (gchar *)xmlNodeGetContent(node);

	if(content == NULL)
		return NULL;

	xmlParserCtxtPtr parser = xmlNewParserCtxt();
	xmlChar *decoded =
		 xmlStringDecodeEntities(parser,
				 (const xmlChar *) content,
				 XML_SUBSTITUTE_REF,
				 0, 0, 0);

	xmlFreeParserCtxt(parser);
	g_free(content);
	
	return (gchar*)decoded;
}

void __set_date_from_node (xmlNode *node, MidgardTimestamp **date)
{
	if (!node)
		return;

	gchar *content = NULL;
	content = (gchar *)xmlNodeGetContent (node);

	if (content == NULL) {
		g_warning ("Can not handle NULL date for datetime property");
		return;
	}

	if (*date != NULL)
		g_free (*date);

	*date = midgard_timestamp_new_from_iso8601 (content);
	xmlFree (content);
	
	return;
}

gint __get_node_content_int(xmlNode *node)
{
	if(!node)
		return 0;

	gchar *content = NULL;
	content = (gchar *)xmlNodeGetContent(node);

	if(content == NULL)
		return 0;

	gint rv = (gint)atoi((gchar *)content);

	g_free(content);

	return rv;
}

guint __get_node_content_uint(xmlNode *node)
{
	if(!node)
		return 0;

	gchar *content = NULL;
	content = (gchar *)xmlNodeGetContent(node);

	if(content == NULL)
		return 0;

	guint rv = (guint)atoi((gchar *)content);

	g_free(content);

	return rv;
}

gboolean __get_node_content_bool(xmlNode *node)
{
	if(!node)
		return FALSE;

	gchar *content = NULL;
	content = (gchar *)xmlNodeGetContent(node);

	if(content == NULL)
		return 0;

	gboolean rv = (gboolean)atoi((gchar *)content);

	g_free(content);

	return rv;
}

void __set_from_xml_node(MidgardDBObject *object, xmlNode *node)
{
	g_assert(object != NULL);

	MidgardMetadata *self = MIDGARD_METADATA(object);
	xmlNode *lnode = NULL;	

	lnode = __metadata_lookup_node(node, "creator");
	self->priv->creator = __get_node_content_string(lnode);

	lnode = __metadata_lookup_node(node, "created");
	__set_date_from_node(lnode, &self->priv->created);	

	lnode = __metadata_lookup_node(node, "revisor");
	self->priv->revisor = __get_node_content_string(lnode);
	
	lnode = __metadata_lookup_node(node, "revised");
	__set_date_from_node(lnode, &self->priv->revised);	

	lnode = __metadata_lookup_node(node, "revision");
	self->priv->revision = __get_node_content_uint(lnode);

	lnode = __metadata_lookup_node(node, "locker");
	self->priv->locker = __get_node_content_string(lnode);
	
	lnode = __metadata_lookup_node(node, "locked");
	__set_date_from_node(lnode, &self->priv->locked);	

	lnode = __metadata_lookup_node(node, "approver");
	self->priv->approver = __get_node_content_string(lnode);
	
	lnode = __metadata_lookup_node(node, "approved");
	__set_date_from_node(lnode, &self->priv->approved);	

	lnode = __metadata_lookup_node(node, "authors");
	self->priv->authors = __get_node_content_string(lnode);
	
	lnode = __metadata_lookup_node(node, "owner");
	self->priv->owner = __get_node_content_string(lnode);

	lnode = __metadata_lookup_node(node, "schedulestart");
	__set_date_from_node(lnode, &self->priv->schedule_start);	

	lnode = __metadata_lookup_node(node, "scheduleend");
	__set_date_from_node(lnode, &self->priv->schedule_end);	

	lnode = __metadata_lookup_node(node, "hidden");
	self->priv->hidden = __get_node_content_bool(lnode);

	lnode = __metadata_lookup_node(node, "navnoentry");
	self->priv->nav_noentry = __get_node_content_bool(lnode);
	
	lnode = __metadata_lookup_node(node, "size");
	self->priv->size = __get_node_content_uint(lnode);

	lnode = __metadata_lookup_node(node, "published");
	__set_date_from_node(lnode, &self->priv->published);	

	lnode = __metadata_lookup_node(node, "score");
	self->priv->score = __get_node_content_int(lnode);
	
	lnode = __metadata_lookup_node(node, "imported");
	__set_date_from_node(lnode, &self->priv->imported);	

	lnode = __metadata_lookup_node(node, "exported");
	__set_date_from_node(lnode, &self->priv->exported);	

	lnode = __metadata_lookup_node(node, "deleted");
	self->priv->deleted = __get_node_content_bool(lnode);
	
	lnode = __metadata_lookup_node(node, "islocked");
	self->priv->is_locked = __get_node_content_bool(lnode);
	
	lnode = __metadata_lookup_node(node, "isapproved");
	self->priv->is_approved = __get_node_content_bool(lnode);
}
