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

#ifndef MIDGARD_CONTENT_MANAGER_JOB_H
#define MIDGARD_CONTENT_MANAGER_JOB_H

#include <glib-object.h>
#include "midgard_model.h"
#include "midgard_object_reference.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_CONTENT_MANAGER_JOB (midgard_content_manager_job_get_type()) 
#define MIDGARD_CONTENT_MANAGER_JOB(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_CONTENT_MANAGER_JOB, MidgardContentManagerJob))
#define MIDGARD_IS_CONTENT_MANAGER_JOB(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_CONTENT_MANAGER_JOB))
#define MIDGARD_CONTENT_MANAGER_JOB_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_CONTENT_MANAGER_JOB, MidgardContentManagerJobIFace))

typedef struct _MidgardContentManagerJob MidgardContentManagerJob;
typedef struct _MidgardContentManagerJobIFace MidgardContentManagerJobIFace;

struct _MidgardContentManagerJobIFace {
	GTypeInterface parent;

	GObject* 		(*get_content_object)	(MidgardContentManagerJob *self, GError **error);
	MidgardObjectReference*	(*get_reference)	(MidgardContentManagerJob *self, GError **error);
	MidgardModel*		(*get_model)		(MidgardContentManagerJob *self, GError **error);
};

GType 			midgard_content_manager_job_get_type		(void);
GObject* 		midgard_content_manager_job_get_content_object	(MidgardContentManagerJob *self, GError **error);
MidgardObjectReference* midgard_content_manager_job_get_reference	(MidgardContentManagerJob *self, GError **error);
MidgardModel* 		midgard_content_manager_job_get_reference	(MidgardContentManagerJob *self, GError **error);

G_END_DECLS

#endif /* MIDGARD_CONTENT_MANAGER_JOB_H */
