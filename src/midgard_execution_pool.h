/* 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_EXECUTION_POOL_H
#define MIDGARD_EXECUTION_POOL_H

#include <glib-object.h>
#include "midgard_pool.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_EXECUTION_POOL (midgard_execution_pool_get_type()) 
#define MIDGARD_EXECUTION_POOL(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_EXECUTION_POOL, MidgardExecutionPool))
#define MIDGARD_EXECUTION_POOL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MIDGARD_TYPE_EXECUTION_POOL, MidgardExecutionPoolClass))
#define MIDGARD_IS_EXECUTION_POOL(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_EXECUTION_POOL))
#define MIDGARD_IS_EXECUTION_POOL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDGARD_EXECUTION_POOL_TYPE))
#define MIDGARD_EXECUTION_POOL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDGARD_EXECUTION_POOL_TYPE, MidgardExecutionPoolClass))

typedef struct _MidgardExecutionPool MidgardExecutionPool;
typedef struct _MidgardExecutionPoolPrivate MidgardExecutionPoolPrivate;
typedef struct _MidgardExecutionPoolClass MidgardExecutionPoolClass;

struct _MidgardExecutionPool {
	GObject parent;

	/* < private > */
	MidgardExecutionPoolPrivate *priv;
};	

struct _MidgardExecutionPoolClass {
	GObjectClass parent;
};

GType 			midgard_execution_pool_get_type	(void);
MidgardExecutionPool*	midgard_execution_pool_new 	(guint max_n_threads);

G_END_DECLS

#endif /* MIDGARD_EXECUTION_POOL_H */
