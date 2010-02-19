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
#include "group_constraint.h"

static void add_sql(MidgardGroupConstraint *group, GString *sql) 
{
	g_assert(group != NULL);
        g_assert(sql != NULL);

	GSList *list = NULL;
	GSList *jlist = NULL;
	GSList *glist = NULL;
	guint i = 0;

	if(g_slist_length(group->constraints) == 0
			&& g_slist_length(group->nested) == 0) {
		
		g_string_append(sql, "1=1");
		return;
	}

	if(g_slist_length(group->constraints) == 1
			&& g_slist_length(group->nested) == 0) {
	
		g_string_append(sql,
				MIDGARD_CORE_QUERY_CONSTRAINT(group->constraints->data)->priv->condition);
		return;
	}

	g_string_append_c(sql, '(');
		
	for(list = group->constraints; list != NULL;
			list = list->next) {
		
		if(i > 0)
			g_string_append_printf(sql, " %s ", group->type);
		
		g_string_append(sql,
				MIDGARD_CORE_QUERY_CONSTRAINT(list->data)->priv->condition);
		i++;
	}

	for(glist = group->nested; glist != NULL;
			glist = glist->next) {
		
		if(i > 0)
			g_string_append_printf(sql, " %s ", group->type);
		
		MidgardGroupConstraint *ngroup = MIDGARD_GROUP_CONSTRAINT(glist->data);
		MidgardGroupConstraintClass *klass = MIDGARD_GROUP_CONSTRAINT_GET_CLASS(ngroup);
		
		klass->add_sql(ngroup, sql);

		i++;
	}

	g_string_append_c(sql, ')');

	list = NULL;

	for(list = group->constraints; list != NULL;
			list = list->next) {
		
		for(jlist = ((MidgardCoreQueryConstraint*)list->data)->priv->joins;
				jlist != NULL; jlist = jlist->next) {
			
			g_string_append(sql, " AND ");
			g_string_append(sql,
					((MidgardCoreQueryConstraintPrivate*)jlist->data)->condition);
		}
	}
}

G_DEFINE_TYPE(MidgardGroupConstraint, midgard_group_constraint, MIDGARD_CORE_TYPE_QUERY_CONSTRAINT)

static void midgard_group_constraint_init(MidgardGroupConstraint *self) 
{
	g_assert(self != NULL);
}

static void midgard_group_constraint_finalize(MidgardGroupConstraint *group) 
{
	g_assert(group != NULL);

	if(group->type != NULL)
		g_free(group->type);

	GSList *list = NULL;

	for(list = group->constraints; list != NULL; list = list->next) {
		
		g_object_unref(list->data);
	}
	g_slist_free(list);
}

static void midgard_group_constraint_class_init(
                MidgardGroupConstraintClass *klass) 
{
	g_assert(klass != NULL);
	
	MIDGARD_GROUP_CONSTRAINT_CLASS(klass)->add_sql = add_sql;
	G_OBJECT_CLASS(klass)->finalize = (void (*)(GObject *self))
		midgard_group_constraint_finalize;
}

static const gchar *valid_group[] = { "OR", "AND", NULL };

MidgardGroupConstraint *midgard_group_constraint_new(const gchar *type) 
{
	g_assert(type != NULL);

	guint i = 0;
	while(valid_group[i] != NULL) {
		
		if (g_str_equal(type, valid_group[i]))
			break;
		
		i++;
	}
	
	if(valid_group[i] == NULL) {
		
		g_warning("Invalid group operator: %s", type);
		return NULL;
	}

	MidgardGroupConstraint *self = 
		g_object_new(MIDGARD_TYPE_GROUP_CONSTRAINT, NULL);
	
	self->constraints = NULL;
	self->type = g_strdup(type);
	self->nested = NULL;
	self->parent_group = NULL;

	return self;
}


void midgard_group_constraint_add_constraint(
                MidgardGroupConstraint *self,
                MidgardCoreQueryConstraint *constraint) 
{
	/* NULL self is acceptable as it might be unitialized builder->priv->orders. 
	 * If it fails, then it should be changed to orders array */
        g_assert(constraint != NULL);

	self->constraints = g_slist_append(self->constraints, constraint);
}
