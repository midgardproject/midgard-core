/* MidgardCore SQLStorageManager routines
 *    
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

#include "midgard_cr_core_sqlstoragemanager.h"
#include "midgard_cr_core_config.h"
#include <libgda/libgda.h>
#include <sql-parser/gda-sql-parser.h>
#include "midgard_cr_core_storage_sql.h"

#define DEFAULT_DBNAME "midgard"
#define DEFAULT_DBUSER "midgard"
#define DEFAULT_DBPASS "midgard"
#define DEFAULT_DBNAME "midgard"
#define DEFAULT_DBTYPE "MYSQL"
#define DEFAULT_DBHOST "localhost"

static void 
cnc_add_part (GString *cnc, const gchar *name, const gchar *value, const gchar *def) 
{
	g_assert(cnc != NULL);
	g_assert(name != NULL);
	if (value == NULL)
		value = def;

	if (*value) {
		gchar *tmp;

		/* Add a separating semicolon if there already are
		 parameters before this one. */
		
		if (cnc->len > 0) {
			g_string_append_c(cnc, ';');
		}
		tmp = gda_rfc1738_encode (name);
		g_string_append(cnc, tmp);
		g_free (tmp);

		g_string_append_c(cnc, '=');
		
		tmp = gda_rfc1738_encode (value);
		g_string_append(cnc, tmp);
		g_free (tmp);
	}
}

static void
__list_all_schema_models (MidgardCRSQLStorageManager *self, GError **error)
{
	/* select all classes */
	GString *query = g_string_new ("SELECT ");
	g_string_append_printf (query, "%s, id FROM %s", TABLE_SCHEMA_COLUMNS, TABLE_NAME_SCHEMA);
	GError *err = NULL;
	
	GdaDataModel *model = midgard_core_storage_sql_get_model (GDA_CONNECTION (self->_cnc), (GdaSqlParser *)self->_parser, query->str, &err);
	g_string_free (query, TRUE);
	/* internal error, return and throw error */
	if (err) {
		g_propagate_error (error, err);
		return;
	}
	/* there's no single class defined, silently return */
	if (!model)
		return;

	guint rows = gda_data_model_get_n_rows (model);
	/* There's a model, but no single record selected, silently return */
	if (rows == 0) {
		g_object_unref(model);
		return;
	}

	MidgardCRSchemaModel **schema_models = g_new (MidgardCRSchemaModel *, rows + 1);

	const GValue *value;
	guint i;
	for (i = 0; i < rows; i++) {
		/* Initialize new SchemaModel for given class name */
		value = gda_data_model_get_typed_value_at (model, 0, i, G_TYPE_STRING, TRUE, NULL);
		MidgardCRSchemaModel *smodel = midgard_cr_schema_model_new (g_value_get_string (value));
		/* Set parent model if set */
		value = gda_data_model_get_typed_value_at (model, 1, i, G_TYPE_STRING, TRUE, NULL);
		if (value && (!g_str_equal (g_value_get_string (value), "SchemaObject"))) { /* FIXME, SchemaObject constant needed */
			MidgardCRSchemaModel *parent = midgard_cr_schema_model_new (g_value_get_string (value));
			midgard_cr_model_set_parent (MIDGARD_CR_MODEL (smodel), MIDGARD_CR_MODEL (parent));
		}
		/* set id */
		value = gda_data_model_get_typed_value_at (model, 2, i, G_TYPE_INT, TRUE, NULL);	
		smodel->_id = (guint) g_value_get_int (value);
		/* Add Schema model to array */
		schema_models[i] = smodel;
	}
	schema_models[i] = NULL; /* terminate with NULL */
	self->_schema_models = schema_models;
	self->_schema_models_length1 = i;
	
	/* select all properties */
	query = g_string_new ("SELECT ");
	g_string_append_printf (query, "%s, id FROM %s", TABLE_SCHEMA_PROPERTIES_COLUMNS, TABLE_NAME_SCHEMA_PROPERTIES);
	err = NULL;
	
	model = midgard_core_storage_sql_get_model (GDA_CONNECTION (self->_cnc), (GdaSqlParser *)self->_parser, query->str, &err);
	g_string_free (query, TRUE);
	/* internal error, return and throw error */
	if (err) {
		g_propagate_error (error, err);
		return;
	}
	/* there's no single property defined, silently return */
	if (!model)
		return;

	rows = gda_data_model_get_n_rows (model);
	/* There's a model, but no single record selected, silently return */
	if (rows == 0) {
		g_object_unref(model);
		return;
	}

	guint j = 0;
	while (self->_schema_models[j] != NULL) {
		const gchar *classname = midgard_cr_model_get_name (MIDGARD_CR_MODEL (self->_schema_models[j]));
		for (i = 0; i < rows; i++) {
			/* Check property's class name */
			value = gda_data_model_get_typed_value_at (model, 0, i, G_TYPE_STRING, TRUE, NULL);
			if (!g_str_equal (g_value_get_string (value), classname))
				continue;	
			/* property_name */
			value = gda_data_model_get_typed_value_at (model, 1, i, G_TYPE_STRING, TRUE, NULL);
			const gchar *property_name = g_value_get_string (value);
			/* gtype_name */
			value = gda_data_model_get_typed_value_at (model, 2, i, G_TYPE_STRING, TRUE, NULL);
			const gchar *gtype_name = g_value_get_string (value);
			/* default_value_string */
			value = gda_data_model_get_typed_value_at (model, 3, i, G_TYPE_STRING, TRUE, NULL);
			const gchar *dvalue = g_value_get_string (value);
			/* property_nick */
			value = gda_data_model_get_typed_value_at (model, 4, i, G_TYPE_STRING, TRUE, NULL);
			const gchar *property_nick = g_value_get_string (value);
			/* description */
			value = gda_data_model_get_typed_value_at (model, 5, i, G_TYPE_STRING, TRUE, NULL);
			const gchar *descr = g_value_get_string (value);
			/* id */
			value = gda_data_model_get_typed_value_at (model, 6, i, G_TYPE_INT, TRUE, NULL);
			guint id = (guint) g_value_get_int (value);

			MidgardCRSchemaModelProperty *property_model = 
				midgard_cr_schema_model_property_new (property_name, gtype_name, dvalue);
			/* FIXME, set nick */
			/* set description */
			midgard_cr_model_property_set_description (MIDGARD_CR_MODEL_PROPERTY (property_model), descr);
			/* set id */
			property_model->_id = id;
			/* Add property model to schema model */
			midgard_cr_model_add_model (MIDGARD_CR_MODEL (self->_schema_models[j]), MIDGARD_CR_MODEL (property_model));
		}
		j++;
	}
	
	g_object_unref (model);
	return;
}

static void
__list_all_storage_models (MidgardCRSQLStorageManager *self, GError **error)
{
	return;
}	

void
midgard_cr_core_sql_storage_manager_load_models (MidgardCRSQLStorageManager *self, GError **error)
{
	guint i = 0;
	if (self->_schema_models) {
		while (self->_schema_models[i] != 0) {
			g_object_unref (self->_schema_models[i]);
		}
		g_free (self->_schema_models);
	}

	i = 0;
	if (self->_storage_models) {
		while (self->_storage_models[i] != 0) {
			g_object_unref (self->_storage_models[i]);
		}
		g_free (self->_storage_models);
	}

	GError *err = NULL;
	__list_all_schema_models (self, &err);
	if (err) {
		g_propagate_error (error, err);
		return;
	}

	__list_all_storage_models (self, &err);
	if (err) {
		g_propagate_error (error, err);
		return;
	}

	return;
}

gboolean 
midgard_cr_core_sql_storage_manager_open (MidgardCRSQLStorageManager *self, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
	
	MidgardCRConfig *config = midgard_cr_sql_storage_manager_get_config (self);
	g_return_val_if_fail (config != NULL, FALSE);

	if (!g_type_name (GDA_TYPE_CONNECTION))
		gda_init ();

	gchar *auth = NULL;
	const gchar *host = midgard_cr_config_get_host (config);
	const gchar *dbname = midgard_cr_config_get_dbname (config);
	const gchar *dbuser = midgard_cr_config_get_dbuser (config);
	const gchar *dbpass = midgard_cr_config_get_dbpass (config);
	const gchar *dbtype = midgard_cr_config_get_dbtype (config);
	gchar *tmpstr = NULL;
	guint port = midgard_cr_config_get_dbport (config);
	guint dbtype_id = config->dbtype_id;

	if (dbtype_id == MIDGARD_CORE_DB_TYPE_SQLITE) {

		gchar *path = NULL;
		const gchar *dbdir = midgard_cr_config_get_dbdir (config);
		if (!dbdir || *dbdir == '\0') {
			const gchar *sqlite_dir[] = {"data", NULL};
			path = midgard_cr_core_config_build_path (sqlite_dir, NULL, TRUE);
		} else {
			path = g_strdup (dbdir);
		}

		tmpstr = g_strconcat ("DB_DIR=", path, ";", "DB_NAME=", dbname, NULL);
		g_free (path);

	} else if (dbtype_id == MIDGARD_CORE_DB_TYPE_ORACLE) {

		GString *cnc = g_string_sized_new (100);
		cnc_add_part (cnc, "TNSNAME", dbname, DEFAULT_DBHOST);
		cnc_add_part (cnc, "HOST", host, DEFAULT_DBHOST);
		cnc_add_part (cnc, "DB_NAME", dbname, DEFAULT_DBTYPE);
		tmpstr = g_string_free (cnc, FALSE);
		cnc = g_string_sized_new (100);
		cnc_add_part (cnc, "USERNAME", dbuser, DEFAULT_DBUSER);
		cnc_add_part (cnc, "PASSWORD", dbpass, DEFAULT_DBPASS);
		auth = g_string_free (cnc, FALSE);

	} else { 
		
		GString *cnc = g_string_sized_new (100);
		cnc_add_part (cnc, "HOST", host, DEFAULT_DBHOST);

		if (port > 0) {

			GString *_strp = g_string_new ("");
			g_string_append_printf (_strp, "%d", port);
			cnc_add_part (cnc, "PORT", _strp->str, "");
			g_string_free (_strp, TRUE);
		}

		cnc_add_part (cnc, "DB_NAME", dbname, DEFAULT_DBTYPE);
		tmpstr = g_string_free (cnc, FALSE);
		GString *auth_str = g_string_sized_new (100);
		cnc_add_part (auth_str, "USERNAME", dbuser, DEFAULT_DBUSER);
		cnc_add_part (auth_str, "PASSWORD", dbpass, DEFAULT_DBPASS);
		auth = g_string_free (auth_str, FALSE);
	}

	GError *cnc_error = NULL;
	GdaConnection *connection = 
		gda_connection_open_from_string (dbtype, tmpstr, auth, GDA_CONNECTION_OPTIONS_NONE, &cnc_error);
	g_free(auth);	

	if (connection == NULL) { 
		g_propagate_error (error, cnc_error);
		g_free (tmpstr);
		return FALSE;	
	} 

	g_free (tmpstr);

	GdaSqlParser *parser = gda_connection_create_parser (connection);
	if (!parser)
		parser = gda_sql_parser_new ();
	self->_parser = G_OBJECT (parser);
	g_assert (self->_parser != NULL);

	self->_cnc = G_OBJECT (connection);

	cnc_error = NULL;
	if (midgard_core_storage_sql_table_exists (GDA_CONNECTION (self->_cnc), TABLE_NAME_SCHEMA))
		midgard_cr_core_sql_storage_manager_load_models (self, &cnc_error);
	if (cnc_error) {
		*error = g_error_new (MIDGARD_CR_STORAGE_MANAGER_ERROR, MIDGARD_CR_STORAGE_MANAGER_ERROR_INTERNAL, 
				"%s", cnc_error->message ? cnc_error->message : "Unknown reason");
		g_clear_error (&cnc_error);
	}

	return TRUE;
}

gboolean 
midgard_cr_core_sql_storage_manager_close (MidgardCRSQLStorageManager *mngr, GError **error)
{
	if (mngr->_cnc == NULL) {
		*error = g_error_new_literal (MIDGARD_CR_STORAGE_MANAGER_ERROR, MIDGARD_CR_STORAGE_MANAGER_ERROR_NOT_OPENED, 
				"Can not close not opened connection.");
		return FALSE;
	}

	if (mngr->_parser) {
		g_object_unref (mngr->_parser);
		mngr->_parser = NULL;
	}

	if (mngr->_cnc) {
		g_object_unref (mngr->_cnc);
		mngr->_cnc = NULL;
	}	

	return TRUE;
}

gboolean 
midgard_cr_core_sql_storage_manager_initialize_storage (MidgardCRSQLStorageManager *manager, GError **error)
{
	g_return_val_if_fail (manager != NULL, FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
	g_return_val_if_fail (GDA_IS_CONNECTION (manager->_cnc), FALSE);
	
	GError *err = NULL;
	GdaConnection *cnc = GDA_CONNECTION (manager->_cnc);
	if (!midgard_core_storage_sql_create_base_tables (cnc, &err)) {
		*error = g_error_new (MIDGARD_CR_STORAGE_MANAGER_ERROR, MIDGARD_CR_STORAGE_MANAGER_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
		return FALSE;
	}

	return TRUE;
}

gboolean  
midgard_cr_core_sql_storage_manager_table_exists (MidgardCRSQLStorageManager *manager, MidgardCRSQLStorageModel *storage_model)
{
	g_return_if_fail (manager != NULL);
	g_return_if_fail (storage_model != NULL);

	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	g_return_if_fail (GDA_IS_CONNECTION (cnc));

	const gchar *tablename = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (storage_model));

	return midgard_core_storage_sql_table_exists (cnc, tablename);
}

void 
midgard_cr_core_sql_storage_manager_table_create (MidgardCRSQLStorageManager *manager, MidgardCRSQLStorageModel *storage_model, GError **error)
{
	g_return_if_fail (manager != NULL);
	g_return_if_fail (storage_model != NULL);

	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	g_return_if_fail (GDA_IS_CONNECTION (cnc));

	const gchar *tablename = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (storage_model));
	const gchar *description = ""; /* TODO, add description property to model if needed */
	const gchar *primary = "id";

	if (midgard_core_storage_sql_table_exists (cnc, tablename))
		return;

	guint n_models;
	guint i;
	/* Do not free models array, it's owned by storage_model */
	MidgardCRModel **models = midgard_cr_model_list_models (MIDGARD_CR_MODEL (storage_model), &n_models);
	for (i = 0; i < n_models; i++) {
		if (midgard_cr_storage_model_property_get_primary (MIDGARD_CR_STORAGE_MODEL_PROPERTY (models[i]))) {
			primary = midgard_cr_model_get_name (models[i]);
			break;
		}
	}

	GError *err = NULL;
	if (!midgard_core_storage_sql_table_create (cnc, tablename, description, primary, &err)) {
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		if (err)
			g_clear_error (&err);
	}
}

void 
midgard_cr_core_sql_storage_manager_table_remove (MidgardCRSQLStorageManager *manager, MidgardCRSQLStorageModel *storage_model, GError **error)
{
	g_return_if_fail (manager != NULL);
	g_return_if_fail (storage_model != NULL);

	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	g_return_if_fail (GDA_IS_CONNECTION (cnc));

	const gchar *tablename = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (storage_model));

	GError *err = NULL;
	if (!midgard_core_storage_sql_table_remove (cnc, tablename, &err)) {
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		if (err)
			g_clear_error (&err);
	}
}

static void
_mdc_from_model_property (MidgardCRSQLStorageModelProperty *property_model, MgdCoreStorageSQLColumn *mdc)
{
	const gchar *colname = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (property_model));
	MidgardCRModel *parent = midgard_cr_model_get_parent (MIDGARD_CR_MODEL (property_model));
	const gchar *tablename = midgard_cr_storage_model_get_location (MIDGARD_CR_STORAGE_MODEL (parent));
	GType col_type = midgard_cr_model_property_get_valuegtype (MIDGARD_CR_MODEL_PROPERTY (property_model));
	const gchar *description = midgard_cr_model_property_get_description (MIDGARD_CR_MODEL_PROPERTY (property_model));

	midgard_core_storage_sql_column_init (mdc, tablename, colname, col_type);
	if (description != NULL)
		mdc->column_desc = description;

	return;
}

gboolean 
midgard_cr_core_sql_storage_manager_column_exists (MidgardCRSQLStorageManager *manager, MidgardCRSQLStorageModelProperty *property_model)
{
	g_return_val_if_fail (manager != NULL, FALSE);
	g_return_val_if_fail (property_model != NULL, FALSE);

	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	g_return_val_if_fail (GDA_IS_CONNECTION (cnc), FALSE);

	MgdCoreStorageSQLColumn mdc = { NULL, NULL };
	_mdc_from_model_property (property_model, &mdc);

	return midgard_core_storage_sql_column_exists (cnc, &mdc);
}

void 
midgard_cr_core_sql_storage_manager_column_create (MidgardCRSQLStorageManager *manager, MidgardCRSQLStorageModelProperty *property_model, GError **error)
{
	g_return_if_fail (manager != NULL);
	g_return_if_fail (property_model != NULL);
	
	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	g_return_if_fail (GDA_IS_CONNECTION (cnc));

	MgdCoreStorageSQLColumn mdc = { NULL, NULL };
	_mdc_from_model_property (property_model, &mdc);

	GError *err = NULL;
	if (!midgard_core_storage_sql_column_create (cnc, &mdc, &err)) {	
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		if (err)
			g_clear_error (&err);
	}

	gboolean has_index = midgard_cr_storage_model_property_get_index (MIDGARD_CR_STORAGE_MODEL_PROPERTY (property_model));
	if (!has_index)
		return;
	mdc.index = TRUE;

	if (midgard_core_storage_sql_index_exists (cnc, &mdc))
		return;

	if (!midgard_core_storage_sql_index_create (cnc, &mdc, &err)) {
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		if (err)
			g_clear_error (&err);
	}

	return;
}

void 
midgard_cr_core_sql_storage_manager_column_update (MidgardCRSQLStorageManager *manager, MidgardCRSQLStorageModelProperty *property_model, GError **error)
{
	g_return_if_fail (manager != NULL);
	g_return_if_fail (property_model != NULL);
	
	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	g_return_if_fail (GDA_IS_CONNECTION (cnc));

	MgdCoreStorageSQLColumn mdc = { NULL, NULL };
	_mdc_from_model_property (property_model, &mdc);

	GError *err = NULL;
	if (!midgard_core_storage_sql_column_update (cnc, &mdc, &err)) {	
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		if (err)
			g_clear_error (&err);
	}

	gboolean has_index = midgard_cr_storage_model_property_get_index (MIDGARD_CR_STORAGE_MODEL_PROPERTY (property_model));
	if (!has_index)
		return;
	mdc.index = TRUE;

	if (midgard_core_storage_sql_index_exists (cnc, &mdc))
		return;

	if (!midgard_core_storage_sql_index_create (cnc, &mdc, &err)) {
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		if (err)
			g_clear_error (&err);
	}

	return;
}

void 
midgard_cr_core_sql_storage_manager_column_remove (MidgardCRSQLStorageManager *manager, MidgardCRSQLStorageModelProperty *property_model, GError **error)
{
	g_return_if_fail (manager != NULL);
	g_return_if_fail (property_model != NULL);
	
	GdaConnection *cnc = (GdaConnection *) manager->_cnc;
	g_return_if_fail (GDA_IS_CONNECTION (cnc));

	MgdCoreStorageSQLColumn mdc = { NULL, NULL };
	_mdc_from_model_property (property_model, &mdc);

	GError *err = NULL;
	if (!midgard_core_storage_sql_column_update (cnc, &mdc, &err)) {	
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		if (err)
			g_clear_error (&err);
	}

	gboolean has_index = midgard_cr_storage_model_property_get_index (MIDGARD_CR_STORAGE_MODEL_PROPERTY (property_model));
	if (!has_index)
		return;
	mdc.index = TRUE;

	if (midgard_core_storage_sql_index_exists (cnc, &mdc))
		return;

	if (!midgard_core_storage_sql_index_create (cnc, &mdc, &err)) {
		*error = g_error_new (MIDGARD_CR_EXECUTABLE_ERROR, MIDGARD_CR_EXECUTABLE_ERROR_INTERNAL,
				"%s", err && err->message ? err->message : "Unknown reason");
		if (err)
			g_clear_error (&err);
	}

	return;
}

