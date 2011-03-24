/*
 * Copyright (c) 2005 Jukka Zitting <jz@yukatan.fi>
 * Copyright (c) 2005, 2007, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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
#include "query_builder.h"
#include "query_constraint.h"
#include "group_constraint.h"
#include "query_order.h"
#include "schema.h"
#include "midgard_core_query_builder.h"
#include "midgard_error.h"
#include "midgard_core_object.h"
#include "midgard_core_query.h"
#include "midgard_datatypes.h"
#include "midgard_reflection_property.h"
#include "midgard_object.h"
#include "midgard_metadata.h"
#include "midgard_user.h"
#include "midgard_core_object_class.h"
#include "midgard_dbobject.h"
#include "midgard_timestamp.h"
#include "midgard_core_metadata.h"
#include "midgard_view.h"
#include "midgard_reflector_object.h"
#include "midgard_core_config.h"

/* Internal prototypes , I am not sure if should be included in API */
gchar *midgard_query_builder_get_object_select(MidgardQueryBuilder *builder, guint select_type);

/*
static gboolean _mqb_is_grouping(MidgardQueryBuilder *builder)
{
	return FALSE;

	if(builder->priv->is_grouping) {
		
		g_warning("Group condition not correctly closed ( missed 'end_group' ?) ");
		return TRUE;
	}
	
	return FALSE;
}
*/

static gboolean __type_is_valid(GType type)
{
	GType _type = type;
	if(!_type)
		return FALSE;

	if(type == MIDGARD_TYPE_DBOBJECT 
			|| type == MIDGARD_TYPE_OBJECT)
		return TRUE;

	/* check MIDGARD_TYPE_OBJECT */
	if(g_type_parent(_type) != MIDGARD_TYPE_DBOBJECT) {
		
		/* if fails, try MIDGARD_TYPE_DBOBJECT */
		if(g_type_parent(_type) == MIDGARD_TYPE_OBJECT
				|| g_type_parent(_type) == MIDGARD_TYPE_VIEW) {
			
			return TRUE;
		
		} else {

			return FALSE;
		}
	}

	return TRUE;
}

/**
 * midgard_query_builder_new:
 * @mgd: #MidgardConnection instance
 * @classname: any #MidgardDBObjectClass derived class' name
 *
 * Returns: new #MidgardQueryBuilder instance or %NULL if target class is not registered in GType system 
 * or it's not #MidgardDBObjectClass class derived one.
 */
MidgardQueryBuilder *midgard_query_builder_new(
        MidgardConnection *mgd, const gchar *classname)
{
        g_assert(mgd != NULL);
        g_assert(classname != NULL);

	GType class_type = g_type_from_name(classname);

	if(!__type_is_valid(class_type)) {

		g_warning("Can not initialize Midgard Query Builder for '%s'. It's not registered GType system class", classname);
		MIDGARD_ERRNO_SET(mgd, MGD_ERR_INVALID_OBJECT);
		return NULL;
	}

        MidgardQueryBuilder *builder = 
		g_object_new(MIDGARD_TYPE_QUERY_BUILDER, NULL);
	
	builder->priv = midgard_query_builder_private_new();

        builder->priv->mgd = mgd;
        builder->priv->type = g_type_from_name(classname);
	builder->priv->include_deleted = FALSE;
        builder->priv->error = 0;	
	
	MidgardDBObjectClass *klass = 
		(MidgardDBObjectClass*) g_type_class_peek(class_type);

	if (klass->dbpriv == NULL) {
		
		g_warning("Given %s class has no storage definitions", g_type_name(class_type));
		g_object_unref(builder);
		return NULL;
	}

	builder->priv->schema = klass->dbpriv->storage_data;
       	
	gchar **tables = g_strsplit(midgard_core_class_get_tables(MIDGARD_DBOBJECT_CLASS(klass)), ",", 0);
	guint i = 0;
	while(tables[i] != NULL) {	
		midgard_core_qb_add_table(builder, 
				(const gchar *)g_strchug(tables[i]));
		i++;
	}
	g_strfreev(tables);

        builder->priv->offset = 0;
        builder->priv->limit = G_MAXUINT;

        if (builder->priv->type && builder->priv->schema) {
                return builder;
        } else {
		g_object_unref(builder);
                return NULL;
        }
}

/**
 * midgard_query_builder_create_builder:
 *
 * Static constructor. Invokes midgard_query_builder_new().
 * This function is added for language bindings, in which, that function
 * can not be invoked explicitly.
 *
 * Returns: (transfer full): new #MidgardQueryBuilder instance
 *
 * Since: 10.05.1
 */
MidgardQueryBuilder *
midgard_query_builder_create_builder (MidgardConnection *mgd, const gchar *classname)
{
	return midgard_query_builder_new (mgd, classname);
}

void midgard_query_builder_free(MidgardQueryBuilder *builder) 
{
	g_assert(builder != NULL);

	midgard_query_builder_private_free(builder->priv);
}

/**
 * midgard_query_builder_add_constraint:
 * @builder: #MidgardQueryBuilder instance
 * @name: property name used for this constraint 
 * @op: comparison operator
 * @value: value used in comparison
 *
 * Adds a constraint to the given query builder. The constraint is
 * expressed as a triple of a field name, a comparison operator, and
 * a comparison value. 
 * 
 * <para>
 * @name referes to a property of the queried Midgard object class. 
 * For example, #MidgardQueryBuilder has been initialized for person 
 * class which has lastname property registered.
 * <example>
 * <programlisting>
 * 
 * GValue value = {0, };
 * g_value_init(&value, G_TYPE_STRING);
 * g_value_set_string(&value, "smith");
 *
 * midgard_query_builder_add_constraint(builder, "lastname", "=", &value);
 * 
 * </programlisting>
 * </example>
 * </para>
 * <para>
 * It also can be name of the linked class property or reserved one. 
 * A dot '.' is used to separate properties for special constraints.
 * If such special constraint property is used, #MidgardQueryBuilder
 * performs right join.
 * <itemizedlist>
 * <listitem><para>
 * First property is the one registered for given class which is a link
 * to property of different class. Second is a property of target class.
 * For example, person object has property 'friend' which holds some identifier
 * (id or guid) to friend class property, and friend class has property 'nick'.
 * In such case we can use constraint and comparison using friend property,
 * even if #MidgardQueryBuilder has been initialized for person class.
 * <example>
 * <programlisting>
 * 
 * GValue value = {0, };
 * g_value_init(&value, G_TYPE_STRING);
 * g_value_set_string(&value, "Lancelot");
 *
 * midgard_query_builder_add_constraint(builder, "friend.nick", "=", &value);
 *
 * </programlisting>
 * </example>
 * </para></listitem>
 * <listitem><para>
 * There are three reserved words which have special meaning for query builder.
 * 'metadata', 'parameter' and 'attachment'. If one of them is used, query builder
 * will make (if necessary) right join and query objects against dependent class table.
 * <example>
 * <programlisting>
 * 
 * GValue value = {0, };
 * g_value_init(&value, G_TYPE_STRING);
 * g_value_set_string(&value, "avatar");
 *
 * midgard_query_builder_add_constraint(builder, "attachment.name", "=", &value);
 *
 * </programlisting>
 * </example>
 * </para></listitem>
 * </itemizedlist>
 * </para>
 *
 * <para>
 * The comparison operator is a string representation of the requested comparison. 
 * Available operators are =, <>, <, >, <=, >=, LIKE, NOT LIKE, IN, INTREE. 
 * </para>
 * 
 * <para>
 * The given @value is copied and converted into the property type before comparison.
 * </para>
 *  
 * Returns: %TRUE if constraint is valid, %FALSE otherwise
 */
gboolean midgard_query_builder_add_constraint(
        MidgardQueryBuilder *builder,
        const gchar *name, const gchar *op, const GValue *value) 
{
	g_assert(builder);
        g_assert(name);
        g_assert(op);
        g_assert(value);
     
	MidgardCoreQueryConstraint *constraint = midgard_core_query_constraint_new();
	midgard_core_query_constraint_set_builder(constraint, builder);
	midgard_core_query_constraint_set_class(constraint, MIDGARD_DBOBJECT_CLASS(g_type_class_peek(builder->priv->type)));
	
	if(!midgard_core_query_constraint_add_operator(constraint, op))
		goto return_false;
		
	if(!midgard_core_query_constraint_parse_property(&constraint, MIDGARD_DBOBJECT_CLASS(g_type_class_peek(builder->priv->type)), name))
		goto return_false;

	if(!midgard_core_query_constraint_add_value(constraint, value))
		goto return_false;
	
	if (!midgard_core_query_constraint_build_condition(constraint))
		goto return_false;

	/* FIXME, table should be stored per every constraint, order, whatever */
	midgard_core_qb_add_table(builder, constraint->priv->prop_left->table);
	
	if(builder->priv->grouping_ref > 0) {

		MidgardGroupConstraint *group = 
			(MidgardGroupConstraint *)builder->priv->group_constraint;
		midgard_group_constraint_add_constraint(group, constraint);
		return TRUE;
	
	} 

	midgard_core_qb_add_constraint(builder, constraint);	

	return TRUE;

return_false:
	g_object_unref (constraint);
	return FALSE;
}

/**
 * midgard_query_builder_add_constraint_with_property:
 * @builder: #MidgardQueryBuilder instance
 * @property_a: property name
 * @op: comparison operator
 * @property_b: property name
 *
 * Adds named property constraint to the given query builder.
 * Unlike add_constraint method, this one accepts property name
 * instead of scalar value. The difference is that with add_constraint
 * method you can compare property with particular value, while using 
 * add_constraint_with_property method you can compare two different 
 * properties without any need to know their values. 
 * For example, you should use this method if you want to select only 
 * those objects which has been revised after publication time, and particular 
 * date doesn't matter.
 *
 * <example>
 * <programlisting>
 * 
 * midgard_query_builder_add_constraint_with_property(builder, "metadata.revised", ">", "metadata.published");
 *
 * </programlisting>
 * </example>
 *
 * @See: midgard_query_builder_add_constraint()
 *
 * Returns: %TRUE if properties' names are valid, %FALSE otherwise
 */ 
gboolean midgard_query_builder_add_constraint_with_property(
		MidgardQueryBuilder *builder, const gchar *property_a,
		const gchar *op, const gchar *property_b)
{
	g_assert(builder != NULL);
	g_assert(property_a != NULL);
	g_assert(op != NULL);
	g_assert(property_b != NULL);

	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_CLASS(g_type_class_peek(builder->priv->type));

	MidgardCoreQueryConstraint *constraint = midgard_core_query_constraint_new();
	midgard_core_query_constraint_set_builder(constraint, builder);
	midgard_core_query_constraint_set_class(constraint, klass);

	if(!midgard_core_query_constraint_parse_property(&constraint, klass, property_a))
		goto return_false;
	constraint->priv->current = constraint->priv->prop_right;
	if(!midgard_core_query_constraint_parse_property(&constraint, klass, property_b))
		goto return_false;

	constraint->priv->condition_operator = g_strdup(op);

	if (!midgard_core_query_constraint_build_condition(constraint))
		goto return_false;

	midgard_core_qb_add_table(builder, constraint->priv->prop_left->table);
	midgard_core_qb_add_table(builder, constraint->priv->prop_right->table);

	if(builder->priv->grouping_ref > 0) {
		
		MidgardGroupConstraint *group = builder->priv->group_constraint;
		midgard_group_constraint_add_constraint(group, constraint);
		return TRUE;
	
	} else {
		
		midgard_core_qb_add_constraint(builder, constraint);
	}
	
	return TRUE;

return_false:
	g_object_unref (constraint);
	return FALSE;
}

/**
 * midgard_query_builder_begin_group:
 * @builder: #MidgardQueryBuilder instance
 * @type: group type
 *
 * Starts a constraint group of the given type. A conjunctive constraint
 * group @type (AND) requires that all component constraints match the
 * queried objects, while a disjunctive group @type (OR) requires just
 * one of the component constraints to match.
 *
 * Allowed group @type: 'AND' or 'OR'
 *
 * Returns: %TRUE if the @type is valid, %FALSE otherwise
 */
gboolean 
midgard_query_builder_begin_group(MidgardQueryBuilder *builder, const gchar *type) 
{
	g_assert(builder != NULL);
	
	const gchar *group_op = type;

	if(group_op == NULL)
		group_op = "OR";

        MidgardGroupConstraint *group = 
		midgard_group_constraint_new(group_op);

	if(!group)
		return FALSE;

	/* Check if this is nested grouping */
	if(builder->priv->grouping_ref > 0) {
		
		MidgardGroupConstraint *pgroup = builder->priv->group_constraint;
		pgroup->nested = g_slist_prepend(pgroup->nested, group);
		group->parent_group = pgroup;
	
	} else {
		
		builder->priv->groups =
			g_slist_prepend(builder->priv->groups, group);
	}
	
	builder->priv->group_constraint = group;
	builder->priv->grouping_ref++;
	builder->priv->is_grouping = TRUE;

	return TRUE;
}

/**
 * midgard_query_builder_end_group:
 * @builder: #MidgardQueryBuilder instance
 * 
 * Closes the most recently opened constraint group. The client should
 * ensure proper nesting by closing all constraint groups before the
 * containing query is executed.
 *
 * Returns: %TRUE if a constraint group was closed, or %FALSE if no
 * open constraint groups were found
 *
 */
gboolean 
midgard_query_builder_end_group (MidgardQueryBuilder *builder) 
{
	g_assert(builder != NULL);

	if(builder->priv->grouping_ref == 0) {
		
		g_warning("Missed 'begin_group' method");
		return FALSE;
	}

	MidgardGroupConstraint *group = NULL;
	
	if(builder->priv->group_constraint)
		group = MIDGARD_GROUP_CONSTRAINT(builder->priv->group_constraint);
	
	if(group && group->parent_group) {
	
		builder->priv->group_constraint = group->parent_group;

	} else {
		
		builder->priv->group_constraint = NULL;
	}

	builder->priv->grouping_ref--;

	return TRUE;
}

/**
 * midgard_query_builder_add_order:
 * @builder: #MidgardQueryBuilder instance
 * @name: property name
 * @dir: sort direction
 *
 * Adds an ordering constraint to the query. An ordering constraint consists
 * of a property name and a sort direction. The objects returned by this
 * query will be sorted by the given property in the given direction
 * (ascending or descending). Multiple ordering constraints are applied in
 * the order they were added.
 *
 * Allowed sort directions: 'ASC' or 'DESC'
 * 
 * Property name pattern is described in midgard_query_builder_add_constraint()
 * 
 * Returns: %TRUE if the ordering constraint is valid, %FALSE otherwise
 *
 */
gboolean 
midgard_query_builder_add_order (MidgardQueryBuilder *builder, const gchar *name, const gchar *dir)
{
        g_assert(builder != NULL);
        g_assert(name != NULL);

	MidgardQueryOrder *order = 
		midgard_core_query_order_new(builder, name, dir);

	if (order) {
		
		midgard_core_qb_add_order(builder, order);
		return TRUE;
	
	} else {

		g_warning("Skipping a ordering constraint specification");
		return FALSE;
	}
}

/**
 * midgard_query_builder_set_limit:
 * @builder: #MidgardQueryBuilder instance
 * @limit: query limit
 * 
 * Sets the maximum number of objects to return when the query is executed.
 * A query will by default return all matching objects, but the @limit setting
 * can be used to restrict the potentially large number of returned objects.
 * The @limit is applied only after the matching objects have been identified
 * and sorted and after the optional start offset has been applied.
 *
 * Setting a @limit on the number of returned objects is normally only
 * reasonable when one or more ordering constraints and optionally an offset
 * setting are applied to the query. 
 *
 * @See: midgard_query_builder_set_offset()
 */
void 
midgard_query_builder_set_limit (MidgardQueryBuilder *builder, guint limit)
{
	g_assert(builder != NULL);

        builder->priv->limit = limit;
}

/**
 * midgard_query_builder_set_offset:
 * @builder: #MidgardQueryBuilder instance
 * @offset: query offset
 *
 * Sets the start @offset of the objects to return when the query is executed.
 * The start @offset is applied after all the matching objects have been
 * identified and sorted according to the given ordering constraints. The
 * first @offset objects are skipped and only the remaining (if any) objects
 * are returned to the client.
 *
 * Setting a start offset is normally only reasonable when one or more
 * ordering constraints are applied to the query. A start offset is usually
 * accompanied by a limit setting.
 * 
 * @See: midgard_query_builder_set_offset()
 */
void 
midgard_query_builder_set_offset (MidgardQueryBuilder *builder, guint offset)
{
        g_assert(builder != NULL);

        builder->priv->offset = offset;
}

static GSList *midgard_query_builder_execute_or_count(MidgardQueryBuilder *builder, MidgardTypeHolder *holder, guint select_type)
{
	g_assert(builder != NULL);

	if (builder->priv->grouping_ref > 0) {
		g_warning("Incorrect constraint grouping. Missed 'end_group'?");
		return NULL;
	}

	MidgardUser *user = midgard_connection_get_user(builder->priv->mgd);

	if (builder->priv->type == MIDGARD_TYPE_USER) {
		if (user && midgard_user_is_user(user)) {
			if (MGD_OBJECT_GUID (user) == NULL) {
				MIDGARD_ERRNO_SET(builder->priv->mgd, MGD_ERR_ACCESS_DENIED);
				g_warning("Type incompatible with Midgard Query Builder");
				return NULL;
			}

			GValue gval = {0, };
			g_value_init(&gval, G_TYPE_STRING);
			g_value_set_string(&gval, MIDGARD_DBOBJECT (user)->dbpriv->guid);

			midgard_query_builder_add_constraint(builder, "guid", "=", &gval);
		}
	}

	g_signal_emit (builder, MIDGARD_QUERY_BUILDER_GET_CLASS (builder)->signal_id_execution_start, 0);

	GSList *list = midgard_core_qb_set_object_from_query(builder, select_type, NULL);

	if (list == NULL) {

		if (holder)
			holder->elements = 0;
	} else {
		
		if (holder)
			holder->elements = g_slist_length(list);
	}

	g_signal_emit (builder, MIDGARD_QUERY_BUILDER_GET_CLASS (builder)->signal_id_execution_end, 0);

	return list;
}

/**
 * midgard_query_builder_execute:
 * @builder: #MidgardQueryBuilder instance
 * @n_objects: a pointer to store number of returned objects
 * 
 * Executes the built query.
 * 
 * Objects in returned array are #MidgardDBObject derived ones,
 * and typecasted to base GObject. You can safely typecast them to 
 * the type, which #MidgardQueryBuilder has been initialized for.
 * 
 * In case of any error, #MidgardConnection error is set.
 * 
 * Returns: (transfer full): NULL terminated array of objects, or NULL if none found
 */  
GObject **midgard_query_builder_execute(
	MidgardQueryBuilder *builder, guint *n_objects)
{
	g_assert(builder != NULL);
	g_assert(n_objects != NULL);

	*n_objects = 0;

	guint i = 0;
	GSList *list = NULL;
	MIDGARD_ERRNO_SET(builder->priv->mgd, MGD_ERR_OK);

	MidgardTypeHolder *holder = g_new(MidgardTypeHolder, 1);

        list = midgard_query_builder_execute_or_count(builder, holder, MQB_SELECT_OBJECT);
	
	if(list == NULL) {	
		g_free(holder);
		return NULL;
	}

	*n_objects = holder->elements;
	MidgardObject **objects = g_new(MidgardObject *, holder->elements+1);	

	GSList *l = NULL;
	for(l = list; l != NULL; l = l->next){
		objects[i] = l->data;
		i++;
	}	
	objects[i] = NULL; /* Terminate by NULL */

	g_slist_free(list);
	g_free(holder);

	return (GObject **)objects;	
}

/** 
 * midgard_query_builder_count:
 * @builder: #MidgardQueryBuilder instance
 *
 * Returns the number of objects that this query would return when executed
 * without limit or start offset settings.
 *
 * Returns: number of object matched by this query
 */
guint 
midgard_query_builder_count (MidgardQueryBuilder *builder)
{
	g_assert(builder != NULL);
	MIDGARD_ERRNO_SET(builder->priv->mgd, MGD_ERR_OK);
	
	GSList *list =
		midgard_query_builder_execute_or_count(builder, NULL, MQB_SELECT_GUID);
	
	if(!list)
		return 0;

	MidgardTypeHolder *holder = (MidgardTypeHolder *)list->data;

	if(!holder) {
		g_slist_free(list);
		return 0;
	}

	guint elements = holder->elements;
	
	g_free(holder);
	g_slist_free(list);
	
	return elements;
}

gchar *midgard_query_builder_get_object_select(
        MidgardQueryBuilder *builder, guint select_type){

        g_assert(builder != NULL);
        g_assert(builder->priv->type);
	
	MidgardObjectClass *klass = (MidgardObjectClass*) g_type_class_peek(builder->priv->type);
	const gchar *table = midgard_core_class_get_table(MIDGARD_DBOBJECT_CLASS(klass));

        if (!table){
                g_warning("Object '%s' has no table or storage defined!", 
                                g_type_name(builder->priv->type));
                return NULL;
        }
		
        GString *select = g_string_new("");

	/* We are getting only guids */
	if(select_type == MQB_SELECT_GUID){
		
		g_string_append(select, "COUNT(*) ");
		return g_string_free(select, FALSE);
	}
	
        /* guid hardcoded ( inherited from MidgardObjectClass )
         * metadata properties hardcoded ( defined in midgard_metadata )
         */ 
	/* FIXME, move this to particular class implementation, so we can reuse MidgardDBObject derived class data only */
	if (MIDGARD_IS_OBJECT_CLASS (klass))
			 g_string_append_printf(select, "%s.guid, ", table);

	if (MIDGARD_DBOBJECT_CLASS(klass)->dbpriv->has_metadata) {

		g_string_append_printf(select, 	
				"%s.metadata_creator, "
				"%s.metadata_created, "
				"%s.metadata_revisor, "
				"%s.metadata_revised, "
				"%s.metadata_revision, "
				"%s.metadata_locker, "
				"%s.metadata_locked, "
				"%s.metadata_approver, "
				"%s.metadata_approved, "
				"%s.metadata_authors, "
				"%s.metadata_owner, "
				"%s.metadata_schedule_start, "
				"%s.metadata_schedule_end, "
				"%s.metadata_hidden, "
				"%s.metadata_nav_noentry, "
				"%s.metadata_size, "
				"%s.metadata_published, "
				"%s.metadata_exported, "
				"%s.metadata_imported, "
				"%s.metadata_deleted, "
				"%s.metadata_score, "
				"%s.metadata_islocked, "
				"%s.metadata_isapproved "
				", ", 
			table, table, table, table, table, table, table, table, table,
			table, table, table, table, table, table, table, table, table,
			table, table, table, table, table); 
		/* TODO, Set select which suits better */	
	} 

	if (builder->priv->schema->sql_select_full == NULL
			|| MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->set_static_sql_select != NULL)
		MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->set_static_sql_select (builder->priv->mgd, MIDGARD_DBOBJECT_CLASS (klass));

	if (builder->priv->schema->sql_select_full != NULL) {
		g_string_append_printf(select, " %s", 
				builder->priv->schema->sql_select_full);
	}

        return g_string_free(select, FALSE);        
}

#define _SET_METADATA_UINT(__prop, __value) \
	if(G_VALUE_HOLDS_UINT(__value)) { \
		__prop = \
			g_value_get_uint(__value); \
	} \
	if(G_VALUE_HOLDS_INT(__value)) { \
		__prop = \
			(guint)g_value_get_int(__value); \
	}  

#define _SET_METADATA_TIMESTAMP(__pname) { \
	g_value_init(&mt, MGD_TYPE_TIMESTAMP); \
	g_value_transform(gvalue, &mt); \
	midgard_core_metadata_set_##__pname(mdata, &mt); \
	g_value_unset(&mt); }

static void __mqb_set_metadata(MidgardMetadata *mdata, GdaDataModel *model, gint i)
{
	guint col = 0;
	const GValue *gvalue;	
	GValue mt = {0, };

	/* creator */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);	
	midgard_core_metadata_set_creator (mdata, gvalue);

	/* created */	
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP(created);

	/* revisor */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	if(G_VALUE_HOLDS_STRING (gvalue)) 
		midgard_core_metadata_set_revisor (mdata, gvalue);
		
	/* revised */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP(revised);

	/* revision */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	mdata->priv->revision = 0;
	_SET_METADATA_UINT(mdata->priv->revision, gvalue);
		
	/* locker */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	if(G_VALUE_HOLDS_STRING (gvalue)) 
		midgard_core_metadata_set_locker (mdata, gvalue);
		
	/* locked */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP(locked);

	/* approver */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	if(G_VALUE_HOLDS_STRING (gvalue)) 
		midgard_core_metadata_set_approver (mdata, gvalue);


	/* approved */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP(approved);

	/* authors */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	if(G_VALUE_HOLDS_STRING (gvalue)) 
		midgard_core_metadata_set_authors (mdata, gvalue);
		
	/* owner */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	if(G_VALUE_HOLDS_STRING (gvalue)) 
		midgard_core_metadata_set_owner (mdata, gvalue);

	/* schedule_start */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP (schedule_start);

	/* schedule_end */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP (schedule_end);

	/* hidden */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	mdata->priv->hidden = FALSE;
	MIDGARD_GET_BOOLEAN_FROM_VALUE (mdata->priv->hidden, gvalue);

	/* nav_noentry */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	MIDGARD_GET_BOOLEAN_FROM_VALUE (mdata->priv->nav_noentry, gvalue);

	/* size */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	mdata->priv->size = 0;
	if(G_VALUE_HOLDS_INT(gvalue)) {
		mdata->priv->size = (guint) g_value_get_int(gvalue);
	}	

	/* published */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP (published);

	/* exported */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP(exported);

	/* imported */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	_SET_METADATA_TIMESTAMP(imported);

	/* deleted */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	MIDGARD_GET_BOOLEAN_FROM_VALUE (mdata->priv->deleted, gvalue);
		
	/* score */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	mdata->priv->score = 0;
	if(G_VALUE_HOLDS_INT(gvalue)) {
		mdata->priv->score = 
			g_value_get_int(gvalue);
	}

	/* islocked */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	MIDGARD_GET_BOOLEAN_FROM_VALUE (mdata->priv->is_locked, gvalue);

	/* isapproved */
	gvalue = midgard_data_model_get_value_at(model, ++col, i);
	MIDGARD_GET_BOOLEAN_FROM_VALUE (mdata->priv->is_approved, gvalue);

	return;
}


GSList *
midgard_core_qb_set_object_from_query (MidgardQueryBuilder *builder, guint select_type, MidgardObject **nobject)
{
        g_assert(builder != NULL);
 
        guint ret_rows, ret_fields;
	MidgardConnection *mgd = builder->priv->mgd;
      
	gchar *sql = midgard_core_qb_get_sql(
			builder, select_type, 
			midgard_query_builder_get_object_select(builder, select_type));

	GSList *olist = NULL;
	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_CLASS(g_type_class_peek(builder->priv->type));

	if (!dbklass) {

		MIDGARD_ERRNO_SET_STRING (mgd, MGD_ERR_INTERNAL, "Failed to peek MidgardDBObjectClass pointer");
		return NULL;
	}

	if (dbklass->dbpriv->set_from_sql) {

		if (select_type != MQB_SELECT_GUID) {

			olist = dbklass->dbpriv->set_from_sql(mgd, builder->priv->type, ((const gchar *)sql));
			g_free(sql);

			return olist;
		}
	}

	GdaDataModel *model = midgard_core_query_get_model(builder->priv->mgd, sql);
	g_free(sql);

	if(!model) 
		return NULL;	

	MidgardObject *object = NULL;
	gint rows, columns;
	const GValue *gvalue = NULL;

	ret_rows = gda_data_model_get_n_rows(model);
	ret_fields =  gda_data_model_get_n_columns(model);

	/* records found , allocate as many objects as many returned rows */ 
	GSList *list = NULL;

	if(ret_rows == 0) {
		g_object_unref(model);
		return list;
	}

	/* We count rows only */
	if(select_type == MQB_SELECT_GUID) {
		
		gvalue = midgard_data_model_get_value_at(model, 0, 0);	
	
		if (!gvalue || !G_IS_VALUE (gvalue)) {
			
			g_object_unref (model);
			return 0;
		}

		MidgardTypeHolder *holder = g_new(MidgardTypeHolder, 1);

		GValue val = {0, };
		g_value_init (&val, G_TYPE_INT64);
		g_value_transform (gvalue, &val);

		holder->elements = (guint)g_value_get_int64((GValue*)&val);
		
		list = g_slist_append(list, holder);	
		
		g_object_unref(model);
		g_value_unset (&val);

		return list;		
	}

	/* Get every row */
	for (rows = 0; rows < ret_rows; rows++) {
	
		if(!nobject)
			object = g_object_new (builder->priv->type, "connection", mgd, NULL);
		else 
			object = *nobject;

		MGD_OBJECT_IN_STORAGE (object) = TRUE;	
				
		if(dbklass->dbpriv->__set_from_sql != NULL) {
			
			dbklass->dbpriv->__set_from_sql(MIDGARD_DBOBJECT(object), 
					model, rows); 

		} else {
		
			/* Compute number of metadata properties */
			guint n_md_props = 0;
			MidgardMetadata *mklass = (MidgardMetadata *)MGD_DBCLASS_METADATA_CLASS (dbklass);
			if (mklass) 
				n_md_props = g_hash_table_size (MGD_DBCLASS_STORAGE_DATA (mklass)->prophash);

			guint __cols = n_md_props + 1; /* Add one for guid */

			/* we have guid and metadata columns first */
			for (columns = __cols; columns < ret_fields; columns++) {	
				
				gvalue = midgard_data_model_get_value_at(model, columns, rows);
				const gchar *coltitle =
					gda_data_model_get_column_title(model, columns);
				GParamSpec *pspec =
					g_object_class_find_property(G_OBJECT_GET_CLASS(object), coltitle);

				if(G_IS_VALUE(gvalue) && !gda_value_is_null(gvalue)) {

					if (!pspec) {
						
						g_warning("Failed to found (unregistered) %s property (%s class)", 
								coltitle, G_OBJECT_TYPE_NAME(object));
						continue;
					}

					if (G_VALUE_TYPE (gvalue) == GDA_TYPE_BLOB
							&& G_TYPE_FUNDAMENTAL (pspec->value_type) == G_TYPE_STRING) {

						gchar *stringified = midgard_core_query_binary_stringify ((GValue*)gvalue);	
						g_object_set (G_OBJECT (object), coltitle, stringified, NULL);
				   		g_free (stringified);
				
					} else if (pspec->value_type != G_VALUE_TYPE (gvalue)) {

						GValue _convert = {0, };
						g_value_init (&_convert, pspec->value_type);	
				
						if (g_value_transform (gvalue, &_convert)) {

							g_object_set_property (G_OBJECT (object), coltitle, &_convert);
					
						} else {

							g_warning ("Failed to convert %s to %s for %s property",
									G_VALUE_TYPE_NAME (gvalue),
									G_VALUE_TYPE_NAME (&_convert), 
									coltitle);
						}

						g_value_unset (&_convert);

					} else {

						g_object_set_property(G_OBJECT(object), coltitle, gvalue);
					}	
				
				} else if (gda_value_is_null(gvalue)) {			

					switch (pspec->value_type) {

						case G_TYPE_INT:
						case G_TYPE_UINT:
						case G_TYPE_BOOLEAN:
							g_object_set(G_OBJECT(object), coltitle, 0, NULL);
							break;

						case G_TYPE_STRING:
							g_object_set(G_OBJECT(object), coltitle, "", NULL);
							break;

						default:
							g_warning("Found (%s) not a value at %d.%d (%s)", 
									g_type_name(pspec->value_type), 
									columns, rows, 
									gda_data_model_get_column_title(model, columns));
							break;
					}
				}
			}	
		}

		/* Set guid */
		const gchar *guid;
		gvalue = midgard_data_model_get_value_at(model, 0, rows);
		if(G_IS_VALUE(gvalue) && G_VALUE_HOLDS_STRING(gvalue)){
			guid = g_value_get_string(gvalue);   
			g_free((gchar *)MIDGARD_DBOBJECT(object)->dbpriv->guid);
			MIDGARD_DBOBJECT(object)->dbpriv->guid = g_strdup(guid);
		}
		
		/* Set metadata */
		MidgardMetadata *metadata = MGD_DBOBJECT_METADATA (object);
	     	if (metadata) {
			GParamSpec *pspec = g_object_class_find_property(G_OBJECT_GET_CLASS(object), "metadata");
			if (pspec)
				__mqb_set_metadata(metadata, model, rows);
		}	

		list = g_slist_append(list, G_OBJECT(object));                		
	}

	g_object_unref(model);
	return list;	
}
 
/**
 * midgard_query_builder_get_type_name:
 * @builder: #MidgardQueryBuilder instance
 * 
 * Returns type name of the type which is currently used by Query Builder.
 * 
 * This function should be used on language binding level , when internal
 * Query Builder's instance is already created and language binding object 
 * should be instanciated.
 *
 * Returned type name is a pointer and is owned by GLib system.
 * Caller shouldn't free it.
 *
 * Returns: name of the class, which query builder is initialized for.
 */ 
const gchar*
midgard_query_builder_get_type_name (MidgardQueryBuilder *builder)
{
	g_assert(builder != NULL);
	return g_type_name(builder->priv->type);
}

/**
 * midgard_query_builder_include_deleted:
 * @builder: #MidgardQueryBuilder instance
 * 
 * Query all objects - deleted and undeleted.
 *
 * This is valid for a class for which metadata is defined ('deleted' property
 * of MidgardMetadata class), or class has 'metadata' property installed. 
 * If none of them is found, this method does nothing.
 */ 
void 
midgard_query_builder_include_deleted (MidgardQueryBuilder *builder)
{
	g_assert(builder);

	builder->priv->include_deleted = TRUE;
}

/* GOBJECT ROUTINES*/

static void _midgard_query_builder_finalize(GObject *object)
{
	midgard_query_builder_free(
			(MidgardQueryBuilder *)object);
}

static void _midgard_query_builder_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardQueryBuilderClass *klass = MIDGARD_QUERY_BUILDER_CLASS (g_class);
	
	gobject_class->finalize = _midgard_query_builder_finalize;
	klass->add_constraint = midgard_query_builder_add_constraint;
	klass->add_constraint_with_property = 
		midgard_query_builder_add_constraint_with_property;
	klass->begin_group = midgard_query_builder_begin_group;
	klass->end_group = midgard_query_builder_end_group;
	klass->add_order = midgard_query_builder_add_order;
	klass->set_limit = midgard_query_builder_set_limit;
	klass->set_offset = midgard_query_builder_set_offset;
	klass->include_deleted = midgard_query_builder_include_deleted;
	klass->execute = midgard_query_builder_execute;
	klass->count = midgard_query_builder_count;

	klass->signal_id_execution_start =
		g_signal_new("execution-start",
				G_TYPE_FROM_CLASS(g_class),
				G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
				G_STRUCT_OFFSET (MidgardQueryBuilderClass, execution_start),
				NULL, /* accumulator */
				NULL, /* accu_data */
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE,
				0);

	klass->signal_id_execution_end =
		g_signal_new("execution-end",
				G_TYPE_FROM_CLASS(g_class),
				G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
				G_STRUCT_OFFSET (MidgardQueryBuilderClass, execution_end),
				NULL, /* accumulator */
				NULL, /* accu_data */
				g_cclosure_marshal_VOID__VOID,
				G_TYPE_NONE,
				0);
}

static void _midgard_query_builder_instance_init(
		GTypeInstance *instance, gpointer g_class)
{

}

/* Registers the type as  a fundamental GType unless already registered. */
GType midgard_query_builder_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardQueryBuilderClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_query_builder_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardQueryBuilder),
			0,              /* n_preallocs */
			(GInstanceInitFunc) _midgard_query_builder_instance_init /* instance_init */
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardQueryBuilder", &info, 0);
	}
	return type;
}

