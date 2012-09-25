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

#ifndef MIDGARD_POOL_H
#define MIDGARD_POOL_H

#include <glib-object.h>

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_POOL (midgard_pool_get_type()) 
#define MIDGARD_POOL(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_POOL, MidgardPool))
#define MIDGARD_IS_POOL(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_POOL))
#define MIDGARD_POOL_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_POOL, MidgardPoolIFace))

typedef struct _MidgardPool MidgardPool;
typedef struct _MidgardPoolIFace MidgardPoolIFace;

struct _MidgardPoolIFace {
	GTypeInterface parent;

	guint		(*get_max_n_resources)		(MidgardPool *self);
	MidgardPool*	(*set_max_n_resources)		(MidgardPool *self, guint n_resources, GError **error);
	guint		(*get_n_resources)		(MidgardPool *self);
	MidgardPool*	(*push)				(MidgardPool *self, GObject *object, GError **error);
};

GType 		midgard_pool_get_type			(void);
guint		midgard_pool_get_max_n_resources	(MidgardPool *self);
MidgardPool*	midgard_pool_set_max_n_resources	(MidgardPool *self, guint n_resources, GError **error);
guint		midgard_pool_get_n_resources		(MidgardPool *self);
MidgardPool*	midgard_pool_push			(MidgardPool *self, GObject *object, GError **error);

G_END_DECLS

#endif /* MIDGARD_POOL_H */
