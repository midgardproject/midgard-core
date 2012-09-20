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

#include "midgard_execution_pool.h"
#include "midgard_executable.h"
#include "midgard_validable.h"
#include "midgard_core_object.h"

struct _MidgardExecutionPoolPrivate {
	GThreadPool *gpool;
};

/**
 * midgard_execution_pool_new:
 * @max_n_threads: maximum number of threads
 * 
 * Returns: new #MidgardExecutionPool
 *
 * Since: 12.09
 */ 
MidgardExecutionPool *             
midgard_execution_pool_new (guint max_n_threads)
{
	MidgardExecutionPool *self = g_object_new (MIDGARD_TYPE_EXECUTION_POOL, "max_n_threads", max_n_threads, NULL);
	return self;
}

guint
_midgard_execution_pool_get_max_n_resources (MidgardPool *iface)
{
	MidgardExecutionPool *self = MIDGARD_EXECUTION_POOL(iface);
	return (guint) g_thread_pool_get_max_threads (self->priv->gpool);
}

guint
_midgard_execution_pool_get_n_resources (MidgardPool *iface)
{
	MidgardExecutionPool *self = MIDGARD_EXECUTION_POOL(iface);
	return (guint) g_thread_pool_get_num_threads (self->priv->gpool);
}

MidgardPool*
_midgard_execution_pool_set_max_n_resources (MidgardPool *iface, guint n_max, GError **error)
{
	MidgardExecutionPool *self = MIDGARD_EXECUTION_POOL(iface);
	GError *err = NULL;
#if GLIB_CHECK_VERSION(2,32,0)
	gboolean is_set = g_thread_pool_set_max_threads (self->priv->gpool, (gint)n_max, &err);
#else 
	g_thread_pool_set_max_threads (self->priv->gpool, (gint)n_max, &err);
#endif
	if (err)
		g_propagate_error (error, err);
	return iface;
}

MidgardPool*
_midgard_execution_pool_push (MidgardPool *iface, GObject *object, GError **error)
{
	g_return_if_fail (MIDGARD_IS_EXECUTION_POOL (iface));

	MidgardExecutionPool *self = MIDGARD_EXECUTION_POOL(iface);

	if (!MIDGARD_IS_EXECUTABLE (object)) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_TYPE_INVALID,
				"Invalid type. Expected Executable, got '%s'", G_OBJECT_TYPE_NAME (object));
		return iface;
	}

	GError *err = NULL;
	/* Add new reference to executor, pool will unref it after real execution is done */
	g_thread_pool_push (self->priv->gpool, g_object_ref(object), &err);
	if (err) {
		g_propagate_error (error, err);
	}
	
	return iface;
}

static gboolean
execution_start_func (gpointer data)
{
	MidgardExecutable *executable = (MidgardExecutable *) data;
	g_signal_emit_by_name (executable, "execution-start", 0);
	return FALSE;
}

static gboolean
execution_end_func (gpointer data)
{
	MidgardExecutable *executable = (MidgardExecutable *) data;
	g_signal_emit_by_name (executable, "execution-end", 0);
	g_object_unref (executable);
	return FALSE;
}

static void
thread_pool_execute (gpointer data, gpointer user_data)
{
	GMainContext *context = (GMainContext*) user_data;
	MidgardExecutable *executable = (MidgardExecutable *) data;
	GError *error = NULL;
	/* Perform real operation and wait till it's finished */
	midgard_executable_execute_async (executable, &error);
	g_idle_add_full (G_PRIORITY_HIGH_IDLE, (GSourceFunc) execution_end_func, g_object_ref (executable), NULL);
	/* New reference has been added when object has been added to pool, unref it */
	g_object_unref (executable);
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
_midgard_execution_pool_pool_iface_init (MidgardPoolIFace *iface)
{
	iface->get_max_n_resources = _midgard_execution_pool_get_max_n_resources;
	iface->get_n_resources =_midgard_execution_pool_get_n_resources;
	iface->set_max_n_resources = _midgard_execution_pool_set_max_n_resources;
	iface->push = _midgard_execution_pool_push;
	return;
}

static void 
_midgard_execution_pool_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardExecutionPool *self = (MidgardExecutionPool*) instance;
	self->priv = g_new(MidgardExecutionPoolPrivate, 1);
	self->priv->gpool = g_thread_pool_new (thread_pool_execute, g_main_context_default(), 1, TRUE, NULL);
	return;
}

static GObject *
_midgard_execution_pool_constructor (GType type,
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
_midgard_execution_pool_dispose (GObject *object)
{
	__parent_class->dispose (object);
}

static void 
_midgard_execution_pool_finalize (GObject *object)
{
	MidgardExecutionPool *self = MIDGARD_EXECUTION_POOL (object);

	while (g_main_context_pending (NULL) == TRUE) {
		g_main_context_iteration (NULL, FALSE);
	}

	g_thread_pool_free (self->priv->gpool, FALSE, TRUE);

	g_free(self->priv);
	self->priv = NULL;

	__parent_class->finalize (object);
}

static void
_midgard_execution_pool_get_property (GObject *object, guint property_id,
		GValue *value, GParamSpec *pspec)
{
	MidgardExecutionPool *self = (MidgardExecutionPool *) object;

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
_midgard_execution_pool_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardExecutionPool *self = (MidgardExecutionPool *) (object);
	
	switch (property_id) {
	
		case PROPERTY_N_THREADS:
			g_thread_pool_set_max_threads (self->priv->gpool, (gint)g_value_get_uint(value), NULL);	
			break;	
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void _midgard_execution_pool_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardExecutionPoolClass *klass = MIDGARD_EXECUTION_POOL_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_execution_pool_constructor;
	gobject_class->dispose = _midgard_execution_pool_dispose;
	gobject_class->finalize = _midgard_execution_pool_finalize;

	gobject_class->set_property = _midgard_execution_pool_set_property;
	gobject_class->get_property = _midgard_execution_pool_get_property;
	
	/* Properties */
	GParamSpec *pspec = g_param_spec_uint ("max_n_threads",
			"",
			"",
			0, G_MAXUINT32, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardExecutionPool:max_n_threads:
	 */
	g_object_class_install_property (gobject_class, PROPERTY_N_THREADS, pspec);
}

GType 
midgard_execution_pool_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardExecutionPoolClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_execution_pool_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardExecutionPool),
			0,              /* n_preallocs */
			_midgard_execution_pool_instance_init /* instance_init */
		};

		static const GInterfaceInfo pool_iface_info = {
			(GInterfaceInitFunc) _midgard_execution_pool_pool_iface_init,
			NULL,	/* interface finalize */	
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardExecutionPool", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_POOL, &pool_iface_info);
	}
	return type;
}

