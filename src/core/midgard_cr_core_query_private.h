/* 
 * Copyright (C) 2006, 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_CR_CORE_QUERY_H
#define MIDGARD_CR_CORE_QUERY_H

#include <libgda/libgda.h>
#include "midgardcr.h"
#include "midgard_cr_core_query_property.h"
#include "midgard_cr_core_query_storage.h"
#include "midgard_cr_core_query_constraint_simple.h"
#include "midgard_cr_core_query_executor.h"

typedef struct _MidgardCRCoreDBColumn MidgardCRCoreDBColumn;
typedef struct _MidgardCRCoreDBJoin MidgardCRCoreDBJoin;

struct _MidgardCRCoreDBColumn {
	const gchar *table_name;
	const gchar *column_name;
	const gchar *column_desc;
	const gchar *dbtype;
	GType gtype;
	gboolean index;
	gboolean unique;
	gboolean autoinc;
	gboolean primary;
	GValue *gvalue;
	const gchar *dvalue;
};

struct _MidgardCRCoreDBJoin {
	gchar *type;
	guint typeid;
	gchar *table;
	GObjectClass *klass;

	/* pointers references */
	//MgdSchemaPropertyAttr *left;
	//MgdSchemaPropertyAttr *right;
};

struct MidgardCRCoreCoreQueryOrder {
	MidgardCRCoreQueryProperty *property;
	const gchar *type;
};

struct _MidgardCRCoreQueryStoragePrivate {
	const gchar *classname;
	gchar *table_alias;
	gchar *table;
};

#define MQE_GET_STORAGE_TABLE(__manager, __storage) \
	__storage->priv->table ? \
	__storage->priv->table : \
	midgard_cr_storage_model_get_location ( \
			(MidgardCRStorageModel*) midgard_cr_core_query_find_table_model_by_name ( \
				MIDGARD_CR_SQL_STORAGE_MANAGER (__manager), \
				__storage->priv->classname) \
			)	

struct _MidgardCRCoreQueryPropertyPrivate {
	GValue value;
	GObjectClass *klass;
	MidgardCRCoreQueryStorage *storage;
};

struct _MidgardCRCoreQueryExecutorPrivate {
	MidgardCRStorageManager *storage_manager;
	MidgardCRCoreQueryStorage *storage;
	guint n_constraints;
	MidgardCRCoreQueryConstraintSimple *constraint;
	GSList *orders;
	GSList *joins;
	gint limit;
	gint offset;
	gpointer resultset;
	guint tableid;
	gchar *table_alias;
	guint joinid;
	GdaSqlStatement *stmt;
	guint results_count;
	gboolean read_only; 
	gboolean include_deleted; /* whether to select deleted objects or not */
	GSList *include_deleted_targets; /* particular targets to select deleted objects from */
};

struct _MidgardCRCoreQueryConstraintSimplePrivate {
	void	(*add_conditions_to_statement)	(MidgardCRCoreQueryExecutor *executor, MidgardCRCoreQueryConstraintSimple *self, 
			GdaSqlStatement *stm, GdaSqlExpr *where_expr_node);
};

#define MQE_SET_TABLE_ALIAS(__executor, __storage) \
	if (__executor->priv->storage == __storage) {\
		g_free (__storage->priv->table_alias) ;\
		__storage->priv->table_alias = g_strdup_printf ("t%d", executor->priv->tableid); \
	} else { \
		if (!__storage->priv->table_alias) \
			__storage->priv->table_alias = g_strdup_printf ("t%d", ++executor->priv->tableid); \
	}

MidgardCRCoreDBJoin	*midgard_cr_core_core_dbjoin_new	(void);
void		midgard_cr_core_core_dbjoin_free	(MidgardCRCoreDBJoin *mdbj);

#define midgard_cr_core_data_model_get_value_at(__model,__col,__row) \
	gda_data_model_get_value_at((__model), (__col), (__row), NULL)

#define midgard_cr_core_data_model_get_value_at_col_name(__model,__col,__row) \
	midgard_cr_core_data_model_get_value_at (__model, gda_data_model_get_column_index (__model, __col), __row);

#define MIDGARD_GET_UINT_FROM_VALUE(__prop, __value) \
	if(G_VALUE_HOLDS_UINT(__value)) { \
		__prop = \
			g_value_get_uint(__value); \
	} \
	if(G_VALUE_HOLDS_INT(__value)) { \
		__prop = \
			(guint)g_value_get_int(__value); \
	} \
	if(G_VALUE_HOLDS_CHAR(__value)) { \
		__prop = \
			(guint)g_value_get_char(__value); \
	}

#define MIDGARD_GET_INT_FROM_VALUE(__prop, __value) \
	if(G_VALUE_HOLDS_UINT(__value)) { \
		__prop = \
			(gint)g_value_get_uint(__value); \
	} \
	if(G_VALUE_HOLDS_INT(__value)) { \
		__prop = \
			g_value_get_int(__value); \
	} \
	if(G_VALUE_HOLDS_CHAR(__value)) { \
		__prop = \
			(gint)g_value_get_char(__value); \
	}

#define MIDGARD_GET_BOOLEAN_FROM_VALUE(__prop, __value) \
	if(G_VALUE_HOLDS_UINT(__value)) { \
		guint __i = g_value_get_uint(__value); \
		if(__i == 1) \
			__prop = TRUE; \
		else if(__i == 0) \
			__prop = FALSE; \
		else \
			g_warning("MidgardCRCore failed to convert guint to gboolean"); \
	} \
	else if(G_VALUE_HOLDS_INT(__value)) { \
		gint __i = g_value_get_int(__value); \
		if(__i == 1) \
			__prop = TRUE; \
		else if(__i == 0) \
			__prop = FALSE; \
		else \
	 		g_warning("MidgardCRCore failed to convert gint to gboolean"); \
	} \
	else if(G_VALUE_HOLDS_STRING(__value)) { \
		gint __i = atoi(g_value_get_string(__value)); \
		if(__i == 0) \
			__prop = FALSE; \
		else if (__i != 0) \
			__prop = TRUE; \
		else \
	 		g_warning("MidgardCRCore failed to convert string to gboolean"); \
	} \
	else if(G_VALUE_HOLDS_CHAR(__value)) { \
		gchar __i = g_value_get_char(__value); \
		if(__i == 1) \
			__prop = TRUE; \
		else if(__i == 0) \
			__prop = FALSE; \
		else \
			g_warning("MidgardCRCore failed to convert gchar to gboolean"); \
	} \
	else { \
		g_warning("Can not convert %s to boolean", G_VALUE_TYPE_NAME(__value)); \
	}

MidgardCRCoreDBColumn *midgard_cr_core_core_dbcolumn_new(void);
gchar	*midgard_cr_core_query_compute_constraint_property (MidgardCRCoreQueryExecutor *executor, MidgardCRCoreQueryStorage *storage, const gchar *name);
MidgardCRSQLTableModel *midgard_cr_core_query_find_table_model_by_name (MidgardCRSQLStorageManager *manager, const gchar *class_name);
MidgardCRSQLTableModel *midgard_cr_core_query_find_object_model_by_name (MidgardCRSQLStorageManager *manager, const gchar *class_name);

#endif /* MIDGARD_CR_CORE_QUERY_H */
