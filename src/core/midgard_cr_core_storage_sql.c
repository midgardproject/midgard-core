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

#include "midgard_cr_core_storage_sql.h"
#include "midgard_cr_core_type.h"
#include "midgard_cr_core_timestamp.h"
#include <sql-parser/gda-sql-parser.h>

#define COLTYPE_INT 	"int"
#define COLTYPE_DATE	"datetime"
#define COLTYPE_STRING	"varchar(255)"
#define COLTYPE_TEXT	"longtext"
#define COLTYPE_FLOAT	"float"

#define PROVIDER_NAME_POSTGRES	"PostgreSQL"

/** 
 * Initialize StorageSQLColumn with default types 
 */ 
void 
midgard_core_storage_sql_column_init (MgdCoreStorageSQLColumn *mdc, const gchar *tablename, const gchar *colname, GType coltype)
{
	midgard_core_storage_sql_column_reset (mdc);	

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
		} else if (coltype == MGD_TYPE_STRING
				|| coltype == G_TYPE_STRING) {
			mdc->dbtype = COLTYPE_STRING;
			mdc->dvalue = "";
		} else if (coltype == MGD_TYPE_LONGTEXT) {
			mdc->dbtype = COLTYPE_TEXT;
			mdc->dvalue = "";
		} else if (coltype == G_TYPE_FLOAT) {
			mdc->dbtype =  COLTYPE_FLOAT;
			mdc->dvalue = "0.00";
		}			
	}

	return;
}

/**
 * Resets column structure.
 */ 
void 
midgard_core_storage_sql_column_reset (MgdCoreStorageSQLColumn *mdc)
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

	gboolean update_meta = _update_gda_meta_store (cnc, tablename, &err);
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
		g_warning ("%s.%s NULL dbtype given. FIX error domain and code", mdc->table_name, mdc->column_name);
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
	if (!mdc->autoinc && !g_str_equal (provider_name, PROVIDER_NAME_POSTGRES))
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
		
	gchar *_sql = NULL;
	GError *_err = NULL;
	_sql = gda_server_provider_render_operation(server, cnc, op, &_err);
	if(_sql) {
		g_debug("Rendering column add: %s", _sql);
		g_clear_error(&_err);
	} else {
		g_warning("Can not prepare SQL query. %s", _err->message);
		g_clear_error(&_err);
		g_object_unref(op);
		return FALSE;
	}
		
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

/* SCHEMA AND MAPPER TABLES */

#define COLUMN_PK_DEFAULT "id"

#define TABLE_NAME_SCHEMA_DESCRIPTION "Stores names of all GObject derived classes registered with Midgard Content Repository"
#define TABLE_NAME_SCHEMA_PROPERTIES "midgard_schema_type_properties"
#define TABLE_NAME_SCHEMA_PROPERTIES_DESCRIPTION "Stores names of all properties registered for Midgard Content Repository classes"

gboolean 
midgard_core_storage_sql_create_schema_tables (GdaConnection *cnc, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GError *err = NULL;
	const gchar *tablename = TABLE_NAME_SCHEMA;

	/* Check if table exists, and silently return TRUE */
	if (midgard_core_storage_sql_table_exists (cnc, tablename))
		return TRUE;

	/* Create table to store class info */
	if (!midgard_core_storage_sql_table_create (cnc, tablename, TABLE_NAME_SCHEMA_DESCRIPTION, COLUMN_PK_DEFAULT, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	/* Create columns */
	MgdCoreStorageSQLColumn mdc = { NULL, NULL };
	const gchar *column_name = "class_name";

	/* CLASS NAME */
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);
	mdc.unique = TRUE;

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* EXTENDS */
	column_name = "extends";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_INT);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* Create table to store properties info */
	tablename = TABLE_NAME_SCHEMA_PROPERTIES;

	/* Check if table exists, and silently return TRUE */
	if (midgard_core_storage_sql_table_exists (cnc, tablename))
		return TRUE;

	if (!midgard_core_storage_sql_table_create (cnc, tablename, TABLE_NAME_SCHEMA_PROPERTIES_DESCRIPTION, COLUMN_PK_DEFAULT, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	/* Create columns */

	/* CLASS */
	column_name = "class_name";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* NAME */
	column_name = "property_name";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* GTYPE NAME */
	column_name = "gtype_name";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* DEFAULT_VALUE STRING */
	column_name = "default_value_string";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* DEFAULT_VALUE INT */
	column_name = "default_value_integer";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_INT);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* DEFAULT_VALUE TIME */
	column_name = "default_value_time";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, MGD_TYPE_TIMESTAMP);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* DEFAULT_VALUE FLOAT */
	column_name = "default_value_float";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_FLOAT);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* PROPERTY NICK */
	column_name = "property_nick";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* DESCRIPTION */
	column_name = "description";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, MGD_TYPE_LONGTEXT);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	return TRUE;
}

#define TABLE_NAME_MAPPER_DESCRIPTION "Stores names of all tables used as storage for Midgard Content Repository classes"
#define TABLE_NAME_MAPPER_PROPERTIES "midgard_mapper_columns"
#define TABLE_NAME_MAPPER_PROPERTIES_DESCRIPTION "Stores names of all columns used as storage for Midgard Content Repository objects"

gboolean 
midgard_core_storage_sql_create_mapper_tables (GdaConnection *cnc, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GError *err = NULL;
	const gchar *tablename = TABLE_NAME_MAPPER;

	/* Check if table exists, and silently return TRUE */
	if (midgard_core_storage_sql_table_exists (cnc, tablename))
		return TRUE;

	/* Create table */
	if (!midgard_core_storage_sql_table_create (cnc, tablename, TABLE_NAME_MAPPER_DESCRIPTION, COLUMN_PK_DEFAULT, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	/* Create columns */
	MgdCoreStorageSQLColumn mdc = { NULL, NULL };
	const gchar *column_name = "class_name";

	/* CLASS NAME */
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);
	mdc.unique = TRUE;

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* TABLE NAME */
	column_name = "table_name";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* DESCRIPTION */
	column_name = "description";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, MGD_TYPE_LONGTEXT);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* Create table to store columns info */

	tablename = TABLE_NAME_MAPPER_PROPERTIES;

	/* Check if table exists, and silently return TRUE */
	if (midgard_core_storage_sql_table_exists (cnc, tablename))
		return TRUE;

	if (!midgard_core_storage_sql_table_create (cnc, tablename, TABLE_NAME_MAPPER_PROPERTIES_DESCRIPTION, COLUMN_PK_DEFAULT, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	/* Create columns */

	/* PROPERTY NAME */
	column_name = "property_name";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	/* COLUMN NAME */
	column_name = "column_name";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* TABLE */
	column_name = "table_name";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_INT);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* GTYPE NAME */
	column_name = "gtype_name";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* COLUMN TYPE */
	column_name = "column_type";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_STRING);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* IS PRIMARY */
	column_name = "is_primary";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_BOOLEAN);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* HAS INDEX */
	column_name = "has_index";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_BOOLEAN);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* IS UNIQUE */
	column_name = "is_unique";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_BOOLEAN);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* IS AUTO INCREMENT */
	column_name = "is_auto_increment";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, G_TYPE_BOOLEAN);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	/* DESCRIPTION */
	column_name = "description";
	midgard_core_storage_sql_column_init (&mdc, tablename, column_name, MGD_TYPE_LONGTEXT);

	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	midgard_core_storage_sql_column_reset (&mdc);

	return TRUE;
}

/**
 * Initializes all tables required by basic SQL storage manager
 */ 
gboolean 
midgard_core_storage_sql_create_base_tables (GdaConnection *cnc, GError **error)
{
	g_return_val_if_fail (cnc != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	GError *err = NULL;

	if (!midgard_core_storage_sql_create_mapper_tables (cnc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	if (!midgard_core_storage_sql_create_schema_tables (cnc, &err)) {
		g_propagate_error (error, err);
		return FALSE;
	}

	return TRUE;
}

gint 
midgard_core_storage_sql_query_execute (GdaConnection *cnc, GdaSqlParser *parser, const gchar *query, GError **error)
{
	g_return_val_if_fail (cnc != NULL, -1);
	g_return_val_if_fail (query != NULL, -1);
	g_return_val_if_fail (error == NULL || *error == NULL, -1);
	
	GdaStatement *stmt;
	GdaSqlParser *local_parser;
	GError *err = NULL;
	
	if (!parser)
		local_parser = gda_connection_create_parser (cnc);
	if (!local_parser)
		local_parser = gda_sql_parser_new ();

	stmt = gda_sql_parser_parse_string (parser, query, NULL, &err);
	if (err) {
		g_propagate_error (error, err);
		if (stmt)
			g_object_unref (stmt);
		return -1;
	}

	gint nr = gda_connection_statement_execute_non_select (cnc, stmt, NULL, NULL, &err);
	g_object_unref (stmt);

	if (err)
		g_propagate_error (error, err);

	return nr;
}

GdaDataModel *
midgard_core_storage_sql_get_model (GdaConnection *cnc, GdaSqlParser *parser, const gchar *query, GError **error)
{
	g_return_val_if_fail (cnc != NULL, NULL);
	g_return_val_if_fail (parser != NULL, NULL);

	GdaStatement *stmt = NULL;
	GdaDataModel *model = NULL;
	GError *err = NULL;
	
	stmt = gda_sql_parser_parse_string (parser, query, NULL, &err);
	if (stmt) {
		model = gda_connection_statement_execute_select (cnc, stmt, NULL, &err);
		g_object_unref (stmt);
	}

	if (err) {
		g_propagate_error (error, err);
		if (model) g_object_unref (model);
		return NULL;
	}

	return model;
}

/**
 * Generates part of INSERT SQL query including column names.
 * Returned string doesn't contain coma at end.
 * For example: col1, col2, col3.
 */ 
gchar *
midgard_cr_core_storage_sql_create_query_insert_columns (GObject *object, MidgardCRSchemaModel *schema, MidgardCRStorageModel *storage)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (schema != NULL, NULL);
	g_return_val_if_fail (storage != NULL, NULL);

	MidgardCRModel *_schema = MIDGARD_CR_MODEL (schema);
	MidgardCRModel *_storage = MIDGARD_CR_MODEL (storage);

	const gchar *objectname = G_OBJECT_TYPE_NAME (object);
	const gchar *schemaname = midgard_cr_model_get_name (_schema);
	const gchar *storagename = midgard_cr_model_get_name (_storage);

	if (!g_str_equal (objectname, schemaname)) {
		g_warning ("Can not generate valid SQL query for %s. SchemaModel of '%s' given", objectname, schemaname);
		return NULL;
	}

	if (!g_str_equal (objectname, storagename)) {
		g_warning ("Can not generate valid SQL query for %s. StorageModel of '%s' given", objectname, storagename);
		return NULL;
	}

	/* list properties of the class */
	guint n_props;	
	guint i;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_GET_CLASS (object), &n_props);

	if (!pspecs) {
		g_warning ("Can not generate valid SQL query for %s. No single property installed", objectname);
		return NULL;
	}
	
	/* Generate INSERT query columns' part */
	GString *query = g_string_new ("");
	gboolean add_coma = FALSE;

	for (i = 0; i < n_props; i++) {
		const gchar *pname = pspecs[i]->name;
		MidgardCRModel *smodel = midgard_cr_model_get_model_by_name (_schema, pname);
		/* No SchemaModel for given property, ignore */
		if (!smodel)
			continue;
		smodel = midgard_cr_model_get_model_by_name (_storage, pname);
		/* No StorageModel for given property, ignore */
		if (!smodel)
			continue;
		/* Schema and Storage models found, add property's column to query */
		g_string_append_printf (query, "%s%s", 
				add_coma ? ", " : "", midgard_cr_storage_model_property_get_location (MIDGARD_CR_SQL_STORAGE_MODEL_PROPERTY (smodel)));
		add_coma = TRUE;
	}

	g_free (pspecs);
	return g_string_free (query, FALSE);
}

/**
 * Generates part of INSERT SQL query used for VALUES.
 * Returned strinf dosn't containt brackets and coma at end.
 * For example: 'string_value', 123, 'Foo'.
 */  
gchar *
midgard_cr_core_storage_sql_create_query_insert_values (GObject *object, MidgardCRSchemaModel *schema, MidgardCRStorageModel *storage)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (schema != NULL, NULL);
	g_return_val_if_fail (storage != NULL, NULL);

	MidgardCRModel *_schema = MIDGARD_CR_MODEL (schema);
	MidgardCRModel *_storage = MIDGARD_CR_MODEL (storage);

	const gchar *objectname = G_OBJECT_TYPE_NAME (object);
	const gchar *schemaname = midgard_cr_model_get_name (_schema);
	const gchar *storagename = midgard_cr_model_get_name (_storage);

	if (!g_str_equal (objectname, schemaname)) {
		g_warning ("Can not generate valid SQL query for %s. SchemaModel of '%s' given", objectname, schemaname);
		return NULL;
	}

	if (!g_str_equal (objectname, storagename)) {
		g_warning ("Can not generate valid SQL query for %s. StorageModel of '%s' given", objectname, storagename);
		return NULL;
	}

	/* list properties of the class */
	guint n_props;	
	guint i;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_GET_CLASS (object), &n_props);

	if (!pspecs) {
		g_warning ("Can not generate valid SQL query for %s. No single property installed", objectname);
		return NULL;
	}
	
	/* Generate INSERT query VALUES part */
	GString *query = g_string_new ("");
	gboolean add_coma = FALSE;

	for (i = 0; i < n_props; i++) {
		const gchar *pname = pspecs[i]->name;
		MidgardCRModel *smodel = midgard_cr_model_get_model_by_name (_schema, pname);
		/* No SchemaModel for given property, ignore */
		if (!smodel)
			continue;
		smodel = midgard_cr_model_get_model_by_name (_storage, pname);
		/* No StorageModel for given property, ignore */
		if (!smodel)
			continue;
		/* Schema and Storage models found, add property's value to query */
		GValue val = {0, };
		g_value_init (&val, pspecs[i]->value_type);
		g_object_get_property (object, pname, &val);
		switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (&val))) {
			case G_TYPE_STRING:
				g_string_append_printf (query, "%s'%s'", add_coma ? ", " : "", g_value_get_string (&val));
				break;
			case G_TYPE_UINT:
				g_string_append_printf (query, "%s%d", add_coma ? ", " : "", g_value_get_uint (&val));
				break;
			case G_TYPE_INT:
				g_string_append_printf (query, "%s%d", add_coma ? ", " : "", g_value_get_int (&val));
				break;
			case G_TYPE_FLOAT:
				g_string_append_printf (query, "%s%g", add_coma ? ", " : "", g_value_get_float (&val));
				break;
			case G_TYPE_BOOLEAN:
				g_string_append_printf (query, "%s%d", add_coma ? ", " : "", g_value_get_boolean (&val));
				break;
			case G_TYPE_OBJECT:
				g_warning ("%s: Please fix it! Including Object in query is not yet implemented!", __PRETTY_FUNCTION__);
				break;
			case G_TYPE_BOXED:
				g_warning ("%s: Do you think I can include Boxed type in SQL Query? Fix it!", __PRETTY_FUNCTION__);
				break;
			default:
				g_warning ("%s: Houston, we have a problem. Default case reached and no single action can be taken.", __PRETTY_FUNCTION__);
				break;
		}
		add_coma = TRUE;
	}

	g_free (pspecs);
	return g_string_free (query, FALSE);
}

/**
 * Generates part of UPDATE SQL query including column names and values.
 * Returned strinf dosn't containt coma at end and.
 * For example: col1='string_value', col2=123, col3='Foo'.
 */  
gchar *
midgard_cr_core_storage_sql_create_query_update (GObject *object, MidgardCRSchemaModel *schema, MidgardCRStorageModel *storage)
{
	g_return_val_if_fail (object != NULL, NULL);
	g_return_val_if_fail (schema != NULL, NULL);
	g_return_val_if_fail (storage != NULL, NULL);

	MidgardCRModel *_schema = MIDGARD_CR_MODEL (schema);
	MidgardCRModel *_storage = MIDGARD_CR_MODEL (storage);

	const gchar *objectname = G_OBJECT_TYPE_NAME (object);
	const gchar *schemaname = midgard_cr_model_get_name (_schema);
	const gchar *storagename = midgard_cr_model_get_name (_storage);

	if (!g_str_equal (objectname, schemaname)) {
		g_warning ("Can not generate valid SQL query for %s. SchemaModel of '%s' given", objectname, schemaname);
		return NULL;
	}

	if (!g_str_equal (objectname, storagename)) {
		g_warning ("Can not generate valid SQL query for %s. StorageModel of '%s' given", objectname, storagename);
		return NULL;
	}

	/* list properties of the class */
	guint n_props;	
	guint i;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_GET_CLASS (object), &n_props);

	if (!pspecs) {
		g_warning ("Can not generate valid SQL query for %s. No single property installed", objectname);
		return NULL;
	}
	
	/* Generate INSERT query VALUES part */
	GString *query = g_string_new ("");
	gboolean add_coma = FALSE;

	for (i = 0; i < n_props; i++) {
		const gchar *pname = pspecs[i]->name;
		MidgardCRModel *smodel = midgard_cr_model_get_model_by_name (_schema, pname);
		/* No SchemaModel for given property, ignore */
		if (!smodel)
			continue;
		smodel = midgard_cr_model_get_model_by_name (_storage, pname);
		/* No StorageModel for given property, ignore */
		if (!smodel)
			continue;

		/* Schema and Storage models found, add property's name and value to query */
		g_string_append_printf (query, "%s%s=", 
				add_coma ? ", " : "", midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (smodel)));

		GValue val = {0, };
		g_value_init (&val, pspecs[i]->value_type);
		g_object_get_property (object, pname, &val);
		switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (&val))) {
			case G_TYPE_STRING:
				g_string_append_printf (query, "'%s'", g_value_get_string (&val));
				break;
			case G_TYPE_UINT:
				g_string_append_printf (query, "%d", g_value_get_uint (&val));
				break;
			case G_TYPE_INT:
				g_string_append_printf (query, "%d", g_value_get_int (&val));
				break;
			case G_TYPE_FLOAT:
				g_string_append_printf (query, "%g", g_value_get_float (&val));
				break;
			case G_TYPE_BOOLEAN:
				g_string_append_printf (query, "%d", g_value_get_boolean (&val));
				break;
			case G_TYPE_OBJECT:
				g_warning ("%s: Please fix it! Including Object in query is not yet implemented!", __PRETTY_FUNCTION__);
				break;
			case G_TYPE_BOXED:
				g_warning ("%s: Do you think I can include Boxed type in SQL Query? Fix it!", __PRETTY_FUNCTION__);
				break;
			default:
				g_warning ("%s: Houston, we have a problem. Default case reached and no single action can be taken.", __PRETTY_FUNCTION__);
				break;
		}
		add_coma = TRUE;
	}

	g_free (pspecs);
	return g_string_free (query, FALSE);
}
