/* 
 * Copyright (C) 2012 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include <libgda/libgda.h>
#include "midgard_sql_query_result.h"
#include "../midgard_query_row.h"
#include "../midgard_query_column.h"
#include "../midgard_query_result.h"
#include "../midgard_validable.h"
#include "../midgard_query_selector.h"

/**
 * midgard_sql_query_result_new:
 * @selector: #MidgardQuerySelector instance
 * @model: a GObject which represents data model
 *
 * This constructor should be used by #MidgardQuerySelector implementation
 *
 * Returns: new #MidgardSqlQueryResult
 *
 * Since: 10.05.6
 */ 
MidgardSqlQueryResult *             
midgard_sql_query_result_new (MidgardQuerySelector *selector, GObject *model)
{
	g_return_val_if_fail (model != NULL, NULL);
	MidgardSqlQueryResult *self = g_object_new (MIDGARD_TYPE_SQL_QUERY_RESULT, "selector", selector, "model", model, NULL);
	return self;
}

/* This is workaround function to set columns.
 * With GDA API, we can not introspect table column using GdaDataModel and GdaColumn */
void
midgard_sql_query_result_set_columns (MidgardSqlQueryResult *self, MidgardSqlQueryColumn **columns, guint n_columns, GError **error)
{
	g_return_if_fail (self != NULL);
	g_return_if_fail (columns != NULL);
	g_return_if_fail (n_columns != 0);

	if (self->columns != NULL) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_INTERNAL,
				"QueryResult holds columns already");
		return;
	}

	self->columns = g_new (MidgardSqlQueryColumn*, n_columns);
	self->n_columns = n_columns;
	guint i;

	for (i = 0; i < n_columns; i++) {
		if (columns[i] != NULL)
			self->columns[i] = g_object_ref (columns[i]);
		else 
			self->columns[i] = NULL;
	}
	return;
}

void                 
_propagate_columns (MidgardSqlQueryResult *self, guint *n_objects, GError **error)
{
	/* No model, no columns. Return NULL and set error */
	if (self->model == NULL || self->n_columns == 0) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_INTERNAL,
				"QueryResult holds empty data model"); 
		return;
	}

	guint i = 0;
	if (self->columns != NULL) {
		*n_objects = self->n_columns;
		return;
	}

	g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_INTERNAL,
			"Columns should be propagated with temporary midgard_sql_query_result_set_columns()");
	return;

	GdaDataModel *model = GDA_DATA_MODEL (self->model);
	self->n_columns = gda_data_model_get_n_columns (model);
	*n_objects = self->n_columns;
	if (self->n_columns == 0)
		return;

	self->columns = g_new (MidgardSqlQueryColumn*, self->n_columns);
	for (i = 0; i < self->n_columns; i++) {	
		MidgardQueryProperty *query_property = 
			midgard_query_property_new (gda_data_model_get_column_title (model, i), NULL);
		self->columns[i] = midgard_sql_query_column_new (query_property, "FIXME", 
			gda_data_model_get_column_name (model, i));
	}

	return;
}

GObject **                 
_midgard_sql_query_result_get_objects (MidgardQueryResult *self, guint *n_objects, GError **error)
{
	/* TODO */
	return NULL;
}

MidgardQueryColumn **                 
_midgard_sql_query_result_get_columns (MidgardQueryResult *result, guint *n_objects, GError **error)
{
	MidgardSqlQueryResult *self = MIDGARD_SQL_QUERY_RESULT (result);
	GError *err = NULL;
	_propagate_columns (self, n_objects, &err);
	*n_objects = self->n_columns;
	if (self->columns == NULL) {
		if (err)
			g_propagate_error (error, err);
		return NULL;
	}

	/* Create new columns array, add new reference to returned columns, caller should free array and unref objects */
	guint i;
	MidgardQueryColumn **columns = g_new (MidgardQueryColumn *, self->n_columns);
	for (i = 0; i < self->n_columns; i++) {
		columns[i] = (MidgardQueryColumn *) g_object_ref (self->columns[i]);	
	}
	
	return columns;
}

void
_propagate_rows (MidgardSqlQueryResult *self, guint *n_objects, GError **error)
{
	/* No model, no rows. Return NULL and set error */
	if (self->model == NULL || self->n_rows == 0) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_INTERNAL,
				"QueryResult holds empty data model"); 
		return;
	}

	/* We have rows, so add new reference to each and return all of them */
	guint i = 0;
	if (self->rows != NULL) {
		*n_objects = self->n_rows;
		for (i = 0; i < self->n_rows; i++) 
			g_object_ref(self->rows[i]);			
		return; 
	}

	GdaDataModel *model = GDA_DATA_MODEL (self->model);
	self->n_rows = gda_data_model_get_n_rows (model);
	if (self->n_rows == 0)
		return ;

	self->rows = g_new (MidgardSqlQueryRow*, self->n_rows);
	for (i = 0; i < self->n_rows; i++) {
		MidgardConnection *mgd = midgard_query_selector_get_connection (self->selector);
		self->rows[i] = midgard_sql_query_row_new (mgd, G_OBJECT (model), i);
		/* Row constructor holds new reference on connection, therefore decrease current one */
		g_object_unref (mgd);
	}
	
	*n_objects = self->n_rows;
}

MidgardQueryRow **
_midgard_sql_query_result_get_rows (MidgardQueryResult *result, guint *n_objects, GError **error)
{
	MidgardSqlQueryResult *self = MIDGARD_SQL_QUERY_RESULT (result);
        GError *err = NULL;
               _propagate_rows (self, n_objects, &err);
               *n_objects = self->n_rows;
        if (self->rows == NULL) {
                 if (err)
			 g_propagate_error (error, err);
                return NULL;
        }

        /* Create new rows array, add new reference to returned rows, caller should free array and unref objects */
        guint i;
        MidgardSqlQueryRow **rows = g_new (MidgardSqlQueryRow *, self->n_rows);
        for (i = 0; i < self->n_rows; i++) {
		rows[i] = (MidgardSqlQueryRow *) g_object_ref (self->rows[i]);
	}

        return (MidgardQueryRow **) rows;
}

gchar **                 
_midgard_sql_query_result_get_column_names (MidgardQueryResult *result, guint *n_names, GError **error)
{
	MidgardSqlQueryResult *self = MIDGARD_SQL_QUERY_RESULT (result);
	/* No model, no columns. Return NULL and set error */
	if (self->model == NULL || self->n_columns == 0) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_INTERNAL,
				"QueryResult holds empty data model"); 
		return NULL;
	}

	*n_names = 0;
	if (self->n_columns == 0)
		return NULL;

	GdaDataModel *model = GDA_DATA_MODEL (self->model);
	*n_names = gda_data_model_get_n_columns (model);
	gchar **names = g_new (gchar *, *n_names + 1);

	guint i;
	for (i = 0; i < *n_names; i++) {
		names[i] = (gchar *) gda_data_model_get_column_name (model, i);
	}
	
	return names;
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_MODEL = 1,
	PROPERTY_SELECTOR
};

static void
_midgard_sql_query_result_iface_init (MidgardQueryResultIFace *iface)
{
	iface->get_objects = _midgard_sql_query_result_get_objects;
	iface->get_columns = _midgard_sql_query_result_get_columns;
	iface->get_rows = _midgard_sql_query_result_get_rows;
	iface->get_column_names = _midgard_sql_query_result_get_column_names;
	return;
}

static void
_midgard_sql_query_result_iface_finalize (MidgardQueryResultIFace *iface)
{
	return;
}

static void 
_midgard_sql_query_result_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardSqlQueryResult *self = (MidgardSqlQueryResult*) instance;
	self->selector = NULL;
	self->model = NULL;
	self->columns = NULL;
	self->rows = NULL;
	self->n_columns = 0;
	self->n_rows = 0;

	return;
}

static GObject *
_midgard_sql_query_result_constructor (GType type,
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
_midgard_sql_query_result_dispose (GObject *object)
{
	MidgardSqlQueryResult *self = MIDGARD_SQL_QUERY_RESULT (object);
	if (self->model) 
		g_object_unref (self->model);
	self->model = NULL;

	if (self->selector)
		g_object_unref (self->selector);
	self->selector = NULL;

	guint i;
	if (self->columns != NULL) {
		for (i = 0; i < self->n_columns; i++) 
			g_object_unref (self->columns[i]);
	}
	self->columns = NULL;

	if (self->rows != NULL) {
		for (i = 0; i < self->n_rows; i++)
			g_object_unref (self->rows[i]);
	}
	self->rows = NULL;

	self->n_columns = 0;
	self->n_rows = 0;

	__parent_class->dispose (object);
}

static void 
_midgard_sql_query_result_finalize (GObject *object)
{
	__parent_class->finalize (object);
}

static void
_midgard_sql_query_result_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	MidgardSqlQueryResult *self = MIDGARD_SQL_QUERY_RESULT (object);

	switch (property_id) {
		
		case PROPERTY_MODEL:
			self->model = g_value_dup_object (value);
			self->n_columns = gda_data_model_get_n_columns (GDA_DATA_MODEL (self->model));
			self->n_rows = gda_data_model_get_n_rows (GDA_DATA_MODEL (self->model));
			break;

		case PROPERTY_SELECTOR:
			self->selector = g_value_dup_object (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;		
	}
}

static void
_midgard_sql_query_result_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	MidgardSqlQueryResult *self = (MidgardSqlQueryResult *) object;
	
	switch (property_id) {
		
		case PROPERTY_MODEL:
		case PROPERTY_SELECTOR:
			/* Write only */ 
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}

static void _midgard_sql_query_result_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardSqlQueryResultClass *klass = MIDGARD_SQL_QUERY_RESULT_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_sql_query_result_constructor;
	gobject_class->dispose = _midgard_sql_query_result_dispose;
	gobject_class->finalize = _midgard_sql_query_result_finalize;
	gobject_class->set_property = _midgard_sql_query_result_set_property;
	gobject_class->get_property = _midgard_sql_query_result_get_property;

	/* PROPERTIES */
	GParamSpec *pspec;
	const gchar *property_name;

	/* model */
	property_name = "model";
	pspec = g_param_spec_object (property_name,
			"Data model's row",
			"Holds a reference to data model",
			GDA_TYPE_DATA_MODEL,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROPERTY_MODEL, pspec);	 

	/* selector */
	property_name = "selector";
	pspec = g_param_spec_object (property_name,
			"QuerySelector",
			"SQL query select executor",
			MIDGARD_TYPE_QUERY_SELECTOR,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROPERTY_SELECTOR, pspec);	 
}

GType 
midgard_sql_query_result_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardSqlQueryResultClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_query_result_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlQueryResult),
			0,              /* n_preallocs */
			_midgard_sql_query_result_instance_init /* instance_init */
		};

		static const GInterfaceInfo iface_info = {
			(GInterfaceInitFunc) _midgard_sql_query_result_iface_init,
			(GInterfaceFinalizeFunc) _midgard_sql_query_result_iface_finalize,	
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSqlQueryResult", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_RESULT, &iface_info);
	}
	return type;
}

