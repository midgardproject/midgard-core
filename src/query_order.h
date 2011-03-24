/* 
 * Copyright (C) 2005 Jukka Zitting <jz@yukatan.fi>
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
#ifndef QUERY_ORDER_H
#define QUERY_ORDER_H

#include <glib.h>
#include <glib-object.h>
#include "midgard_object.h"
#include "query_constraint.h"

typedef struct MidgardQueryOrder MidgardQueryOrder;

/* This structure is public now as we need it 
 * while adding special orders for types like 
 * metadata or referenced ones */

/*
 * The opaque query order type.
 */
struct MidgardQueryOrder {
	MidgardCoreQueryConstraint *constraint;
};

MidgardQueryOrder *midgard_core_query_order_new(
	MidgardQueryBuilder *builder, const gchar *name, const gchar *dir);

const gchar *midgard_core_query_order_get_sql(MidgardQueryOrder *order);

void midgard_core_query_order_free(MidgardQueryOrder *order);

#endif /* QUERY_ORDER_H */
