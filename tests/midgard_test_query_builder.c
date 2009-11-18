/* 
 * Copyright (C) 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_test_query_builder.h"
#include "midgard_test_object_basic.h"

GObject **midgard_test_query_builder_list_all_unlocked(MidgardConnection *mgd, const gchar *name)
{
	MidgardObjectClass *klass = g_type_class_peek (g_type_from_name (name));
	if (!midgard_object_class_has_metadata (klass))
		return NULL;

	MidgardQueryBuilder *builder =
		midgard_query_builder_new(mgd, name);
	g_assert(builder != NULL);
	/* include deleted, as all objects in tests should be already deleted */
	midgard_query_builder_include_deleted(builder);

	GValue val = {0, };
	g_value_init(&val, G_TYPE_BOOLEAN);
	g_value_set_boolean(&val, FALSE);
	midgard_query_builder_add_constraint(builder, "metadata.islocked", "=", &val);
	g_value_unset(&val);

	guint n_objects;
	GObject **objects = midgard_query_builder_execute(builder, &n_objects);

	return objects;
}
