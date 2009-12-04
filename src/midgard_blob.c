/* 
 * Copyright (C) 2006, 2007, 2008, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_blob.h"
#include "midgard_error.h"
#include "midgard_config.h"
#include "midgard_core_object.h"
#include "uuid.h"
#include <glib/gstdio.h>

/* Properties */
enum {
	MIDGARD_BLOB_PGUID = 1,
	MIDGARD_BLOB_CONTENT
};

/* Build attachment's full path. 
 * Create new relative location for attachment if it's empty. 
 * WARNING! Keep in mind that attachment's location might be changed 
 * between midgard_blob method calls! */
static void __get_filepath(MidgardBlob *self)
{
	if(self->priv->filepath)
		return;

	gchar *fname = NULL;
	gchar *location = NULL;
	gchar *up_a, *up_b;
	MidgardObject *attachment = self->priv->attachment;

	GParamSpec *pspec =
		g_object_class_find_property(
				G_OBJECT_GET_CLASS(G_OBJECT(attachment)), "location");

	/* FIXME, we can not depend on just location property here.
	 * This must be defined in mgdschema file. */
	if(!pspec){
		midgard_set_error(self->priv->mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_USER_DATA,
				"Invalid Object. "
				"Blobs can not be attached to %s",
				G_OBJECT_TYPE_NAME(G_OBJECT(attachment)));
		return;
	}

	g_object_get(G_OBJECT(self->priv->attachment), "location", &location, NULL);

	if(location == NULL || *location == '\0') {
		
		fname = midgard_uuid_new();
		up_a = g_strdup(" ");
		up_a[0] = g_ascii_toupper(fname[0]);
		up_b = g_strdup(" ");
		up_b[0] = g_ascii_toupper(fname[1]);
		/* g_strdup is used to avoid filling every byte, which should be done with g_new(gchar, 2) */
		location = g_build_path(G_DIR_SEPARATOR_S, 
				up_a, up_b, fname, NULL);
	
		g_free(fname);
		g_free(up_a);
		g_free(up_b);

		g_object_set(G_OBJECT(self->priv->attachment), "location", location , NULL);

	}

	self->priv->location = location;

	self->priv->filepath = 
		g_build_path(G_DIR_SEPARATOR_S, 
				self->priv->blobdir, self->priv->location, NULL);
}

/* Create private channel */
static void __get_channel(MidgardBlob *self, const gchar *mode)
{
	if(self->priv->channel)
		return;

	GError *err = NULL;
	GIOChannel *channel = self->priv->channel;
	gchar *filepath = self->priv->filepath;
	if(channel == NULL)
		channel = g_io_channel_new_file(filepath, mode ? mode : "w", &err);
	/* Keep "binary" mode. It's useless on *nix but needed for WIN32 */
	
	if(!channel){
		midgard_set_error(self->priv->mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_USER_DATA,
				" %s ",
				err->message);
		g_clear_error(&err);
		return;
	}
	
	g_clear_error(&err);
	
	GIOStatus status = g_io_channel_set_encoding(channel,
			(const gchar *)self->priv->encoding, &err);
	
	if(status != G_IO_STATUS_NORMAL) {
		
		midgard_set_error(self->priv->mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_USER_DATA,
				" %s ",
				err->message);
		g_clear_error(&err);
		g_io_channel_shutdown(channel, TRUE, NULL);
		return;
	}

	self->priv->channel = channel;
}

/**
 * midgard_blob_new: 
 * @attachment: #MidgardObject of MIDGARD_TYPE_ATTACHMENT type.
 * @encoding: file encoding
 * 
 * Default encoding is UTF-8. Set NULL @encoding if such is required. 
 *
 * Instatiate new Midgard Blob object for the given midgard_attachment object. 
 * This is almost the same constructor as g_object_new, but unlike that one,
 * midgard_blob_new requires MidgardObject (midgard_attachment) object's pointer.
 *
 * This constructor defines new relative path for attachment, if midgard_attachment 
 * is associated with midgard_blob and its location is empty. 
 * In any other case, location is not changed.
 * 
 * Returns: newly instatiated #MidgardBlob object or %NULL on failure
 */
MidgardBlob *midgard_blob_new(MidgardObject *attachment, const gchar *encoding)
{
	g_assert(attachment != NULL);
	
	MidgardConnection *mgd = attachment->dbpriv->mgd;

	if(mgd == NULL) {
		g_critical("MidgardConnection not found for given attachment");
		return NULL;
	}

	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	const gchar *blobdir = attachment->dbpriv->mgd->priv->config->blobdir;
	if(!g_file_test(blobdir,G_FILE_TEST_EXISTS)) {
		midgard_set_error(mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_INTERNAL,
				" Blobs directory doesn't exist. %s", attachment->dbpriv->mgd->priv->config->blobdir);
		return NULL;
	}

	if(!g_file_test(blobdir,G_FILE_TEST_IS_DIR)) {
		g_warning("Defined blobs directory is not directory");
		return NULL;
	}

	MidgardBlob *self = g_object_new(MIDGARD_TYPE_BLOB, NULL);
	self->priv->attachment = attachment;
	self->priv->mgd = attachment->dbpriv->mgd;
	self->priv->blobdir = g_strdup(blobdir);
	self->priv->channel = NULL;
	self->priv->encoding = NULL;
	if(encoding != NULL)
		self->priv->encoding = g_strdup(encoding);
	
	__get_filepath(self);

	if(self->priv->filepath == NULL) {		
		g_object_unref(self);
		return NULL;
	}
	
	return self;
}

/**
 * midgard_blob_read_content:
 * @self: MidgardBlob self instance
 * @bytes_read: number of bytes read
 *
 * Returned content should be freed when no longer needed.
 * @bytes_read holds size of returned content. 
 *
 * This function should be used to get content of small files.
 * For large and huge ones midgard_blob_get_handler should be used
 * to get file handle.
 * 
 * Returns: content of the file, or %NULL on failure
 */ 
gchar *midgard_blob_read_content(MidgardBlob *self, gsize *bytes_read)
{
	g_assert(self != NULL);

	MidgardConnection *mgd = self->priv->mgd;
	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	__get_filepath(self);

	if(!self->priv->filepath) {
		 midgard_set_error(mgd,
				 MGD_GENERIC_ERROR,
				 MGD_ERR_USER_DATA,
				 "Invalid attachment. "
				 "Can not read file from empty location");
		 return NULL;
	}	

	__get_channel(self, "r");
	if(!self->priv->channel)
		return NULL;

	GIOChannel *channel = self->priv->channel;

	gchar *content = NULL;
	GError *err = NULL;
	GIOStatus status;
	const gchar *err_msg = "";

	/* Rewind. Channel could be already used for writing. */
	status = g_io_channel_seek_position(channel, 0, G_SEEK_SET, &err);
	if(status != G_IO_STATUS_NORMAL) {
		
		if(err != NULL) 
			err_msg = err->message;
		
		midgard_set_error(self->priv->mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_INTERNAL,
				" %s ",
				err_msg);

		return NULL;
	}
	g_clear_error(&err);

	g_io_channel_set_encoding (channel, NULL, NULL);
	status =
		g_io_channel_read_to_end(channel,
				&content,
				bytes_read,
				&err);
	
	/* FIXME, I have no idea how to determine file encoding */
	/* Let's set UTF-8 and try again */
	if(status == G_IO_STATUS_ERROR) {

		if(err && err->domain == G_CONVERT_ERROR){
			g_io_channel_set_encoding (channel, "UTF-8", NULL);
			g_clear_error(&err);
			status =
				g_io_channel_read_to_end(channel,
						&content,
						bytes_read,
						&err);
		}
	}	

	if(err) g_clear_error(&err);

	err_msg = "";

	if(status == G_IO_STATUS_NORMAL && *bytes_read == 0){	
		
		if(err != NULL)
			err_msg = err->message;

		midgard_set_error(self->priv->mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_INTERNAL,
				" %s ",
				err_msg);

		return NULL;
	}
	
	if(err) g_clear_error(&err);

	return content;
}

/**
 * midgard_blob_write_content:
 * @self: #MidgardBlob self instance.
 * @content: content which should be written to file.
 * 
 * Write given @content to a file.
 * 
 * Returns: %TRUE if content has been written to file, %FALSE otherwise.
 */ 
gboolean  midgard_blob_write_content(MidgardBlob *self,	const gchar *content)
{
	g_assert(self != NULL);
	g_assert(content != NULL);

	MidgardConnection *mgd = self->priv->mgd;
	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	__get_filepath(self);
	
	if(!self->priv->filepath) {
		midgard_set_error(self->priv->mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_USER_DATA,
				"Invalid attachment. "
				"Can not read file from empty location");
		return FALSE;
	}

	__get_channel(self, "w");
	if(!self->priv->channel)
		return FALSE;

	GIOChannel *channel = self->priv->channel;
	GIOStatus status;
	GError *err = NULL;

	status = g_io_channel_write_chars(channel, content,
			strlen(content), NULL, &err);

	g_io_channel_flush(channel, NULL);

	if(status != G_IO_STATUS_NORMAL) {
		midgard_set_error(self->priv->mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_INTERNAL,
				" %s ",
				err->message);
		g_clear_error(&err);

		return FALSE;
	}

	return TRUE;	
}

/**
 * midgard_blob_get_handler:
 * @self: #MidgardBlob instance
 * @mode: fopen mode (r, w, a, b). Default is 'w'.
 *
 * The main idea is to get file handler. On C level it returns
 * GIOChannel, but language bindings could return typical file handler 
 * or anything else which is needed for particular language.
 *
 * Returned channel is owned by midgard_blob and should not be freed
 * or unreferenced.
 *
 * Returns: GIOChannel or %NULL
 */
GIOChannel *midgard_blob_get_handler(MidgardBlob *self, const gchar *mode)
{
	g_assert(self != NULL);
		
	MidgardConnection *mgd = self->priv->mgd;
	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	__get_channel(self, mode);
	if(!self->priv->channel)
		return NULL;

	return self->priv->channel;
}

/** 
 * midgard_blob_get_path:
 * @self: #MidgardBlob instance
 *
 * Returned path is owned by midgard_blob and should not be freed.
 * It basically contains blobdir and relative file's location.
 *
 * Returns: absolute path or %NULL if path is not set yet.
 */ 
const gchar *midgard_blob_get_path(MidgardBlob *self)
{
	g_assert(self != NULL);

	MidgardConnection *mgd = self->priv->mgd;
	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	__get_filepath(self);
	if(!self->priv->filepath)
		return NULL;
	
	return self->priv->filepath;
}

/**
 * midgard_blob_exists:
 * @self: #MidgardBlob instance
 *
 * Check if file associated with midgard_blob exists.
 * This function will also return FALSE, if file is not yet associated.
 *
 * Returns: %TRUE if file exists, %FALSE otherwise
 */ 
gboolean midgard_blob_exists(MidgardBlob *self)
{
	g_assert(self != NULL);
	
	MidgardConnection *mgd = self->priv->mgd;
	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	__get_filepath(self);    
	if(!self->priv->filepath)
		return FALSE; 

	if(g_file_test(self->priv->filepath, G_FILE_TEST_IS_DIR)){
		
		midgard_set_error(self->priv->mgd,
				MGD_GENERIC_ERROR,
				MGD_ERR_INTERNAL,
				" '%s' is a directory ", 
				self->priv->filepath);
		return FALSE;
	}


	return g_file_test(self->priv->filepath, G_FILE_TEST_IS_REGULAR);
}

/**
 * midgard_blob_remove_file:
 * @self: #MidgardBlob self instance.
 *
 * Deletes a file which is associated with blob and located at
 * attachment's location which is initialized for blob.
 * #midgard_blob_exists should be invoked if file may be already 
 * deleted, for example when one file is shared among many attachments.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 */ 
gboolean midgard_blob_remove_file(MidgardBlob *self)
{
	g_assert(self != NULL);

	MidgardConnection *mgd = self->priv->mgd;
	MIDGARD_ERRNO_SET(mgd, MGD_ERR_OK);

	__get_filepath(self);
	if(!self->priv->filepath) {
		MIDGARD_ERRNO_SET(self->priv->mgd, MGD_ERR_INTERNAL);
		g_warning("Can not delete '%s'. No file location",
				self->priv->filepath);
		return FALSE;
	}
	
	if(!midgard_blob_exists(self)) {
		MIDGARD_ERRNO_SET(self->priv->mgd, MGD_ERR_INTERNAL);
		g_warning("Can not delete '%s'. File doesn't exists",
				self->priv->filepath);	
		return FALSE;
	}

	guint deleted = g_remove(self->priv->filepath);

	if(deleted == 0)
		return TRUE;

	g_warning("Can not delete '%s'. Unknown reason", self->priv->filepath);
	MIDGARD_ERRNO_SET(self->priv->mgd, MGD_ERR_INTERNAL);

	return FALSE;
}

/* GOBJECT ROUTINES */

static void _midgard_blob_instance_init(
		 GTypeInstance *instance, gpointer g_class)
{
	MidgardBlob *self = (MidgardBlob *) instance;
	self->priv = g_new(MidgardBlobPrivate, 1);
	
	self->priv->attachment = NULL;
	self->priv->location = NULL;
	self->priv->blobdir = NULL;
	self->priv->filepath = NULL;
	self->priv->channel = NULL;
	self->priv->encoding = NULL;
	self->priv->content = NULL;
	self->priv->parentguid = NULL;
}

static void _midgard_blob_finalize(GObject *object)
{
	g_assert(object);

	MidgardBlob *self = (MidgardBlob *) object;

	g_free(self->priv->blobdir);
	self->priv->blobdir = NULL;

	g_free(self->priv->location);
	self->priv->location = NULL;

	g_free(self->priv->filepath);
	self->priv->filepath = NULL;

	if(self->priv->channel)
		g_io_channel_unref(self->priv->channel);

	g_free((gchar *) self->priv->parentguid);
	self->priv->parentguid = NULL;
	g_free(self->priv->content);
	self->priv->content = NULL;

	g_free(self->priv->encoding);
	self->priv->encoding = NULL;

	g_free(self->priv);
}

static void
_midgard_blob_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardBlob *self = (MidgardBlob *) object;
	
	switch (property_id) {
		
		case MIDGARD_BLOB_PGUID:
			g_value_set_string(value, self->priv->parentguid);
			break;
			
		case MIDGARD_BLOB_CONTENT:
			g_value_set_string(value, self->priv->content);
			break;
			
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
			break;
	}
}

static void
_midgard_blob_set_property (GObject *object, guint property_id,
		                const GValue *value, GParamSpec *pspec)
{
	MidgardBlob *self = (MidgardBlob *) object;
	
	switch (property_id) {
		
		case MIDGARD_BLOB_PGUID:
			g_free((gchar *)self->priv->parentguid);
			self->priv->parentguid = g_value_dup_string(value);
			break;
			
		case MIDGARD_BLOB_CONTENT:
			g_free(self->priv->content);
			self->priv->content = g_value_dup_string(value);
			break;
			
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,pspec);
			break;
	}
}

static void _midgard_blob_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardBlobClass *klass = MIDGARD_BLOB_CLASS (g_class);
	
	gobject_class->finalize = _midgard_blob_finalize;
	gobject_class->set_property = _midgard_blob_set_property;
	gobject_class->get_property = _midgard_blob_get_property;

	klass->read_content = midgard_blob_read_content;
	klass->write_content = midgard_blob_write_content;
	klass->get_path = midgard_blob_get_path;
	klass->exists = midgard_blob_exists;
	klass->get_handler = midgard_blob_get_handler;

	GParamSpec *pspec;
	pspec = g_param_spec_string ("parentguid",
			"",
			"",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_BLOB_PGUID,
			pspec);
	
	pspec = g_param_spec_string ("content",
			"",
			"",
			"",
			G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,
			MIDGARD_BLOB_CONTENT,
			pspec);
}

/* Returns Midgard Blob Gtype value. */
GType midgard_blob_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardBlobClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_blob_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardBlob),
			0,              /* n_preallocs */
			(GInstanceInitFunc) _midgard_blob_instance_init /* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT,
				"midgard_blob",
				&info, 0);
	}
	return type;
}



