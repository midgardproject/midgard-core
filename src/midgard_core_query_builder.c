/* 
 * Copyright (c) 2005 Jukka Zitting <jz@yukatan.fi>
 * Copyright (C) 2006, 2007 Piotr Pokora <piotrek.pokora@gmail.com>
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
#include "midgard_schema.h"
#include "midgard_core_query_builder.h"
#include "query_builder.h"
#include "query_constraint.h"
#include "group_constraint.h"
#include "query_order.h"
#include "midgard_object.h"
#include "schema.h"
#include "midgard_reflection_property.h"
#include "midgard_core_object_class.h"
#include "midgard_metadata.h"
#include "midgard_user.h"
#include "midgard_core_object.h"
#include "midgard_object_class.h"

#define _RESERVED_BLOB_NAME "attachment"
#define _RESERVED_BLOB_TABLE "blobs"
#define _RESERVED_PARAM_NAME "parameter"
#define _RESERVED_PARAM_TABLE "record_extension"

MidgardQueryBuilderPrivate *midgard_query_builder_private_new(void)
{
	MidgardQueryBuilderPrivate *mqbp = 
		g_new(MidgardQueryBuilderPrivate, 1);

	mqbp->constraints = NULL;
	mqbp->groups = NULL;
	mqbp->orders = NULL;

	mqbp->tables = g_hash_table_new_full(g_str_hash, g_str_equal,
			g_free, NULL);

	mqbp->value = NULL;
	mqbp->is_grouping = FALSE;
	mqbp->grouping_ref = 0;
	mqbp->group_constraint = NULL;
	mqbp->joins = NULL;
	mqbp->select_distinct = FALSE;
	mqbp->param_join_exists = FALSE;
	mqbp->blob_join_exists = FALSE;

	return mqbp;
}

void midgard_query_builder_private_free(MidgardQueryBuilderPrivate *mqbp)
{
	g_assert(mqbp != NULL);

	GSList *list = NULL;

	/* free constraints */
	for(list = mqbp->constraints; list != NULL; list = list->next) {
		if(list->data != NULL && G_IS_OBJECT(list->data))
			g_object_unref(G_OBJECT(list->data));
	}
	if(list)
		g_slist_free(list);
	list = NULL;

	/* free groups */
	for(list = mqbp->groups; list != NULL; list = list->next) {
		g_object_unref(list->data);
	}
	if(list)
		g_slist_free(list);
	list = NULL;

	/* free orders */
	for(list = mqbp->orders; list != NULL; list = list->next) {
		midgard_core_query_order_free((MidgardQueryOrder *)list->data);
	}
	if(list)
		g_slist_free(list);
	list = NULL;

	/* free joins */
	for(list = mqbp->joins; list != NULL; list = list->next) {
		midgard_core_query_constraint_private_free((MidgardCoreQueryConstraintPrivate*) list->data);
	}
	if(list)
		g_slist_free(list);
	list = NULL;

	/* free tables */
	g_hash_table_destroy(mqbp->tables);

	/* free itself */
	g_free(mqbp);
}

void midgard_core_qb_add_table(MidgardQueryBuilder *builder, const gchar *table)
{
	g_assert(builder != NULL);
	g_assert(table != NULL);

	g_hash_table_insert(builder->priv->tables, g_strdup((gchar *) table), NULL);

	return ;
}

void midgard_core_qb_add_constraint(MidgardQueryBuilder *builder,
		MidgardCoreQueryConstraint *constraint)
{
	g_assert(builder != NULL);
	g_assert(constraint != NULL);

	builder->priv->constraints = 
		g_slist_append(builder->priv->constraints, constraint);	
}

void midgard_core_qb_add_order(MidgardQueryBuilder *builder,
		MidgardQueryOrder *order)
{
	g_assert(builder != NULL);
	g_assert(order != NULL);

	builder->priv->orders = 
		g_slist_append(builder->priv->orders, order);	
}

static void _add_table_foreach(gpointer key, gpointer value, gpointer userdata)
{
	GSList **list = (GSList **) userdata;

	*list = g_slist_append(*list, key);
}

static void __sql_add_constraints(GString *sql, MidgardQueryBuilder *builder)
{
	GSList *clist = NULL;
	GSList *jlist = NULL;
	guint j = 0;
	
	for(clist = builder->priv->constraints;
			clist != NULL; clist = clist->next) {
		
		if(j > 0)
			g_string_append(sql, " AND ");
		
		g_string_append(sql,
				MIDGARD_CORE_QUERY_CONSTRAINT(clist->data)->priv->condition);
		
		for(jlist = ((MidgardCoreQueryConstraint*)clist->data)->priv->joins;
				jlist != NULL; jlist = jlist->next) {
			
			g_string_append(sql, " AND ");
			g_string_append(sql,
					((MidgardCoreQueryConstraintPrivate*)jlist->data)->condition);
		}
		
		j++;
	}

	if(builder->priv->constraints == NULL 
			&& builder->priv->groups == NULL)
		g_string_append(sql, "1=1");
}

gchar *midgard_core_qb_get_sql(
		MidgardQueryBuilder *builder, guint mode, gchar *select)
{
	g_assert (builder != NULL);
        g_assert (builder->priv->schema->table);    

        /* FIXME, error reporting needed */
        if (builder->priv->error != 0) {
		g_warning("Can not create query");
		return NULL;
        }	

	/* SELECT */
        GString *sql = g_string_new ("SELECT ");
	
	if (builder->priv->select_distinct)
		g_string_append (sql, "DISTINCT ");
	
	g_string_append (sql, select);
	g_free (select);

	/* FROM */
        g_string_append (sql, " FROM ");
	GSList *tlist = NULL;
	g_hash_table_foreach (builder->priv->tables, _add_table_foreach, &tlist);

	GString *tables = g_string_new ("");

	guint l = 0;
	GSList *nlist; 
	for (nlist = tlist; nlist != NULL; nlist = nlist->next) {
	
		if(l < 1)
			g_string_append(tables, (gchar *) nlist->data);
		else 
			g_string_append_printf(tables, 
					", %s", (gchar *) nlist->data);
		l++;
	}
	if(tlist)
		g_slist_free(tlist);

	g_string_append(sql, tables->str);

	/* WHERE */
        g_string_append(sql, " WHERE ");
	
	__sql_add_constraints(sql, builder);

	g_string_free(tables, TRUE);

	/* OR&AND groupings */
	guint i = 0;
	builder->priv->groups = g_slist_reverse(builder->priv->groups);
	
	if(builder->priv->groups != NULL) {
		
		if(builder->priv->constraints)
			g_string_append(sql, " AND ");
		
		GSList *glist = NULL;
		MidgardGroupConstraintClass *klass;
		
		for(glist = builder->priv->groups;
				glist != NULL; glist = glist->next) {
			
			if(i > 0)
				g_string_append(sql, " AND ");
			
			klass = MIDGARD_GROUP_CONSTRAINT_GET_CLASS(
					MIDGARD_GROUP_CONSTRAINT(glist->data));
			klass->add_sql(MIDGARD_GROUP_CONSTRAINT(glist->data), sql);
			
			i++;
		}
	}

	/* Join tables from ORDER BY */
	GSList *olist = NULL;
	GSList *jlist = NULL;
	if(builder->priv->orders != NULL) {

		for(olist = builder->priv->orders;
				olist != NULL; olist = olist->next) {
		
			for(jlist = ((MidgardQueryOrder*)olist->data)->constraint->priv->joins;
					jlist != NULL; jlist = jlist->next) {
				
				g_string_append(sql, " AND ");				
				g_string_append(sql,
						((MidgardCoreQueryConstraintPrivate*)jlist->data)->condition);                
			}
		}
	}

	/* Add joins */
	if(builder->priv->joins != NULL) {
		
		for(jlist = builder->priv->joins;
				jlist != NULL; jlist = jlist->next) {
		
			g_string_append(sql, " AND ");                          
			g_string_append(sql,
					((MidgardCoreQueryConstraintPrivate*)jlist->data)->condition);                
		}
	}

	MidgardDBObjectClass *dbklass = g_type_class_peek(builder->priv->type);

	if (dbklass->dbpriv->has_metadata) {
	
		if(!builder->priv->include_deleted) {
	
			g_string_append(sql, " AND ");
			g_string_append_printf(sql,
					" %s.metadata_deleted = 0",
					builder->priv->schema->table);
		}
	}

	/* ORDER BY */
	olist = NULL;
	i = 0;
	if(builder->priv->orders != NULL) {
		
		g_string_append(sql, " ORDER BY ");

		for(olist = builder->priv->orders; 
				olist != NULL; olist = olist->next) {

			if(i > 0)
				g_string_append(sql, ", ");
		
			MidgardQueryOrder *order = 
				(MidgardQueryOrder *)olist->data;
			g_string_append(sql,
					midgard_core_query_order_get_sql(order));

			i++;
		}
	}

        if (mode < MQB_SELECT_COUNT && builder->priv->limit != G_MAXUINT) 
		g_string_append_printf(sql, " LIMIT %u", builder->priv->limit);

	if (builder->priv->offset != 0) 
		g_string_append_printf(sql, " OFFSET %u", builder->priv->offset);

        return g_string_free(sql, FALSE);
}

gchar * midgard_core_qb_get_sql_as(
		MidgardQueryBuilder *builder, const gchar *name)
{
	g_assert(builder != NULL);
	g_assert(name != NULL);

	g_warning("FIXME, it's not supported");

	return NULL;
}

static void __join_reference(MidgardQueryBuilder *builder, const gchar *src_table, const gchar *ref_table)
{
	/* Join guid and parent_guid */
	MidgardCoreQueryConstraint *jc = midgard_core_query_constraint_new();
	GString *jstr = g_string_new("");
	g_string_append_printf(jstr,"%s.parent_guid = %s.guid",
			ref_table, src_table);
	jc->priv->condition = g_strdup(jstr->str);
	g_string_free(jstr, TRUE);
	
	midgard_core_qb_add_constraint(builder,jc);
	
	/* limit to records which are not deleted */
	MidgardCoreQueryConstraint *dc = midgard_core_query_constraint_new();
	GString *dstr = g_string_new("");
	g_string_append_printf(dstr,"%s.metadata_deleted = 0", ref_table);
	dc->priv->condition = g_strdup(dstr->str);
	g_string_free(dstr, TRUE);
	
	midgard_core_qb_add_constraint(builder,dc);
	
	return;
}

static gboolean __add_join(GSList **list, MidgardCoreQueryConstraintPrivate *mqcp)
{
	GSList *clist = NULL;
	gboolean exists = FALSE;
	if(mqcp->condition == NULL)
		return exists;
	
	for(clist = *list; clist != NULL; clist = clist->next) {
		
		if(((MidgardCoreQueryConstraintPrivate *)clist->data)->condition == NULL) 
			continue;
		
		if(g_str_equal(((MidgardCoreQueryConstraintPrivate *)clist->data)->condition, mqcp->condition))
			exists = TRUE;
	}
	
	if(!exists) {

		*list = g_slist_append(*list, mqcp);
		return TRUE;
	}
	
	return FALSE;
}

static gboolean __set_schema_property_attr(
		MidgardQueryBuilder *builder, MidgardDBObjectClass **klass, 
		MidgardCoreQueryConstraint **constraint, const gchar *name, gboolean do_link)
{
	MgdSchemaPropertyAttr *attr = NULL;
	const gchar *target_property = NULL;
	const gchar *link_property = NULL;

	/*
	GParamSpec *pspec = 
		g_object_class_find_property(G_OBJECT_CLASS(*klass), name);

	if(!pspec) {

		g_warning("%s is not registered property of %s class",
				name, G_OBJECT_CLASS_NAME(*klass)); 
		return FALSE;
	}*/

	/* Set klass and property so later we use this info when 
	 * gvalue typecasting should be done */
	(*constraint)->priv->klass = G_OBJECT_CLASS(*klass);
	(*constraint)->priv->propname = name;
	//(*constraint)->priv->pspec = pspec;

	/* Reserved properties */
	if(g_str_equal("metadata", name)) {
		
		*klass = g_type_class_peek(MIDGARD_TYPE_METADATA);
		return TRUE;
	} 
	
	if(g_str_equal(_RESERVED_BLOB_NAME, name)) {

		if (builder && !builder->priv->blob_join_exists) {

			__join_reference(builder, (const gchar *)(*constraint)->priv->current->table, _RESERVED_BLOB_TABLE);
			builder->priv->blob_join_exists = TRUE;
		}

		*klass = 
			g_type_class_peek(g_type_from_name("midgard_attachment"));
		(*constraint)->priv->current->table = _RESERVED_BLOB_TABLE;
		
		return TRUE;
	
	} else 	if(g_str_equal(_RESERVED_PARAM_NAME, name)) {
		
		if (builder && !builder->priv->param_join_exists) {

			__join_reference(builder, (const gchar *)(*constraint)->priv->current->table, _RESERVED_PARAM_TABLE);
			builder->priv->param_join_exists = TRUE;
		}

		*klass = 
			g_type_class_peek(g_type_from_name("midgard_parameter"));
		(*constraint)->priv->current->table = _RESERVED_PARAM_TABLE;

		/* Select DISTINCT.
		 * Constraints might be grouped with "OR" operator and duplicated 
		 * records can be returned explicitly */
		if (builder) {
			builder->priv->select_distinct = TRUE;
		}

		return TRUE;
	
	/* User defined properties */
	} else {

		GParamSpec *pspec = 
			g_object_class_find_property(G_OBJECT_CLASS(*klass), name);
		
		if(!pspec) {
			
			g_warning("%s is not registered property of %s class",
					name, G_OBJECT_CLASS_NAME(*klass)); 
			return FALSE;
		}
		
		/* Set klass and property so later we use this info when 
		 * * gvalue typecasting should be done */
		(*constraint)->priv->klass = G_OBJECT_CLASS(*klass);
		(*constraint)->priv->propname = name;
		(*constraint)->priv->pspec = pspec;

		attr = midgard_core_class_get_property_attr(
				MIDGARD_DBOBJECT_CLASS(*klass), name);

		target_property = NULL;
		link_property = NULL;

		MidgardReflectionProperty *mrp =
			midgard_reflection_property_new(MIDGARD_DBOBJECT_CLASS(*klass));
		
		if(midgard_reflection_property_is_link(mrp, name) && do_link) {

			const gchar *target_property = 
				midgard_reflection_property_get_link_target(mrp, name);
			const gchar *link_klass = 
				midgard_reflection_property_get_link_name(mrp, name);
			gboolean is_link = TRUE;

			if((*constraint)->priv->current->link_target == NULL) {
				
				MidgardCoreQueryConstraintPrivate *mqcp =
					midgard_core_query_constraint_private_new();

				mqcp->current->is_link = is_link;
				mqcp->current->link_target = target_property;
				mqcp->condition_operator = g_strdup("=");

				mqcp->prop_left->table =
					midgard_core_class_get_property_table(
							MIDGARD_DBOBJECT_CLASS(*klass), name);
				mqcp->prop_left->field =
					midgard_core_class_get_property_colname(
							MIDGARD_DBOBJECT_CLASS(*klass), name);
				MidgardObjectClass *lclass = 
					g_type_class_peek(g_type_from_name(link_klass));
				mqcp->prop_right->table =
					midgard_core_class_get_property_table(
							MIDGARD_DBOBJECT_CLASS(lclass), target_property);
				mqcp->prop_right->field = 
					midgard_core_class_get_property_colname(
							MIDGARD_DBOBJECT_CLASS(lclass), target_property);

				GString *cond = g_string_new("");
				g_string_append_printf(cond, 
						"%s.%s %s %s.%s",
						mqcp->prop_left->table,
						mqcp->prop_left->field,
						mqcp->condition_operator,
						mqcp->prop_right->table,
						mqcp->prop_right->field);

				mqcp->condition = g_string_free(cond, FALSE);

				if (builder) {
					midgard_core_qb_add_table(builder, mqcp->prop_left->table);
					midgard_core_qb_add_table(builder, mqcp->prop_right->table);
				}

				if (builder) {
					if(!__add_join(&builder->priv->joins, mqcp)) {
						midgard_core_query_constraint_private_free(mqcp);
					}
				}
			}

			*klass = (MidgardDBObjectClass *) midgard_reflection_property_get_link_class(mrp, name);
			(*constraint)->priv->current->table =
				midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(*klass));

			g_object_unref(mrp);
			return TRUE;
		}
		
		g_object_unref(mrp);
	}

	const gchar *table, *field;

	if (g_str_equal(name, "guid")) {
		
		table = midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(*klass));
		field = "guid";
	
	} else {
		
		table = attr->table;
		field = attr->field;
	}

	if(table != NULL)
		(*constraint)->priv->current->table = table;
	
	(*constraint)->priv->current->field = field;		

	return TRUE;
}

gboolean 
midgard_core_query_constraint_parse_property (MidgardCoreQueryConstraint **constraint, MidgardDBObjectClass *klass, const gchar *name)
{
	g_assert(name != NULL);

	if (!klass)
		klass = MIDGARD_DBOBJECT_CLASS((*constraint)->priv->klass);

	(*constraint)->priv->current->table = midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(klass)); 

	/* This is optional and might be set to NULL explicitly.
	   We need to parse property for any class which needs this */
	MidgardQueryBuilder *builder = (*constraint)->priv->builder;

	gchar **spltd = g_strsplit(name, ".", 0);
	guint i = 0;

	/* We can support max 3 tokens */
	while(spltd[i] != NULL)
		i++;

	if(i == 1) {
	
		g_strfreev(spltd);

		/* FIXME , it should be done by some more generic function */
		GParamSpec *pspec = 
			g_object_class_find_property(G_OBJECT_CLASS(klass), name);

		if(!pspec) {
			
			g_warning("%s is not a member of %s", 
					name, G_OBJECT_CLASS_NAME(klass));
			return FALSE;
		}

		const gchar *stable = 
			midgard_core_class_get_property_table(
					MIDGARD_DBOBJECT_CLASS(klass), name);
		(*constraint)->priv->pspec = pspec;
		(*constraint)->priv->current->field = 
			midgard_core_class_get_property_colname(
					MIDGARD_DBOBJECT_CLASS(klass), name);
		(*constraint)->priv->current->table = stable;
		
		if (builder)
			midgard_core_qb_add_table(builder, stable);

		(*constraint)->priv->klass = G_OBJECT_CLASS(klass);
		(*constraint)->priv->propname = name;
					
		if(g_str_equal(name, "guid")) 
			(*constraint)->priv->current->field = "guid";

		return TRUE;
	}

	if(i > 3) {
		g_warning("Failed to parse '%s'. At most 3 tokens allowed",
				name);
		g_strfreev(spltd);
		
		return FALSE;
	}

	gboolean parsed = FALSE;
	gboolean do_link = TRUE;
	guint j = 0;
	while(spltd[j] != NULL) {
		
		if(j+1 == i)
			do_link = FALSE;

		parsed = __set_schema_property_attr(
				builder, &klass, 
				constraint, (const gchar *) spltd[j], do_link);
		
		if(!parsed) {
			
			g_strfreev(spltd);
			return FALSE;
		}
		
		j++;
	}

	g_strfreev(spltd);

	return TRUE;
}

GObject **midgard_core_qb_execute(MidgardQueryBuilder *builder, guint *n_objects)
{
	g_assert(builder != NULL);

	GList *list = midgard_core_qb_set_object_from_query(builder, MQB_SELECT_OBJECT, NULL);

	if(list == NULL) 
		return NULL;
	
	guint i = 0;
	*n_objects = g_list_length(list);
	MidgardObject **objects = g_new(MidgardObject *, *n_objects+1);

	for( ; list; list = list->next){

		objects[i] = list->data;
		i++;
	}

	objects[i] = NULL; /* Terminate by NULL */

	g_list_free(list);

	return (GObject **)objects;
}

