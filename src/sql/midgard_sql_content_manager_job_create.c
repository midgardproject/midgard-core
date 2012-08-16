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
#include "midgard_sql_content_manager_job_create.h"
#include "../midgard_core_query.h"
#include "../midgard_validable.h"
#include "../midgard_executable.h"
#include "../midgard_job.h"

/* GOBJECT ROUTINES */

static GObjectClass *parent_class= NULL;

static GObject *
_midgard_sql_content_manager_job_create_constructor (GType type,
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
_midgard_sql_content_manager_job_create_dispose (GObject *object)
{
	parent_class->dispose (object);
}

static void 
_midgard_sql_content_manager_job_create_finalize (GObject *object)
{
	parent_class->finalize (object);
}

static void 
_midgard_sql_content_manager_job_create_class_init (MidgardSqlContentManagerJobCreateClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_sql_content_manager_job_create_constructor;
	object_class->dispose = _midgard_sql_content_manager_job_create_dispose;
	object_class->finalize = _midgard_sql_content_manager_job_create_finalize;
}

/* Executable iface */
static void
_midgard_sql_content_manager_job_create_executable_iface_init (MidgardExecutableIFace *iface)
{
	iface->execute = NULL;
	iface->execute_async = NULL;
}

/* Job iface */
static void
_midgard_sql_content_manager_job_create_job_iface_init (MidgardJobIFace *iface)
{
	MidgardJobIFace *parent_iface = g_type_interface_peek_parent (iface);
	iface->is_running = parent_iface->is_running; 
	iface->is_executed = parent_iface->is_executed; 
	iface->is_failed = parent_iface->is_failed;
}

/* ContentManagerJob iface */
static void
_midgard_sql_content_manager_job_create_content_manager_job_iface_init (MidgardContentManagerJobIFace *iface)
{
	MidgardContentManagerJobIFace *parent_iface = g_type_interface_peek_parent (iface);
	iface->get_content_object = parent_iface->get_content_object; 
	iface->get_reference = parent_iface->get_reference;
	iface->get_model = parent_iface->get_model;
}

/* Validable iface */
static void
_midgard_sql_content_manager_job_create_validable_iface_init (MidgardValidableIFace *iface)
{
	MidgardValidableIFace *parent_iface = g_type_interface_peek_parent (iface);
	iface->validate = parent_iface->validate;
	iface->is_valid = parent_iface->is_valid;
}

GType
midgard_sql_content_manager_job_create_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardSqlContentManagerJobCreateClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_content_manager_job_create_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlContentManagerJobCreate),
			0,              /* n_preallocs */
			NULL		/* instance_init */
		};

		static const GInterfaceInfo executable_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_create_executable_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo job_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_create_job_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo content_manager_job_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_create_content_manager_job_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		static const GInterfaceInfo validable_info = {
			(GInterfaceInitFunc) _midgard_sql_content_manager_job_create_validable_iface_init,
			NULL,   /* interface_finalize */
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSqlContentManagerJobCreate", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_EXECUTABLE, &executable_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_JOB, &job_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_VALIDABLE, &validable_info);
		g_type_add_interface_static (type, MIDGARD_TYPE_CONTENT_MANAGER_JOB, &content_manager_job_info);
	}
	return type;
}
