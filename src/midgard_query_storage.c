/* 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_query_storage.h"
#include "midgard_core_query.h"
#include "midgard_core_object_class.h"

/**
 * midgard_query_storage_new:
 * @classname: name of the #MidgardDBObject derived class 
 *
 * Initializes new object which represents #MidgardDBObject derived one's storage
 *
 * Returns: new #MidgardQueryStorage or %NULL on failure
 * Since: 10.05
 */ 
MidgardQueryStorage*
midgard_query_storage_new (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, NULL);

	GType type = g_type_from_name (classname);
	if (!type) {
		g_warning ("Class %s is not registered in GType system", classname);
		return NULL;
	}

	GObjectClass *klass = G_OBJECT_CLASS (g_type_class_peek (type));
	if (!klass) {
		g_warning ("Can not find %s class", classname);
		return NULL;
	}

	g_return_val_if_fail (MIDGARD_IS_DBOBJECT_CLASS (klass), NULL);

	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_CLASS (klass);

	MidgardQueryStorage *self = g_object_new (MIDGARD_TYPE_QUERY_STORAGE, "dbclass", classname, NULL);
	self->priv->klass = dbklass;
	self->priv->table = midgard_core_class_get_table (dbklass);

	return self;
}

/* GOBJECT ROUTINES */

enum {
	MIDGARD_QUERY_STORAGE_DBCLASS = 1
};


GObjectClass *__parent_class = NULL;

static void
__midgard_query_storage_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardQueryStorage *self = (MidgardQueryStorage *) instance;

	self->priv = g_new (MidgardQueryStoragePrivate, 1);
	self->priv->klass = NULL;
	self->priv->table_alias = NULL;
	self->priv->table = NULL;
	self->priv->classname = NULL;
}

static GObject *
__midgard_query_storage_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (__parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return object;
}

static void
__midgard_query_storage_dispose (GObject *object)
{
	__parent_class->dispose (object);
}

static void 
__midgard_query_storage_finalize (GObject *object)
{
	MidgardQueryStorage *self = MIDGARD_QUERY_STORAGE (object);
	
	g_free (self->priv->table_alias);
	self->priv->table_alias = NULL;

	g_free (self->priv);
	self->priv = NULL;

	__parent_class->finalize (object);
}

static void
__midgard_query_storage_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardQueryStorage *self = (MidgardQueryStorage *) object;

	switch (property_id) {
		
		case MIDGARD_QUERY_STORAGE_DBCLASS:
			g_value_set_string (value, self->priv->classname);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
__midgard_query_storage_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardQueryStorage *self = (MidgardQueryStorage *) (object);
	MidgardDBObjectClass *dbklass = NULL;

	switch (property_id) {

		case MIDGARD_QUERY_STORAGE_DBCLASS:
			dbklass = g_type_class_peek (g_type_from_name (g_value_get_string (value)));
			g_return_if_fail (MIDGARD_IS_DBOBJECT_CLASS (dbklass));
			if (dbklass) {
				self->priv->klass = dbklass;
				self->priv->classname = G_OBJECT_CLASS_NAME (G_OBJECT_CLASS (dbklass));
				self->priv->table = midgard_core_class_get_table (dbklass);
			}
			break;

  		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void 
__midgard_query_storage_class_init (
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);

	gobject_class->constructor = __midgard_query_storage_constructor;
	gobject_class->dispose = __midgard_query_storage_dispose;
	gobject_class->finalize = __midgard_query_storage_finalize;

	gobject_class->set_property = __midgard_query_storage_set_property;
	gobject_class->get_property = __midgard_query_storage_get_property;

	/* Properties */
	GParamSpec *pspec = g_param_spec_string ("dbclass",
			"MidgardDBObject derived class name.",
			"",
			"",
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	/**
	 * MidgardQueryStorage:dbclass:
	 * 
	 * Holds the name of the class which, #MidgardQueryStorage has been initialized for.
	 * 
	 */  
	g_object_class_install_property (gobject_class, MIDGARD_QUERY_STORAGE_DBCLASS, pspec);
}

GType
midgard_query_storage_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardQueryStorageClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) __midgard_query_storage_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardQueryStorage),
			0,              /* n_preallocs */
			__midgard_query_storage_instance_init /* instance init */	
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryStorage", &info, 0);
	}
	return type;
}
