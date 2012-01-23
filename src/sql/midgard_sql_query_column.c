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

#include "midgard_sql_query_column.h"
#include "../midgard_query_column.h"

/**
 * midgard_sql_query_column_new:
 * @query_property: #MidgardQueryProperty instance 
 * @name: column name
 * 
 * Returns: #MidgardSqlQueryColumn instance
 * 
 * Since: 10.06
 */ 
MidgardSqlQueryColumn *
midgard_sql_query_column_new (MidgardQueryProperty *query_property, const gchar *name)
{
	MidgardSqlQueryColumn *self = g_object_new (MIDGARD_TYPE_SQL_QUERY_COLUMN, "query_property", query_property, "name", name, NULL);
	return self;
}

MidgardQueryProperty *
_midgard_sql_query_column_get_query_property (MidgardQueryColumn *iface, GError **error)
{
	MidgardSqlQueryColumn *self = (MidgardSqlQueryColumn*) iface;
	g_return_val_if_fail (self != NULL, NULL);
	/* TODO, set error if query_property is NULL */
	return g_object_ref (self->query_property);
}

gchar *
_midgard_sql_query_column_get_name (MidgardQueryColumn *iface, GError **error)
{
	MidgardSqlQueryColumn *self = (MidgardSqlQueryColumn*) iface;
	g_return_val_if_fail (self != NULL, NULL);
	/* TODO, set error if name is NULL */
	return self->name;
}

/* GOBJECT ROUTINES */

static GObjectClass *__parent_class= NULL;

enum {
	PROPERTY_QUERY_PROP = 1,
	PROPERTY_NAME
};

static void
_midgard_sql_query_column_iface_init (MidgardQueryColumnIFace *iface)
{
	iface->get_query_property = _midgard_sql_query_column_get_query_property;
	iface->get_name = _midgard_sql_query_column_get_name;
	return;
}

static void
_midgard_sql_query_column_iface_finalize (MidgardQueryColumnIFace *iface)
{
	return;
}


static void 
_midgard_sql_query_column_instance_init (GTypeInstance *instance, gpointer g_class)
{
	MidgardSqlQueryColumn *self = (MidgardSqlQueryColumn*) instance;
	self->query_property = NULL;
	self->name = NULL;

	return;
}

static GObject *
_midgard_sql_query_column_constructor (GType type,
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
_midgard_sql_query_column_dispose (GObject *object)
{
	MidgardSqlQueryColumn *self = MIDGARD_SQL_QUERY_COLUMN (object);
	if (self->query_property) 
		g_object_unref (self->query_property);
	self->query_property = NULL;

	if (self->name)
		g_free(self->name);
	self->name = NULL;

	__parent_class->dispose (object);
}

static void 
_midgard_sql_query_column_finalize (GObject *object)
{
	__parent_class->finalize (object);
}

static void
_midgard_sql_query_column_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	MidgardSqlQueryColumn *self = MIDGARD_SQL_QUERY_COLUMN (object);

	switch (property_id) {
		
		case PROPERTY_QUERY_PROP:
			self->query_property = g_value_dup_object (value);
			break;

		case PROPERTY_NAME:
			g_free (self->name);
			self->name = g_value_dup_string (value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;		
	}
}

static void
_midgard_sql_query_column_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	MidgardSqlQueryColumn *self = (MidgardSqlQueryColumn *) object;
	
	switch (property_id) {
		
		case PROPERTY_QUERY_PROP:
		case PROPERTY_NAME:
			/* Read only */ 
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, pspec);
			break;
	}
}



static void _midgard_sql_query_column_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardSqlQueryColumnClass *klass = MIDGARD_SQL_QUERY_COLUMN_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->constructor = _midgard_sql_query_column_constructor;
	gobject_class->dispose = _midgard_sql_query_column_dispose;
	gobject_class->finalize = _midgard_sql_query_column_finalize;
	gobject_class->set_property = _midgard_sql_query_column_set_property;
	gobject_class->get_property = _midgard_sql_query_column_get_property;

	/* PROPERTIES */
	GParamSpec *pspec;
	const gchar *property_name;

	/* queryproperty */
	property_name = "queryproperty";
	pspec = g_param_spec_object (property_name,
			"QueryProperty",
			"Holds a reference to property name and it's storage",
			MIDGARD_TYPE_SQL_QUERY_COLUMN,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROPERTY_QUERY_PROP, pspec);	 

	/* name */
	property_name = "name";
	pspec = g_param_spec_string (property_name,
			"SqlQueryColumn name",
			"",
			"",
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property (gobject_class, PROPERTY_NAME, pspec);
}

GType 
midgard_sql_query_column_get_type(void)
{
       	static GType type = 0;
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardSqlQueryColumnClass),
			NULL,	/* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_sql_query_column_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardSqlQueryColumn),
			0,              /* n_preallocs */
			_midgard_sql_query_column_instance_init /* instance_init */
		};

		static const GInterfaceInfo iface_info = {
			(GInterfaceInitFunc) _midgard_sql_query_column_iface_init,
			(GInterfaceFinalizeFunc) _midgard_sql_query_column_iface_finalize,	
			NULL    /* interface_data */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardSqlQueryColumn", &info, 0);
		g_type_add_interface_static (type, MIDGARD_TYPE_QUERY_COLUMN, &iface_info);
	}
	return type;
}

