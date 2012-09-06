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

#include "midgard_sql_content_manager.h"
#include "../midgard_content_manager_job.h"
#include "../midgard_content_manager.h"
#include "midgard_sql_content_manager_job_load.h"
#include "midgard_sql_content_manager_job_create.h"
#include "midgard_sql_content_manager_job_update.h"
#include "midgard_sql_content_manager_job_delete.h"
#include "midgard_sql_content_manager_job_purge.h"
#include "../midgard_validable.h"

struct _MidgardSqlContentManagerPrivate {
	MidgardConnection *connection;
};

/* MidgardSqlContentManager properties */
enum {
	PROPERTY_CONNECTION = 1
};

MidgardConnection*
_midgard_sql_content_manager_content_manager_iface_get_connection (MidgardContentManager *iface, GError **error)
{
	g_return_val_if_fail (iface != NULL, NULL);
	MidgardSqlContentManager *self = MIDGARD_SQL_CONTENT_MANAGER (iface);
	if (self->priv->connection == NULL)
		return NULL;
	return g_object_ref (self->priv->connection);
}

MidgardContentManagerJob*
_midgard_sql_content_manager_content_manager_iface_create_job (MidgardContentManager *iface, MidgardContentManagerJobType type, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error)
{
	g_return_val_if_fail (iface != NULL, NULL);

	MidgardSqlContentManager *self = MIDGARD_SQL_CONTENT_MANAGER (iface);
	g_return_val_if_fail (self->priv->connection != NULL, NULL);

	GType job_type;

	switch (type) {
		case  MIDGARD_CONTENT_MANAGER_JOB_LOAD:
			job_type = MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_LOAD;
			break;
		case  MIDGARD_CONTENT_MANAGER_JOB_CREATE:
			job_type = MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_CREATE;
			break;
		case  MIDGARD_CONTENT_MANAGER_JOB_UPDATE:
			job_type = MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_UPDATE;
			break;
		case  MIDGARD_CONTENT_MANAGER_JOB_DELETE:
			job_type = MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_DELETE;
			break;
		case  MIDGARD_CONTENT_MANAGER_JOB_PURGE:
			job_type = MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB_PURGE;
			break;
		default:
			g_set_error (error,
					MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_TYPE_INVALID, 
					"Can not create new job. Invalid job type");
			return NULL;
	}

	MidgardContentManagerJob *job = g_object_new (job_type, 
			"connection", self->priv->connection,
			"contentobject", content,
			"reference", reference, 
			"model", model, 
			NULL);

	return job;
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class= NULL;

static void
_midgard_sql_content_manager_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardSqlContentManager *object = (MidgardSqlContentManager *) instance;

	MIDGARD_SQL_CONTENT_MANAGER (object)->priv = g_new (MidgardSqlContentManagerPrivate, 1);
	MIDGARD_SQL_CONTENT_MANAGER (object)->priv->connection = NULL;
}

static GObject *
_midgard_sql_content_manager_constructor (GType type,
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
_midgard_sql_content_manager_dispose (GObject *object)
{
	MidgardSqlContentManager *self = (MidgardSqlContentManager *) object;

	if (MIDGARD_SQL_CONTENT_MANAGER (self)->priv->connection) {
		g_object_unref (MIDGARD_SQL_CONTENT_MANAGER (self)->priv->connection);
		MIDGARD_SQL_CONTENT_MANAGER (self)->priv->connection = NULL;
	}
	
	parent_class->dispose (object);
}

static void 
_midgard_sql_content_manager_finalize (GObject *object)
{
	MidgardSqlContentManager *self = MIDGARD_SQL_CONTENT_MANAGER (object);

	g_free (self->priv);
	self->priv = NULL;

	parent_class->finalize (object);
}

static void
_midgard_sql_content_manager_set_property (GObject *object, guint property_id,
		const GValue *value, GParamSpec *pspec)
{
	MidgardSqlContentManager *self = MIDGARD_SQL_CONTENT_MANAGER (object);

	switch (property_id) {
		
		case PROPERTY_CONNECTION:
			self->priv->connection = g_value_dup_object (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void 
_midgard_sql_content_manager_class_init (MidgardSqlContentManagerClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_sql_content_manager_constructor;
	object_class->dispose = _midgard_sql_content_manager_dispose;
	object_class->finalize = _midgard_sql_content_manager_finalize;
	object_class->set_property = _midgard_sql_content_manager_set_property;

	/* PROPERTIES */
	GParamSpec *pspec = g_param_spec_object ("connection",
			"", "",
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_WRITABLE  | G_PARAM_CONSTRUCT_ONLY);
	/**
	 * MidgardSqlContentManager:connection:
	 * 
	 * Midgard connection
	 */
	g_object_class_install_property (object_class, PROPERTY_CONNECTION, pspec);
}

/* ContentManager iface */
static void
_midgard_sql_content_manager_content_manager_iface_init (MidgardContentManagerIFace *iface)
{
	iface->get_connection = _midgard_sql_content_manager_content_manager_iface_get_connection;
	iface->create_job = _midgard_sql_content_manager_content_manager_iface_create_job;
}

GType
midgard_sql_content_manager_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardSqlContentManagerClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_content_manager_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlContentManager),
			0,              /* n_preallocs */
			_midgard_sql_content_manager_instance_init /* instance_init */
		};

		static const GInterfaceInfo content_manager_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_content_manager_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSqlContentManager", &info, G_TYPE_FLAG_ABSTRACT);
		g_type_add_interface_static (type, MIDGARD_TYPE_CONTENT_MANAGER, &content_manager_info);
	}
	return type;
}
