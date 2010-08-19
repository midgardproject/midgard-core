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

void midgard_core_storage_sql_column_new (MgdCoreStorageSQLColumn *mdc, const gchar *tablename, const gchar *fieldname, GType fieldtype);

gint midgard_core_storage_sql_query_execute (GdaConnection *cnc, const gchar *query, gboolean ignore_error, GError **error);

GdaDataModel *midgard_core_storage_sql_get_model (GdaConnection *cnc, const gchar *query);

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

#endif /* MIDGARD_CORE_STORAGE_SQL_H */
