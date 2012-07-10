/* 
 * Copyright (C) 2012 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_sql_async_content_manager.h"
#include "../midgard_content_manager.h"

struct _MidgardSqlAsyncContentManagerPrivate {
	MidgardConnection *mgd;
};

/**
 * midgard_sql_async_content_manager_new:
 * @mgd: #MidgardConnection instance
 * 
 * Returns: new #MidgardSqlAsyncContentManager
 *
 * Since: 10.05.8
 */ 
MidgardSqlAsyncContentManager *             
midgard_sql_async_content_manager_new (MidgardConnection *mgd)
{
	g_return_val_if_fail (mgd != NULL, NULL);

	MidgardSqlAsyncContentManager *self = g_object_new (MIDGARD_TYPE_SQL_ASYNC_CONTENT_MANAGER, "connection", mgd, NULL);
	return self;
}

GObject* 
_midgard_sql_async_content_manager_get_by_id (MidgardContentManager *self, 
		MidgardObjectReference *reference, MidgardModel *model, GError **error) {

}

static void
_midgard_sql_async_content_manager_load (MidgardContentManager *self, 
		GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error) {

}

static void
_midgard_sql_async_content_manager_exists (MidgardContentManager *self, 
		GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error) {

}

static void
_midgard_sql_async_content_manager_create (MidgardContentManager *self, 
		GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error) {

}

static void
_midgard_sql_async_content_manager_update (MidgardContentManager *self, 
		GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error) {

}

static void
_midgard_sql_async_content_manager_save (MidgardContentManager *self, 
		GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error) {

}

static void
_midgard_sql_async_content_manager_remove (MidgardContentManager *self, 
		GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error) {

}

static void
_midgard_sql_async_content_manager_purge (MidgardContentManager *self, 
		GObject *object, MidgardObjectReference *reference, MidgardModel *model, GError **error) {

}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_CONNECTION = 1,
};


static void
_midgard_content_manager_iface_init (MidgardContentManagerIFace *iface)
{
	iface->get_by_id = _midgard_sql_async_content_manager_get_by_id;
	iface->load = _midgard_sql_async_content_manager_load;
	iface->exists = _midgard_sql_async_content_manager_exists;
	iface->create = _midgard_sql_async_content_manager_create;
	iface->update = _midgard_sql_async_content_manager_update;
	iface->save = _midgard_sql_async_content_manager_save;
	iface->remove = _midgard_sql_async_content_manager_remove;
	iface->purge = _midgard_sql_async_content_manager_purge;

	return;
}

static void 
_midgard_sql_async_content_manager_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardSqlAsyncContentManager *self = (MidgardSqlAsyncContentManager*) instance;
	self->priv = g_new(MidgardSqlAsyncContentManagerPrivate, 1);
	self->priv->mgd = NULL;
	return;
}

static GObject *
_midgard_sql_async_content_manager_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (__parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);
	return G_OBJECT(object);
}

static void
_midgard_sql_async_content_manager_dispose (GObject *object)
{
	MidgardSqlAsyncContentManager *self = MIDGARD_SQL_ASYNC_CONTENT_MANAGER (object);
	if (self->priv->mgd) 
		g_object_unref (self->priv->mgd);
	self->priv->mgd = NULL;

	__parent_class->dispose (object);
}

static void 
_midgard_sql_async_content_manager_finalize (GObject *object)
{
	__parent_class->finalize (object);
}

static void
_midgard_sql_async_content_manager_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardSqlAsyncContentManager *self = (MidgardSqlAsyncContentManager *) object;

	switch (property_id) {
		
		case PROPERTY_CONNECTION:
			g_value_set_object (value, self->priv->mgd);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
_midgard_sql_async_content_manager_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardSqlAsyncContentManager *self = (MidgardSqlAsyncContentManager *) (object);
	
	switch (property_id) {
	
		case PROPERTY_CONNECTION:
			self->priv->mgd = g_value_dup_object (value);
			break;
		
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void _midgard_sql_async_content_manager_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardSqlAsyncContentManagerClass *klass = MIDGARD_SQL_ASYNC_CONTENT_MANAGER_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_sql_async_content_manager_constructor;
	gobject_class->dispose = _midgard_sql_async_content_manager_dispose;
	gobject_class->finalize = _midgard_sql_async_content_manager_finalize;

	gobject_class->set_property = _midgard_sql_async_content_manager_set_property;
	gobject_class->get_property = _midgard_sql_async_content_manager_get_property;
	
	/* Properties */
	GParamSpec *pspec = g_param_spec_object ("connection",
			"",
			"",
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardSqlAsyncContentManager:connection:
	 */
	g_object_class_install_property (gobject_class, PROPERTY_CONNECTION, pspec);

}

GType 
midgard_sql_async_content_manager_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardSqlAsyncContentManagerClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_async_content_manager_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlAsyncContentManager),
			0,              /* n_preallocs */
			_midgard_sql_async_content_manager_instance_init /* instance_init */
		};

		static const GInterfaceInfo content_manager_iface_info = {
			(GInterfaceInitFunc) _midgard_content_manager_iface_init,
			NULL,	/* interface finalize */	
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSqlAsyncContentManager", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_CONTENT_MANAGER, &content_manager_iface_info);
	}
	return type;
}

