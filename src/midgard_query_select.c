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

#include "midgard_query_select.h"
#include "midgard_core_query.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"

MidgardQuerySelect *
midgard_query_select_new (MidgardConnection *mgd, MidgardQueryStorage *storage)
{
	g_return_val_if_fail (mgd != NULL, NULL);
	g_return_val_if_fail (storage != NULL, NULL);

	MidgardQuerySelect *self = g_object_new (MIDGARD_TYPE_QUERY_SELECT, NULL);
	self->priv->mgd = mgd;
	self->priv->storage = storage;

	return self;
}

gboolean
_midgard_query_select_set_constraint (MidgardQuerySelect *self, MidgardQuerySimpleConstraint *constraint)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (constraint != NULL, FALSE);

	self->priv->constraint = constraint;
}

gboolean
_midgard_query_select_set_limit (MidgardQuerySelect *self, guint limit)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (limit > 0, FALSE);

	self->priv->limit = limit;
}

gboolean 
_midgard_query_select_set_offset (MidgardQuerySelect *self, guint offset)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (offset > -1, FALSE);

	self->priv->offset = offset;
}

static gchar* valid_order_types[] = {"ASC", "DESC", NULL};

gboolean
_midgard_query_select_add_order (MidgardQuerySelect *self, MidgardQueryProperty *property, const gchar *type)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (property != NULL, FALSE);


	/* TODO */

	return FALSE;
}

gboolean
_midgard_query_select_add_join (MidgardQuerySelect *self, const gchar *join_type, 
		MidgardQueryProperty *left_property, MidgardQueryProperty *right_property)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (join_type != NULL, FALSE);
	g_return_val_if_fail (left_property != NULL, FALSE);
	g_return_val_if_fail (right_property != NULL, FALSE);

	/* TODO */

	return FALSE;
}

gboolean 
_midgard_query_select_execute (MidgardQuerySelect *self)
{
	g_return_val_if_fail (self != NULL, FALSE);

	if (!self->priv->storage) {
		/* FIXME, handle error */
		g_warning ("Missed QueryStorage associated with QuerySelect");
		return FALSE;
	}
	
	MidgardDBObjectClass *klass = self->priv->storage->klass;
	if (!klass->dbpriv->add_fields_to_select_statement) {
		/* FIXME, handle error */
		g_warning ("Missed private DBObjectClass' fields to statement helper");
		return FALSE;
	}

	GdaConnection *cnc = self->priv->mgd->priv->connection;
	GdaSqlStatement *sql_stm;
	GdaSqlStatementSelect *sss;
	sql_stm = gda_sql_statement_new (GDA_SQL_STATEMENT_SELECT);
	sss = (GdaSqlStatementSelect*) sql_stm->contents;
	g_assert (GDA_SQL_ANY_PART (sss)->type == GDA_SQL_ANY_STMT_SELECT);

	/* Create targets (FROM) */
	sss->from = gda_sql_select_from_new (GDA_SQL_ANY_PART (sss));
	GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (sss->from));
	s_target->table_name = g_strdup (midgard_core_class_get_table (klass));
	sss->from->targets = g_slist_append (sss->from->targets, s_target);
	GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
	GValue *tval = g_new0 (GValue, 1);
	g_value_init (tval, G_TYPE_STRING);
	g_value_set_string (tval, "page");
	texpr->value = tval;
	s_target->expr = texpr;

	/* Add fields for all properties registered per class */
	klass->dbpriv->add_fields_to_select_statement (klass, sss);

	/* Create condition */
	GdaSqlExpr *where;
	where = gda_sql_expr_new (GDA_SQL_ANY_PART (sss));
	sss->where_cond = where;

	/* Create statement */
	GdaStatement *stmt = gda_statement_new ();	
	g_object_set (G_OBJECT (stmt), "structure", sql_stm, NULL);
	gda_sql_statement_free (sql_stm);

	gchar *debug_sql = gda_connection_statement_to_sql (cnc, stmt, NULL, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
	g_print ("%s", debug_sql);
	g_free (debug_sql);

	/* execute statement */
	GError *error = NULL;
	gda_connection_statement_execute_select (cnc, stmt, NULL, &error);
	if (error)
		g_print ("Execute error - %s", error->message);

	return FALSE;
}

/* GOBJECT ROUTINES */

static GObjectClass *parent_class= NULL;

static GObject *
_midgard_query_select_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	return G_OBJECT(object);
}

static void
_midgard_query_select_dispose (GObject *object)
{
	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (object);
	parent_class->dispose (object);
}

static void 
_midgard_query_select_finalize (GObject *object)
{
	MidgardQuerySelect *self = MIDGARD_QUERY_SELECT (object);

	parent_class->finalize;
}

static void 
_midgard_query_select_class_init (MidgardQuerySelectClass *klass, gpointer class_data)
{
       	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);

	object_class->constructor = _midgard_query_select_constructor;
	object_class->dispose = _midgard_query_select_dispose;
	object_class->finalize = _midgard_query_select_finalize;

	klass->set_constraint = _midgard_query_select_set_constraint;
	klass->set_limit = _midgard_query_select_set_limit;
	klass->set_offset = _midgard_query_select_set_offset;
	klass->add_order = _midgard_query_select_add_order;
	klass->add_join = _midgard_query_select_add_join;
	klass->execute = _midgard_query_select_execute;
}

GType
midgard_query_select_get_type (void)
{
   	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardQuerySelectClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_query_select_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardQuerySelect),
			0,              /* n_preallocs */
			NULL /* instance_init */
		};
		type = g_type_register_static (MIDGARD_TYPE_QUERY_EXECUTOR, "MidgardQuerySelect", &info, 0);
	}
	return type;
}
