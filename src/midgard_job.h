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

#ifndef MIDGARD_JOB_H
#define MIDGARD_JOB_H

#include <glib-object.h>
#include "midgard_model.h"
#include "midgard_object_reference.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_JOB (midgard_job_get_type()) 
#define MIDGARD_JOB(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_JOB, MidgardJob))
#define MIDGARD_IS_JOB(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_JOB))
#define MIDGARD_JOB_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_JOB, MidgardJobIFace))

typedef struct _MidgardJob MidgardJob;
typedef struct _MidgardJobIFace MidgardJobIFace;

struct _MidgardJobIFace {
	GTypeInterface parent;

	gboolean 	(*is_running)	(MidgardJob *self);
	gboolean	(*is_executed)	(MidgardJob *self);
	gboolean	(*is_failed)	(MidgardJob *self);
};

GType 			midgard_job_get_type		(void);
gboolean 		midgard_job_is_running		(MidgardJob *self);
gboolean 		midgard_job_is_executed		(MidgardJob *self);
gboolean 		midgard_job_is_failed		(MidgardJob *self);

G_END_DECLS

#endif /* MIDGARD_JOB_H */
