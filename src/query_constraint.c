/* 
 * Copyright (C) 2005 Jukka Zitting <jz@yukatan.fi>
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
#include "query_constraint.h"
#include "midgard_object.h"
#include "midgard_core_query_builder.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include <libgda/libgda.h>

static void add_sql(MidgardCoreQueryConstraint *constraint, GString *sql) {
        g_assert(constraint);
        g_assert(sql);
        g_string_append(sql, "1=1");
}

G_DEFINE_TYPE(MidgardCoreQueryConstraint, midgard_core_query_constraint, G_TYPE_OBJECT)

void midgard_core_query_constraint_add_sql(
                MidgardCoreQueryConstraint *constraint, GString *sql) {
        g_assert(constraint);
        g_assert(sql);
        MIDGARD_CORE_QUERY_CONSTRAINT_GET_CLASS(constraint)->add_sql(constraint, sql);
}

static const char *valid_operator[] = {
	"=", ">", "<", "<>", "<=", ">=", "LIKE", "NOT LIKE", "IN", "NOT IN", "INTREE",
	NULL
};

gboolean midgard_core_query_constraint_add_operator(MidgardCoreQueryConstraint *constraint, const gchar *op)
{
	if(!midgard_core_query_constraint_operator_is_valid(op))
		return FALSE;
	
	constraint->priv->condition_operator = g_strdup(op);
	
	return TRUE;
}

gboolean midgard_core_query_constraint_operator_is_valid(const gchar *op)
{
	guint i;
	for (i = 0; valid_operator[i] != NULL; i++) {
		
		if (g_str_equal(op, valid_operator[i])) 
			return TRUE;
		
	}

	if (valid_operator[i] == NULL) {
		
		/* FIXME, emmit signal here */
		g_warning("Invalid comparison operator %s", op);
		return FALSE;
	}

	return TRUE;
}

gboolean midgard_core_query_constraint_add_value(
		MidgardCoreQueryConstraint *constraint, const GValue *value)
{
	g_assert(constraint != NULL);
	g_assert(value != NULL);

	if(constraint->priv->klass == NULL) {

		g_warning("Can not add value. Constraint initialized for NULL class");
		return FALSE;

	}

	if(constraint->priv->propname == NULL) {
		
		g_warning("Can not add value. Constraint initialized for NULL property");
		return FALSE;
	}

	if(constraint->priv->value != NULL) {
		
		g_warning("Can not add value. Value already associated with constraint");
		return FALSE;
	}

	GValue *target = NULL;
	const gchar *strv = NULL;

	/* Check if value has correct type, so we can do anything with it */
	switch(G_VALUE_TYPE(value)) {
		
		case G_TYPE_STRING:
				
			strv = g_value_get_string(value);	
			gchar *utf8_string;
			gboolean is_utf8 = TRUE;

			if(strv != NULL) 
				is_utf8 = g_utf8_validate(strv, -1, NULL);

			if(!is_utf8) {
				
				utf8_string = g_locale_to_utf8(strv, -1, NULL, NULL, NULL);
				if(utf8_string == NULL) {
					
					g_warning("UTF-8 converion failed");
					return FALSE;
				}
				
				target = g_new0(GValue, 1);
				g_value_init(target, G_TYPE_STRING);
				g_value_set_string(target, utf8_string);
				constraint->priv->value = target;

				g_free(utf8_string);

				return TRUE;
			}

			break;

		case G_TYPE_INT:
		case G_TYPE_UINT:
		case G_TYPE_FLOAT:
		case G_TYPE_BOOLEAN:
			/* do nothing, it's ok */
			break;

		case G_TYPE_BOXED:

			if (G_VALUE_TYPE(value) == MGD_TYPE_TIMESTAMP) {

				target = g_new0(GValue, 1);
				g_value_init(target, G_TYPE_STRING);
				g_value_transform(value, target);
				constraint->priv->value = target;

				return TRUE;
			}

			break;

		default:

			if (G_VALUE_TYPE(value) == MGD_TYPE_TIMESTAMP) {

				target = g_new0(GValue, 1);
				g_value_init(target, G_TYPE_STRING);
				g_value_transform(value, target);
				constraint->priv->value = target;	

				return TRUE;
			}

			if(G_VALUE_TYPE(value) != G_TYPE_VALUE_ARRAY) {
				
				g_warning("Can not add value. Unexpected constraint value type %s", 
						G_VALUE_TYPE_NAME(value)); 
				return FALSE;
			}
	}

	target = g_new0(GValue, 1);
	g_value_init(target, G_VALUE_TYPE(value));
	g_value_copy(value, target);
	constraint->priv->value = target;

	return TRUE;
}

static void __condition_append_value(GString *str, 
		MidgardCoreQueryConstraint *constraint, GValue *value)
{
	gchar *escaped;
	if(value == NULL)
		value = constraint->priv->value;

	GdaConnection *cnc = NULL;
	MidgardQueryBuilder *builder = constraint->priv->builder;
	if (builder)
		cnc = builder->priv->mgd->priv->connection;

	switch(G_VALUE_TYPE(value)) {

		case G_TYPE_STRING:
			if (cnc) {
				escaped =
					gda_connection_value_to_sql_string(
							constraint->priv->builder->priv->mgd->priv->connection,
							value);
				g_string_append_printf(str, "%s", escaped);
				g_free(escaped);
			} else {
				g_string_append_printf (str, "'%s'", g_value_get_string (value));
			}
			break;

		case G_TYPE_UINT:
			g_string_append_printf(str, "%u", g_value_get_uint(value));
			break;

		case G_TYPE_INT:
			g_string_append_printf(str, "%d", g_value_get_int(value));
			break;

		case G_TYPE_FLOAT:
			g_string_append_printf(str, "%g", g_value_get_float(value));
			break;

		case G_TYPE_BOOLEAN:
			if (g_value_get_boolean(value)) {
				g_string_append_c(str, '1');
			} else {
				g_string_append_c(str, '0');
			}
			break;	

		default:
			
			if (G_VALUE_TYPE(value) == MGD_TYPE_TIMESTAMP) {

				GValue str_val = {0, };
				g_value_init(&str_val, G_TYPE_STRING);
				g_value_transform(value, &str_val);

				g_string_append_printf(str, "'%s'", g_value_get_string(&str_val));

				g_value_unset(&str_val);
			
			} else {

				g_warning("Impossible happened. Unsupported value type. foxtrot uniform charlie kilo");
				/* should not happen */
			}
			break;
	}
}

static void __transform_value(MidgardCoreQueryConstraint *self)
{
	GValue *value = self->priv->value;
	GValue *target = g_new0(GValue, 1);
	g_value_init(target, G_PARAM_SPEC_VALUE_TYPE(self->priv->pspec));
	gboolean value_unset = TRUE;
	
	/* Convert the value to the correct type. */
	if (!g_param_value_convert((GParamSpec *)self->priv->pspec, value, target, FALSE)) {
		
		value_unset = FALSE;
		/* The default conversions failed, try custom conversions */
		if (G_VALUE_HOLDS_STRING(value) && G_VALUE_HOLDS_UINT(target)) {

			guint uvalue = strtoul(g_value_get_string(value), NULL, 0);
			g_value_set_uint(target, uvalue);
			value_unset = TRUE;

		} else if (G_VALUE_HOLDS_STRING(value) && G_VALUE_HOLDS_INT(target)) {

			gint ivalue = strtoul(g_value_get_string(value), NULL, 0);
			g_value_set_int(target, ivalue);
			value_unset = TRUE;

		} else {

			if (value && target) {
	
				g_warning("Invalid value conversion from %s to %s",
						G_VALUE_TYPE_NAME(value),
						G_VALUE_TYPE_NAME(target));
			}
		}
	}

	if(value_unset) {
		
		g_value_unset(value);
		g_free(value);

		self->priv->value = target;

		return;
	}

	g_value_unset(target);
	g_free(target);

	return;
}

gboolean midgard_core_query_constraint_build_condition(
		MidgardCoreQueryConstraint *constraint)
{
	g_assert(constraint != NULL);
	
	GdaConnection *cnc = constraint->priv->builder->priv->mgd->priv->connection;
	gchar *q_table = NULL;
	gchar *q_field = NULL;
	guint i;

	GString *cond = g_string_new("");
	q_table = gda_connection_quote_sql_identifier (cnc, constraint->priv->prop_left->table);
	q_field = gda_connection_quote_sql_identifier (cnc, constraint->priv->prop_left->field);
	g_string_append_printf(cond, "%s.%s ", q_table, q_field);
	g_free (q_table);
	g_free (q_field);

	/* No value, just use left and right conditions */
	if(constraint->priv->value == NULL) {
		
		q_table = gda_connection_quote_sql_identifier (cnc, constraint->priv->prop_right->table);
		q_field = gda_connection_quote_sql_identifier (cnc, constraint->priv->prop_right->field);
		g_string_append_printf(cond,
				"%s %s.%s", constraint->priv->condition_operator, q_table, q_field);
		g_free (q_table);
		g_free (q_field);		

		constraint->priv->condition = g_string_free(cond, FALSE);

		return TRUE;
	
	/* Handle the case when IN or NOT IN operator is used */	
	} else if(g_str_equal("IN", constraint->priv->condition_operator)
			|| g_str_equal("NOT IN", constraint->priv->condition_operator)) {

		if (!G_VALUE_HOLDS(constraint->priv->value, G_TYPE_VALUE_ARRAY)) {

			g_warning("'IN' and 'NOT IN' operators expects array as value");
			g_string_free(cond, TRUE);
			return FALSE;
		}

		g_string_append(cond, constraint->priv->condition_operator);		
		g_string_append_c(cond, ' ');

		GValueArray *array = 
			(GValueArray *) g_value_get_boxed(constraint->priv->value);
	
		if (array->n_values == 0) {
			
			g_warning("Invalid empty array value");
			g_string_free(cond, TRUE);
			return FALSE;
		}

		g_string_append(cond, "( ");
		
		for (i = 0; i < array->n_values; i++) {
			if (i > 0) {
				g_string_append_c(cond, ',');
			}	
			__condition_append_value(cond, constraint, 
					g_value_array_get_nth(array, i));
		}

		g_string_append(cond, " )");
		constraint->priv->condition = g_string_free(cond, FALSE);
		return TRUE;
		
	/* INTREE */
	} else if(g_str_equal("INTREE", constraint->priv->condition_operator)) {

		GValue *value = constraint->priv->value;

		gint is_integer = 2;
		if(!G_VALUE_HOLDS(value, G_TYPE_UINT)) {
			is_integer--;
		}
		
		if(!G_VALUE_HOLDS(value, G_TYPE_INT)){
			is_integer--;
		}

		
		if(is_integer == 0){
			g_warning(" Only integers values supported by INTREE operator");
			return FALSE;
		}
		
		g_string_append(cond, "IN ");
	
		guint tid = 0;
		if(G_VALUE_HOLDS(value, G_TYPE_UINT))
			tid = g_value_get_uint(value);
		
		if(G_VALUE_HOLDS(value, G_TYPE_INT))
			tid = (guint)g_value_get_int(value);
		
		guint *prnts = NULL;
		/* FIXME 
		guint *prnts = _midgard_tree_ids(
				MIDGARD_CONNECTION(constraint->priv->builder->priv->mgd),
				MIDGARD_OBJECT_CLASS(parent_class),
				tid);
				*/
		g_string_append_c(cond, '(');
		if(prnts) {
			
			i = 0;
			do {
				if(i > 0)
					g_string_append(cond,",");
				
				g_string_append_printf(cond,
						"%d", prnts[i]);
				i++;
				
			} while (prnts[i]);
	
			g_free(prnts);

		} else {
			
			g_string_append_printf(cond, "%d", tid);
		}
		g_string_append_c(cond, ')');
		constraint->priv->condition = g_string_free(cond, FALSE);
		return TRUE;

	/* Any other case when value is associated with constraint */
	} else if(constraint->priv->value != NULL) {
		
		g_string_append(cond, constraint->priv->condition_operator);
		g_string_append_c(cond, ' ');
		__transform_value(constraint);
		__condition_append_value(cond, constraint, NULL);
		constraint->priv->condition = g_string_free(cond, FALSE);
		return TRUE;

	/* This seems to be impossible with current implementation */
	} else {

		g_string_free(cond, TRUE);
		g_warning("No to mają awarie prądu...");
		return FALSE;
	}

	return FALSE;
}

void            
midgard_core_query_constraint_set_builder (MidgardCoreQueryConstraint *self, MidgardQueryBuilder *builder)
{
	g_assert(self != NULL);
	g_assert(builder != NULL);

	self->priv->builder = builder;
}

void            
midgard_core_query_constraint_set_class (MidgardCoreQueryConstraint *self, MidgardDBObjectClass *klass)
{
	g_assert(self != NULL);
	g_assert(klass != NULL);

	self->priv->klass = G_OBJECT_CLASS(klass);
}

/* GOBJECT ROUTINES */

MidgardCoreQueryConstraintPrivate *
midgard_core_query_constraint_private_new (void)
{
	MidgardCoreQueryConstraintPrivate *priv = g_new (MidgardCoreQueryConstraintPrivate, 1);
	
	/* We need empty structures. We will reuse available pointers */
	priv->prop_left = midgard_core_schema_type_property_attr_new_empty();
	priv->prop_right = midgard_core_schema_type_property_attr_new_empty();

	priv->condition_operator = NULL;
	priv->joins = NULL;
	priv->condition = NULL;
	priv->value = NULL;
	priv->order_dir = NULL;
	
	/* constants, do not free */
	priv->klass = NULL;
	priv->propname = NULL;
	priv->pspec = NULL;
	priv->builder = NULL;

	/* set current pointer to "the left condition" */
	priv->current = priv->prop_left;

	return priv;
}

void midgard_core_query_constraint_private_free(MidgardCoreQueryConstraintPrivate *mqcp)
{
	g_assert(mqcp != NULL);

	/* Just free, they were initialized empty */
	g_free (mqcp->prop_left);
	g_free (mqcp->prop_right);

	if(mqcp->condition_operator != NULL)
		g_free(mqcp->condition_operator);

	if(mqcp->condition != NULL)
		g_free(mqcp->condition);

	if(mqcp->value) {
		
		g_value_unset(mqcp->value);
		g_free(mqcp->value);
	}

	if(mqcp->order_dir) 
		g_free(mqcp->order_dir);

	GSList *list;
	
	for(list = mqcp->joins; list != NULL; list = list->next) {
		midgard_core_query_constraint_private_free(
				(MidgardCoreQueryConstraintPrivate*)list->data);
	}
	g_slist_free(list);

	g_free(mqcp);
}

static void midgard_core_query_constraint_init(MidgardCoreQueryConstraint *self) 
{
	g_assert(self != NULL);

	self->priv = midgard_core_query_constraint_private_new();
}

static void midgard_core_query_constraint_finalize(GObject *object)
{
	g_assert(object != NULL);

	MidgardCoreQueryConstraint *self = MIDGARD_CORE_QUERY_CONSTRAINT(object);
	midgard_core_query_constraint_private_free(self->priv);		
}

static void midgard_core_query_constraint_class_init(
                MidgardCoreQueryConstraintClass *klass) 
{
        g_assert(klass != NULL);
	
	klass->parent.finalize = midgard_core_query_constraint_finalize;
	klass->add_sql = add_sql;
}

MidgardCoreQueryConstraint *midgard_core_query_constraint_new(void)
{
	MidgardCoreQueryConstraint *self = 
		g_object_new(MIDGARD_CORE_TYPE_QUERY_CONSTRAINT, NULL);

	return self;
}
