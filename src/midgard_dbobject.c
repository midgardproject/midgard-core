/* 
 * Copyright (C) 2007, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_dbobject.h"
#include "schema.h"
#include "midgard_core_object.h"
#include "midgard_core_object_class.h"
#include "midgard_object_class.h"

static GObjectClass *parent_class= NULL;

/* Create GdaSqlSelectField for every property registered for the class. */
void
_add_fields_to_select_statement (MidgardDBObjectClass *klass, GdaSqlStatementSelect *select, const gchar *table_name)
{
	guint n_prop;
	guint i;
	GdaSqlSelectField *select_field;
	GdaSqlExpr *expr;
	GValue *val;
	gchar *table_field;
	GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (klass), &n_prop);
	if (!pspecs)
		return;

	const gchar *property_table = NULL;

	for (i = 0; i < n_prop; i++) {

		const gchar *property = pspecs[i]->name;
		const gchar *property_field = midgard_core_class_get_property_colname (klass, property);
		property_table = midgard_core_class_get_property_table (klass, property); 
		if (property_table && table_name)
			property_table = table_name;

		/* Ignore properties with NULL storage and those of object type */
		if (!property_table || pspecs[i]->value_type == G_TYPE_OBJECT)
			continue;

       		select_field = gda_sql_select_field_new (GDA_SQL_ANY_PART (select));
		/*select_field->field_name = g_strdup (property_field);
		select_field->table_name = g_strdup (property_table);*/
		select_field->as = g_strdup (property);
		select->expr_list = g_slist_append (select->expr_list, select_field);
		expr = gda_sql_expr_new (GDA_SQL_ANY_PART (select_field));
		val = g_new0 (GValue, 1);
		g_value_init (val, G_TYPE_STRING);
		table_field = g_strconcat (property_table, ".", property_field, NULL);
		g_value_set_string (val, table_field);
		g_free (table_field);
		expr->value = val;
		select_field->expr = expr;
	}

	g_free (pspecs);

	if (!klass->dbpriv->has_metadata)
		return;

	/* Check if metadata provides own method to add fields. If not, use given class storage. */
	if (MIDGARD_IS_OBJECT_CLASS (klass)) {

		MidgardMetadataClass *mklass = (MidgardMetadataClass *) midgard_object_class_get_metadata_class (MIDGARD_OBJECT_CLASS (klass));
		if (!mklass)
			return;

		if (MIDGARD_DBOBJECT_CLASS (mklass)->dbpriv->add_fields_to_select_statement) {
			MIDGARD_DBOBJECT_CLASS (mklass)->dbpriv->add_fields_to_select_statement (MIDGARD_DBOBJECT_CLASS (mklass), select, table_name);
			return;
		}

		const gchar *table = midgard_core_class_get_table (klass);
		if (table_name)
			table = table_name;

		/* TODO, Once we stabilize use case, refactor this below to minimize code usage */
		GParamSpec **pspecs = g_object_class_list_properties (G_OBJECT_CLASS (mklass), &n_prop);
		if (!pspecs)
			return;
		
		for (i = 0; i < n_prop; i++) {

			const gchar *property = pspecs[i]->name;
			const gchar *property_field = midgard_core_class_get_property_colname (MIDGARD_DBOBJECT_CLASS (mklass), property);	

			if (pspecs[i]->value_type == G_TYPE_OBJECT)
				continue;
			
			select_field = gda_sql_select_field_new (GDA_SQL_ANY_PART (select));
			/*select_field->field_name = g_strdup (property_field);
			select_field->table_name = g_strdup (table);*/
			select_field->as = g_strdup (property);
			select->expr_list = g_slist_append (select->expr_list, select_field);
			expr = gda_sql_expr_new (GDA_SQL_ANY_PART (select_field));
			val = g_new0 (GValue, 1);
			g_value_init (val, G_TYPE_STRING);
			table_field = g_strconcat (table, ".", property_field, NULL);
			g_value_set_string (val, table_field);
			g_free (table_field);
			expr->value = val;
			select_field->expr = expr;
		}

		g_free (pspecs);
	}

	return;
}

static GObject *
midgard_dbobject_constructor (GType type,
		guint n_construct_properties,
		GObjectConstructParam *construct_properties) 
{
	GObject *object = (GObject *)
		G_OBJECT_CLASS (parent_class)->constructor (type,
				n_construct_properties,
				construct_properties);

	MIDGARD_DBOBJECT (object)->dbpriv = g_new(MidgardDBObjectPrivate, 1);
	MIDGARD_DBOBJECT (object)->dbpriv->mgd = NULL; /* read only */
	MIDGARD_DBOBJECT (object)->dbpriv->guid = NULL;
	MIDGARD_DBOBJECT (object)->dbpriv->datamodel = NULL;
	MIDGARD_DBOBJECT (object)->dbpriv->row = -1;	
	MIDGARD_DBOBJECT (object)->dbpriv->has_metadata = TRUE;
	MIDGARD_DBOBJECT (object)->dbpriv->metadata = NULL;

	MIDGARD_DBOBJECT (object)->dbpriv->storage_data =
		MIDGARD_DBOBJECT_GET_CLASS (object)->dbpriv->storage_data;

	return G_OBJECT(object);
}

static void 
midgard_dbobject_dispose (GObject *object)
{
	MidgardDBObject *self = MIDGARD_DBOBJECT (object);
	if (self->dbpriv->datamodel && G_IS_OBJECT (self->dbpriv->datamodel))
		g_object_unref(self->dbpriv->datamodel);

	self->dbpriv->row = -1;

	/* Do not nullify metadata object, we might be in the middle of refcount decreasing */
	if (self->dbpriv->metadata && G_IS_OBJECT (self->dbpriv->metadata)) 
		g_object_unref (self->dbpriv->metadata);

	parent_class->dispose (object);
}


static void 
midgard_dbobject_finalize (GObject *object)
{
	MidgardDBObject *self = MIDGARD_DBOBJECT(object);

	if (!self)
		return;

	if (!self->dbpriv)
		return;

	g_free((gchar *)self->dbpriv->guid);
	self->dbpriv->guid = NULL;

	g_free(self->dbpriv);
	self->dbpriv = NULL;

	parent_class->finalize (object);
}

static const MidgardConnection *__get_connection(MidgardDBObject *self)
{
	g_assert(self != NULL);
	return self->dbpriv->mgd;
}

static void 
midgard_dbobject_class_init (MidgardDBObjectClass *klass, gpointer g_class_data)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	parent_class = g_type_class_peek_parent (klass);
	
	object_class->constructor = midgard_dbobject_constructor;
	object_class->dispose = midgard_dbobject_dispose;
	object_class->finalize = midgard_dbobject_finalize;

	klass->get_connection = __get_connection;

	klass->dbpriv = g_new (MidgardDBObjectPrivate, 1);
	klass->dbpriv->create_storage = NULL;
	klass->dbpriv->update_storage = NULL;
	klass->dbpriv->storage_exists = NULL;
	klass->dbpriv->delete_storage = NULL;
	klass->dbpriv->add_fields_to_select_statement = _add_fields_to_select_statement;
}

/* Registers the type as a fundamental GType unless already registered. */ 
GType 
midgard_dbobject_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardDBObjectClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) midgard_dbobject_class_init, 
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardDBObject),
			0,              /* n_preallocs */  
			NULL
		};
		type = g_type_register_static (G_TYPE_OBJECT, "MidgardDBObject", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
