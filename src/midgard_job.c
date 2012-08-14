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

#include "midgard_job.h"
#include "midgard_executable.h"

/**
 * midgard_job_is_running:
 * @self: #MidgardJob instance
 *
 * Returns: %TRUE if job is running, %FALSE otherwise
 *
 * Since: 10.05.8
 */ 
gboolean
midgard_job_is_running (MidgardJob *self)
{
	return MIDGARD_JOB_GET_INTERFACE (self)->is_running (self);
}

/**
 * midgard_job_is_executed:
 * @self: #MidgardJob instance
 *
 * Returns: %TRUE if job is executed, %FALSE otherwise
 *
 * Since: 10.05.8
 */ 
gboolean
midgard_job_is_executed (MidgardJob *self)
{
	return MIDGARD_JOB_GET_INTERFACE (self)->is_executed (self);
}

/**
 * midgard_job_is_failed:
 * @self: #MidgardJob instance
 *
 * Returns: %TRUE if job failed, %FALSE otherwise
 *
 * Since: 10.05.8
 */ 
gboolean
midgard_job_is_failed (MidgardJob *self)
{
	return MIDGARD_JOB_GET_INTERFACE (self)->is_failed (self);
}

/* GOBJECT ROUTINES */

GType
midgard_job_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	  	static const GTypeInfo info = {
			sizeof (MidgardJobIFace),
			NULL,   /* base_init */
			NULL,   /* base_finalize */
			NULL,   /* class_init */
			NULL,   /* class_finalize */
			NULL,   /* class_data */
			0,
			0,      /* n_preallocs */
			NULL    /* instance_init */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "MidgardJob", &info, 0);
		g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
		g_type_interface_add_prerequisite (type, MIDGARD_TYPE_EXECUTABLE);
    	}
    	return type;
}
