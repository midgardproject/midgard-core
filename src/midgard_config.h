/* 
 * Copyright (C) 2006, 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_CONFIG_H
#define MIDGARD_CONFIG_H

#include "midgard_type.h"
#include "midgard_defs.h"

/* convention macros */
#define MIDGARD_TYPE_CONFIG (midgard_config_get_type())
#define MIDGARD_CONFIG(object)  \
	(G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_CONFIG, MidgardConfig))
#define MIDGARD_CONFIG_CLASS(klass)  \
	(G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_CONFIG, MidgardConfigClass))
#define MIDGARD_IS_CONFIG(object)   \
	(G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_CONFIG))
#define MIDGARD_IS_CONFIG_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_CONFIG))
#define MIDGARD_CONFIG_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_CONFIG, MidgardConfigClass))

typedef struct MidgardConfig MidgardConfig;
typedef struct MidgardConfig midgard_config;
typedef struct MidgardConfigClass MidgardConfigClass;
typedef struct _MidgardConfigPrivate MidgardConfigPrivate;

struct MidgardConfig{
	GObject parent;
	
	/* MidgardDatabase */
	gchar *dbtype;
	gchar *host;
	guint port;
	gchar *database;
	gchar *dbuser;
	gchar *dbpass;
	gchar *logfilename;	
	gchar *loglevel;
	gboolean tablecreate;
	gboolean tableupdate;
	FILE  *logfile;
	GIOChannel *log_channel;
	gchar *mgdusername;
	gchar *mgdpassword;
	gboolean testunit;
	guint loghandler;
	guint authtype;
	gchar *pamfile;
	gboolean gdathreads;

	/* MidgardDir */
	gchar *blobdir;
	gchar *sharedir;
	gchar *vardir;
	gchar *cachedir;

	/* < private > */
	gchar *confdir;
	gchar *schemadir;
	gchar *viewsdir;
	MidgardConfigPrivate *priv;
};

struct MidgardConfigClass{
	GObjectClass parent;
	
	/* class members */
	gboolean (*read_config) (MidgardConfig *self, 
				const gchar *filename, gboolean user, GError **error);
	gboolean (*save_config) (MidgardConfig *self,
				const gchar *name, gboolean user, GError **error);
	gchar (**list_files) (gboolean user);
	gboolean (*create_midgard_tables)(MidgardConfig *self,
					MidgardConnection *mgd);
	gboolean (*create_class_table) (MidgardConfig *self,
					MidgardObjectClass *klass, 
					MidgardConnection *mgd);
};

GType midgard_config_get_type(void);

MidgardConfig 	*midgard_config_new			(void);
gboolean 	midgard_config_read_file		(MidgardConfig *self, const gchar *filename, gboolean user, GError **error);
gboolean 	midgard_config_read_file_at_path	(MidgardConfig *self, const gchar *filepath, GError **error);
gboolean 	midgard_config_read_data		(MidgardConfig *self, const gchar *data, GError **error);
gchar 		**midgard_config_list_files		(gboolean user);
gboolean 	midgard_config_save_file		(MidgardConfig *self, const gchar *name, gboolean user, GError **error);
gboolean 	midgard_config_create_midgard_tables	(MidgardConfig *self, MidgardConnection *mgd);
gboolean 	midgard_config_create_class_table	(MidgardConfig *self, MidgardObjectClass *klass, MidgardConnection *mgd);
gboolean 	midgard_config_update_class_table	(MidgardConfig *self, MidgardObjectClass *klass, MidgardConnection *mgd);
gboolean 	midgard_config_class_table_exists	(MidgardConfig *self, MidgardObjectClass *klass, MidgardConnection *mgd);
gboolean 	midgard_config_create_blobdir		(MidgardConfig *self);

/* API helpers, getters */
const gchar *midgard_config_get_database_name(MidgardConfig *self);
const gchar *midgard_config_get_database_type(MidgardConfig *self);
const gchar *midgard_config_get_database_username(MidgardConfig *self);
const gchar *midgard_config_get_database_password(MidgardConfig *self);
const gchar *midgard_config_get_database_host(MidgardConfig *self);
guint midgard_config_get_database_port(MidgardConfig *self);
const gchar *midgard_config_get_blobdir(MidgardConfig *self);
const gchar *midgard_config_get_logfile(MidgardConfig *self);
const gchar *midgard_config_get_loglevel(MidgardConfig *self);
gboolean midgard_config_get_table_create(MidgardConfig *self);
gboolean midgard_config_get_table_update(MidgardConfig *self);
gboolean midgard_config_get_testunit(MidgardConfig *self);
const gchar *midgard_config_get_midgard_username(MidgardConfig *self);
const gchar *midgard_config_get_midgard_password(MidgardConfig *self);
const gchar *midgard_config_get_authtype(MidgardConfig *self);
const gchar *midgard_config_get_pamfile(MidgardConfig *self);
const gchar *midgard_config_get_schema_dir(MidgardConfig *self);

/* setters */
void midgard_config_set_database_name(MidgardConfig *self, const gchar *name);
void midgard_config_set_database_type(MidgardConfig *self, const gchar *type);
void midgard_config_set_database_username(MidgardConfig *self, const gchar *username);
void midgard_config_set_database_password(MidgardConfig *self, const gchar *password);
void midgard_config_set_database_host(MidgardConfig *self, const gchar *host);
void midgard_config_set_database_port(MidgardConfig *self, guint port);
void midgard_config_set_blobdir(MidgardConfig *self, const gchar *blobdir);
void midgard_config_set_logfile(MidgardConfig *self, const gchar *logfile);
void midgard_config_set_loglevel(MidgardConfig *self, const gchar *loglevel);
void midgard_config_set_table_create(MidgardConfig *self, gboolean toggle);
void midgard_config_set_table_update(MidgardConfig *self, gboolean toggle);
void midgard_config_set_testunit(MidgardConfig *self, gboolean toggle);
void midgard_config_set_midgard_username(MidgardConfig *self, const gchar *username);
void midgard_config_set_midgard_password(MidgardConfig *self, const gchar *password);
void midgard_config_set_authtype(MidgardConfig *self, const gchar *authtype);
void midgard_config_set_pamfile(MidgardConfig *self, const gchar *pamfile);
void midgard_config_set_schema_dir(MidgardConfig *self, const gchar *dir);

#endif /* MIDGARD_CONFIG_H */
