/* 
 * Copyright (C) 2006, 2007, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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
 *   */

#include "midgard_cr_core_query_private.h"
#include <sql-parser/gda-sql-parser.h>

#define _RESERVED_BLOB_NAME "attachment"
#define _RESERVED_BLOB_TABLE "blobs"
#define _RESERVED_PARAM_NAME "parameter"
#define _RESERVED_PARAM_TABLE "record_extension"
#define _RESERVED_METADATA_NAME "metadata"
#define _RESERVED_GUID_NAME "guid"

typedef struct {
	const GObjectClass *klass;
	const gchar *table;
	const gchar *table_alias;
	const gchar *target_table;
	const gchar *target_table_alias;
	const gchar *colname;
	const gchar *target_colname;
	MidgardCRCoreQueryExecutor *executor;
} Psh;

static void 
__add_join (Psh *holder) 
{
	MidgardCRCoreQueryExecutor *executor = MIDGARD_CR_CORE_QUERY_EXECUTOR (holder->executor);
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;
	GdaSqlSelectFrom *from = select->from;
	GdaSqlSelectJoin *join = gda_sql_select_join_new (GDA_SQL_ANY_PART (from));
	join->type = GDA_SQL_SELECT_JOIN_LEFT;

	GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (join));
        expr->value = gda_value_new (G_TYPE_STRING);
	g_value_take_string (expr->value, g_strdup_printf ("%s.%s = %s.%s", 
				holder->table_alias, holder->colname, holder->target_table_alias, holder->target_colname));
	join->expr = expr;
	join->position = ++executor->priv->joinid;

	gda_sql_select_from_take_new_join (from , join);

	GdaSqlSelectTarget *s_target = gda_sql_select_target_new (GDA_SQL_ANY_PART (from));
	s_target->table_name = g_strdup (holder->target_table);
	s_target->as = g_strdup (holder->target_table_alias);
	gda_sql_select_from_take_new_target (from, s_target);

	/* Set target expression */     
	GdaSqlExpr *texpr = gda_sql_expr_new (GDA_SQL_ANY_PART (s_target));
	GValue *tval = g_new0 (GValue, 1);
	g_value_init (tval, G_TYPE_STRING);
	g_value_set_string (tval, s_target->table_name);
	texpr->value = tval;
	s_target->expr = texpr;
}

void 
__exclude_deleted_constraint (MidgardCRCoreQueryExecutor *executor, const gchar *table_alias)
{
	GdaSqlStatement *sql_stm = executor->priv->stmt;
	GdaSqlStatementSelect *select = (GdaSqlStatementSelect *) sql_stm->contents;
	GdaSqlExpr *where = select->where_cond;
	GdaSqlOperation *operation = where->cond;

	GdaSqlExpr *expr = gda_sql_expr_new (GDA_SQL_ANY_PART (operation));
	expr->value = gda_value_new (G_TYPE_STRING);
	g_value_set_string (expr->value, "t1.metadata_deleted = 0");
	operation->operands = g_slist_append (operation->operands, expr);
}

gboolean 
__compute_reserved_property_constraint (Psh *holder, const gchar *token_1, const gchar *token_2)
{
	/* metadata */
	/*
	if (g_str_equal (_RESERVED_METADATA_NAME, token_1)) {
		MidgardCRCoreMetadataClass *mklass = g_type_class_peek (MIDGARD_TYPE_METADATA);
		const gchar *property_field = midgard_cr_core_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (mklass), token_2);
		if (!property_field)
			return FALSE;
		holder->colname = property_field;
		return TRUE;
	}*/

	/* parameter */
	/*if (g_str_equal (_RESERVED_PARAM_NAME, token_1)) {
		holder->klass = g_type_class_peek (g_type_from_name ("midgard_cr_core_parameter"));
		holder->target_colname = "parent_guid";
		holder->target_table = _RESERVED_PARAM_TABLE;
		holder->target_table_alias = g_strdup_printf ("t%d", ++holder->executor->priv->tableid);
		holder->colname = _RESERVED_GUID_NAME;
		__add_join (holder);
		holder->colname = midgard_cr_core_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (holder->klass), token_2);
		holder->table = holder->target_table;
		holder->table_alias = holder->target_table_alias;
		return TRUE;
	}*/

	/* attachment */
	/*if (g_str_equal (_RESERVED_BLOB_NAME, token_1)) {
		holder->klass = g_type_class_peek (g_type_from_name ("midgard_cr_core_attachment"));
		holder->target_colname = "parent_guid";
		holder->target_table = _RESERVED_BLOB_TABLE;
		holder->target_table_alias = g_strdup_printf ("t%d", ++holder->executor->priv->tableid);
		holder->colname = _RESERVED_GUID_NAME;
		__add_join (holder);
		holder->colname = midgard_cr_core_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (holder->klass), token_2);
		holder->table = holder->target_table;
		holder->table_alias = holder->target_table_alias;
		return TRUE;
	}*/

	/* fallback to default */
	/*MidgardCRCoreReflectionProperty *mrp = midgard_cr_core_reflection_property_new (MIDGARD_DBOBJECT_CLASS (holder->klass));
	holder->colname = midgard_cr_core_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (holder->klass), token_1);
	// Add implicit join if property is a link 
	if (midgard_cr_core_reflection_property_is_link(mrp, token_1)) {
		holder->klass = midgard_cr_core_reflection_property_get_link_class (mrp, token_1);
		const gchar *target_property = midgard_cr_core_reflection_property_get_link_target (mrp, token_1);
		holder->target_colname = midgard_cr_core_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (holder->klass), target_property);
		holder->target_table = midgard_cr_core_core_class_get_table (MIDGARD_DBOBJECT_CLASS (holder->klass));
		holder->target_table_alias = g_strdup_printf ("t%d", ++holder->executor->priv->tableid);
		__add_join (holder);
		holder->table = holder->target_table;
		holder->table_alias = holder->target_table_alias;
	}*/

	return TRUE;
}

MidgardCRSQLTableModel *
midgard_cr_core_query_find_table_model_by_name (MidgardCRSQLStorageManager *manager, const gchar *class_name)
{
	MidgardCRStorageModel** models = manager->_storage_models;
	guint i = 0;
	for (i = 0; models[i] != NULL; i++) {
		if (g_str_equal (class_name, midgard_cr_model_get_name (MIDGARD_CR_MODEL (models[i]))))
			return MIDGARD_CR_SQL_TABLE_MODEL (models[i]);
	}
	return NULL;
}

static MidgardCRModel *
__find_column_model_by_name (MidgardCRSQLStorageManager *manager, const gchar *class_name, const gchar *property_name)
{
	MidgardCRSQLTableModel *table_model = midgard_cr_core_query_find_table_model_by_name (manager, class_name);
	if (!table_model)
		return NULL;
	return midgard_cr_model_get_model_by_name (MIDGARD_CR_MODEL (table_model), property_name);
}

static const gchar *
__get_property_colname (MidgardCRSQLStorageManager *manager, const gchar *class_name, const gchar *property_name)
{
	MidgardCRModel *column_model = __find_column_model_by_name (manager, class_name, property_name);
	if (!column_model)
		return NULL;
	return midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (column_model));
}

gchar *
midgard_cr_core_query_compute_constraint_property (MidgardCRCoreQueryExecutor *executor,
		                MidgardCRCoreQueryStorage *storage, const gchar *name)
{
	g_return_val_if_fail (executor != NULL, FALSE);
	g_return_val_if_fail (name != NULL, FALSE);

	/* Set table alias if it's not set */
	if (storage)
		MQE_SET_TABLE_ALIAS (executor, storage);

	MidgardCRSQLStorageManager *manager = (MidgardCRSQLStorageManager*) executor->priv->storage_manager;
	GObjectClass *klass = g_type_class_peek (g_type_from_name (executor->priv->storage->priv->classname));
	MidgardCRSQLTableModel *table_model = midgard_cr_core_query_find_table_model_by_name (manager, executor->priv->storage->priv->classname);

	gchar *table_field = NULL;
	gchar *table_alias = executor->priv->table_alias;
	const gchar *table = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (table_model));
	if (storage) {
		//table = executor->priv->storage->priv->table;
		table_alias = storage->priv->table_alias;
		klass = g_type_class_peek (g_type_from_name (storage->priv->classname));	
	}

      	gchar **spltd = g_strsplit(name, ".", 0);
	guint i = 0;
	guint j = 0;

	/* We can support max 3 tokens */
	while(spltd[i] != NULL)
		i++;

	gchar *q_table = NULL;
	gchar *q_field = NULL;

	/* case: property */
	if (i == 1) {
		const gchar *property_field = 
			__get_property_colname (MIDGARD_CR_SQL_STORAGE_MANAGER (executor->priv->storage_manager), G_OBJECT_CLASS_NAME (klass), name);
		if (!property_field) {
			g_warning ("%s doesn't seem to be registered for %s", name, G_OBJECT_CLASS_NAME (klass));
			g_strfreev (spltd);
			return NULL;
		}
		q_table = gda_connection_quote_sql_identifier (MCQE_CNC (executor), table_alias);
		q_field = gda_connection_quote_sql_identifier (MCQE_CNC (executor), property_field);
		table_field = g_strdup_printf ("%s.%s", q_table, q_field);
	} else if (i < 4) {
		/* Set all pointers we need to generate valid tables' names, aliases or joins */
		Psh holder = {NULL, };
		holder.table = table;
		holder.table_alias = table_alias;
		holder.klass = klass;
		holder.executor = MIDGARD_CR_CORE_QUERY_EXECUTOR (executor);
		holder.colname = NULL;
		
		while (spltd[j] != NULL) {
			if (spltd[j+1] == NULL)
				break;
			/* Set all pointers we need to generate valid tables' names, aliases or joins */
			/* case: metadata.property, attachment.property, property.link, etc */	
			if (!__compute_reserved_property_constraint (&holder, spltd[j], spltd[j+1]))
				break;
			j++;
		}	
		
		if (holder.table_alias && holder.colname) {
			q_table = gda_connection_quote_sql_identifier (MCQE_CNC (executor), holder.table_alias);
			q_field = gda_connection_quote_sql_identifier (MCQE_CNC (executor), holder.colname);
			table_field = g_strdup_printf ("%s.%s", q_table, q_field);
		}

	} else {
		  g_warning("Failed to parse '%s'. At most 3 tokens allowed", name);
	}

	g_strfreev (spltd);
	g_free (q_table);
	g_free (q_field);

	return table_field;
}


