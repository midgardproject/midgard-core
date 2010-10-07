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
 *   */

#ifndef MIDGARD_CORE_STORAGE_SQL_H
#define MIDGARD_CORE_STORAGE_SQL_H

#include <libgda/libgda.h>
#include "midgard3.h"

#define TABLE_NAME_MAPPER "midgard_mapper_type"
#define TABLE_NAME_MAPPER_PROPERTIES "midgard_mapper_columns"
#define TABLE_MAPPER_COLUMNS "class_name, table_name, description"
#define TABLE_MAPPER_PROPERTIES_COLUMNS "property_name, column_name, table_name, gtype_name, column_type, is_primary, has_index, is_unique, is_auto_increment, description"
#define TABLE_NAME_SCHEMA "midgard_schema_type"
#define TABLE_NAME_SCHEMA_PROPERTIES "midgard_schema_type_properties"
#define TABLE_SCHEMA_COLUMNS "class_name, extends"
#define TABLE_SCHEMA_PROPERTIES_COLUMNS "class_name, property_name, gtype_name, default_value_string, property_nick, description"

typedef struct _MgdCoreStorageSQLColumn MgdCoreStorageSQLColumn;

struct _MgdCoreStorageSQLColumn {
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

void midgard_core_storage_sql_column_init (MgdCoreStorageSQLColumn *mdc, const gchar *tablename, const gchar *fieldname, GType fieldtype);
void midgard_core_storage_sql_column_reset (MgdCoreStorageSQLColumn *mdc);

gint midgard_core_storage_sql_query_execute (GdaConnection *cnc, GdaSqlParser *parser, const gchar *query, GError **error);

GdaDataModel *midgard_core_storage_sql_get_model (GdaConnection *cnc, GdaSqlParser *parser, const gchar *query, GError **error);

gboolean midgard_core_storage_sql_table_exists (GdaConnection *cnc, const gchar *tablename);
gboolean midgard_core_storage_sql_table_remove (GdaConnection *cnc, const gchar *tablename, GError **error);
gboolean midgard_core_storage_sql_table_create (GdaConnection *cnc, const gchar *tablename, const gchar *descr, const gchar *primary, GError **error); 
gboolean midgard_core_storage_sql_column_exists (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc);
gboolean midgard_core_storage_sql_column_create (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error);
gboolean midgard_core_storage_sql_column_update (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error);
gboolean midgard_core_storage_sql_column_remove (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error);

gboolean midgard_core_storage_sql_index_exists (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc);
gboolean midgard_core_storage_sql_index_create (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error);
gboolean midgard_core_storage_sql_index_remove (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error);

gboolean midgard_core_storage_sql_create_base_tables (GdaConnection *cnc, GError **error);

/* INSERT */
gchar *midgard_cr_core_storage_sql_create_query_insert_columns (GObject *object, MidgardCRSchemaModel *schema, MidgardCRStorageModel *storage);
gchar *midgard_cr_core_storage_sql_create_query_insert_values (GObject *object, MidgardCRSchemaModel *schema, MidgardCRStorageModel *storage);
gchar *midgard_cr_core_storage_sql_create_query_insert (GObject *object, MidgardCRSchemaModel *schema, MidgardCRStorageModel *storage);
/* UPDATE */
gchar *midgard_cr_core_storage_sql_create_query_update (GObject *object, MidgardCRSchemaModel *schema, MidgardCRStorageModel *storage);
/* DELETE */

#endif /* MIDGARD_CORE_STORAGE_SQL_H */
