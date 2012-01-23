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

#include "midgard_sql_query_row.h"
#include "../midgard_query_row.h"

/**
 * midgard_sql_query_row_new:
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
midgard_sql_query_row_new (GObject *model, guint row)
{
	g_return_val_if_fail (model != NULL, NULL);
	MidgardSqlQueryRow *self = g_object_new (MIDGARD_TYPE_SQL_QUERY_ROW, "model", model, "row", row, NULL);
	return self;
}


const GValue *                 
_midgard_sql_query_row_get_value (MidgardQueryRow *self, const gchar *column_name, GError **error)
{

}

GValueArray *
_midgard_sql_query_row_get_values (MidgardQueryRow *self)
{

}

GObject *
_midgard_sql_query_row_get_object (MidgardQueryRow *self, const gchar *column_name, GError **error)
{

}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_MODEL = 1,
	PROPERTY_ROW
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

