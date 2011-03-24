/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_view.h"
#include "midgard_core_views.h"
#include "midgard_core_object_class.h"
#include "midgard_core_query.h"
#include "schema.h"
#include "midgard_dbobject.h"
#include "midgard_core_object.h"
#include "midgard_connection.h"
#include "midgard_core_config.h"
#include "midgard_error.h"

static GObjectClass *__view_parent_class= NULL;

static gboolean
_midgard_view_derived_storage_exists (MidgardConnection *mgd, MidgardDBObjectClass *klass);

static GSList*
__midgard_view_derived_object_set_from_sql(MidgardConnection *mgd, GType type, const gchar *sql)
{
	g_return_val_if_fail(mgd != NULL, NULL);
	g_return_val_if_fail(sql != NULL, NULL);

	GSList *olist = NULL;

	GdaDataModel *model =
		midgard_core_query_get_model(mgd, sql);

	if (!model)
		return olist;

	guint ret_rows = gda_data_model_get_n_rows(model);	

	if (ret_rows == 0) {

		g_object_unref(model);	
		return olist;
	}

	guint rows;

	for (rows = 0; rows < ret_rows; rows++) {
		
		MidgardView *view = g_object_new(type, "connection", mgd, NULL);

		/* Every view object holds reference to the same model.
		 * And has different row which is used to determine its property location */
		MIDGARD_DBOBJECT(view)->dbpriv->datamodel = model;
		MIDGARD_DBOBJECT(view)->dbpriv->row = rows;

		/* Increase datamodel reference count.
		 * It will be freed with the last object returned by the model */  
		if (rows > 0)
			g_object_ref(model);

		olist = g_slist_append(olist, (gpointer)view);
	}

	return olist;
}	

/* Create GdaSqlSelectField for every property registered for the class. */
static void
__midgard_view_derived_add_fields_to_select_statement (MidgardDBObjectClass *klass, MidgardConnection *mgd, GdaSqlStatementSelect *select, const gchar *table_name)
{
	guint n_prop;
	guint i;
	GdaSqlSelectField *select_field;
	GdaSqlExpr *expr;
	GValue *val;
	gchar *table_field;
	GdaConnection *cnc = mgd->priv->connection;

	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (klass), &n_prop);
	if (!pspecs)
		return;

	const gchar *property_table = NULL;

	for (i = 0; i < n_prop; i++) {

		MgdSchemaPropertyAttr *prop_attr = midgard_core_class_get_property_attr (klass, pspecs[i]->name);
		if (!prop_attr)
			continue; 

		const gchar *property = pspecs[i]->name;
		const gchar *property_field = prop_attr->name;
		property_table = midgard_core_class_get_property_table (klass, property); 
		if (property_table && table_name)
			property_table = table_name;

		/* FIXME, ugly workaround to keep prop_attr->field with proper value */
		g_free ((gchar *)prop_attr->field);
		prop_attr->field = g_strdup (property);

		/* Ignore properties with NULL storage and those of object type */
		if (!property_table || pspecs[i]->value_type == G_TYPE_OBJECT)
			continue;

       		select_field = gda_sql_select_field_new (GDA_SQL_ANY_PART (select));
		/*select_field->field_name = g_strdup (property_field);
		select_field->table_name = g_strdup (property_table);*/
		select_field->as = gda_connection_quote_sql_identifier (cnc, property);
		select->expr_list = g_slist_append (select->expr_list, select_field);
		expr = gda_sql_expr_new (GDA_SQL_ANY_PART (select_field));
		val = g_new0 (GValue, 1);
		g_value_init (val, G_TYPE_STRING);
		gchar *q_table = gda_connection_quote_sql_identifier (cnc, property_table);
		gchar *q_field = gda_connection_quote_sql_identifier (cnc, property_field);
		table_field = g_strconcat (q_table, ".", q_field, NULL);
		g_value_set_string (val, table_field);
		g_free (q_table);
		g_free (q_field);
		g_free (table_field);
		expr->value = val;
		select_field->expr = expr;
	}	

	return;
}

static GObject *
__midgard_view_derived_object_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties)
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (__view_parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	 MIDGARD_DBOBJECT(object)->dbpriv->storage_data =
		 MIDGARD_DBOBJECT_GET_CLASS(object)->dbpriv->storage_data;
 
	 return object;
}

static void
_midgard_view_derived_object_set_property (GObject *object, guint prop_id,
		const GValue *value, GParamSpec   *pspec)
{
	/* Do not set view properties.
	 * We are going to use underlying GDataModel (if assigned). */
	return;
}

static void
_midgard_view_derived_object_get_property (GObject *object, guint prop_id,
		GValue *value, GParamSpec   *pspec)
{	
	MidgardDBObject *self = MIDGARD_DBOBJECT(object);

	if (!self->dbpriv->datamodel)
		return;

	const GValue *dvalue = 
		midgard_data_model_get_value_at_col_name(self->dbpriv->datamodel, pspec->name, self->dbpriv->row);

	// Hack to workaround bug in gda-sqlite (returns INVALID for empty strings)
	if (G_VALUE_HOLDS (dvalue, G_TYPE_INVALID) && G_VALUE_HOLDS_STRING (value)) {

		g_value_set_string (value, "");
		return;
	}

	/* We can have integer value for registered boolean type.
	   Therefore, transform value */
	if (G_VALUE_TYPE(value) != G_VALUE_TYPE(dvalue)) {

		if (!g_value_transform(dvalue, value))
			g_warning("%s to %s conversion failed for midgard_view.%s", 
					G_VALUE_TYPE_NAME(dvalue), G_VALUE_TYPE_NAME(value), pspec->name);
		return;
	}

	g_value_copy(dvalue, value);
	return;
}

static gboolean 
_midgard_view_derived_create_storage (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(klass != NULL, FALSE);

	MgdSchemaTypeAttr *type_attr = midgard_core_class_get_type_attr(klass);
	
	/* Do not depend on CREATE VIEW IF NOT EXISTS */
	if (_midgard_view_derived_storage_exists (mgd, klass)) {

		mgd_info ("View %s exists. Skipping create: OK", type_attr->name);
		return TRUE;
	}

	if (type_attr->sql_create_view == NULL)
		type_attr->sql_create_view = midgard_core_view_build_create_view_command (mgd, klass);

	gint rv = midgard_core_query_execute(mgd, type_attr->sql_create_view, FALSE);

	if (rv == -1)
		return FALSE;

	return TRUE;	
}

static gboolean 
_midgard_view_derived_update_storage( MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(klass != NULL, FALSE);	

	/* View can not be updated, so we delete it and create again */
	klass->dbpriv->delete_storage (mgd, klass);
	return klass->dbpriv->create_storage (mgd, klass);
}

static gboolean 
_midgard_view_derived_storage_exists (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(klass != NULL, FALSE);		

	MgdSchemaTypeAttr *type_attr = midgard_core_class_get_type_attr(klass);
	const gchar *viewname = type_attr->name;

	GdaMetaContext mcontext = {"_views", 1, NULL, NULL};
	mcontext.column_names = g_new (gchar *, 1);
	mcontext.column_names[0] = "table_name";
	mcontext.column_values = g_new (GValue *, 1);
	g_value_set_string ((mcontext.column_values[0] = gda_value_new (G_TYPE_STRING)), viewname);
	GError *error = NULL;

	if (!gda_connection_update_meta_store (mgd->priv->connection, &mcontext, &error)) {
		gda_value_free (mcontext.column_values[0]);
		g_warning("Failed to update meta data for table '%s': %s", viewname,
				error && error->message ? error->message : "No detail");
		if (error)
			g_error_free(error);
		return TRUE;
	}

	GdaDataModel *dm_schema =
		gda_connection_get_meta_store_data (mgd->priv->connection,
				GDA_CONNECTION_META_VIEWS, NULL, 1,
				"name", mcontext.column_values[0],
				NULL);

	gda_value_free (mcontext.column_values[0]);
	g_free (mcontext.column_names);
	g_free (mcontext.column_values);

	if(!dm_schema) {
		g_error("Failed to retrieve views schema");
		return TRUE;
	}

	gboolean retval = TRUE;
	if (gda_data_model_get_n_rows (dm_schema) == 0)
		retval = FALSE;
	g_object_unref (dm_schema);

	return retval;
}

static gboolean 
_midgard_view_derived_delete_storage(MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	g_return_val_if_fail(mgd != NULL, FALSE);
	g_return_val_if_fail(klass != NULL, FALSE);	

	MgdSchemaTypeAttr *type_attr = midgard_core_class_get_type_attr(klass);

	/* Should work with at least: MySQL, SQLite and PostgreSQL */
	GString *query = g_string_new("DROP VIEW ");
	g_string_append(query, type_attr->name);

	gint rv = midgard_core_query_execute(mgd, query->str, FALSE);
	
	g_string_free(query, TRUE);

	if (rv == -1)
		return FALSE;

	return TRUE;	
}

static void
_midgard_view_derived_set_static_sql_select (MidgardConnection *mgd, MidgardDBObjectClass *klass)
{
	guint n_props;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (klass), &n_props);
	GdaConnection *cnc = mgd->priv->connection;
	MgdSchemaTypeAttr *type = midgard_core_class_get_type_attr (klass);

	g_free (type->sql_select_full);
	guint i;
	GString *ssf = g_string_new ("");
	gboolean add_coma = FALSE;
	
	for (i = 0; i < n_props; i++) {
		
		if (G_TYPE_FUNDAMENTAL (pspecs[i]->value_type) == G_TYPE_OBJECT) {
			if (i == 1)
				add_coma = FALSE;
			continue;
		}

		/* Replace tablefield for every property */
		MgdSchemaPropertyAttr *prop_attr = g_hash_table_lookup (type->prophash, pspecs[i]->name);
		if (!prop_attr) {
			g_warning ("%s property not registered for %s", pspecs[i]->name, type->name);
			g_error ("Failed to register view class");
		}

		gchar *q_table = gda_connection_quote_sql_identifier (cnc, type->table);
		gchar *q_field = gda_connection_quote_sql_identifier (cnc, prop_attr->field);
		gchar *q_name = gda_connection_quote_sql_identifier (cnc, pspecs[i]->name);

		g_string_append_printf (ssf, "%s%s.%s AS %s", 
				add_coma ? ", " : " ",
				q_table, q_field, q_name);
		
		gchar *t_table = gda_connection_quote_sql_identifier (cnc, type->name);	
		midgard_core_schema_type_property_set_tablefield (prop_attr, t_table, q_name);
	
		g_free (q_table);
		g_free (q_field); 
		g_free (q_name);
		g_free (t_table);

		add_coma = TRUE;
	}
	
	type->sql_select_full = g_string_free (ssf, FALSE);
	g_free (pspecs);
}

static void
__midgard_view_derived_class_init(gpointer g_class, gpointer class_data)
{
	MgdSchemaTypeAttr *data = (MgdSchemaTypeAttr *) class_data;
	GObjectClass *gobject_class = G_OBJECT_CLASS(g_class);
	MidgardViewClass *mklass = (MidgardViewClass *) g_class;

	guint idx;

	__view_parent_class = g_type_class_peek_parent (g_class);

	gobject_class->set_property = _midgard_view_derived_object_set_property;
	gobject_class->get_property = _midgard_view_derived_object_get_property;
	//gobject_class->finalize = _object_finalize;
	gobject_class->constructor = __midgard_view_derived_object_constructor;
	//gobject_class->dispose = __midgard_view_derived_object_dispose;
	mklass->get_connection = MIDGARD_DBOBJECT_CLASS(mklass)->get_connection;

	if(mklass) {

		mklass->dbpriv = g_new(MidgardDBObjectPrivate, 1);
		mklass->dbpriv->has_metadata = FALSE;	
		mklass->dbpriv->storage_data = data;
		mklass->dbpriv->set_from_sql = __midgard_view_derived_object_set_from_sql;
		mklass->dbpriv->set_from_xml_node = NULL;

		mklass->dbpriv->create_storage = _midgard_view_derived_create_storage;
		mklass->dbpriv->update_storage = _midgard_view_derived_update_storage;
		mklass->dbpriv->storage_exists = _midgard_view_derived_storage_exists;
		mklass->dbpriv->delete_storage = _midgard_view_derived_delete_storage;

		mklass->dbpriv->add_fields_to_select_statement = __midgard_view_derived_add_fields_to_select_statement; 
		mklass->dbpriv->set_static_sql_select = _midgard_view_derived_set_static_sql_select;	
	}

	g_type_class_add_private (g_class, sizeof(MgdSchemaTypeAttr));

      	/* List parent class properties so we can set current class base_index */
	guint n_prop;

	GParamSpec **pspecs =
		g_object_class_list_properties (g_type_class_peek_parent(g_class), &n_prop);

	g_free(pspecs);

	if (data) {
		data->properties = g_malloc(sizeof(MgdSchemaPropertyAttr*) * (data->num_properties+1));
		data->base_index = n_prop;

		/* Note, that we start numbering from 1 , not from 0. property_id must be > 0 */
	 	for (idx = 1; idx <= data->num_properties; idx++) {
			/* g_warning("Installing property id %d :: %s",
			   idx, data->params[idx-1]->name); */
			g_object_class_install_property(
					gobject_class,
					data->base_index + idx ,
	  				data->params[idx-1]);
		}
	}
}

static void
__midgard_view_derived_instance_init(GTypeInstance *instance, gpointer g_class)
{

}

GType
midgard_core_view_type_register(MgdSchemaTypeAttr *data, GType parent_type)
{
	g_assert(data != NULL);
	g_assert(parent_type != 0);
	
	const gchar *class_name = data->name;

	GType class_type = g_type_from_name(class_name);

	if (class_type) 
      		return class_type;
	{
       
		GTypeInfo *midgard_type_info = g_new0 (GTypeInfo, 1);

		if(data == NULL)
			data = g_new(MgdSchemaTypeAttr, 1);

		midgard_type_info->class_size = sizeof(MidgardViewClass);
		midgard_type_info->base_init = NULL;
		midgard_type_info->base_finalize = NULL;
		midgard_type_info->class_init  = __midgard_view_derived_class_init;
		midgard_type_info->class_finalize  = NULL;
		midgard_type_info->class_data = data;

		/* our own instance size is 0 but it should include space for a parent, therefore add it */
		midgard_type_info->instance_size = sizeof(MidgardView);
		midgard_type_info->n_preallocs = 0;
		midgard_type_info->instance_init = __midgard_view_derived_instance_init;
		midgard_type_info->value_table = NULL;
		GType type = g_type_register_static(
				MIDGARD_TYPE_VIEW, class_name, midgard_type_info, 0);
		
		g_free(midgard_type_info);
		return type;
	}
}

/* MIDGARD_VIEW ABSTRACT CLASS ROUTINES */

static GObjectClass *__parent_class= NULL;

static GObject *
__midgard_view_constructor (GType type,
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
__midgard_view_dispose (GObject *object)
{
	__parent_class->dispose (object);
}

static void _midgard_view_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardViewClass *klass = MIDGARD_VIEW_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = __midgard_view_constructor;
	gobject_class->dispose = __midgard_view_dispose;
	gobject_class->finalize = __parent_class->finalize;
	gobject_class->set_property = NULL;
	gobject_class->get_property = NULL;

	klass->get_connection = MIDGARD_DBOBJECT_CLASS(__parent_class)->get_connection;
}

GType midgard_view_get_type(void)
{
       	static GType type = 0;
 
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardViewClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_view_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardView),
			0,              /* n_preallocs */
			NULL		/* instance_init */
		};

		type = g_type_register_static (MIDGARD_TYPE_DBOBJECT, "MidgardView", &info, G_TYPE_FLAG_ABSTRACT);
	}

	return type;
}

