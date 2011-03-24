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

#include "midgard_replicator.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include "midgard_object.h"
#include <sys/stat.h>
#include "midgard_blob.h"
#include "midgard_timestamp.h"
#include "midgard_error.h"
#include "midgard_user.h"
#include "midgard_core_query.h"
#include "guid.h"
#include "midgard_schema_object_factory.h"

#define __mr_dbus_send(_obj, _action) \
	gchar *_dbus_path = g_strconcat("/replication/", \
			G_OBJECT_TYPE_NAME(G_OBJECT(_obj)), \
			"/", _action, NULL); \
	midgard_core_dbus_send_serialized_object(_obj, _dbus_path); \
        g_free(_dbus_path);

/**
 * midgard_replicator_serialize:
 * @object: GObject (or derived class) instance
 *
 * Returns: serialized objects as xml content or %NULL on failure.
 */ 
gchar *
midgard_replicator_serialize (GObject *object) 
{
	g_return_val_if_fail (object != NULL, NULL);

	return midgard_core_object_to_xml(object);
}

/**
 * midgard_replicator_export:
 * @object: #MidgardDBObject instance
 *
 * Given object is not serialized. Its storage record is marked as exported.
 * 
 * @see midgard_object_update() to find out possible error codes returned in case of #MidgardObject failure.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 */ 
gboolean 
midgard_replicator_export (MidgardDBObject *object) 
{
	g_return_val_if_fail (object != NULL, FALSE);

	MidgardConnection *mgd = MGD_OBJECT_CNC(object);
	g_return_val_if_fail (mgd != NULL, FALSE);

	const gchar *guid = MGD_OBJECT_GUID(object);

	if (guid == NULL){
	
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INVALID_PROPERTY_VALUE, "Empty guid value! ");
		return FALSE;
	}

	/* FIXME, invoke update as MidgardDBObject virtual method */
	if (MIDGARD_IS_OBJECT (object)) {
		
		g_signal_emit(object, MIDGARD_OBJECT_GET_CLASS(object)->signal_action_export, 0);
		return _midgard_object_update(MIDGARD_OBJECT(object), OBJECT_UPDATE_EXPORTED, NULL);
	}
	
	return FALSE;
}

/**
 * midgard_replicator_export_by_guid:
 * @mgd: #MidgardConnection instance
 * @guid: guid which identifies object to be exported
 *
 * Marks object's storage record as exported.
 *
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * Given guid is NULL or empty string (MGD_ERR_INVALID_PROPERTY_VALUE)
 * </para></listitem>
 * <listitem><para>
 * Object identified by given guid doesn't exist (MGD_ERR_NOT_EXISTS)
 * </para></listitem>
 * <listitem><para>
 * Object identified by given guid is purged (MGD_ERR_OBJECT_PURGED)
 * </para></listitem>
 * <listitem><para>
 * Internal storage error (MGD_ERR_INTERNAL)
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE on success, %FALSE otherwise
 */ 
gboolean 
midgard_replicator_export_by_guid (MidgardConnection *mgd, const gchar *guid)
{
	g_return_val_if_fail (mgd != NULL, FALSE);

	if(guid == NULL || (guid && (*guid == '\0'))) {

		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INVALID_PROPERTY_VALUE, "Empty or NULL guid given");
		return FALSE;
	}

	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	GString *sql = g_string_new("SELECT ");
	g_string_append_printf(sql,
			"typename, object_action FROM repligard "
			"WHERE guid = '%s' ",
			guid);
	
	GdaDataModel *model =
		midgard_core_query_get_model(mgd, sql->str);
	guint rows = gda_data_model_get_n_rows(model);
	g_string_free(sql, TRUE);
	
	if(rows == 0) {
		
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_NOT_EXISTS);
		g_object_unref(model);
		return FALSE;
	}
	
	const GValue *value = midgard_data_model_get_value_at(model, 0, 0);
	const gchar *classname = g_value_get_string(value);
	value = midgard_data_model_get_value_at(model, 0, 1);
	guint action = MGD_OBJECT_ACTION_NONE;
	MIDGARD_GET_UINT_FROM_VALUE(action, value);

	MidgardObjectClass *klass = 
		MIDGARD_OBJECT_GET_CLASS_BY_NAME(classname);

	if(klass == NULL) {
	
		g_warning("Failed to get class pointer for '%s', an object identified with '%s'", classname, guid);
		g_object_unref(model);
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);
		return FALSE;
	}

	g_object_unref(model);

	const gchar *table = 
		midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(klass)); 
	GValue tval = {0, };
	gchar *timeupdated;
	gint qr;

	switch(action) {
		
		case MGD_OBJECT_ACTION_PURGE:
			MIDGARD_ERRNO_SET(mgd, MGD_ERR_OBJECT_PURGED);
			return FALSE;
			break;

		default:
			sql = g_string_new("UPDATE ");
			g_string_append_printf(sql,
					"%s SET ",
					table);


			midgard_timestamp_new_current (&tval);
			timeupdated = midgard_timestamp_get_string_from_value (&tval);
			
			g_string_append_printf(sql, "metadata_exported='%s' WHERE guid = '%s' ", 
					timeupdated, guid);

			qr = midgard_core_query_execute(mgd, sql->str, TRUE);
			g_string_free(sql, TRUE);

			g_value_unset (&tval);
			g_free(timeupdated);

			if (qr == 0) {
				MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);
				return FALSE;
			}

			return TRUE;

			break;
	}

	return FALSE;
}

/**
 * midgard_replicator_export_purged:
 * @mgd: #MidgardConnection instance
 * @classname: name of #MidgardObjectClass derived one
 * @startdate: optional start date 
 * @enddate: optional end date
 *
 * Exports all purged objects of given class. If @startdate or @enddate are not NULL,
 * all objects which were purged between dates will be exported.
 *
 * Returns: xml buffer with serialized objects or %NULL if there are no objects matching given criteria.
 */ 
gchar *
midgard_replicator_export_purged (MidgardConnection *mgd, const gchar *classname, const gchar *startdate, const gchar *enddate)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (classname != NULL, NULL);

	MidgardDBObjectClass *klass = g_type_class_peek (g_type_from_name (classname));
	g_return_val_if_fail (klass != NULL, NULL);

	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	GString *sql = g_string_new(" SELECT guid, object_action_date ");
	g_string_append_printf(sql, " FROM repligard where typename = '%s' "
			" AND object_action = %d",
			G_OBJECT_CLASS_NAME(klass),
			MGD_OBJECT_ACTION_PURGE);

	if(startdate != NULL){
		g_string_append_printf(sql, " AND object_action_date > '%s' ",
				startdate);
	}

	if(enddate != NULL) {
		g_string_append_printf(sql, " AND object_action_date < '%s' ",
				enddate);
	}

	GdaDataModel *model =
		midgard_core_query_get_model(mgd, sql->str);
	guint rows = gda_data_model_get_n_rows(model);
	g_string_free(sql, TRUE);

	if(!model) 
		return NULL;

	if(rows == 0) {
		
		if(model) g_object_unref(model);
		return NULL;
	}

	xmlNode *object_node;
	xmlDoc *doc = 
		midgard_core_object_create_xml_doc();
	xmlNode *root_node = 
		xmlDocGetRootElement(doc);
	
	guint i;
	for(i = 0; i < rows; i++) {
		
		gboolean free_converted = FALSE;
		const gchar *purged = NULL;
		GValue strv = {0, };

		const GValue *value = 
			midgard_data_model_get_value_at(model, 0, i);
		const gchar *guid = g_value_get_string(value);
		
		value = midgard_data_model_get_value_at(model, 1, i);
		if(G_IS_VALUE(value) && G_VALUE_TYPE(value) == GDA_TYPE_TIMESTAMP) {
			
			g_value_init(&strv, G_TYPE_STRING);
			free_converted = TRUE;
			if(g_value_transform((const GValue *)value, &strv)) 
				purged = g_value_get_string(&strv);
	
		} else {

			purged = g_value_get_string(value);
		}
			
		object_node = xmlNewNode(NULL, BAD_CAST G_OBJECT_CLASS_NAME(klass));
		xmlNewProp(object_node, BAD_CAST "purge", BAD_CAST "yes");
		xmlNewProp(object_node, BAD_CAST "guid", BAD_CAST guid);
		xmlNewProp(object_node, BAD_CAST "purged", BAD_CAST purged);
		xmlAddChild(root_node, object_node);

		if(free_converted)
			g_value_unset(&strv);
	}

	g_object_unref(model);
	xmlChar *buf;
	gint size;
	xmlDocDumpFormatMemoryEnc(doc, &buf, &size, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	
	return (gchar*) buf;
}

/**
 * midgard_replicator_serialize_blob:
 * @object: #MidgardObject of MIDGARD_TYPE_ATTACHMENT type
 *
 * Serialize midgard_blob binary data.
 *
 * Returns: Newly allocated xml buffer, which holds blob data base64 encoded, or %NULL.
 */    
gchar *
midgard_replicator_serialize_blob (MidgardObject *object)
{	
	GType attachment_type = g_type_from_name ("midgard_attachment");
	g_return_val_if_fail (G_OBJECT_TYPE (object) == attachment_type, NULL);

	MidgardBlob *blob = midgard_blob_new(object, NULL);

	if(!blob) 
		return NULL;
	
	gsize bytes_read = 0;
	gchar *content = midgard_blob_read_content(blob, &bytes_read);
	
	if(!content) {

		g_object_unref(blob);
		return NULL;
	}

	gchar *encoded =
		g_base64_encode((const guchar *)content, bytes_read);
	g_free(content);

	xmlDoc *doc = midgard_core_object_create_xml_doc();
	xmlNode *root_node =
		xmlDocGetRootElement(doc);
	xmlNode *blob_node = 
		xmlNewTextChild(root_node, NULL,
				(const xmlChar*)
				"midgard_blob",
				BAD_CAST encoded);
	xmlNewProp(blob_node, BAD_CAST "guid",
			BAD_CAST MGD_OBJECT_GUID (object));

	g_free(encoded);

	xmlChar *buf;
	gint size;
	xmlDocDumpFormatMemoryEnc(doc, &buf, &size, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	
	return (gchar*) buf;
}

/**
 * midgard_replicator_export_blob:
 * @object: #MidgardObject of MIDGARD_TYPE_ATTACHMENT type
 *
 * Alias for midgard_replicator_serialize_blob().
 *
 * Returns: serialized object as xml data
 */ 
gchar *
midgard_replicator_export_blob(MidgardObject *object)
{
	return midgard_replicator_serialize_blob(object);
}

/**
 * midgard_replicator_unserialize:
 * @mgd: #MidgardConnection instance
 * @xml: xml buffer which holds serialized object
 * @force: toggle to force unserialization
 *
 * Returns: (transfer full): Newly allocated array of GObjects
 */ 
GObject **
midgard_replicator_unserialize (MidgardConnection *mgd, const gchar *xml, gboolean force)
{
	return midgard_core_object_from_xml(mgd, xml, force); 
}

/**
 * midgard_replicator_import_object:
 * @object: #MidgardDBObject instance
 * @force: toggle to force import
 *
 * Imports given object to underlying storage
 *
 * Cases to return %FALSE:
 * <itemizedlist>
 * <listitem><para>
 * Given guid is NULL or empty string (MGD_ERR_INVALID_PROPERTY_VALUE)
 * </para></listitem>
 * <listitem><para>
 * Object is already imported (MGD_ERR_OBJECT_IMPORTED)
 * </para></listitem>
 * <listitem><para>
 * Object identified is deleted (MGD_ERR_OBJECT_DELETED)
 * </para></listitem>
 * <listitem><para>
 * Object doesn't provide metadata one (MGD_ERR_NO_METADATA)
 * </para></listitem>
 * </itemizedlist>
 *
 * Set @force toggle if you want to import object even if it's already imported or deleted.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 */ 
gboolean 
midgard_replicator_import_object (MidgardDBObject *object, gboolean force)
{	
	MidgardConnection *mgd = MGD_OBJECT_CNC (object);
	const gchar *guid = MGD_OBJECT_GUID (object);

	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (g_type_is_a (G_OBJECT_TYPE (object), MIDGARD_TYPE_DBOBJECT), FALSE);

	MIDGARD_ERRNO_SET(object->dbpriv->mgd, MGD_ERR_OK);
	gboolean ret_val = FALSE;

	if (guid == NULL || (guid && *guid == '\0')) {		
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INVALID_PROPERTY_VALUE, "NULL or empty guid");
		return ret_val;
	}

	MidgardMetadata *metadata = MGD_DBOBJECT_METADATA (object);
	if (!metadata) {
		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_NO_METADATA, "Metadata class not available.");
		return ret_val;
	}

	g_signal_emit(object, MIDGARD_OBJECT_GET_CLASS(object)->signal_action_import, 0);

	/* Get object from database */
	MidgardQueryBuilder *builder = midgard_query_builder_new(mgd, G_OBJECT_TYPE_NAME(object));
	if(!builder)
		return FALSE;

	GValue pval = {0, };
	/* Add guid constraint */
	g_value_init(&pval,G_TYPE_STRING);
	g_object_get_property(G_OBJECT(object), "guid", &pval);
	midgard_query_builder_add_constraint(builder, "guid", "=", &pval);
	g_value_unset(&pval);
	
	/* Get db object which is "elder" than imported one */
	/*
	g_value_init(&pval,G_TYPE_STRING);
	g_object_get_property(G_OBJECT(object->metadata), "revised", &pval);
	midgard_query_builder_add_constraint(builder, 
			"metadata.revised", "<", &pval);
	g_value_unset(&pval);
	*/

	/* Get deleted or undeleted object */
	midgard_query_builder_include_deleted (builder);

	guint n_objects;
	GObject **_dbobjects = midgard_query_builder_execute (builder, &n_objects);
	MidgardObject *dbobject;

	if (!_dbobjects){
	
		g_object_unref (G_OBJECT (builder));
		
		if ((mgd->errnum == MGD_ERR_OBJECT_PURGED) && force) {

			/* we need to delete repligard entry here 
			 * In any other case we need to change core's API a bit
			 * and make create method more complicated with 
			 * additional needless cases */

			GString *sql = g_string_new ("DELETE from repligard WHERE ");
			g_string_append_printf (sql,
					"typename = '%s' AND guid = '%s' ",
					G_OBJECT_TYPE_NAME (object),
					MGD_OBJECT_GUID (object));
			midgard_core_query_execute (MGD_OBJECT_CNC (object), sql->str, TRUE);
			g_string_free (sql, TRUE);
			
			ret_val =  _midgard_object_create (MIDGARD_OBJECT (object), MGD_OBJECT_GUID (object), OBJECT_UPDATE_IMPORTED);		
			return ret_val;
		}

		if (mgd->errnum == MGD_ERR_NOT_EXISTS || mgd->errnum == MGD_ERR_OK) {

			ret_val =  _midgard_object_create (MIDGARD_OBJECT(object), MGD_OBJECT_GUID(object), OBJECT_UPDATE_IMPORTED);
			return ret_val;
		}

	} else {

		gchar *updated, *dbupdated;
		dbobject = (MidgardObject *)_dbobjects[0];
		g_free (_dbobjects);

		GValue updated_timestamp = {0, };
		g_value_init (&updated_timestamp, MGD_TYPE_TIMESTAMP);
		GValue dbupdated_timestamp = {0, };
		g_value_init (&dbupdated_timestamp, MGD_TYPE_TIMESTAMP);
		
		metadata = MGD_DBOBJECT_METADATA (object);

		/* Compare revised datetimes. We must know if imported 
		 * object is newer than that one which exists in database */
		g_object_get_property (G_OBJECT (metadata), "revised", &updated_timestamp);
		g_object_get_property (G_OBJECT (metadata), "revised", &dbupdated_timestamp);

		updated = midgard_timestamp_get_string_from_value ((const GValue *)&updated_timestamp);
		dbupdated = midgard_timestamp_get_string_from_value ((const GValue *)&dbupdated_timestamp);

		g_value_unset (&updated_timestamp);
		g_value_unset (&dbupdated_timestamp);

		/* We can use g_ascii_strcasecmp as it type cast every single 
		 * pointer to integer and unsigned char returning substract result */		
		gint datecmp;
	
	     	if (updated == NULL) {

			g_warning ("Trying to import ivalid object. metadata.revised property holds NULL (Object: %s)", 
					MGD_OBJECT_GUID (object));

			if (dbupdated)
				g_free (dbupdated);

			g_object_unref (dbobject);

			return FALSE;
		}

		if (dbupdated == NULL) {

			g_warning ("Trying to import object for invalid object stored in database. metadata.revised property holds NULL (Database object:%s)", MGD_OBJECT_GUID (object));

			if (updated)
				g_free (updated);

			g_object_unref (dbobject);

			return FALSE;
		}

		if (force) {

			datecmp = -1;
		
		} else {
			
			datecmp = g_ascii_strcasecmp ((const gchar *)dbupdated, (const gchar *)updated);
		}

		g_free(updated);
		g_free(dbupdated);

		gboolean deleted;
		gboolean ret;

		if (datecmp > 0 || datecmp == 0) {
			
			/* Database object is more recent or exactly the same */	
			g_object_unref (dbobject);	
			MIDGARD_ERRNO_SET (mgd, MGD_ERR_OBJECT_IMPORTED);
	
			return FALSE;

		} else if (datecmp < 0) {
			
			/* Database object is elder */	
			
			/* DELETE */
			g_object_get (G_OBJECT (metadata), "deleted", &deleted, NULL);
			/* Imported object is marked as deleted , so 
			 * * we delete object from database */
			if (deleted) {

				ret = midgard_object_delete (dbobject, FALSE);
				g_object_unref (dbobject);
				return ret;
			}

			/* UPDATE */
		
			MidgardMetadata *db_metadata = MGD_DBOBJECT_METADATA (dbobject);

			/* Check if dbobject is deleted */
			g_object_get (db_metadata, "deleted", &deleted, NULL);

			guint undelete;
			g_object_get (G_OBJECT (metadata), "deleted", &undelete, NULL);

			if ((deleted && !undelete)) {
				midgard_schema_object_factory_object_undelete (mgd, MGD_OBJECT_GUID (dbobject));
				goto _update_object;
			}

			if (deleted && !force) {
				
				MIDGARD_ERRNO_SET (mgd, MGD_ERR_OBJECT_DELETED);
				g_object_unref (dbobject);
				return FALSE;
			}

			_update_object:
			ret = _midgard_object_update (MIDGARD_OBJECT (object), OBJECT_UPDATE_IMPORTED, NULL);
			g_object_unref (dbobject);
			return ret;
		}		
	}

	return FALSE;
}


static gboolean __import_blob_from_xml(	MidgardConnection *mgd,
					xmlDoc *doc, 
					xmlNode *node)
{
	gchar *content;
	struct stat statbuf;
	const gchar *guid = (const gchar *)xmlGetProp(node, BAD_CAST "guid");

	if (!guid) {
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);
		g_warning("Object's guid is empty. Can not import blob file.");
		g_free((gchar *)guid);
		xmlFreeDoc(doc);
		return FALSE;
	}

	if (!midgard_is_guid((const gchar *)guid)) {
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_INTERNAL);
		g_warning("'%s' is not a valid guid", guid);
		g_free((gchar *)guid);
		xmlFreeDoc(doc);
		return FALSE;
	}

	MidgardObject *object = midgard_schema_object_factory_get_object_by_guid (mgd, guid);

	/* TODO , Add more error messages to inform about object state. 
	 * One is already set by midgard_object_class_get_object_by_guid */
	if (!object) {
		g_free((gchar *)guid);
		xmlFreeDoc(doc);
		return FALSE;
	}

	/* FIXME, define even macro to get blobdir ( core level only ) */
	gchar *blobdir = MIDGARD_DBOBJECT (object)->dbpriv->mgd->priv->config->blobdir;

	if (!blobdir || (*blobdir != '/')
			|| (stat(blobdir, &statbuf) != 0)
			|| !S_ISDIR(statbuf.st_mode)) {
		g_warning("Blobs directory is not set");
		g_free((gchar *)guid);
		xmlFreeDoc(doc);
		return FALSE;
	}

	gchar *location;
	gchar *blobpath = NULL;
	g_object_get(G_OBJECT(object), "location", &location, NULL);
	if (strlen(location) > 1) {
		blobpath = g_strconcat(blobdir, "/", location, NULL);
	}

	/* TODO, Find the way to get content and not its copy */
	/* node->content doesn't seem to hold it */
	content = (gchar *)xmlNodeGetContent(node);

	gsize content_length = (gsize) strlen(content);
	guchar *decoded = g_base64_decode(content, &content_length);
	g_free(content);

	FILE *fp = fopen(blobpath, "w+");
	if (NULL != fp) {
		g_free(decoded);
		g_free((gchar *)guid);
		xmlFreeDoc(doc);
		return FALSE;
	}
	size_t res_length = fwrite(decoded, sizeof(char), content_length, fp);
	fclose(fp);

	g_free(decoded);
	g_free((gchar *)guid);
	xmlFreeDoc(doc);

	if (res_length < content_length) {
		return FALSE;
	}

	return TRUE;
}

/**
 * midgard_replicator_import_from_xml:
 * @mgd: #MidgardConnection instance
 * @xml: data buffer which holds serialized object
 * @force: toggle to force import
 *
 * This method tries to import all objects which could be unserialized from gievn xml.
 * It's not atomic. Check error code returned from midgard_connection_get_error().
 *
 */ 
void 
midgard_replicator_import_from_xml (MidgardConnection *mgd,  const gchar *xml, gboolean force)
{
	g_return_if_fail (mgd != NULL);
	g_return_if_fail (xml != NULL);
	
	xmlDoc *doc = NULL;
	xmlNode *root_node = NULL;
	midgard_core_object_get_xml_doc(mgd, xml, &doc, &root_node);
	
	if(doc == NULL || root_node == NULL)
		return;

	xmlNodePtr child = _get_type_node(root_node->children);
	if(!child) {
		g_warning("Can not get midgard type name from the given xml");
		xmlFreeDoc(doc);
		return;
	}

	GType object_type = g_type_from_name((const gchar *)child->name);
	
	if(object_type == MIDGARD_TYPE_BLOB) {
		/* it will destroy xmlDoc */
		__import_blob_from_xml(mgd, doc, child);
		return;
	}

	xmlChar *attr, *guid_attr;
	MidgardObject *dbobject;

	for(; child; child = _get_type_node(child->next)) {
		
		attr = xmlGetProp(child, BAD_CAST "purge");
		guid_attr = xmlGetProp(child, BAD_CAST "guid");

		if(attr && g_str_equal(attr, "yes")) {
			
			dbobject = midgard_schema_object_factory_get_object_by_guid (mgd, (const gchar *)guid_attr);

			if(dbobject || 
					( !dbobject && 
					 (mgd->errnum == MGD_ERR_OBJECT_DELETED)
					 )) {
		
				midgard_object_purge(dbobject, FALSE);
				if(dbobject)
					g_object_unref(dbobject);
				xmlFree(attr);
				xmlFree(guid_attr);
				continue;
			}
		}

		xmlFree(attr);

		MidgardObject *object =
			midgard_object_new(mgd, (const gchar *)child->name, NULL);
		if(!object) {
			g_warning("Can not create %s instance", child->name);
			xmlFreeDoc(doc);
			xmlFree(attr);
			xmlFree(guid_attr);
			continue;
		}

		if (guid_attr) {
			MGD_OBJECT_GUID (object) = (const gchar *)g_strdup((gchar *)guid_attr);
		}

		if(!_nodes2object(G_OBJECT(object), child->children, force)) {
			xmlFree(guid_attr);
			g_object_unref(object);
			continue;
		}			
		
		if (!midgard_replicator_import_object (MIDGARD_DBOBJECT (object), force)) {

			xmlFree (guid_attr);
			g_object_unref (object);
			continue;

		} else {

			xmlFree (guid_attr);
			g_object_unref (object);
		}
	}

	xmlFreeDoc (doc);
}

/* GOBJECT ROUTINES */

static void _midgard_replicator_class_init(
		gpointer g_class, gpointer g_class_data)
{
	return;
}

/* Returns MidgardReplicator type. */
GType midgard_replicator_get_type(void)
{
	static GType type = 0;

	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardReplicatorClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_replicator_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardReplicator),
			0,              /* n_preallocs */
			NULL /* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardReplicator", &info, 0);
	}

	return type;
}
