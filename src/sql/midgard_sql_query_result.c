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

/**
 * midgard_sql_query_result_new:
 * @mgd: #MidgardConnection instance
 * @model: a GObject which represents data model
 *
 * This constructor should be used by #MidgardQuerySelector implementation
 *
 * Returns: new #MidgardSqlQueryResult
 *
 * Since: 10.05.6
 */ 
MidgardSqlQueryResult *             
midgard_sql_query_result_new (MidgardConnection *mgd, GObject *model)
{
	g_return_val_if_fail (model != NULL, NULL);
	MidgardSqlQueryResult *self = g_object_new (MIDGARD_TYPE_SQL_QUERY_RESULT, "connection", mgd, "model", model, NULL);
	return self;
}


GObject **                 
_midgard_sql_query_result_get_objects (MidgardQueryResult *self, guint *n_objects, GError **error)
{

}

MidgardQueryColumn **                 
_midgard_sql_query_result_get_columns (MidgardQueryResult *self, guint *n_objects, GError **error)
{

}

MidgardQueryRow **                 
_midgard_sql_query_result_get_rows (MidgardQueryResult *self, guint *n_objects, GError **error)
{

}

gchar **                 
_midgard_sql_query_result_get_column_names (MidgardQueryResult *self, guint *n_names, GError **error)
{

}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_MODEL = 1,
	PROPERTY_CONNECTION
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
	self->model = NULL;

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

	if (self->mgd)
		g_object_unref (self->mgd);
	self->mgd = NULL;

	guint i = 0;
	GObject **objects = NULL;
	if (self->columns != NULL) {
		for (objects = (GObject **)self->columns; objects[i] != NULL; i++)
			g_object_unref (objects[i]);
	}
	self->columns = NULL;

	i = 0;
	if (self->rows != NULL) {
		for (objects = (GObject **)self->rows; objects[i] != NULL; i++)
			g_object_unref (objects[i]);
	}
	self->rows = NULL;

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
			break;

		case PROPERTY_CONNECTION:
			self->mgd = g_value_dup_object (value);
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
		case PROPERTY_CONNECTION:
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

	/* connection */
	property_name = "connection";
	pspec = g_param_spec_object (property_name,
			"Connection",
			"Connection to underlying database",
			MIDGARD_TYPE_CONNECTION,
			G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROPERTY_MODEL, pspec);	 
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

