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

#include "midgard_core_storage_sql.h"
#include "midgard_type.h"
#include "midgard_timestamp.h"

#define COLTYPE_INT 	"int"
#define COLTYPE_DATE	"datetime"
#define COLTYPE_STRING	"varchar(255)"
#define COLTYPE_TEXT	"longtext"

#define PROVIDER_NAME_POSTGRES	"PostgreSQL"

/** 
 * Initialize StorageSQLColumn with default types 
 */ 
void 
midgard_core_storage_sql_column_init (MgdCoreStorageSQLColumn *mdc, const gchar *tablename, const gchar *colname, GType coltype)
{
	 mdc->table_name = NULL;
	 mdc->column_name = NULL;
	 mdc->column_desc = NULL;
	 mdc->dbtype = NULL;
	 mdc->gtype = 0;
	 mdc->index = FALSE;
	 mdc->unique = FALSE;
	 mdc->gvalue = NULL;
	 mdc->primary = FALSE;
	 mdc->dvalue = NULL;
	 mdc->autoinc = FALSE;

	if (tablename)
		mdc->table_name = tablename;

	if (colname)
		mdc->column_name = colname;

	if (coltype) {
		mdc->gtype = coltype;
		if (coltype == MGD_TYPE_TIMESTAMP) {
			mdc->dbtype = COLTYPE_DATE;
			mdc->dvalue = MIDGARD_TIMESTAMP_DEFAULT;
		} else if (coltype == MGD_TYPE_BOOLEAN
				|| coltype == MGD_TYPE_INT) {
			mdc->dbtype = COLTYPE_INT;
			mdc->dvalue = "0";
		} else if (coltype == MGD_TYPE_STRING) {
			mdc->dbtype = COLTYPE_STRING;
			mdc->dvalue = "";
		}		
	}

	return;
}

gboolean 
_update_gda_meta_store (GdaConnection *cnc, const gchar *tablename, GError **error)
{
	/* Update meta store */
	GError *err = NULL;
	GdaMetaStruct *mstruct;
	GdaMetaDbObject *dbo;
	GValue *table_value;
	GdaMetaStore *store = gda_connection_get_meta_store (cnc);

	if (!gda_connection_update_meta_store (cnc, NULL, &err)) {
		g_propagate_error (error, err);
		return FALSE;	
	}

	mstruct = gda_meta_struct_new (store, GDA_META_STRUCT_FEATURE_NONE);
	table_value = gda_value_new (G_TYPE_STRING);
	g_value_set_string (table_value, tablename);
	dbo = gda_meta_struct_complement (mstruct, GDA_META_DB_TABLE, NULL, NULL, table_value, &err);
	gda_value_free (table_value);
	g_object_unref (mstruct);

	if (!dbo) {
		g_propagate_error (error, err);
		return FALSE;
	}

	return TRUE;
}

/**
 * Checks whether table with given name exists. 
 */ 
gboolean 
midgard_core_storage_sql_table_exists (GdaConnection *cnc, const gchar *tablename)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (tablename != NULL, FALSE);

	GdaMetaContext mcontext = {"_tables", 1, NULL, NULL};
        mcontext.column_names = g_new (gchar *, 1);
        mcontext.column_names[0] = "table_name";
        mcontext.column_values = g_new (GValue *, 1);
        g_value_set_string ((mcontext.column_values[0] = gda_value_new (G_TYPE_STRING)), tablename);
	GError *error = NULL;
	
	if (!gda_connection_update_meta_store (cnc, &mcontext, &error)) {
		gda_value_free (mcontext.column_values[0]);
		g_warning("Failed to update meta data for table '%s': %s", tablename, 
				error && error->message ? error->message : "No detail");
		if (error)
			g_error_free(error);
		return TRUE;
	}

	GdaDataModel *dm_schema =
		gda_connection_get_meta_store_data (cnc,
						    GDA_CONNECTION_META_TABLES, NULL, 1,
                                                   "name", mcontext.column_values[0],
                                                   NULL);

	gda_value_free (mcontext.column_values[0]);
	g_free (mcontext.column_names);
	g_free (mcontext.column_values);

	if(!dm_schema) {
		g_error("Failed to retrieve tables schema");
		return TRUE;
	}

	gboolean retval = TRUE;
	if (gda_data_model_get_n_rows (dm_schema) == 0)
		retval = FALSE;
	g_object_unref (dm_schema);
	return retval;
}

/**
 * Remove table with given name
 */ 
gboolean 
midgard_core_storage_sql_table_remove (GdaConnection *cnc, const gchar *tablename, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (tablename != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GdaServerOperation *op;
	GdaServerProvider *server;
	
	server = gda_connection_get_provider (cnc);
	GError *err = NULL;
	op = gda_server_provider_create_operation (server, cnc,	GDA_SERVER_OPERATION_DROP_TABLE, NULL, &err);

	if(!op) {
		g_propagate_error (error, err);
		return FALSE;		
	}

	gda_server_operation_set_value_at (op, tablename, NULL, "/TABLE_DEF_P/TABLE_NAME");

	gboolean dropped = gda_server_provider_perform_operation (server, cnc, op, &err);
	if (!dropped) {
		g_propagate_error (error, err);
		return FALSE;
	}

	gboolean update_meta =  _update_gda_meta_store (cnc, tablename, &err);
	if (!update_meta)
		g_propagate_error (error, err);

	return update_meta;
}

/**
 * Create table with given name.
 */ 
gboolean 
midgard_core_storage_sql_table_create (GdaConnection *cnc, const gchar *tablename, const gchar *descr, const gchar *primary, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (tablename != NULL, FALSE);
	g_return_val_if_fail (descr != NULL, FALSE);
	g_return_val_if_fail (primary != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GdaServerProvider *server = gda_connection_get_provider (cnc);
	GError *err = NULL;
	GdaServerOperation *op = gda_server_provider_create_operation (server, cnc, GDA_SERVER_OPERATION_CREATE_TABLE, NULL, &err);
	const gchar *provider_name = gda_connection_get_provider_name (cnc);

	if(!op) {
		g_propagate_error (error, err);
		return FALSE;
	}

      	gda_server_operation_set_value_at (op, tablename, NULL, "/TABLE_DEF_P/TABLE_NAME");
	gda_server_operation_set_value_at (op, tablename, NULL, "/TABLE_DEF_P/TABLE_IFNOTEXISTS");

	/* Add primary field if defined */
	if (primary) {
		gda_server_operation_set_value_at (op, primary, NULL, "/FIELDS_A/@COLUMN_NAME/%d", 0);

		/* PostgreSQL requires 'SERIAL' identifier...  */
		if (g_str_equal (provider_name, PROVIDER_NAME_POSTGRES)) {
			gda_server_operation_set_value_at (op, "SERIAL", NULL, "/FIELDS_A/@COLUMN_TYPE/%d", 0);
		} else {
			gda_server_operation_set_value_at (op, "INTEGER", NULL, "/FIELDS_A/@COLUMN_TYPE/%d", 0);
		}

		gda_server_operation_set_value_at (op, "TRUE", NULL, "/FIELDS_A/@COLUMN_PKEY/%d", 0);
		gda_server_operation_set_value_at (op, "TRUE", NULL, "/FIELDS_A/@COLUMN_AUTOINC/%d", 0);
	}

	/* TODO, either debug, or propagate via profiler if possible 
	gchar *_sql = gda_server_provider_render_operation (server, cnc, op, NULL);
	g_debug ("Render create table: %s", _sql);
	*/

	gboolean created = gda_server_provider_perform_operation (server, cnc, op, &err);

	g_object_unref (op);

	if(!created) {
		g_propagate_error (error, err);
		return FALSE;
	}

	gboolean update_meta =  _update_gda_meta_store (cnc, tablename, &err);
	if (!update_meta)
		g_propagate_error (error, err);

	return update_meta;
}

/**
 * Check whether named column exists.
 */
gboolean 
midgard_core_storage_sql_column_exists (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (mdc != NULL, FALSE);

	GdaMetaContext mcontext = {"_columns", 2, NULL, NULL};
        mcontext.column_names = g_new (gchar *, 2);
        mcontext.column_names[0] = "table_name";
        mcontext.column_names[1] = "column_name";
        mcontext.column_values = g_new (GValue *, 2);
        g_value_set_string ((mcontext.column_values[0] = gda_value_new (G_TYPE_STRING)), mdc->table_name);
        g_value_set_string ((mcontext.column_values[1] = gda_value_new (G_TYPE_STRING)), mdc->column_name);
	GError *error = NULL;
	if (!gda_connection_update_meta_store (cnc, &mcontext, &error)) {
		gda_value_free (mcontext.column_values[0]);
		/* FIXME, there should be warning, but for some reason 
		 * either SQLite or gda-sqlite provider is buggy */	
		g_message ("Failed to update meta data for table '%s': %s", mdc->table_name, 
				error && error->message ? error->message : "No detail");
		if (error)
			g_error_free(error);

		return FALSE;
	}

	GdaDataModel *dm_schema =
		gda_connection_get_meta_store_data (cnc,
						    GDA_CONNECTION_META_FIELDS, NULL, 2,
						    "name", mcontext.column_values[0],
						    "field_name", mcontext.column_values[1]);
	gda_value_free (mcontext.column_values[0]);
	g_free (mcontext.column_names);
	g_free (mcontext.column_values);

	if(!dm_schema) {
		g_error("Failed to retrieve tables schema");
		return TRUE;
	}

	gboolean retval = TRUE;
	if (gda_data_model_get_n_rows (dm_schema) == 0)
		retval = FALSE;
	g_object_unref (dm_schema);
	return retval;

}

/**
 * Add new named column.
 */  
gboolean 
midgard_core_storage_sql_column_create (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (mdc->table_name != NULL, FALSE);
	g_return_val_if_fail (mdc->column_name != NULL, FALSE);

	if (!mdc->column_desc)
		mdc->column_desc = mdc->column_name;

	const gchar *column_name = mdc->column_name;
	const gchar *table_name = mdc->table_name;

	if (mdc->dbtype == NULL) {
		/* FIXME */
		g_warning ("NULL dbtype given. FIX error domain and code");
		return FALSE;
	}

	if (g_str_equal (mdc->dbtype, "longtext"))
		mdc->dbtype = "text";

	const gchar *column_type_str = mdc->dbtype;
	const gchar *provider_name = gda_connection_get_provider_name (cnc);
	if (g_str_equal (provider_name, PROVIDER_NAME_POSTGRES)
			&& (g_str_equal (mdc->dbtype, "datetime"))) {
		column_type_str = "timestamp";
	} 

	GError *err = NULL;
	GdaServerProvider *server = gda_connection_get_provider (cnc);
	GdaServerOperation *op = gda_server_provider_create_operation (server, cnc, GDA_SERVER_OPERATION_ADD_COLUMN, NULL, &err);
	
	if(!op) {
		g_propagate_error (error, err);		
		return FALSE;
	}
	
	gda_server_operation_set_value_at (op, table_name, NULL, "/COLUMN_DEF_P/TABLE_NAME");
	/* NOT NULL not implemented in PostgreSQL 8.1 ( 28.02.2007 ) */
	if (!g_str_equal (provider_name, PROVIDER_NAME_POSTGRES))
		gda_server_operation_set_value_at (op, "true", NULL, "/COLUMN_DEF_P/COLUMN_NNUL");

	gda_server_operation_set_value_at (op, column_name, NULL, "/COLUMN_DEF_P/COLUMN_NAME");
	gda_server_operation_set_value_at (op, column_type_str, NULL, "/COLUMN_DEF_P/COLUMN_TYPE");

	gchar *dval = NULL; 
	if(mdc->dvalue) {
		switch(mdc->gtype) {
			case MGD_TYPE_UINT:
			case MGD_TYPE_INT:
			case MGD_TYPE_FLOAT:
			case MGD_TYPE_BOOLEAN:
				dval = g_strdup (mdc->dvalue);
				break;
			default:
				dval  = g_strdup_printf ("'%s'", mdc->dvalue);
				break;
		}
	} 
	
	if (!mdc->dvalue)
		dval = g_strdup("\'\'");

	/* Default value */
	/* Default value not implemented in PostgreSQL 8.1 ( 28.02.2007 ) */
	if (!mdc->autoinc && g_str_equal (provider_name, PROVIDER_NAME_POSTGRES))
		gda_server_operation_set_value_at (op, dval, NULL, "/COLUMN_DEF_P/COLUMN_DEFAULT");
	g_free(dval);

	/* Primary key */
	if (mdc->primary)
		gda_server_operation_set_value_at (op, "true", NULL, "/COLUMN_DEF_P/COLUMN_PKEY");

	/* Auto increment key */
	if (mdc->autoinc) 
		gda_server_operation_set_value_at (op, "true", NULL, "/COLUMN_DEF_P/COLUMN_AUTOINC");

	/* Unique key */
	if (mdc->unique) 
		gda_server_operation_set_value_at(op, "true", NULL, "/COLUMN_DEF_P/COLUMN_UNIQUE");

	/* TODO, either print debug message or propagate it via profiler */
	/*	
	gchar *_sql = NULL;
	_sql = gda_server_provider_render_operation(server, cnc, op, &error);
	if(_sql) {
		g_debug("Rendering column add: %s", _sql);
		g_clear_error(&error);
	} else {
		g_warning("Can not prepare SQL query. %s", error->message);
		g_clear_error(&error);
		g_object_unref(op);
		return FALSE;
	}
	*/

	gboolean created = gda_server_provider_perform_operation (server, cnc, op, &err);
	g_object_unref (op);
	if(!created) {
		g_propagate_error (error, err);
		return FALSE;
	}

	return TRUE;
}

/**
 * Update named column 
 */ 
gboolean 
midgard_core_storage_sql_column_update (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (mdc != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	/* TODO */
	/* Create new column */
	/* INSERT while SELECT */
	/* Drop old column */
}

/**
 * Remove named column 
 */ 
gboolean 
midgard_core_storage_sql_column_remove (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (mdc != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GdaServerOperation *op;
	GdaServerProvider *server;
	
	server = gda_connection_get_provider (cnc);
	GError *err = NULL;
	op = gda_server_provider_create_operation (server, cnc,	GDA_SERVER_OPERATION_DROP_COLUMN, NULL, &err);

	if(!op) {
		g_propagate_error (error, err);
		return FALSE;		
	}

	const gchar *tablename = mdc->table_name;
	const gchar *colname = mdc->column_name;

	gda_server_operation_set_value_at (op, tablename, NULL, "/COLUMN_DEF_P/TABLE_NAME");
	gda_server_operation_set_value_at (op, colname, NULL, "/COLUMN_DEF_P/COLUMN_NAME");

	gboolean dropped = gda_server_provider_perform_operation (server, cnc, op, &err);
	if (!dropped) {
		g_propagate_error (error, err);
		return FALSE;
	}

	gboolean update_meta =  _update_gda_meta_store (cnc, tablename, &err);
	if (!update_meta)
		g_propagate_error (error, err);

	return update_meta;
}

gchar *
_get_index_name (MgdCoreStorageSQLColumn *mdc)
{
	g_return_val_if_fail (mdc->table_name != NULL, NULL);
	g_return_val_if_fail (mdc->column_name != NULL, NULL);

	return g_strconcat(mdc->table_name, "_", mdc->column_name, "_idx", NULL);
}

/**
 * Check whether named index exists
 */ 
gboolean 
midgard_core_storage_sql_index_exists (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc)
{
	g_return_val_if_fail (cnc != NULL, FALSE);

	GdaMetaStore *store = gda_connection_get_meta_store (cnc);
	GdaServerProvider *provider = gda_connection_get_provider (cnc);
	const gchar *sql = "SELECT index_name FROM _table_indexes WHERE table_schema=##schema::string AND table_name=##tname::string";
	GObjectClass *g_class = G_OBJECT_GET_CLASS (provider);
	GDA_SERVER_PROVIDER_CLASS (g_class);
	const gchar *dbname = GDA_SERVER_PROVIDER_CLASS (g_class)->get_database (provider, cnc);
	gboolean index_exists = FALSE;

	/* Database name - table_schema */
	GValue tsval = {0, };
	g_value_init (&tsval, G_TYPE_STRING);
	g_value_set_string (&tsval, dbname);
	
	/* Table name - table_name */
	GValue tnval = {0, };
	g_value_init (&tnval, G_TYPE_STRING);
	g_value_set_string (&tnval, mdc->table_name);
	
	GdaDataModel *data_model = gda_meta_store_extract (store, sql, NULL, "schema", &tsval, "tname", &tnval, NULL);
	g_value_unset (&tsval);
	g_value_unset (&tnval);

	if (!data_model) 
		return index_exists;

	gint rows = gda_data_model_get_n_rows (data_model);
	gint i = 0;
	if (rows == -1) {
		/* FIXME g_warning ("Invalid number of rows for given indexes data model"); */
		g_object_unref (data_model);
		return index_exists;
	}

	gchar *index_name = _get_index_name (mdc);

	for (i = 0; i < rows; i++) {
		const GValue *inval = gda_data_model_get_value_at (data_model, 0, i, NULL);
		if (g_str_equal (index_name, g_value_get_string (inval)))
			index_exists = TRUE;
   	}

	g_free (index_name);
        g_object_unref (data_model);

	return index_exists;
}

/**
 * Create named index 
 */ 
gboolean 
midgard_core_storage_sql_index_create (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (mdc->table_name != NULL, FALSE);
	g_return_val_if_fail (mdc->column_name != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	if(!mdc->index) 
		return TRUE;

	if(!mdc->column_desc)
		mdc->column_desc = mdc->column_name;

	const gchar *column_name = mdc->column_name;
	const gchar *table_name = mdc->table_name;

	GError *err = NULL;
	GdaServerProvider *server = gda_connection_get_provider(cnc);
	GdaServerOperation *op = gda_server_provider_create_operation (server, cnc, GDA_SERVER_OPERATION_CREATE_INDEX, NULL, &err);
	
	if(!op) {
		g_propagate_error (error, err);
		return FALSE;
	}
	
	gchar *index_name = _get_index_name (mdc);
	gda_server_operation_set_value_at (op, index_name, NULL, "/INDEX_DEF_P/INDEX_NAME");
	g_free(index_name);

	gda_server_operation_set_value_at (op, table_name, NULL, "/INDEX_DEF_P/INDEX_ON_TABLE");
	gda_server_operation_set_value_at (op, column_name, NULL, "/INDEX_FIELDS_S/0/INDEX_FIELD");
	gda_server_operation_set_value_at (op, " ", NULL, "/INDEX_DEF_P/INDEX_TYPE");

	gboolean created = gda_server_provider_perform_operation (server, cnc, op, &err);

	g_object_unref (op);

	if (!created) {
		g_propagate_error (error, err);
		return FALSE;
	}

	return TRUE;
}

gboolean 
midgard_core_storage_sql_index_remove (GdaConnection *cnc, MgdCoreStorageSQLColumn *mdc, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (mdc->table_name != NULL, FALSE);
	g_return_val_if_fail (mdc->column_name != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	if(!mdc->index) 
		return TRUE;

	if(!mdc->column_desc)
		mdc->column_desc = mdc->column_name;

	const gchar *column_name = mdc->column_name;
	const gchar *table_name = mdc->table_name;

	GError *err = NULL;
	GdaServerProvider *server = gda_connection_get_provider(cnc);
	GdaServerOperation *op = gda_server_provider_create_operation (server, cnc, GDA_SERVER_OPERATION_DROP_INDEX, NULL, &err);
	
	if(!op) {
		g_propagate_error (error, err);
		return FALSE;
	}
	
	gchar *index_name = _get_index_name (mdc);
	gda_server_operation_set_value_at (op, index_name, NULL, "/INDEX_DEF_P/INDEX_NAME");
	g_free(index_name);

	gda_server_operation_set_value_at (op, table_name, NULL, "/INDEX_DEF_P/INDEX_ON_TABLE");
	gda_server_operation_set_value_at (op, column_name, NULL, "/INDEX_FIELDS_S/0/INDEX_FIELD");
	gda_server_operation_set_value_at (op, " ", NULL, "/INDEX_DEF_P/INDEX_TYPE");

	gboolean created = gda_server_provider_perform_operation (server, cnc, op, &err);

	g_object_unref (op);

	if (!created) {
		g_propagate_error (error, err);
		return FALSE;
	}

	return TRUE;
}

gboolean 
midgard_core_storage_sql_create_base_tables (GdaConnection *cnc, GError **error)
{

}

gint 
midgard_core_storage_sql_query_execute (GdaConnection *cnc, const gchar *query, gboolean ignore_error, GError **error)
{

}

GdaDataModel *
midgard_core_storage_sql_get_model (GdaConnection *cnc, const gchar *query)
{

}
