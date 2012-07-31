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

#include "midgard_thread_pool.h"
#include "midgard_executable.h"
#include "midgard_validable.h"
#include "midgard_core_object.h"

/**
 * midgard_thread_pool_new:
 * @max_n_threads: maximum number of threads
 * 
 * Returns: new #MidgardThreadPool
 *
 * Since: 10.05.8
 */ 
MidgardThreadPool *             
midgard_thread_pool_new (guint max_n_threads)
{
	MidgardThreadPool *self = g_object_new (MIDGARD_TYPE_THREAD_POOL, "max_n_threads", max_n_threads, NULL);
	return self;
}

guint
_midgard_thread_pool_get_max_n_resources (MidgardPool *iface)
{
	MidgardThreadPool *self = MIDGARD_THREAD_POOL(iface);
	return (guint) g_thread_pool_get_max_threads (self->priv->gpool);
}

guint
_midgard_thread_pool_get_n_resources (MidgardPool *iface)
{
	MidgardThreadPool *self = MIDGARD_THREAD_POOL(iface);
	return (guint) g_thread_pool_get_num_threads (self->priv->gpool);
}

MidgardPool*
_midgard_thread_pool_set_max_n_resources (MidgardPool *iface, guint n_max, GError **error)
{
	MidgardThreadPool *self = MIDGARD_THREAD_POOL(iface);
	GError *err = NULL;
	gboolean is_set = g_thread_pool_set_max_threads (self->priv->gpool, (gint)n_max, &err);
	if (err)
		g_propagate_error (error, err);
	return iface;
}

static void
thread_pool_execute (gpointer data, gpointer user_data)
{
	MidgardExecutable *executable = (MidgardExecutable *) data;
	GError *error = NULL;
	/* Perform real operation and wait till it's finished */
	midgard_executable_execute (executable, &error);
	/* Operation is done, emit signal */
	midgard_executable_execution_end (executable);
	if (error) {
		/* TODO, handle error */
	}
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_N_THREADS = 1
};


static void
_midgard_thread_pool_pool_iface_init (MidgardPoolIFace *iface)
{
	iface->get_max_n_resources = _midgard_thread_pool_get_max_n_resources;
	iface->get_n_resources =_midgard_thread_pool_get_n_resources;
	iface->set_max_n_resources = _midgard_thread_pool_set_max_n_resources;
	return;
}

static void 
_midgard_thread_pool_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardThreadPool *self = (MidgardThreadPool*) instance;
	self->priv = g_new(MidgardThreadPoolPrivate, 1);
	self->priv->gpool = g_thread_pool_new (thread_pool_execute, NULL, 1, TRUE, NULL);
	return;
}

static GObject *
_midgard_thread_pool_constructor (GType type,
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
_midgard_thread_pool_dispose (GObject *object)
{
	__parent_class->dispose (object);
}

static void 
_midgard_thread_pool_finalize (GObject *object)
{
	MidgardThreadPool *self = MIDGARD_THREAD_POOL (object);

	g_thread_pool_free (self->priv->gpool, FALSE, TRUE);

	g_free(self->priv);
	self->priv = NULL;

	__parent_class->finalize (object);
}

static void
_midgard_thread_pool_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardThreadPool *self = (MidgardThreadPool *) object;

	switch (property_id) {
		
		case PROPERTY_N_THREADS:
			g_value_set_uint (value, g_thread_pool_get_max_threads (self->priv->gpool));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void
_midgard_thread_pool_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardThreadPool *self = (MidgardThreadPool *) (object);
	
	switch (property_id) {
	
		case PROPERTY_N_THREADS:
			g_thread_pool_set_max_threads (self->priv->gpool, (gint)g_value_get_uint(value), NULL);	
			break;	
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void _midgard_thread_pool_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardThreadPoolClass *klass = MIDGARD_THREAD_POOL_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_thread_pool_constructor;
	gobject_class->dispose = _midgard_thread_pool_dispose;
	gobject_class->finalize = _midgard_thread_pool_finalize;

	gobject_class->set_property = _midgard_thread_pool_set_property;
	gobject_class->get_property = _midgard_thread_pool_get_property;
	
	/* Properties */
	GParamSpec *pspec = g_param_spec_uint ("max_n_threads",
			"",
			"",
			0, G_MAXUINT32, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardThreadPool:max_n_threads:
	 */
	g_object_class_install_property (gobject_class, PROPERTY_N_THREADS, pspec);
}

GType 
midgard_thread_pool_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardThreadPoolClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_thread_pool_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardThreadPool),
			0,              /* n_preallocs */
			_midgard_thread_pool_instance_init /* instance_init */
		};

		static const GInterfaceInfo pool_iface_info = {
			(GInterfaceInitFunc) _midgard_thread_pool_pool_iface_init,
			NULL,	/* interface finalize */	
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardThreadPool", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_POOL, &pool_iface_info);
	}
	return type;
}

