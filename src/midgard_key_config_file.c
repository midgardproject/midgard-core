/* midgard_key_config_file.c generated by valac, the Vala compiler
 * generated from midgard_key_config_file.vala, do not modify */

/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <glib/gstdio.h>


#define MIDGARD_TYPE_KEY_CONFIG (midgard_key_config_get_type ())
#define MIDGARD_KEY_CONFIG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MIDGARD_TYPE_KEY_CONFIG, MidgardKeyConfig))
#define MIDGARD_KEY_CONFIG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_KEY_CONFIG, MidgardKeyConfigClass))
#define MIDGARD_IS_KEY_CONFIG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MIDGARD_TYPE_KEY_CONFIG))
#define MIDGARD_IS_KEY_CONFIG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_KEY_CONFIG))
#define MIDGARD_KEY_CONFIG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_KEY_CONFIG, MidgardKeyConfigClass))

typedef struct _MidgardKeyConfig MidgardKeyConfig;
typedef struct _MidgardKeyConfigClass MidgardKeyConfigClass;
typedef struct _MidgardKeyConfigPrivate MidgardKeyConfigPrivate;

#define MIDGARD_TYPE_KEY_CONFIG_CONTEXT (midgard_key_config_context_get_type ())
#define MIDGARD_KEY_CONFIG_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MIDGARD_TYPE_KEY_CONFIG_CONTEXT, MidgardKeyConfigContext))
#define MIDGARD_KEY_CONFIG_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_KEY_CONFIG_CONTEXT, MidgardKeyConfigContextClass))
#define MIDGARD_IS_KEY_CONFIG_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MIDGARD_TYPE_KEY_CONFIG_CONTEXT))
#define MIDGARD_IS_KEY_CONFIG_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_KEY_CONFIG_CONTEXT))
#define MIDGARD_KEY_CONFIG_CONTEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_KEY_CONFIG_CONTEXT, MidgardKeyConfigContextClass))

typedef struct _MidgardKeyConfigContext MidgardKeyConfigContext;
typedef struct _MidgardKeyConfigContextClass MidgardKeyConfigContextClass;

#define MIDGARD_TYPE_KEY_CONFIG_FILE (midgard_key_config_file_get_type ())
#define MIDGARD_KEY_CONFIG_FILE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MIDGARD_TYPE_KEY_CONFIG_FILE, MidgardKeyConfigFile))
#define MIDGARD_KEY_CONFIG_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_KEY_CONFIG_FILE, MidgardKeyConfigFileClass))
#define MIDGARD_IS_KEY_CONFIG_FILE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MIDGARD_TYPE_KEY_CONFIG_FILE))
#define MIDGARD_IS_KEY_CONFIG_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_KEY_CONFIG_FILE))
#define MIDGARD_KEY_CONFIG_FILE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_KEY_CONFIG_FILE, MidgardKeyConfigFileClass))

typedef struct _MidgardKeyConfigFile MidgardKeyConfigFile;
typedef struct _MidgardKeyConfigFileClass MidgardKeyConfigFileClass;
typedef struct _MidgardKeyConfigFilePrivate MidgardKeyConfigFilePrivate;
#define _g_key_file_free0(var) ((var == NULL) ? NULL : (var = (g_key_file_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))

#define MIDGARD_TYPE_KEY_CONFIG_FILE_CONTEXT (midgard_key_config_file_context_get_type ())
#define MIDGARD_KEY_CONFIG_FILE_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MIDGARD_TYPE_KEY_CONFIG_FILE_CONTEXT, MidgardKeyConfigFileContext))
#define MIDGARD_KEY_CONFIG_FILE_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_KEY_CONFIG_FILE_CONTEXT, MidgardKeyConfigFileContextClass))
#define MIDGARD_IS_KEY_CONFIG_FILE_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MIDGARD_TYPE_KEY_CONFIG_FILE_CONTEXT))
#define MIDGARD_IS_KEY_CONFIG_FILE_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_KEY_CONFIG_FILE_CONTEXT))
#define MIDGARD_KEY_CONFIG_FILE_CONTEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_KEY_CONFIG_FILE_CONTEXT, MidgardKeyConfigFileContextClass))

typedef struct _MidgardKeyConfigFileContext MidgardKeyConfigFileContext;
typedef struct _MidgardKeyConfigFileContextClass MidgardKeyConfigFileContextClass;

struct _MidgardKeyConfig {
	GObject parent_instance;
	MidgardKeyConfigPrivate * priv;
	char* path;
	MidgardKeyConfigContext* context;
};

struct _MidgardKeyConfigClass {
	GObjectClass parent_class;
	void (*set_value) (MidgardKeyConfig* self, const char* group, const char* key, const char* value);
	char* (*get_value) (MidgardKeyConfig* self, const char* group, const char* key);
	void (*set_comment) (MidgardKeyConfig* self, const char* group, const char* key, const char* comment);
	char* (*get_comment) (MidgardKeyConfig* self, const char* group, const char* key);
	char** (*list_groups) (MidgardKeyConfig* self, int* result_length1);
	gboolean (*group_exists) (MidgardKeyConfig* self, const char* name);
	gboolean (*delete_group) (MidgardKeyConfig* self, const char* name);
	gboolean (*store) (MidgardKeyConfig* self);
	gboolean (*load_from_data) (MidgardKeyConfig* self, const char* data);
	char* (*to_data) (MidgardKeyConfig* self);
};

struct _MidgardKeyConfigFile {
	MidgardKeyConfig parent_instance;
	MidgardKeyConfigFilePrivate * priv;
};

struct _MidgardKeyConfigFileClass {
	MidgardKeyConfigClass parent_class;
};

struct _MidgardKeyConfigFilePrivate {
	GKeyFile* key_file;
	char* file_path;
	gboolean file_exists;
};

typedef enum  {
	KEY_CONFIG_CONTEXT_ERROR_PATH_IS_NOT_ABSOLUTE
} KeyConfigContextError;
#define KEY_CONFIG_CONTEXT_ERROR key_config_context_error_quark ()

static gpointer midgard_key_config_file_parent_class = NULL;

#define MIDGARD_CONFIG_EXTENSION ".mcfg"
GType midgard_key_config_get_type (void);
GType midgard_key_config_context_get_type (void);
GType midgard_key_config_file_get_type (void);
#define MIDGARD_KEY_CONFIG_FILE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), MIDGARD_TYPE_KEY_CONFIG_FILE, MidgardKeyConfigFilePrivate))
enum  {
	MIDGARD_KEY_CONFIG_FILE_DUMMY_PROPERTY
};
MidgardKeyConfig* midgard_key_config_construct (GType object_type, MidgardKeyConfigContext* keyctx, const char* path);
const char* midgard_key_config_context_get_context_path (MidgardKeyConfigContext* self);
GQuark key_config_context_error_quark (void);
GType midgard_key_config_file_context_get_type (void);
MidgardKeyConfigFile* midgard_key_config_file_new (MidgardKeyConfigFileContext* keyctx, const char* path, GError** error);
MidgardKeyConfigFile* midgard_key_config_file_construct (GType object_type, MidgardKeyConfigFileContext* keyctx, const char* path, GError** error);
static void midgard_key_config_file_real_set_value (MidgardKeyConfig* base, const char* group, const char* key, const char* value);
static char* midgard_key_config_file_real_get_value (MidgardKeyConfig* base, const char* group, const char* key);
static void midgard_key_config_file_real_set_comment (MidgardKeyConfig* base, const char* group, const char* key, const char* comment);
static char* midgard_key_config_file_real_get_comment (MidgardKeyConfig* base, const char* group, const char* key);
static char** midgard_key_config_file_real_list_groups (MidgardKeyConfig* base, int* result_length1);
static gboolean midgard_key_config_file_real_group_exists (MidgardKeyConfig* base, const char* name);
static gboolean midgard_key_config_file_real_delete_group (MidgardKeyConfig* base, const char* name);
static gboolean midgard_key_config_file_real_store (MidgardKeyConfig* base);
static gboolean midgard_key_config_file_real_load_from_data (MidgardKeyConfig* base, const char* data);
static char* midgard_key_config_file_real_to_data (MidgardKeyConfig* base);
static void midgard_key_config_file_finalize (GObject* obj);



MidgardKeyConfigFile* midgard_key_config_file_construct (GType object_type, MidgardKeyConfigFileContext* keyctx, const char* path, GError** error) {
	GError * _inner_error_;
	MidgardKeyConfigFile * self;
	GKeyFile* _tmp2_;
	g_return_val_if_fail (keyctx != NULL, NULL);
	g_return_val_if_fail (path != NULL, NULL);
	_inner_error_ = NULL;
	self = (MidgardKeyConfigFile*) midgard_key_config_construct (object_type, (MidgardKeyConfigContext*) keyctx, path);
	if (g_str_has_suffix (path, MIDGARD_CONFIG_EXTENSION)) {
		char* _tmp0_;
		self->priv->file_path = (_tmp0_ = g_build_filename (midgard_key_config_context_get_context_path ((MidgardKeyConfigContext*) keyctx), path, NULL), _g_free0 (self->priv->file_path), _tmp0_);
	} else {
		char* _tmp1_;
		self->priv->file_path = (_tmp1_ = g_build_filename (midgard_key_config_context_get_context_path ((MidgardKeyConfigContext*) keyctx), path, MIDGARD_CONFIG_EXTENSION, NULL), _g_free0 (self->priv->file_path), _tmp1_);
	}
	if (!g_path_is_absolute (self->priv->file_path)) {
		_inner_error_ = g_error_new (KEY_CONFIG_CONTEXT_ERROR, KEY_CONFIG_CONTEXT_ERROR_PATH_IS_NOT_ABSOLUTE, "Absolute paths are not accepted while %s is passed.", self->priv->file_path);
		if (_inner_error_ != NULL) {
			if (_inner_error_->domain == KEY_CONFIG_CONTEXT_ERROR) {
				g_propagate_error (error, _inner_error_);
				return NULL;
			} else {
				g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
				g_clear_error (&_inner_error_);
				return NULL;
			}
		}
	}
	self->priv->key_file = (_tmp2_ = g_key_file_new (), _g_key_file_free0 (self->priv->key_file), _tmp2_);
	if (g_file_test (self->priv->file_path, G_FILE_TEST_IS_REGULAR)) {
		self->priv->file_exists = TRUE;
		{
			g_key_file_load_from_file (self->priv->key_file, self->priv->file_path, G_KEY_FILE_KEEP_TRANSLATIONS, &_inner_error_);
			if (_inner_error_ != NULL) {
				goto __catch2_g_error;
				goto __finally2;
			}
		}
		goto __finally2;
		__catch2_g_error:
		{
			GError * e;
			e = _inner_error_;
			_inner_error_ = NULL;
			{
				self->priv->file_exists = FALSE;
				_g_error_free0 (e);
			}
		}
		__finally2:
		if (_inner_error_ != NULL) {
			if (_inner_error_->domain == KEY_CONFIG_CONTEXT_ERROR) {
				g_propagate_error (error, _inner_error_);
				return NULL;
			} else {
				g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
				g_clear_error (&_inner_error_);
				return NULL;
			}
		}
	}
	return self;
}


MidgardKeyConfigFile* midgard_key_config_file_new (MidgardKeyConfigFileContext* keyctx, const char* path, GError** error) {
	return midgard_key_config_file_construct (MIDGARD_TYPE_KEY_CONFIG_FILE, keyctx, path, error);
}


static void midgard_key_config_file_real_set_value (MidgardKeyConfig* base, const char* group, const char* key, const char* value) {
	MidgardKeyConfigFile * self;
	self = (MidgardKeyConfigFile*) base;
	g_return_if_fail (group != NULL);
	g_return_if_fail (key != NULL);
	g_return_if_fail (value != NULL);
	g_key_file_set_string (self->priv->key_file, group, key, value);
}


static char* midgard_key_config_file_real_get_value (MidgardKeyConfig* base, const char* group, const char* key) {
	MidgardKeyConfigFile * self;
	char* result;
	GError * _inner_error_;
	self = (MidgardKeyConfigFile*) base;
	g_return_val_if_fail (group != NULL, NULL);
	g_return_val_if_fail (key != NULL, NULL);
	_inner_error_ = NULL;
	{
		gboolean _tmp0_;
		_tmp0_ = g_key_file_has_key (self->priv->key_file, group, key, &_inner_error_);
		if (_inner_error_ != NULL) {
			if (_inner_error_->domain == G_KEY_FILE_ERROR) {
				goto __catch3_g_key_file_error;
			}
			goto __finally3;
		}
		if (_tmp0_) {
			char* _tmp1_;
			_tmp1_ = g_key_file_get_string (self->priv->key_file, group, key, &_inner_error_);
			if (_inner_error_ != NULL) {
				if (_inner_error_->domain == G_KEY_FILE_ERROR) {
					goto __catch3_g_key_file_error;
				}
				goto __finally3;
			}
			result = _tmp1_;
			return result;
		}
	}
	goto __finally3;
	__catch3_g_key_file_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		{
			result = NULL;
			_g_error_free0 (e);
			return result;
		}
	}
	__finally3:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	result = NULL;
	return result;
}


static void midgard_key_config_file_real_set_comment (MidgardKeyConfig* base, const char* group, const char* key, const char* comment) {
	MidgardKeyConfigFile * self;
	GError * _inner_error_;
	self = (MidgardKeyConfigFile*) base;
	g_return_if_fail (group != NULL);
	g_return_if_fail (key != NULL);
	g_return_if_fail (comment != NULL);
	_inner_error_ = NULL;
	{
		g_key_file_set_comment (self->priv->key_file, group, key, comment, &_inner_error_);
		if (_inner_error_ != NULL) {
			goto __catch4_g_error;
			goto __finally4;
		}
	}
	goto __finally4;
	__catch4_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		{
			g_warning ("midgard_key_config_file.vala:74: Failed to set comment string for key %s, group %s. Error is %s", key, group, e->message);
			_g_error_free0 (e);
		}
	}
	__finally4:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
		g_clear_error (&_inner_error_);
		return;
	}
}


static char* midgard_key_config_file_real_get_comment (MidgardKeyConfig* base, const char* group, const char* key) {
	MidgardKeyConfigFile * self;
	char* result;
	GError * _inner_error_;
	self = (MidgardKeyConfigFile*) base;
	g_return_val_if_fail (group != NULL, NULL);
	g_return_val_if_fail (key != NULL, NULL);
	_inner_error_ = NULL;
	{
		char* _tmp0_;
		_tmp0_ = g_key_file_get_comment (self->priv->key_file, group, key, &_inner_error_);
		if (_inner_error_ != NULL) {
			goto __catch5_g_error;
			goto __finally5;
		}
		result = _tmp0_;
		return result;
	}
	goto __finally5;
	__catch5_g_error:
	{
		/*Error object is not used within catch statement, clear it*/
		g_clear_error (&_inner_error_);
		_inner_error_ = NULL;
		{
			result = NULL;
			return result;
		}
	}
	__finally5:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
		g_clear_error (&_inner_error_);
		return NULL;
	}
}


static char** midgard_key_config_file_real_list_groups (MidgardKeyConfig* base, int* result_length1) {
	MidgardKeyConfigFile * self;
	char** result;
	gsize _tmp0_;
	char** _tmp1_;
	self = (MidgardKeyConfigFile*) base;
	result = (_tmp1_ = g_key_file_get_groups (self->priv->key_file, &_tmp0_), *result_length1 = _tmp0_, _tmp1_);
	return result;
}


static gboolean midgard_key_config_file_real_group_exists (MidgardKeyConfig* base, const char* name) {
	MidgardKeyConfigFile * self;
	gboolean result;
	self = (MidgardKeyConfigFile*) base;
	g_return_val_if_fail (name != NULL, FALSE);
	result = g_key_file_has_group (self->priv->key_file, name);
	return result;
}


static gboolean midgard_key_config_file_real_delete_group (MidgardKeyConfig* base, const char* name) {
	MidgardKeyConfigFile * self;
	gboolean result;
	GError * _inner_error_;
	self = (MidgardKeyConfigFile*) base;
	g_return_val_if_fail (name != NULL, FALSE);
	_inner_error_ = NULL;
	{
		g_key_file_remove_group (self->priv->key_file, name, &_inner_error_);
		if (_inner_error_ != NULL) {
			goto __catch6_g_error;
			goto __finally6;
		}
		result = TRUE;
		return result;
	}
	goto __finally6;
	__catch6_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		{
			g_warning ("midgard_key_config_file.vala:99: Failed to delete group %s. Error is %s", name, e->message);
			_g_error_free0 (e);
		}
	}
	__finally6:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
		g_clear_error (&_inner_error_);
		return FALSE;
	}
	result = FALSE;
	return result;
}


static gboolean midgard_key_config_file_real_store (MidgardKeyConfig* base) {
	MidgardKeyConfigFile * self;
	gboolean result;
	GError * _inner_error_;
	self = (MidgardKeyConfigFile*) base;
	_inner_error_ = NULL;
	if (!self->priv->file_exists) {
		char* basedir;
		basedir = g_path_get_dirname (self->priv->file_path);
		if (!g_file_test (basedir, G_FILE_TEST_IS_DIR)) {
			gint created;
			created = g_mkdir_with_parents (basedir, 0750);
			if (created == (-1)) {
				result = FALSE;
				_g_free0 (basedir);
				return result;
			}
		}
		_g_free0 (basedir);
	}
	{
		char* _tmp0_;
		gboolean _tmp1_;
		gboolean _tmp2_;
		_tmp2_ = (_tmp1_ = g_file_set_contents (self->priv->file_path, _tmp0_ = g_key_file_to_data (self->priv->key_file, NULL, NULL), -1, &_inner_error_), _g_free0 (_tmp0_), _tmp1_);
		if (_inner_error_ != NULL) {
			goto __catch7_g_error;
			goto __finally7;
		}
		result = _tmp2_;
		return result;
	}
	goto __finally7;
	__catch7_g_error:
	{
		/*Error object is not used within catch statement, clear it*/
		g_clear_error (&_inner_error_);
		_inner_error_ = NULL;
		{
			result = FALSE;
			return result;
		}
	}
	__finally7:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
		g_clear_error (&_inner_error_);
		return FALSE;
	}
}


static glong string_get_length (const char* self) {
	glong result;
	g_return_val_if_fail (self != NULL, 0L);
	result = g_utf8_strlen (self, -1);
	return result;
}


static gboolean midgard_key_config_file_real_load_from_data (MidgardKeyConfig* base, const char* data) {
	MidgardKeyConfigFile * self;
	gboolean result;
	GError * _inner_error_;
	self = (MidgardKeyConfigFile*) base;
	g_return_val_if_fail (data != NULL, FALSE);
	_inner_error_ = NULL;
	if (self->priv->file_exists) {
		result = FALSE;
		return result;
	}
	{
		gboolean _tmp0_;
		_tmp0_ = g_key_file_load_from_data (self->priv->key_file, data, (gulong) string_get_length (data), G_KEY_FILE_KEEP_TRANSLATIONS, &_inner_error_);
		if (_inner_error_ != NULL) {
			goto __catch8_g_error;
			goto __finally8;
		}
		result = _tmp0_;
		return result;
	}
	goto __finally8;
	__catch8_g_error:
	{
		/*Error object is not used within catch statement, clear it*/
		g_clear_error (&_inner_error_);
		_inner_error_ = NULL;
		{
			result = FALSE;
			return result;
		}
	}
	__finally8:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s", __FILE__, __LINE__, _inner_error_->message);
		g_clear_error (&_inner_error_);
		return FALSE;
	}
}


static char* midgard_key_config_file_real_to_data (MidgardKeyConfig* base) {
	MidgardKeyConfigFile * self;
	char* result;
	self = (MidgardKeyConfigFile*) base;
	if (!self->priv->file_exists) {
		result = NULL;
		return result;
	}
	result = g_key_file_to_data (self->priv->key_file, NULL, NULL);
	return result;
}


static void midgard_key_config_file_class_init (MidgardKeyConfigFileClass * klass) {
	midgard_key_config_file_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (MidgardKeyConfigFilePrivate));
	MIDGARD_KEY_CONFIG_CLASS (klass)->set_value = midgard_key_config_file_real_set_value;
	MIDGARD_KEY_CONFIG_CLASS (klass)->get_value = midgard_key_config_file_real_get_value;
	MIDGARD_KEY_CONFIG_CLASS (klass)->set_comment = midgard_key_config_file_real_set_comment;
	MIDGARD_KEY_CONFIG_CLASS (klass)->get_comment = midgard_key_config_file_real_get_comment;
	MIDGARD_KEY_CONFIG_CLASS (klass)->list_groups = midgard_key_config_file_real_list_groups;
	MIDGARD_KEY_CONFIG_CLASS (klass)->group_exists = midgard_key_config_file_real_group_exists;
	MIDGARD_KEY_CONFIG_CLASS (klass)->delete_group = midgard_key_config_file_real_delete_group;
	MIDGARD_KEY_CONFIG_CLASS (klass)->store = midgard_key_config_file_real_store;
	MIDGARD_KEY_CONFIG_CLASS (klass)->load_from_data = midgard_key_config_file_real_load_from_data;
	MIDGARD_KEY_CONFIG_CLASS (klass)->to_data = midgard_key_config_file_real_to_data;
	G_OBJECT_CLASS (klass)->finalize = midgard_key_config_file_finalize;
}


static void midgard_key_config_file_instance_init (MidgardKeyConfigFile * self) {
	self->priv = MIDGARD_KEY_CONFIG_FILE_GET_PRIVATE (self);
	self->priv->key_file = NULL;
	self->priv->file_path = NULL;
	self->priv->file_exists = FALSE;
}


static void midgard_key_config_file_finalize (GObject* obj) {
	MidgardKeyConfigFile * self;
	self = MIDGARD_KEY_CONFIG_FILE (obj);
	_g_key_file_free0 (self->priv->key_file);
	_g_free0 (self->priv->file_path);
	G_OBJECT_CLASS (midgard_key_config_file_parent_class)->finalize (obj);
}


GType midgard_key_config_file_get_type (void) {
	static volatile gsize midgard_key_config_file_type_id__volatile = 0;
	if (g_once_init_enter (&midgard_key_config_file_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (MidgardKeyConfigFileClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) midgard_key_config_file_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (MidgardKeyConfigFile), 0, (GInstanceInitFunc) midgard_key_config_file_instance_init, NULL };
		GType midgard_key_config_file_type_id;
		midgard_key_config_file_type_id = g_type_register_static (MIDGARD_TYPE_KEY_CONFIG, "MidgardKeyConfigFile", &g_define_type_info, 0);
		g_once_init_leave (&midgard_key_config_file_type_id__volatile, midgard_key_config_file_type_id);
	}
	return midgard_key_config_file_type_id__volatile;
}




