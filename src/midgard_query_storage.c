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

MidgardQueryStorage*
midgard_query_storage_new (const gchar *classname)
{
	g_return_val_if_fail (classname != NULL, NULL);

	GType type = g_type_from_name (classname);
	if (!type) {
		g_warning ("Class %s is not registered in GType system", classname);
		return NULL;
	}

	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_CLASS (g_type_class_peek (type));
	if (!klass) {
		g_warning ("Can not find %s class", classname);
		return NULL;
	}

	MidgardQueryStorage *self = g_object_new (MIDGARD_TYPE_QUERY_STORAGE, NULL);
	self->priv->klass = klass;
	self->priv->table = midgard_core_class_get_table (klass);

	return self;
}

/* GOBJECT ROUTINES */

GObjectClass *__parent_class = NULL;

static GObject *
__midgard_query_storage_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (__parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	MIDGARD_QUERY_STORAGE (object)->priv = g_new (MidgardQueryStoragePrivate, 1);
	MIDGARD_QUERY_STORAGE (object)->priv->klass = NULL;
	MIDGARD_QUERY_STORAGE (object)->priv->table_alias = NULL;
	MIDGARD_QUERY_STORAGE (object)->priv->table = NULL;

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
__midgard_query_storage_class_init (
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);

	gobject_class->constructor = __midgard_query_storage_constructor;
	gobject_class->dispose = __midgard_query_storage_dispose;
	gobject_class->finalize = __midgard_query_storage_finalize;
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
			NULL /* instance init */	
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryStorage", &info, 0);
	}
	return type;
}
