/* 
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_repligard.h"
#include "midgard_core_object.h"
#include "midgard_core_query.h"
#include "guid.h"
#include "midgard_error.h"
#include "midgard_core_workspace.h"
#include <sql-parser/gda-sql-parser.h>

#define MGD_REPLIGARD_TABLE "repligard"

/**
 * midgard_repligard_new:
 * @mgd: #MidgardConnection instance
 *
 * Simplified constructor.
 *
 * Returns: #MidgardRepligard instance
 * Since: 10.05.5
 */ 
MidgardRepligard *
midgard_repligard_new (MidgardConnection *mgd)
{
	MidgardRepligard *self = g_object_new (MIDGARD_TYPE_REPLIGARD, "connection", mgd, NULL);
	return self;
}

/**
 * midgard_repligard_create_object_info:
 * @self: #MidgardRepligard instance
 * @object: #MidgardObject to create record info 
 * @error: pointer to store error
 *
 * For given @object, creates replication record info. 
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05.5
 */
gboolean
midgard_repligard_create_object_info (MidgardRepligard *self, MidgardObject *object, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (object != NULL, FALSE);

	GError *err = NULL;
	MidgardConnection *mgd = MGD_OBJECT_CNC (self);
	GdaConnection *cnc = mgd->priv->connection; 
	MidgardDBObjectClass *rklass = MIDGARD_DBOBJECT_GET_CLASS (self);
	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_GET_CLASS (object);

	GdaStatement *insert = MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->get_statement_insert (rklass, mgd);
	g_return_val_if_fail (insert != NULL, FALSE);
	GdaSet *params = rklass->dbpriv->get_statement_insert_params (rklass, mgd);
	if (!params) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
				"Failed to get parameters for prepared INSERT statement (%s).",
				G_OBJECT_CLASS_NAME (rklass));
		return FALSE;
	}

	/* Guid */
	gda_set_set_holder_value (params, &err, "guid", MGD_OBJECT_GUID (object));
	if (err) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
				"Failed to set guid INSERT parameter: %s.",
				err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
		return FALSE;
	}

	/* Typename */
	gda_set_set_holder_value (params, &err, "typename", G_OBJECT_TYPE_NAME (object));
	if (err) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
				"Failed to set typename INSERT parameter: %s.",
				err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
		return FALSE;
	}

	/* Object action */
	gda_set_set_holder_value (params, &err, "object_action", MGD_OBJECT_ACTION_CREATE);
	if (err) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
				"Failed to set object_action INSERT parameter: %s.",
				err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
		return FALSE;
	}

	if (MGD_CNC_USES_WORKSPACE (mgd) && dbklass->dbpriv->uses_workspace) {
		guint ws_id = MGD_CNC_WORKSPACE_ID (mgd);
		/* workspace_id */
		gda_set_set_holder_value (params, &err, "workspace_id", ws_id);
		if (err) {
			g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
					"Failed to set workspace_id INSERT parameter: %s.",
					err && err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
			return FALSE;
		}
	}

	if (MGD_CNC_DEBUG (mgd)) {
		gchar *debug_sql = gda_connection_statement_to_sql (cnc, insert, params, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
		g_debug ("Create repligard record info: %s", debug_sql);
		g_free (debug_sql);
	}

	gboolean inserted = (gda_connection_statement_execute_non_select (cnc, insert, params, NULL, &err) == -1) ? FALSE : TRUE;
	
	if (!inserted) {
		g_propagate_error (error, err);
		return FALSE;
	}
	
	return inserted;
}

/**
 * midgard_repligard_update_object_info:
 * @self: #MidgardRepligard instance
 * @object: #MidgardObject to create record info 
 * @action: action to set 
 * @error: pointer to store error
 *
 * For given @object, updates replication record info. 
 *
 * Returns: %TRUE on success, %FALSE otherwise
 *
 * Since: 10.05.5
 */
gboolean
midgard_repligard_update_object_info (MidgardRepligard *self, MidgardObject *object, guint action, GError **error)
{
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (object != NULL, FALSE);

	GError *err = NULL;
	MidgardConnection *mgd = MGD_OBJECT_CNC (self);
	GdaConnection *cnc = mgd->priv->connection; 
	MidgardDBObjectClass *rklass = MIDGARD_DBOBJECT_GET_CLASS (self);
	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_GET_CLASS (object);

	GdaStatement *insert = MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->get_statement_insert (rklass, mgd);
	g_return_val_if_fail (insert != NULL, FALSE);
	GdaSet *params = rklass->dbpriv->get_statement_insert_params (rklass, mgd);
	if (!params) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
				"Failed to get parameters for prepared INSERT statement (%s).",
				G_OBJECT_CLASS_NAME (rklass));
		return FALSE;
	}

	/* Guid */
	gda_set_set_holder_value (params, &err, "guid", MGD_OBJECT_GUID (object));
	if (err) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
				"Failed to set guid INSERT parameter: %s.",
				err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
		return FALSE;
	}

	/* Object action */
	gda_set_set_holder_value (params, &err, "object_action", MGD_OBJECT_ACTION_CREATE);
	if (err) {
		g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
				"Failed to set object_action INSERT parameter: %s.",
				err && err->message ? err->message : "Unknown reason");
		g_clear_error (&err);
		return FALSE;
	}

	if (MGD_CNC_USES_WORKSPACE (mgd) && dbklass->dbpriv->uses_workspace) {
		guint ws_id = MGD_CNC_WORKSPACE_ID (mgd);
		/* workspace_id */
		gda_set_set_holder_value (params, &err, "workspace_id", ws_id);
		if (err) {
			g_set_error (error, MIDGARD_GENERIC_ERROR, MIDGARD_GENERIC_ERROR_INTERNAL,
					"Failed to set workspace_id INSERT parameter: %s.",
					err && err->message ? err->message : "Unknown reason");
			g_clear_error (&err);
			return FALSE;
		}
	}

	if (MGD_CNC_DEBUG (mgd)) {
		gchar *debug_sql = gda_connection_statement_to_sql (cnc, insert, params, GDA_STATEMENT_SQL_PRETTY, NULL, NULL);
		g_debug ("Update repligard record info: %s", debug_sql);
		g_free (debug_sql);
	}

	gboolean updated = (gda_connection_statement_execute_non_select (cnc, insert, params, NULL, &err) == -1) ? FALSE : TRUE;
	
	if (!updated) {
		g_propagate_error (error, err);
		return FALSE;
	}
	
	return updated;
}

gboolean
midgard_repligard_purge_object_info (MidgardRepligard *self, MidgardObject *object, guint action, GError **error)
{
	return FALSE;
}


/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_OBJECT_GUID = 1,
	PROPERTY_TYPE_NAME,
	PROPERTY_OBJECT_ACTION,
	PROPERTY_WORKSPACE_ID
};


static void 
_midgard_repligard_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardRepligard *self = (MidgardRepligard *) instance;
	
	self->object_guid = NULL;
	self->type_name = NULL;
	self->object_action = 0;
	self->workspace_id = 0;

	return;
}

static GObject *
_midgard_repligard_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (__parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);
	return G_OBJECT(object);
}

static void
_midgard_repligard_dispose (GObject *object)
{
	MidgardRepligard *self = MIDGARD_REPLIGARD (object);

	__parent_class->dispose (object);
}

static void 
_midgard_repligard_finalize (GObject *object)
{
	MidgardRepligard *self = MIDGARD_REPLIGARD (object);

	g_free (self->object_guid);
	self->object_guid = NULL;

	g_free (self->type_name);
	self->type_name = NULL;

	self->object_action = 0;
	self->workspace_id = 0;

	__parent_class->finalize (object);
}

static void
_midgard_repligard_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	MidgardRepligard *self = MIDGARD_REPLIGARD (object);

	switch (property_id) {
		
		case PROPERTY_OBJECT_GUID:
			g_free (self->object_guid);
			self->object_guid = g_value_dup_string (value);
			break;

		case PROPERTY_TYPE_NAME:
			g_free (self->type_name);
			self->type_name = g_value_dup_string (value);
			break;

		case PROPERTY_OBJECT_ACTION:
			self->object_action = g_value_get_uint (value);
			break;

		case PROPERTY_WORKSPACE_ID:
			self->workspace_id = g_value_get_uint (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;		
	}
}

static void
_midgard_repligard_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	MidgardRepligard *self = (MidgardRepligard *) object;
	
	switch (property_id) {
		
		case PROPERTY_OBJECT_GUID:
			g_value_set_string (value, self->object_guid);
			break;

		case PROPERTY_TYPE_NAME:
			g_value_set_string (value, self->type_name);
			break;

		case PROPERTY_OBJECT_ACTION:
			g_value_set_uint (value, self->object_action);
			break;

		case PROPERTY_WORKSPACE_ID:
			g_value_set_uint (value, self->workspace_id);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static gboolean
_repligard_storage_exists (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (klass != NULL, FALSE);

	return midgard_core_table_exists (mgd, MGD_REPLIGARD_TABLE);
}

static gboolean
_repligard_storage_delete (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail (mgd != NULL, FALSE);
	g_return_val_if_fail (klass != NULL, FALSE);

	g_warning ("MidgardRepligard storage can not be deleted");
	return FALSE;
}

static void
_set_from_data_model (MidgardDBObject *self, GdaDataModel *model, gint row, guint column_id)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (model != NULL);
	g_return_if_fail (row > -1);

	return;
}

static void
__initialize_statement_insert_query_parameters (MidgardDBObjectClass *klass, const gchar *query_string, gboolean add_workspace)
{
	GdaSqlParser *parser = gda_sql_parser_new ();
	GdaStatement *stmt;
	GError *error = NULL;
	stmt = gda_sql_parser_parse_string (parser, query_string, NULL, &error);

	if (!stmt) {
		g_error ("Couldn't create %s class prepared statement. %s", 
				G_OBJECT_CLASS_NAME (klass), error && error->message ? error->message : "Unknown reason");
		return;
	}

	GdaSet *params; 
	if (!gda_statement_get_parameters (stmt, &params, &error)) {
		g_error ("Failed to create GdaSet for %s class. %s", 
				G_OBJECT_CLASS_NAME (klass), error && error->message ? error->message : "Unknown reason");
	}
	
	if (add_workspace) {
		klass->dbpriv->_workspace_statement_insert = stmt;
		klass->dbpriv->_workspace_statement_insert_params = params;
		return;
	}
	
	klass->dbpriv->_statement_insert = stmt;
	klass->dbpriv->_statement_insert_params = params;

	return;
}

static gchar *
__initialize_statement_insert_query_string (MidgardDBObjectClass *klass, gboolean add_workspace)
{
	GString *sql = g_string_new ("INSERT INTO ");

	g_string_append (sql, MGD_REPLIGARD_TABLE);
	if (!add_workspace) {	
		g_string_append (sql, "(guid, typename, object_action) VALUES (##guid::string, ##typename::string, ##object_action::guint)");
	} else {
		g_string_append (sql, "(guid, typename, object_action, workspace_id) "
				"VALUES (##guid::string, ##typename::string, ##object_action::guint, ##workspace_id::guint)");
	}

	return g_string_free (sql, FALSE);
}

static GdaStatement *
__get_statement_insert (MidgardDBObjectClass *klass, MidgardConnection *mgd)
{
	gchar *query = NULL;

	/* Try workspace statement first */
	if (klass->dbpriv->uses_workspace && (mgd && MGD_CNC_USES_WORKSPACE (mgd))) {		
		if (!klass->dbpriv->_workspace_statement_insert) {
			query = __initialize_statement_insert_query_string (klass, TRUE);
			__initialize_statement_insert_query_parameters (klass, query, TRUE);
			g_free (query);
		}

		return klass->dbpriv->_workspace_statement_insert;
	}

	if (!klass->dbpriv->_statement_insert) {
		query = __initialize_statement_insert_query_string (klass, FALSE);
		__initialize_statement_insert_query_parameters (klass, query, FALSE);
		g_free (query);
	}

	return klass->dbpriv->_statement_insert;
}

static GdaSet *
__get_statement_insert_params (MidgardDBObjectClass *klass, MidgardConnection *mgd)
{
	GdaStatement *stmt = klass->dbpriv->get_statement_insert (klass, mgd);
	if (!stmt) {
		g_error ("Failed to get UPDATE GdaStatement and GdaSet (%s)", G_OBJECT_CLASS_NAME (klass));
		return NULL;
	}

	if (klass->dbpriv->uses_workspace && (mgd && MGD_CNC_USES_WORKSPACE (mgd)))
		return klass->dbpriv->_workspace_statement_insert_params;

	return klass->dbpriv->_statement_insert_params;
}

static void
__initialize_statement_update_query_parameters (MidgardDBObjectClass *klass, const gchar *query_string, gboolean add_workspace)
{
	GdaSqlParser *parser = gda_sql_parser_new ();
	GdaStatement *stmt;
	GError *error = NULL;
	stmt = gda_sql_parser_parse_string (parser, query_string, NULL, &error);

	if (!stmt) {
		g_error ("Couldn't create %s class UPDATE prepared statement. %s", 
				G_OBJECT_CLASS_NAME (klass), error && error->message ? error->message : "Unknown reason");
		return;
	}

	GdaSet *params; 
	if (!gda_statement_get_parameters (stmt, &params, &error)) {
		g_error ("Failed to create UPDATE GdaSet for %s class. %s", 
				G_OBJECT_CLASS_NAME (klass), error && error->message ? error->message : "Unknown reason");
	}
	
	if (add_workspace) {
		klass->dbpriv->_workspace_statement_update = stmt;
		klass->dbpriv->_workspace_statement_update_params = params;
		return;
	}
	
	klass->dbpriv->_statement_update = stmt;
	klass->dbpriv->_statement_update_params = params;

	return;
}

static gchar *
__initialize_statement_update_query_string (MidgardDBObjectClass *klass, gboolean add_workspace)
{
	GString *sql = g_string_new ("");
	g_string_append_printf (sql, "UPDATE %s SET object_action=##object_action::guint ", MGD_REPLIGARD_TABLE);

	if (add_workspace) 
		g_string_append (sql, "WHERE guid=##guid::string AND workspace_id=##workspace_id::guint ");
	else 
		g_string_append (sql, "WHERE guid=##guid::string ");		

	return g_string_free (sql, FALSE);
}

static GdaStatement *
__get_statement_update (MidgardDBObjectClass *klass, MidgardConnection *mgd)
{
	gchar *query = NULL;

	/* Try workspace statement first */
	if (klass->dbpriv->uses_workspace && (mgd && MGD_CNC_USES_WORKSPACE (mgd))) {
		
		if (!klass->dbpriv->_workspace_statement_update) {
			query = __initialize_statement_update_query_string (klass, TRUE);
			__initialize_statement_update_query_parameters (klass, query, TRUE);
			g_free (query);
		}

		return klass->dbpriv->_workspace_statement_update;
	}

	if (!klass->dbpriv->_statement_update) {
		query = __initialize_statement_update_query_string (klass, FALSE);
		__initialize_statement_update_query_parameters (klass, query, FALSE);
		g_free (query);
	}

	return klass->dbpriv->_statement_update;
}

static GdaSet *
__get_statement_update_params (MidgardDBObjectClass *klass, MidgardConnection *mgd)
{	
	GdaStatement *stmt = klass->dbpriv->get_statement_update (klass, mgd);
	if (!stmt) {
		g_error ("Failed to get GdaStatement and GdaSet (%s)", G_OBJECT_CLASS_NAME (klass));
		return NULL;
	}

	if (klass->dbpriv->uses_workspace && (mgd && MGD_CNC_USES_WORKSPACE (mgd)))
		return klass->dbpriv->_workspace_statement_update_params;

	return klass->dbpriv->_statement_update_params;
}


static void _midgard_repligard_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardRepligardClass *klass = MIDGARD_REPLIGARD_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_repligard_constructor;
	gobject_class->dispose = _midgard_repligard_dispose;
	gobject_class->finalize = _midgard_repligard_finalize;
	gobject_class->set_property = _midgard_repligard_set_property;
	gobject_class->get_property = _midgard_repligard_get_property;

	/* PROPERTIES */
	MgdSchemaPropertyAttr *prop_attr;
	MgdSchemaTypeAttr *type_attr = midgard_core_schema_type_attr_new();

	GParamSpec *pspec;
	const gchar *property_name;

	/* Object guid */
	property_name = "reference";
	pspec = g_param_spec_string (property_name,
			"Guid of an object for which record info is created",
			"",
			"", G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,	PROPERTY_OBJECT_GUID, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_STRING;
	prop_attr->field = g_strdup ("guid");
	prop_attr->table = g_strdup (MGD_REPLIGARD_TABLE);
	prop_attr->tablefield = g_strjoin (".", MGD_REPLIGARD_TABLE, "guid", NULL);
	g_hash_table_insert (type_attr->prophash, g_strdup ((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer) property_name);

	/* Type name */
	property_name = "type";
	pspec = g_param_spec_string (property_name,
			"Name of the class of reference object",
			"",
			"", G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,	PROPERTY_TYPE_NAME, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_STRING;
	prop_attr->field = g_strdup ("typename");
	prop_attr->table = g_strdup (MGD_REPLIGARD_TABLE);
	prop_attr->tablefield = g_strjoin (".", MGD_REPLIGARD_TABLE, "typename", NULL);
	g_hash_table_insert (type_attr->prophash, g_strdup ((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer) property_name);

	/* Action ID */
	property_name = "action";
	pspec = g_param_spec_uint (property_name,
			"Action ID",
			"",
			0, G_MAXUINT32, 0, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,	PROPERTY_OBJECT_ACTION, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup ("object_action");
	prop_attr->table = g_strdup (MGD_REPLIGARD_TABLE);
	prop_attr->tablefield = g_strjoin (".", MGD_REPLIGARD_TABLE, "object_action", NULL);
	g_hash_table_insert (type_attr->prophash, g_strdup ((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer) property_name);

	/* Workspace ID */
	property_name = "workspace";
	pspec = g_param_spec_uint (property_name,
			"Workspace ID",
			"",
			0, G_MAXUINT32, 0, G_PARAM_READWRITE);
	g_object_class_install_property (gobject_class,	PROPERTY_WORKSPACE_ID, pspec);
	prop_attr = midgard_core_schema_type_property_attr_new();
	prop_attr->gtype = MGD_TYPE_UINT;
	prop_attr->field = g_strdup ("workspace_id");
	prop_attr->table = g_strdup (MGD_REPLIGARD_TABLE);
	prop_attr->tablefield = g_strjoin (".", MGD_REPLIGARD_TABLE, "workspace_id", NULL);
	g_hash_table_insert (type_attr->prophash, g_strdup ((gchar *)property_name), prop_attr);
	type_attr->_properties_list = g_slist_append (type_attr->_properties_list, (gpointer) property_name);


	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv = midgard_core_dbobject_private_new ();
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data = type_attr;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data->table = g_strdup (MGD_REPLIGARD_TABLE);
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_data->tables = g_strdup (MGD_REPLIGARD_TABLE);
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->has_metadata = FALSE;
	
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->create_storage = midgard_core_query_create_class_storage;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->update_storage = midgard_core_query_update_class_storage;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->storage_exists = _repligard_storage_exists;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->delete_storage = _repligard_storage_delete;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_insert = __get_statement_insert;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_insert_params = __get_statement_insert_params;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_update = __get_statement_update;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->get_statement_update_params = __get_statement_update_params;
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->add_fields_to_select_statement = MIDGARD_DBOBJECT_CLASS (__parent_class)->dbpriv->add_fields_to_select_statement;	
	MIDGARD_DBOBJECT_CLASS (klass)->dbpriv->set_from_data_model = NULL;
}

GType 
midgard_repligard_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardRepligardClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_repligard_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardRepligard),
			0,              /* n_preallocs */
			_midgard_repligard_instance_init /* instance_init */
		};

		type = g_type_register_static (MIDGARD_TYPE_DBOBJECT, "MidgardRepligard", &info, 0);	
	}
	return type;
}

