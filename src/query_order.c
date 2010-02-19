/* 
 * Copyright (C) 2007 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include <config.h>
#include "query_order.h"
#include <string.h>
#include "midgard_object.h"
#include "midgard_core_query_builder.h"
#include "query_constraint.h"

static const gchar *valid_dirs[] = { "ASC", "DESC", NULL };

MidgardQueryOrder *midgard_core_query_order_new(
		MidgardQueryBuilder *builder, const gchar *name, const gchar *dir)
{
	g_assert(builder != NULL);
	g_assert(name != NULL);

	const gchar *user_dir = dir;

	if(user_dir == NULL)
		user_dir = "ASC";

	guint i = 0;
	while(valid_dirs[i] != NULL) {
		
		if (g_str_equal(user_dir, valid_dirs[i])) 
			break;

		i++;
	}

	if(valid_dirs[i] == NULL) {
		
		 g_warning("Invalid order direction: %s", dir);
		 return NULL;
	}

        MidgardQueryOrder *order = g_new(MidgardQueryOrder, 1);

	order->constraint = midgard_core_query_constraint_new();
	order->constraint->priv->order_dir = g_strdup(dir);

	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_CLASS(g_type_class_peek(builder->priv->type));

	if(!midgard_core_query_constraint_parse_property(&(order->constraint), klass, name)) {

		midgard_core_query_order_free(order);
		return FALSE;
	}

	midgard_core_qb_add_table(builder, order->constraint->priv->prop_left->table);

        return order;
}

const gchar *midgard_core_query_order_get_sql(MidgardQueryOrder *order)
{
        g_assert(order != NULL);

	if(order->constraint->priv->condition != NULL)
		return (const gchar *) order->constraint->priv->condition;

	GString *cond = g_string_new("");
	g_string_append_printf(cond,
			"%s.%s %s",
			order->constraint->priv->prop_left->table,
			order->constraint->priv->prop_left->field,
			order->constraint->priv->order_dir);
	order->constraint->priv->condition = g_string_free(cond, FALSE);

	return (const gchar *) order->constraint->priv->condition;
}

void midgard_core_query_order_free(MidgardQueryOrder *order) 
{
        g_assert(order != NULL);

	g_object_unref(order->constraint);
        g_free(order);
}
