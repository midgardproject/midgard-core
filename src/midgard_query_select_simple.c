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

#include "midgard_query_select_simple.h"
#include "midgard_query_storage.h"
#include "midgard_query_value.h"
#include "midgard_query_property.h"
#include "midgard_query_constraint.h"
#include "midgard_query_select.h"
#include "midgard_query_constraint_group.h"

MidgardDBObject	*
midgard_query_select_simple_get_object (MidgardConnection *mgd, const gchar *classname, const gchar *property, ...)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (classname != NULL, NULL);
	g_return_val_if_fail (property != NULL, NULL);

	/* Initialize storage */
	MidgardQueryStorage *mqs = midgard_query_storage_new (classname);

	GSList *query_values = NULL;
	GSList *query_props = NULL;
	GSList *query_cnstrs = NULL;
	GSList *l = NULL;
	const gchar *pname = property;
	const gchar *op = NULL;
	GValue *val;

	/* Create query property, value and constraint for every given property */
	va_list (args);
	va_start (args, property);
	while (pname != NULL) {
		MidgardQueryProperty *mqp = midgard_query_property_new (pname, NULL);
		query_props = g_slist_append (query_props, mqp);
		op = va_arg (args, gchar *);
		val = va_arg (args, GValue *);
		MidgardQueryValue *mqv = midgard_query_value_create_with_value (val);
		query_values = g_slist_append (query_values, mqv);
	      	MidgardQueryConstraint *mqc = midgard_query_constraint_new (mqp, op, MIDGARD_QUERY_HOLDER (mqv), NULL);
		query_cnstrs = g_slist_append (query_cnstrs, mqc);

	}
	va_end (args);

	/* Initialize query select */
	MidgardQuerySelect *mqselect = midgard_query_select_new (mgd, mqs);
	midgard_query_select_toggle_read_only (mqselect, FALSE);

	/* Initialize query group */
	MidgardQueryConstraintGroup *mqcg = midgard_query_constraint_group_new ();

	for (l = query_cnstrs; l != NULL; l = l->next) {
		midgard_query_constraint_group_add_constraint (mqcg, MIDGARD_QUERY_CONSTRAINT_SIMPLE (l->data));
	}

	midgard_query_executor_set_constraint (MIDGARD_QUERY_EXECUTOR (mqselect), MIDGARD_QUERY_CONSTRAINT_SIMPLE (mqcg));
	midgard_query_executor_execute (MIDGARD_QUERY_EXECUTOR (mqselect));

	guint n_objects;
	MidgardDBObject **objects = midgard_query_select_list_objects (mqselect, &n_objects);
	MidgardDBObject *ret = NULL;
	if (objects && n_objects == 1);
		ret = objects[0];

	g_object_unref (mqs);
	g_object_unref (mqcg);
	g_object_unref (mqselect);
	if (objects)
		g_free (objects);

	/* Free lists */
	l = NULL;
	for (l = query_props; l != NULL; l = l->next) {
		g_object_unref (G_OBJECT (l->data));
	}
	g_slist_free (query_props);

	l = NULL;
	for (l = query_values; l != NULL; l = l->next) {
		g_object_unref (G_OBJECT (l->data));
	}
	g_slist_free (query_values);

	l = NULL;
	for (l = query_cnstrs; l != NULL; l = l->next) {
		g_object_unref (G_OBJECT (l->data));
	}
	g_slist_free (query_cnstrs);

	return ret;
}
