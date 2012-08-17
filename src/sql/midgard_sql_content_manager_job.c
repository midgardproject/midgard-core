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

#include "midgard_sql_content_manager_job.h"
#include "midgard_core_query.h"
#include "midgard_validable.h"
#include "midgard_executable.h"
#include "midgard_job.h"
#include "midgard_content_manager_job.h"

struct _MidgardSqlContentManagerJobPrivate {
	GObject *content_object;
	MidgardObjectReference *reference;
	MidgardModel *model;
	MidgardConnection *connection;
	gboolean is_running;
	gboolean is_executed;
	gboolean is_failed;
	gboolean is_valid;
};

/* MidgardSqlContentManagerJob properties */
enum {
	PROPERTY_CONTENT_OBJECT = 1,
	PROPERTY_REFERENCE,
	PROPERTY_MODEL,
	PROPERTY_CONNECTION
};

/**
 * midgard_sql_content_manager_job_get_connection:
 * @self: #MidgardSqlContentManagerJob instance
 * @error: pointer to store returned error
 *
 * Get job's connection. 
 *
 * Returns: (transfer full): #MidgardConnection instance or %NULL on failure
 *
 * Since: 10.05.08
 */ 
MidgardConnection*
midgard_sql_content_manager_job_get_connection (MidgardSqlContentManagerJob *self, GError **error)
{
	g_return_val_if_fail (self != NULL, NULL);
	if (self->priv->connection == NULL)
		return self->priv->connection;
	return g_object_ref (self->priv->connection);
}


static gboolean
_midgard_sql_content_manager_job_job_iface_is_running (MidgardJob *iface)
{
	g_return_val_if_fail (iface != NULL, FALSE);
	MidgardSqlContentManagerJob *job = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	return job->priv->is_running;
}

static gboolean
_midgard_sql_content_manager_job_job_iface_is_executed (MidgardJob *iface)
{
	g_return_val_if_fail (iface != NULL, FALSE);
	MidgardSqlContentManagerJob *job = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	return job->priv->is_executed;
}

static gboolean
_midgard_sql_content_manager_job_job_iface_is_failed (MidgardJob *iface)
{
	g_return_val_if_fail (iface != NULL, FALSE);
	MidgardSqlContentManagerJob *job = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	return job->priv->is_failed;
}

static GObject*
 _midgard_sql_content_manager_job_content_manager_job_iface_get_content_object (MidgardContentManagerJob *iface, GError **error)
{
	g_return_val_if_fail (iface != NULL, NULL);
	MidgardSqlContentManagerJob *job = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	if (job->priv->content_object == NULL) {
		/* TODO, set error*/
	}
	return (GObject *) g_object_ref (job->priv->content_object);
}

static MidgardObjectReference*
 _midgard_sql_content_manager_job_content_manager_job_iface_get_reference (MidgardContentManagerJob *iface, GError **error)
{
	g_return_val_if_fail (iface != NULL, NULL);
	MidgardSqlContentManagerJob *job = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	if (job->priv->content_object == NULL) {
		/* TODO, set error*/
	}
	return (MidgardObjectReference *) g_object_ref (job->priv->reference);
}

static MidgardModel*
 _midgard_sql_content_manager_job_content_manager_job_iface_get_model (MidgardContentManagerJob *iface, GError **error)
{
	g_return_val_if_fail (iface != NULL, NULL);
	MidgardSqlContentManagerJob *job = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	if (job->priv->model == NULL) {
		/* TODO, set error if required */
		return NULL;
	}
	return (MidgardModel *) g_object_ref (job->priv->model);
}

static void 
_midgard_sql_content_manager_job_validable_iface_validate (MidgardValidable *iface, GError **error)
{
	g_return_if_fail (iface != NULL);
	MidgardSqlContentManagerJob *job = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	/* TODO, validate */
}

static gboolean
_midgard_sql_content_manager_job_validable_iface_is_valid (MidgardValidable *iface)
{
	g_return_if_fail (iface != NULL);
	MidgardSqlContentManagerJob *job = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	return job->priv->is_valid;
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class= NULL;

static void
_midgard_sql_content_manager_job_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardSqlContentManagerJob *object = (MidgardSqlContentManagerJob *) instance;

	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv = g_new (MidgardSqlContentManagerJobPrivate, 1);
	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv->connection = NULL;
	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv->content_object = NULL;
	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv->reference = NULL;
	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv->model = NULL;
	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv->is_running = FALSE;
	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv->is_executed = FALSE;
	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv->is_failed = FALSE;
	MIDGARD_SQL_CONTENT_MANAGER_JOB (object)->priv->is_valid = FALSE;
}

static GObject *
_midgard_sql_content_manager_job_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return G_OBJECT(object);
}

static void
_midgard_sql_content_manager_job_dispose (GObject *object)
{
	MidgardSqlContentManagerJob *self = (MidgardSqlContentManagerJob *) object;

	if (MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->connection) {
		g_object_unref (MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->connection);
		MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->connection = NULL;
	}
	
	if (MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->content_object) {
		g_object_unref (MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->content_object);
		MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->content_object = NULL;
	}

	if (MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->reference) {
		g_object_unref (MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->reference);
		MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->reference = NULL;
	}

	if (MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->model) {
		g_object_unref (MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->model);
		MIDGARD_SQL_CONTENT_MANAGER_JOB (self)->priv->model = NULL;
	}

	parent_class->dispose (object);
}

static void 
_midgard_sql_content_manager_job_finalize (GObject *object)
{
	MidgardSqlContentManagerJob *self = MIDGARD_SQL_CONTENT_MANAGER_JOB (object);

	self->priv->is_running = FALSE;
	self->priv->is_executed = FALSE;
	self->priv->is_failed = FALSE;
	self->priv->is_valid = FALSE;

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
_midgard_sql_content_manager_job_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardSqlContentManagerJob *self = MIDGARD_SQL_CONTENT_MANAGER_JOB (object);

	switch (property_id) {
		
		case PROPERTY_CONTENT_OBJECT:
			self->priv->content_object = g_value_dup_object (value);
			break;

		case PROPERTY_REFERENCE:
			self->priv->reference = g_value_dup_object (value);
			break;

		case PROPERTY_MODEL:
			self->priv->model = g_value_dup_object (value);
			break;

		case PROPERTY_CONNECTION:
			self->priv->connection = g_value_dup_object (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void 
_midgard_sql_content_manager_job_class_init (MidgardSqlContentManagerJobClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_sql_content_manager_job_constructor;
	object_class->dispose = _midgard_sql_content_manager_job_dispose;
	object_class->finalize = _midgard_sql_content_manager_job_finalize;
	object_class->set_property = _midgard_sql_content_manager_job_set_property;

	/* PROPERTIES */
	GParamSpec *pspec = g_param_spec_object ("contentobject",
			"", "",
			G_TYPE_OBJECT,
			G_PARAM_WRITABLE  | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardSqlContentManagerJob:contentobject:
	 * 
	 * Content object
	 */
	g_object_class_install_property (object_class, PROPERTY_CONTENT_OBJECT, pspec);

	pspec = g_param_spec_object ("reference",
			"", "",
			MIDGARD_TYPE_OBJECT_REFERENCE,
			G_PARAM_WRITABLE  | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardSqlContentManagerJob:reference:
	 * 
	 * Reference object
	 */
	g_object_class_install_property (object_class, PROPERTY_REFERENCE, pspec);

	pspec = g_param_spec_object ("model",
			"", "",
			MIDGARD_TYPE_MODEL,
			G_PARAM_WRITABLE  | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardSqlContentManagerJob:model:
	 * 
	 * Model for content object
	 */
	g_object_class_install_property (object_class, PROPERTY_MODEL, pspec);

	pspec = g_param_spec_object ("connection",
			"", "",
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_WRITABLE  | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardSqlContentManagerJob:connection:
	 * 
	 * Midgard connection
	 */
	g_object_class_install_property (object_class, PROPERTY_CONNECTION, pspec);
}

/* Executable iface */
static void
_midgard_sql_content_manager_job_executable_iface_init (MidgardExecutableIFace *iface)
{
	iface->execute = NULL;
	iface->execute_async = NULL;
}

/* Job iface */
static void
_midgard_sql_content_manager_job_job_iface_init (MidgardJobIFace *iface)
{
	iface->is_running = _midgard_sql_content_manager_job_job_iface_is_running;
	iface->is_executed = _midgard_sql_content_manager_job_job_iface_is_executed;
	iface->is_failed = _midgard_sql_content_manager_job_job_iface_is_failed;
}

/* ContentManagerJob iface */
static void
_midgard_sql_content_manager_job_content_manager_job_iface_init (MidgardContentManagerJobIFace *iface)
{
	iface->get_content_object = _midgard_sql_content_manager_job_content_manager_job_iface_get_content_object;
	iface->get_reference = _midgard_sql_content_manager_job_content_manager_job_iface_get_reference;
	iface->get_model = _midgard_sql_content_manager_job_content_manager_job_iface_get_model;
}

/* Validable iface */
static void
_midgard_sql_content_manager_job_validable_iface_init (MidgardValidableIFace *iface)
{
	iface->validate = _midgard_sql_content_manager_job_validable_iface_validate;
	iface->is_valid = _midgard_sql_content_manager_job_validable_iface_is_valid;
}

GType
midgard_sql_content_manager_job_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardSqlContentManagerJobClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_content_manager_job_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlContentManagerJob),
			0,              /* n_preallocs */
			_midgard_sql_content_manager_job_instance_init /* instance_init */
		};

		static const GInterfaceInfo executable_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_executable_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo job_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_job_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo content_manager_job_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_content_manager_job_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo validable_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_validable_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSqlContentManagerJob", &info, G_TYPE_FLAG_ABSTRACT);
		g_type_add_interface_static (type, MIDGARD_TYPE_EXECUTABLE, &executable_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_JOB, &job_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_VALIDABLE, &validable_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_CONTENT_MANAGER_JOB, &content_manager_job_info);
	}
	return type;
}
