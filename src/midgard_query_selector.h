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

#ifndef MIDGARD_QUERY_SELECTOR_H
#define MIDGARD_QUERY_SELECTOR_H

#include <glib-object.h>
#include "midgard_query_result.h"

G_BEGIN_DECLS

/* convention macros */
#define MIDGARD_TYPE_QUERY_SELECTOR (midgard_query_selector_get_type()) 
#define MIDGARD_QUERY_SELECTOR(object)  (G_TYPE_CHECK_INSTANCE_CAST ((object),MIDGARD_TYPE_QUERY_SELECTOR, MidgardQuerySelector))
#define MIDGARD_IS_QUERY_SELECTOR(object) (G_TYPE_CHECK_INSTANCE_TYPE ((object), MIDGARD_TYPE_QUERY_SELECTOR))
#define MIDGARD_QUERY_SELECTOR_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), MIDGARD_TYPE_QUERY_SELECTOR, MidgardQuerySelectorIFace))

typedef struct _MidgardQuerySelector MidgardQuerySelector;
typedef struct _MidgardQuerySelectorIFace MidgardQuerySelectorIFace;

struct _MidgardQuerySelectorIFace {
	GTypeInterface parent;

	MidgardQueryResult*	(*get_query_result)	(MidgardQuerySelector *self, GError **error);
	MidgardConnection*	(*get_connection)	(MidgardQuerySelector *self);
};

GType 			midgard_query_selector_get_type		(void);
MidgardQueryResult*	midgard_query_selector_get_query_result	(MidgardQuerySelector *self, GError **error);
MidgardConnection*	midgard_query_selector_get_connection	(MidgardQuerySelector *self);

G_END_DECLS

#endif /* MIDGARD_QUERY_SELECTOR_H */
