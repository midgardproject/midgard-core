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
#include "midgard_sql_query_row.h"
#include "../midgard_query_row.h"
#include "../midgard_validable.h"

/**
 * midgard_sql_query_row_new:
 * @mgd: #MidgardConnection instance
 * @model: a GObject which represents data model
 * @row: Row's index in a given model
 *
 * This constructor should be used by #MidgardQueryResult implementation
 *
 * Returns: new #MidgardSqlQueryRow
 *
 * Since: 10.05.6
 */ 
MidgardSqlQueryRow *             
midgard_sql_query_row_new (MidgardConnection *mgd, GObject *model, guint row)
{
	g_return_val_if_fail (model != NULL, NULL);
	MidgardSqlQueryRow *self = g_object_new (MIDGARD_TYPE_SQL_QUERY_ROW, "connection", mgd, "model", model, "row", row, NULL);
	return self;
}


const GValue *                 
_midgard_sql_query_row_get_value (MidgardQueryRow *self, const gchar *column_name, GError **error)
{
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (column_name != NULL, NULL);
	GdaDataModel *model = GDA_DATA_MODEL (MIDGARD_SQL_QUERY_ROW(self)->model);
	g_return_val_if_fail (model != NULL, NULL);

	if (!(gda_data_model_get_access_flags (model) & GDA_DATA_MODEL_ACCESS_CURSOR)) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_INTERNAL,
				"Expected data model iterator");
		return NULL;
	}

	if (GDA_IS_DATA_MODEL_ITER (model))
		return gda_data_model_iter_get_value_for_field (GDA_DATA_MODEL_ITER (model), column_name);

	return gda_data_model_get_value_at (GDA_DATA_MODEL (model), 
		gda_data_model_get_column_index(model, column_name), 
		MIDGARD_SQL_QUERY_ROW(self)->row, 
		NULL
	);
}

GValueArray *
_midgard_sql_query_row_get_values (MidgardQueryRow *self, GError **error)
{
	g_return_val_if_fail (self != NULL, NULL);
	GdaDataModel *model = GDA_DATA_MODEL (MIDGARD_SQL_QUERY_ROW (self)->model);
	g_return_val_if_fail (model != NULL, NULL);

	GdaDataModelIter *iter = GDA_DATA_MODEL_ITER (model);
	if (gda_data_model_iter_move_to_row (iter, 0)) {
		g_set_error (error, MIDGARD_VALIDATION_ERROR, MIDGARD_VALIDATION_ERROR_INTERNAL,
				"Can not reset model iterator");
	}

	guint cols = gda_data_model_get_n_columns (model);
	GValueArray *varray = g_value_array_new (cols);

	while (gda_data_model_iter_move_next (iter) == TRUE) {
		gint row_id = gda_data_model_iter_get_row (iter);
		g_value_array_append (varray, gda_data_model_iter_get_value_at (iter, row_id));
	}

	return varray;
}

GObject *
_midgard_sql_query_row_get_object (MidgardQueryRow *self, const gchar *column_name, GError **error)
{
	GError *err = NULL;
	const GValue *val = midgard_query_row_get_value (self, column_name, &err);
	if (val == NULL) {
		g_propagate_error (error, err);
		return NULL;
	}

	/* TODO, get storage and class name 
	const gchar *classname = NULL; 
	MidgardObject *object = midgard_object_new(self->connection, classname, val);
	if (!object) {
		g_set_error (MIDGARD_GENERIC_ERROR, MIDGARD_ERR_NOT_EXISTS,
				"Can not fetch object for given '%s' column name", column_name);
		return NULL;
	}
	return object;
	*/

	return NULL;
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_MODEL = 1,
	PROPERTY_ROW, 
	PROPERTY_CONNECTION
};

static void
_midgard_sql_query_row_iface_init (MidgardQueryRowIFace *iface)
{
	iface->get_value = _midgard_sql_query_row_get_value;
	iface->get_values = _midgard_sql_query_row_get_values;
	iface->get_object = _midgard_sql_query_row_get_object;
	return;
}

static void
_midgard_sql_query_row_iface_finalize (MidgardQueryRowIFace *iface)
{
	return;
}


static void 
_midgard_sql_query_row_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardSqlQueryRow *self = (MidgardSqlQueryRow*) instance;
	self->model = NULL;

	return;
}

static GObject *
_midgard_sql_query_row_constructor (GType type,
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
_midgard_sql_query_row_dispose (GObject *object)
{
	MidgardSqlQueryRow *self = MIDGARD_SQL_QUERY_ROW (object);
	if (self->model) 
		g_object_unref (self->model);
	self->model = NULL;

	if (self->mgd)
		g_object_unref (self->mgd);
	self->mgd = NULL;

	__parent_class->dispose (object);
}

static void 
_midgard_sql_query_row_finalize (GObject *object)
{
	__parent_class->finalize (object);
}

static void
_midgard_sql_query_row_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	MidgardSqlQueryRow *self = MIDGARD_SQL_QUERY_ROW (object);

	switch (property_id) {
		
		case PROPERTY_MODEL:
			self->model = g_value_dup_object (value);
			break;

		case PROPERTY_CONNECTION:
			self->mgd = g_value_dup_object (value);
			break;

		case PROPERTY_ROW:
			self->row = g_value_get_uint (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;		
	}
}

static void
_midgard_sql_query_row_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	MidgardSqlQueryRow *self = (MidgardSqlQueryRow *) object;
	
	switch (property_id) {
		
		case PROPERTY_MODEL:
		case PROPERTY_ROW:
		case PROPERTY_CONNECTION:
			/* Write only */ 
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}



static void _midgard_sql_query_row_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardSqlQueryRowClass *klass = MIDGARD_SQL_QUERY_ROW_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_sql_query_row_constructor;
	gobject_class->dispose = _midgard_sql_query_row_dispose;
	gobject_class->finalize = _midgard_sql_query_row_finalize;
	gobject_class->set_property = _midgard_sql_query_row_set_property;
	gobject_class->get_property = _midgard_sql_query_row_get_property;

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

	/* connection */
	property_name = "connection";
	pspec = g_param_spec_object (property_name,
			"Connection",
			"Connection to underlying database",
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROPERTY_MODEL, pspec);	 

	/* row */
	property_name = "row";
	pspec = g_param_spec_uint (property_name, 
			"Rows' index", 
			"Index of a row in a model",
			0, G_MAXUINT32, 0, 
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROPERTY_ROW, pspec);	 
}

GType 
midgard_sql_query_row_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardSqlQueryRowClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_query_row_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlQueryRow),
			0,              /* n_preallocs */
			_midgard_sql_query_row_instance_init /* instance_init */
		};

		static const GInterfaceInfo iface_info = {
			(GInterfaceInitFunc) _midgard_sql_query_row_iface_init,
			(GInterfaceFinalizeFunc) _midgard_sql_query_row_iface_finalize,	
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSqlQueryRow", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_ROW, &iface_info);
	}
	return type;
}

