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
#include "query_group_constraint.h"
#include "query_constraint.h"
#include "midgard_object.h"

G_DEFINE_TYPE(MidgardCoreQueryGroupConstraint, midgard_core_query_group_constraint, G_TYPE_OBJECT)

MidgardCoreQueryGroupConstraint *midgard_core_query_group_constraint_new(void)
{
	MidgardCoreQueryGroupConstraint *self =
		g_object_new(MIDGARD_CORE_TYPE_QUERY_GROUP_CONSTRAINT, NULL);
	
	return self;
}

static const gchar *valid_types[] = {"OR", "AND", NULL };

gboolean midgard_core_query_group_constraint_type_is_valid(const gchar *type)
{
	guint i;
	
	for (i = 0; valid_types[i] != NULL; i++) {
		
		if (g_str_equal(type, valid_types[i]))
			return TRUE;
		
	}
	
	if (valid_types[i] == NULL) {
		
		/* FIXME, emmit signal here */
		g_warning("Invalid group operator type %s", type);
		return FALSE;
	}
	
	return TRUE;
}

void midgard_core_query_group_constraint_add_sql(
		MidgardCoreQueryGroupConstraint *self, GString *sql)
{
	g_assert(self != NULL);
	g_assert(sql != NULL);

	if(self->priv->constraints == NULL)
		return;

	g_string_append_printf(sql, 
			" %s ( ", 
			self->priv->group_type);

	

	g_string_append(sql, " )");
}

/* GOBJECT ROUTINES */
MidgardCoreQueryGroupConstraintPrivate *midgard_core_query_group_constraint_private_new()
{
	MidgardCoreQueryGroupConstraintPrivate *priv = 
		g_new(MidgardCoreQueryGroupConstraintPrivate, 1);
	priv->group_type = NULL;
	priv->constraints = NULL;

	return priv;
}

void midgard_core_query_group_constraint_private_free(
		MidgardCoreQueryGroupConstraintPrivate *priv)
{
	g_assert(priv != NULL);

	if(priv->group_type != NULL)
		g_free(priv->group_type);

	GSList *list = NULL;
	
	/* free constraints */ 
	for(list = priv->constraints; list != NULL; list = list->next) 
		g_object_unref(list->data);
	
	if(list)
		g_slist_free(list);

	return;
}

static void midgard_core_query_group_constraint_init(MidgardCoreQueryGroupConstraint *self)
{
	g_assert(self != NULL);
	
	self->priv = midgard_core_query_group_constraint_private_new();
}

static void midgard_core_query_group_constraint_finalize(GObject *object)
{
	g_assert(object != NULL);
	
	MidgardCoreQueryGroupConstraint *self = MIDGARD_CORE_QUERY_GROUP_CONSTRAINT(object);
	midgard_core_query_group_constraint_private_free(self->priv);
}

static void midgard_core_query_group_constraint_class_init(
		                MidgardCoreQueryGroupConstraintClass *klass)
{
	g_assert(klass != NULL);
	
	klass->parent.finalize = midgard_core_query_group_constraint_finalize;
	klass->add_sql = midgard_core_query_group_constraint_add_sql;
}
