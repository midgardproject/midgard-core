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

#ifndef MIDGARD_CONTENT_MANAGER_H
#define MIDGARD_CONTENT_MANAGER_H

#include <glib-object.h>
#include "midgard_model.h"
#include "midgard_object_reference.h"
#include "midgard_content_manager_job.h"

G_BEGIN_DECLS

/**
 * MidgardContentManagerJobType:
 *
 * Enum values to define job type.
 * Since: 12.09
 */ 

typedef enum {
	MIDGARD_CONTENT_MANAGER_JOB_LOAD,
	MIDGARD_CONTENT_MANAGER_JOB_CREATE,
	MIDGARD_CONTENT_MANAGER_JOB_UPDATE,
	MIDGARD_CONTENT_MANAGER_JOB_DELETE,
	MIDGARD_CONTENT_MANAGER_JOB_PURGE
} MidgardContentManagerJobType;

/* convention macros */
#define MIDGARD_TYPE_CONTENT_MANAGER (midgard_content_manager_get_type()) 
#define MIDGARD_CONTENT_MANAGER(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_CONTENT_MANAGER, MidgardContentManager))
#define MIDGARD_IS_CONTENT_MANAGER(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_CONTENT_MANAGER))
#define MIDGARD_CONTENT_MANAGER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_CONTENT_MANAGER, MidgardContentManagerIFace))

typedef struct _MidgardContentManager MidgardContentManager;
typedef struct _MidgardContentManagerIFace MidgardContentManagerIFace;

struct _MidgardContentManagerIFace {
	GTypeInterface parent;

	MidgardConnection*		(*get_connection)	(MidgardContentManager *self, GError **error);
	MidgardContentManagerJob*	(*create_job)		(MidgardContentManager *self, 
			MidgardContentManagerJobType type, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);
};

GType 				midgard_content_manager_get_type		(void);
MidgardConnection*		midgard_content_manager_get_connection		(MidgardContentManager *self, GError **error);
MidgardContentManagerJob*	midgard_content_manager_create_job		(MidgardContentManager *self, MidgardContentManagerJobType type, GObject *content, MidgardObjectReference *reference, MidgardModel *model, GError **error);

G_END_DECLS

#endif /* MIDGARD_CONTENT_MANAGER_H */
