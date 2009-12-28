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

#include "midgard_dbobject.h"
#include "schema.h"
#include "midgard_core_object.h"

static GObjectClass *parent_class= NULL;

static GObject *
midgard_dbobject_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties) 
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	MIDGARD_DBOBJECT (object)->dbpriv = g_new(MidgardDBObjectPrivate, 1);
	MIDGARD_DBOBJECT (object)->dbpriv->mgd = NULL; /* read only */
	MIDGARD_DBOBJECT (object)->dbpriv->guid = NULL;
	MIDGARD_DBOBJECT (object)->dbpriv->datamodel = NULL;
	MIDGARD_DBOBJECT (object)->dbpriv->row = -1;	
	MIDGARD_DBOBJECT (object)->dbpriv->has_metadata = TRUE;
	MIDGARD_DBOBJECT (object)->dbpriv->metadata = NULL;

	MIDGARD_DBOBJECT (object)->dbpriv->storage_data =
		MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->storage_data;

	return G_OBJECT(object);
}

static void 
midgard_dbobject_dispose (GObject *object)
{
	parent_class->dispose (object);
}

static void 
midgard_dbobject_finalize (GObject *object)
{
	MidgardDBObject *self = MIDGARD_DBOBJECT(object);

	if (!self)
		return;

	if (!self->dbpriv)
		return;

	g_free((gchar *)self->dbpriv->guid);
	self->dbpriv->guid = NULL;

	if (self->dbpriv->datamodel)
		g_object_unref(self->dbpriv->datamodel);

	self->dbpriv->row = -1;

	/* Do not nullify metadata object, we might be in the middle of refcount decreasing */
	if (self->dbpriv->metadata)
		g_object_unref (self->dbpriv->metadata);

	g_free(self->dbpriv);
	self->dbpriv = NULL;

	parent_class->finalize (object);
}

static const MidgardConnection *__get_connection(MidgardDBObject *self)
{
	g_assert(self != NULL);
	return self->dbpriv->mgd;
}

static void 
midgard_dbobject_class_init (MidgardDBObjectClass *klass, gpointer g_class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);
	
	object_class->constructor = midgard_dbobject_constructor;
	object_class->dispose = midgard_dbobject_dispose;
	object_class->finalize = midgard_dbobject_finalize;

	klass->get_connection = __get_connection;

	klass->dbpriv = g_new(MidgardDBObjectPrivate, 1);
	klass->dbpriv->create_storage = NULL;
	klass->dbpriv->update_storage = NULL;
	klass->dbpriv->storage_exists = NULL;
	klass->dbpriv->delete_storage = NULL;
}

/* Registers the type as a fundamental GType unless already registered. */ 
GType 
midgard_dbobject_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardDBObjectClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) midgard_dbobject_class_init, 
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardDBObject),
			0,              /* n_preallocs */  
			NULL
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardDBObject", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
