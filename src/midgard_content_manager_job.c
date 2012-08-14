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

#include "midgard_content_manager_job.h"
#include "midgard_job.h"
#include "midgard_validable.h"

/**
 * midgard_content_manager_job_get_content_object:
 * @self: #MidgardContentManagerJob instance
 * @error: pointer to store returned error
 *
 * Returns: #GObject derived instance which is volatile content data holder
 *
 * Since: 10.05.8
 */ 
GObject*
midgard_content_manager_job_get_content_object (MidgardContentManagerJob *self, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_JOB_GET_INTERFACE (self)->get_content_object (self, error);
}

/**
 * midgard_content_manager_job_get_reference:
 * @self: #MidgardContentManagerJob instance
 * @error: pointer to store returned error
 *
 * Returns: #MidgardObjectReference instance which identifies object in this job
 *
 * Since: 10.05.8
 */ 
MidgardObjectReference*
midgard_content_manager_job_get_reference (MidgardContentManagerJob *self, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_JOB_GET_INTERFACE (self)->get_reference (self, error);
}

/**
 * midgard_content_manager_job_get_model:
 * @self: #MidgardContentManagerJob instance
 * @error: pointer to store returned error
 *
 * Returns: #MidgardModel instance which is a model for content object in this job or %NULL
 *
 * Since: 10.05.8
 */ 
MidgardModel*
midgard_content_manager_job_get_model (MidgardContentManagerJob *self, GError **error)
{
	return MIDGARD_CONTENT_MANAGER_JOB_GET_INTERFACE (self)->get_model (self, error);
}

/* GOBJECT ROUTINES */

GType
midgard_content_manager_job_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardContentManagerJobIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardContentManagerJob", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
		g_type_interface_add_prerequisite (type, MIDGARD_TYPE_JOB);
		g_type_interface_add_prerequisite (type, MIDGARD_TYPE_VALIDABLE);
    	}
    	return type;
}
