/* 
 * Copyright (C) 2007, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_USER_H
#define MIDGARD_USER_H

#include <glib-object.h>
#include "midgard_dbobject.h"
#include "midgard_object.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_USER (midgard_user_get_type())
#define MIDGARD_USER(object)  \
	        (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_USER, MidgardUser))
#define MIDGARD_USER_CLASS(klass)  \
	        (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_USER, MidgardUserClass))
#define MIDGARD_IS_USER(object)   \
	        (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_USER))
#define MIDGARD_IS_USER_CLASS(klass) \
	        (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_TYPE_USER))
#define MIDGARD_USER_GET_CLASS(obj) \
	        (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_TYPE_USER, MidgardUserClass))

typedef struct _MidgardUser midgard_user;
typedef struct _MidgardUserClass MidgardUserClass;
typedef struct _MidgardUserPrivate MidgardUserPrivate; 

struct _MidgardUserClass{
	GObjectClass parent;

	MidgardDBObjectPrivate *dbpriv;
	
	/* methods */
	const MidgardConnection *(*get_connection) (MidgardDBObject *);
	
	/* API methods */
	MgdObject *(*get_person)		(MidgardUser *self);
	gboolean        (*login)              	(MidgardUser *self);
	gboolean        (*logout)             	(MidgardUser *self);
	MidgardUser     *(*get)               	(MidgardConnection *mgd, guint n_params, const GParameter *parameters);
	MidgardUser     **(*query) 		(MidgardConnection *mgd, guint n_params, const GParameter *parameters);
	gboolean        (*create)           	(MidgardUser *self);
	gboolean        (*update)             	(MidgardUser *self);
	gboolean 	(*is_user)		(MidgardUser *self);
	gboolean 	(*is_admin)		(MidgardUser *self);
};

struct _MidgardUser{
	GObject parent;
	MidgardDBObjectPrivate *dbpriv;
	/* < private > */
	MidgardUserPrivate *priv;
	/* MidgardMetadata *metadata; */
};

enum MidgardUserHashType {
	MIDGARD_USER_HASH_LEGACY = 0,
	MIDGARD_USER_HASH_LEGACY_PLAIN,
	MIDGARD_USER_HASH_CRYPT,
	MIDGARD_USER_HASH_MD5,
	MIDGARD_USER_HASH_PLAIN, 
	MIDGARD_USER_HASH_SHA1,
	MIDGARD_USER_HASH_PAM
};

#define MIDGARD_USER_TYPE_NONE 0
#define MIDGARD_USER_TYPE_USER 1
#define MIDGARD_USER_TYPE_ADMIN 2

GType midgard_user_get_type(void);

MidgardUser 		*midgard_user_new		(MidgardConnection *mgd, guint n_params, const GParameter *parameters);
MidgardUser 		*midgard_user_get		(MidgardConnection *mgd, guint n_params, const GParameter *parameters);
MidgardUser 		*midgard_user_quick_login	(MidgardConnection *mgd, const gchar *login, const gchar *password);
MidgardUser 		**midgard_user_query		(MidgardConnection *mgd, guint n_params, const GParameter *parameters);
gboolean		midgard_user_create		(MidgardUser *self);
gboolean 		midgard_user_update		(MidgardUser *self);
gboolean 		midgard_user_is_user		(MidgardUser *self);
gboolean 		midgard_user_is_admin		(MidgardUser *self);
MgdObject		*midgard_user_get_person	(MidgardUser *self);
gboolean 		midgard_user_set_person		(MidgardUser *self, MidgardObject *person);
gboolean		midgard_user_login 		(MidgardUser *self);
gboolean 		midgard_user_logout 		(MidgardUser *self);

/* Deprecated */
MidgardUser 	*midgard_user_auth	(MidgardConnection *mgd, const gchar *name, const gchar *password, const gchar *sitegroup, gboolean trusted);
gboolean 	midgard_user_set_active	(MidgardUser *user, gboolean flag);


G_END_DECLS

#endif /* MIDGARD_USER_H */
