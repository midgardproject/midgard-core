/* 
 * Copyright (C) 2006 Piotr Pokora <piotrek.pokora@gmail.com>
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

#ifndef MIDGARD_CORE_QUERY_H
#define MIDGARD_CORE_QUERY_H

#include "midgard_connection.h"
#include "midgard_dbobject.h"
#include <libgda/libgda.h>
#include "schema.h"
#include "midgard_query_property.h"
#include "midgard_query_storage.h"
#include "midgard_query_constraint_simple.h"
#include "midgard_query_executor.h"

typedef struct _MidgardDBColumn MidgardDBColumn;
typedef struct _MidgardDBJoin MidgardDBJoin;

struct _MidgardDBColumn {
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

struct _MidgardDBJoin {
	gchar *type;
	guint typeid;
	gchar *table;
	MidgardDBObjectClass *klass;
	gchar *left_table;
	gchar *left_field;
	MidgardDBObjectClass *left_klass;
	gchar *right_table;
	gchar *right_field;
	MidgardDBObjectClass *right_klass;
	gchar *left_tablefield;
	gchar *right_tablefield;

	/* pointers references */
	MgdSchemaPropertyAttr *left;
	MgdSchemaPropertyAttr *right;
};

struct MidgardCoreQueryOrder {
	MidgardQueryProperty *property;
	const gchar *type;
};

struct _MidgardQueryStoragePrivate {
	MidgardDBObjectClass *klass;
	gchar *table_alias;
	const gchar *table;
	const gchar *classname;
	gboolean is_valid;
};

struct _MidgardQueryPropertyPrivate {
	GValue value;
	MidgardDBObjectClass *klass;
	MidgardQueryStorage *storage;
	gboolean is_valid;
};

struct _MidgardQueryExecutorPrivate {
	MidgardConnection *mgd;
	MidgardQueryStorage *storage;
	guint n_constraints;
	MidgardQueryConstraintSimple *constraint;
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
	gboolean include_deleted;
	gboolean is_valid;
};

struct _MidgardQueryConstraintSimplePrivate {
	void	(*add_conditions_to_statement)	(MidgardQueryExecutor *executor, MidgardQueryConstraintSimple *self, 
			GdaSqlStatement *stm, GdaSqlExpr *where_expr_node, GError **error);
};

#define MQE_SET_TABLE_ALIAS(__executor, __storage) \
	if (__executor->priv->storage == __storage) {\
		g_free (__storage->priv->table_alias) ;\
		__storage->priv->table_alias = g_strdup_printf ("t%d", executor->priv->tableid); \
	} else { \
		if (!__storage->priv->table_alias) \
			__storage->priv->table_alias = g_strdup_printf ("t%d", ++executor->priv->tableid); \
	}

MidgardDBJoin	*midgard_core_dbjoin_new	(void);
void		midgard_core_dbjoin_free	(MidgardDBJoin *mdbj);

#ifdef HAVE_LIBGDA_4
#define midgard_data_model_get_value_at(__model,__col,__row) \
	gda_data_model_get_value_at((__model), (__col), (__row), NULL)
#else
#define midgard_data_model_get_value_at(__model,__col,__row) \
	gda_data_model_get_value_at((__model), (__col), (__row))
#endif

#ifdef HAVE_LIBGDA_4
#define midgard_data_model_get_value_at_col_name(__model,__col,__row) \
	midgard_data_model_get_value_at (__model, gda_data_model_get_column_index (__model, __col), __row);
#else
#define midgard_data_model_get_value_at_col_name(__model,__col,__row) \
	gda_data_model_get_value_at_col_name (__model, __col, __row)
#endif

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
	if (__value == NULL) { \
		g_warning ("Invalid value (NULL). Converting to default FALSE boolean value"); \
		__prop = FALSE; \
	} else if(G_VALUE_HOLDS_UINT(__value)) { \
		guint __i = g_value_get_uint(__value); \
		if(__i == 1) \
			__prop = TRUE; \
		else if(__i == 0) \
			__prop = FALSE; \
		else \
			g_warning("Midgard failed to convert guint to gboolean"); \
	} \
	else if(G_VALUE_HOLDS_INT(__value)) { \
		gint __i = g_value_get_int(__value); \
		if(__i == 1) \
			__prop = TRUE; \
		else if(__i == 0) \
			__prop = FALSE; \
		else \
	 		g_warning("Midgard failed to convert gint to gboolean"); \
	} \
	else if(G_VALUE_HOLDS_STRING(__value)) { \
		gint __i = atoi(g_value_get_string(__value)); \
		if(__i == 0) \
			__prop = FALSE; \
		else if (__i != 0) \
			__prop = TRUE; \
		else \
	 		g_warning("Midgard failed to convert string to gboolean"); \
	} \
	else if(G_VALUE_HOLDS_CHAR(__value)) { \
		gchar __i = g_value_get_char(__value); \
		if(__i == 1) \
			__prop = TRUE; \
		else if(__i == 0) \
			__prop = FALSE; \
		else \
			g_warning("Midgard failed to convert gchar to gboolean"); \
	} \
	else { \
		g_warning("Can not convert %s to boolean", G_VALUE_TYPE_NAME(__value)); \
	}

MidgardDBColumn *midgard_core_dbcolumn_new(void);

gchar *midgard_core_query_where_guid(
			const gchar *table,
			const gchar *guid);

gint midgard_core_query_execute(
			MidgardConnection *mgd, 
			const gchar *query,
			gboolean ignore_error);

GdaDataModel *midgard_core_query_get_model(
			MidgardConnection *mgd, 
			const gchar *query);

GValue *midgard_core_query_get_field_value(
			MidgardConnection *mgd,
			const gchar *field,
			const gchar *table, 
			const gchar *where);

gboolean midgard_core_query_get_object_value(
			MidgardDBObject *dbobject,
			const gchar *column,
			GValue *value);

guint midgard_core_query_get_id(
			MidgardConnection *mgd, 
			const gchar *table,
			const gchar *guid);

guint* midgard_core_query_get_tree_ids(
			MidgardConnection *mgd,
			MidgardObjectClass *klass,
			guint tid);

gint midgard_core_query_insert_records(
			MidgardConnection *mgd,
			const gchar *table, 
			GList *cols, 
			GList *values, guint query_type, const gchar *where);

gboolean midgard_core_query_update_object_fields(
			MidgardDBObject *object, 
			const gchar *propname, ...);

gboolean midgard_core_query_create_table(
			MidgardConnection *mgd,
			const gchar *descr, 
			const gchar *tablename, 
			const gchar *primary);

gboolean midgard_core_query_add_column(
			MidgardConnection *mgd,
			MidgardDBColumn *mdc);

gboolean midgard_core_query_add_index(
			MidgardConnection *mgd,
			MidgardDBColumn *mdc);

gboolean midgard_core_query_create_metadata_columns(
			MidgardConnection *mgd, 
			const gchar *tablename);

gboolean midgard_core_query_create_basic_db(
			MidgardConnection *mgd);

gboolean midgard_core_query_create_class_storage(
			MidgardConnection *mgd, 
			MidgardDBObjectClass *klass);

gboolean midgard_core_query_update_class_storage(
			MidgardConnection *mgd, 
			MidgardDBObjectClass *klass);

gboolean midgard_core_table_exists(
			MidgardConnection *mgd, 
			const gchar *tablename);

GdaDataModel 		*midgard_core_query_get_dbobject_model 		(MidgardConnection *mgd, MidgardDBObjectClass *klass, guint n_params, const GParameter *parameters);
gboolean		midgard_core_query_create_dbobject_record 	(MidgardDBObject *object);
gint			midgard_core_query_update_dbobject_record 	(MidgardDBObject *object, GError **error);
gchar                   *midgard_core_query_binary_stringify            (GValue *src_value); 
gchar 			*midgard_core_query_compute_constraint_property	(MidgardQueryExecutor *executor, MidgardQueryStorage *storage, const gchar *name, GError **error);
gchar 			*midgard_core_query_unescape_string (MidgardConnection *mgd, const gchar *str);
void			midgard_core_query_get_object (MidgardConnection *mgd, const gchar *classname, MidgardDBObject **object, GError **error, const gchar *property, ...);

#endif /* MIDGARD_CORE_QUERY_H */
