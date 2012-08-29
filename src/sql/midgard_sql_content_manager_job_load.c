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

#include "../midgard_content_manager_job.h"
#include "midgard_sql_content_manager_job_load.h"
#include "../midgard_core_query.h"
#include "../midgard_validable.h"
#include "../midgard_executable.h"
#include "../midgard_job.h"
#include "../midgard_object.h"
#include "../midgard_core_object.h"
#include "../midgard_model_reference.h"

static void 
_validate (MidgardContentManagerJob *job, GError **error)
{
	GError *err = NULL;

	/* Common validation */
	MidgardValidable *validable = MIDGARD_VALIDABLE (job);
	if (!midgard_validable_is_valid (validable)) {
		midgard_validable_validate (validable, &err);
		if (err) {
			g_propagate_error (error, err);
			return;
		}
	}

	/* To load object we must have valid reference, so check it */
	MidgardObjectReference *ref = midgard_content_manager_job_get_reference (job, &err);
	if (err) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_REFERENCE_INVALID,
				"%s", err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
		goto free_objects;
	}

	/* Check property name, we must select data with constraint */
	const gchar *property = midgard_model_get_name (MIDGARD_MODEL (ref), &err);
	if (err) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR,  MIDGARD_VALIDATION_ERROR_NAME_INVALID, 
				"%s", err && err->message ? err->message : "No details for invalid model name");
		g_clear_error (&err);
		goto free_objects;
	}

	/* Content object is already validated */
	MidgardObject *content_object = (MidgardObject *) midgard_content_manager_job_get_content_object (job, &err);

	/* Check if property is registered for the class */
	GParamSpec *pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (content_object), property);
	if (!pspec) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR,  MIDGARD_VALIDATION_ERROR_REFERENCE_INVALID, 
				"Property %s not registered for %s", 
				property, G_OBJECT_TYPE_NAME (content_object));
		goto free_objects;
	}

free_objects:
	if (content_object)
		g_object_unref (content_object);
	if (ref)
		g_object_unref (ref);
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
_midgard_sql_content_manager_job_load_execute (MidgardExecutable *iface, gboolean async, GError **error)
{
	GError *err = NULL;
	GValue *id_val;
	MidgardContentManagerJob *job = MIDGARD_CONTENT_MANAGER_JOB (iface);
	MidgardObjectReference *ref = midgard_content_manager_job_get_reference (job, &err);
	MidgardConnection *mgd = NULL;
	MidgardObject *content_object = NULL;
        

	/* Validate */
	_validate (job, &err);
	if (err) {
		g_propagate_error (error, err);
		goto free_objects;
	}

	if (async == FALSE)
		midgard_executable_execution_start (iface);

	/* Get content object, it should be validated already */
	content_object = (MidgardObject *) midgard_content_manager_job_get_content_object (job, &err);

	/* Get connection, it should be validated already */
	MidgardSqlContentManagerJob *job_sql = MIDGARD_SQL_CONTENT_MANAGER_JOB (iface);
	mgd = midgard_sql_content_manager_job_get_connection (job_sql, NULL);

	/* Get guid or ID via reference */
	id_val = midgard_model_reference_get_id_value (MIDGARD_MODEL_REFERENCE (ref), &err);
	if (err) {
		g_set_error (error, MIDGARD_EXECUTION_ERROR,  MIDGARD_EXECUTION_ERROR_COMMAND_INVALID_DATA, 
				"%s", err && err->message ? err->message : "No details for invalid command data");
		g_clear_error (&err);
		goto free_objects;
	}

	/* Get the name of the property */
	const gchar *property = midgard_model_get_name (MIDGARD_MODEL (ref), &err);
	if (err) {
		g_set_error (error, MIDGARD_EXECUTION_ERROR,  MIDGARD_EXECUTION_ERROR_COMMAND_INVALID_DATA, 
				"%s", err && err->message ? err->message : "No details for invalid model name");
		g_clear_error (&err);
		goto free_objects;
	}

	midgard_core_query_get_object (mgd, NULL, (MidgardDBObject **) &content_object, FALSE, &err, property, &id_val, NULL);
	g_value_unset (id_val);
	if (err) {
		g_set_error (error, MIDGARD_EXECUTION_ERROR,  MIDGARD_EXECUTION_ERROR_COMMAND_INVALID, 
				"%s", err && err->message ? err->message : "No details for invalid command");
		g_clear_error (&err);
		goto free_objects;
	}

	if (async == FALSE) {
		midgard_executable_execution_end (iface);
	} else {
		/* signal emission idle */
		g_idle_add_full (G_PRIORITY_HIGH_IDLE, (GSourceFunc) execution_end_func, g_object_ref (iface), NULL);
	}


free_objects:
	if (ref)
		g_object_unref (ref);
	if (content_object)
		g_object_unref (content_object);
	if (mgd)
		g_object_unref (mgd);
}

static void
_midgard_sql_content_manager_job_load_executable_iface_execute (MidgardExecutable *iface, GError **error)
{
	_midgard_sql_content_manager_job_load_execute (iface, FALSE, error);
}

static void
_midgard_sql_content_manager_job_load_executable_iface_execute_async (MidgardExecutable *iface, GError **error)
{
	_midgard_sql_content_manager_job_load_execute (iface, TRUE, error);
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class= NULL;

static GObject *
_midgard_sql_content_manager_job_load_constructor (GType type,
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
_midgard_sql_content_manager_job_load_dispose (GObject *object)
{
	parent_class->dispose (object);
}

static void 
_midgard_sql_content_manager_job_load_finalize (GObject *object)
{
	parent_class->finalize (object);
}

static void 
_midgard_sql_content_manager_job_load_class_init (MidgardSqlContentManagerJobLoadClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_sql_content_manager_job_load_constructor;
	object_class->dispose = _midgard_sql_content_manager_job_load_dispose;
	object_class->finalize = _midgard_sql_content_manager_job_load_finalize;
}

/* Executable iface */
static void
_midgard_sql_content_manager_job_load_executable_iface_init (MidgardExecutableIFace *iface)
{
	iface->execute = _midgard_sql_content_manager_job_load_executable_iface_execute;
	iface->execute_async = _midgard_sql_content_manager_job_load_executable_iface_execute_async;
}

/* Job iface */
static void
_midgard_sql_content_manager_job_load_job_iface_init (MidgardJobIFace *iface)
{
	MidgardJobIFace *parent_iface = g_type_interface_peek_parent (iface);
	iface->is_running = parent_iface->is_running; 
	iface->is_executed = parent_iface->is_executed; 
	iface->is_failed = parent_iface->is_failed;
}

/* ContentManagerJob iface */
static void
_midgard_sql_content_manager_job_load_content_manager_job_iface_init (MidgardContentManagerJobIFace *iface)
{
	MidgardContentManagerJobIFace *parent_iface = g_type_interface_peek_parent (iface);
	iface->get_content_object = parent_iface->get_content_object; 
	iface->get_reference = parent_iface->get_reference;
	iface->get_model = parent_iface->get_model;
}

/* Validable iface */
static void
_midgard_sql_content_manager_job_load_validable_iface_init (MidgardValidableIFace *iface)
{
	MidgardValidableIFace *parent_iface = g_type_interface_peek_parent (iface);
	iface->validate = parent_iface->validate;
	iface->is_valid = parent_iface->is_valid;
}

GType
midgard_sql_content_manager_job_load_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardSqlContentManagerJobLoadClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_content_manager_job_load_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlContentManagerJobLoad),
			0,              /* n_preallocs */
			NULL		/* instance_init */
		};

		static const GInterfaceInfo executable_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_load_executable_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo job_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_load_job_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo content_manager_job_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_load_content_manager_job_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo validable_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_load_validable_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		type = g_type_register_static (MIDGARD_TYPE_SQL_CONTENT_MANAGER_JOB, "MidgardSqlContentManagerJobLoad", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_EXECUTABLE, &executable_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_JOB, &job_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_VALIDABLE, &validable_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_CONTENT_MANAGER_JOB, &content_manager_job_info);
	}
	return type;
}
