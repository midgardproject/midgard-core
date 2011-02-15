/* 
 * Copyright (C) 2006, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_collector.h"
#include "midgard_object.h"
#include "query_builder.h"
#include "midgard_core_query_builder.h"
#include <libgda/libgda.h>
#include "midgard_core_query.h"
#include "midgard_core_object_class.h"
#include "midgard_core_object.h"

struct _MidgardCollectorPrivate{
	const gchar *typename;
	GType type;
	const gchar *domain;
	GValue *domain_value;
	const gchar *keyname;
	GValue *keyname_value;
	MidgardQueryBuilder *builder;
	MidgardObjectClass *klass;
	GList *values;
	GData *datalist;
	MidgardConnection *mgd;
};

static const gchar *_collector_find_class_property(
		MidgardCollector *self, const gchar *propname)
{
	const gchar *table_field = 
		midgard_core_class_get_property_tablefield(
				MIDGARD_DBOBJECT_CLASS(self->priv->klass), 
				propname);

	/* Let's try metadata class */
	if(table_field == NULL) {
		 
		MidgardMetadataClass *mklass =
			(MidgardMetadataClass*) g_type_class_peek(g_type_from_name("MidgardMetadata"));
		GParamSpec *pspec = g_object_class_find_property (G_OBJECT_CLASS (mklass), propname);
		/* We do not need table.col alias, so dummy empty string is fine in this case */
		if (pspec)
			return "";
	}

	return table_field;
}

static void _unset_subkey_value(gpointer value)
{
	if(value != NULL) {
		/* FIXME */
		/* g_value_unset((GValue *)value);
		g_free(value);
		value = NULL; */
	}
}

static void __unset_subkey_valuelist (gpointer value)
{
	if(value != NULL){
		/* FIXME */
		/* GData *datalist = (GData *)value;
		g_datalist_clear(&datalist);
		value = NULL; */
	}
}

/**
 * midgard_collector_new:
 * @mgd: #MidgardConnection instance
 * @typename: name of given class, which collector is initialized for 
 * @domain: collection' domain
 * @value: domain's constraint value
 * 
 * @typename should be any #MidgardDBObject derived class name.
 * @domain is property name which is registered for given class, and should not be 
 * unique per object or record. In other words, @domain is a common property (and value)
 * for group of objects expected in collection.
 * @value must be of domain property type, and is owned by #MidgardCollector instance.
 * If you need reuse given value, make a copy. 
 *
 * Cases to return NULL:
 *
 * <itemizedlist>
 * <listitem><para>
 * @mgd #MidgardConnection instance is invalid
 * </para></listitem>
 * <listitem><para>
 * @typename is not registered in GType system or it's not #MidgardDBObject derived 
 * </para></listitem>
 * <listitem><para>
 * @domain property is not registered for given @typename class 
 * </para></listitem>
 * <listitem><para>
 * @value is of invalid type
 * </para></listitem>
 * </itemizedlist> 
 * 
 * Returns: #MidgardCollector instance, or %NULL on failure 
 */ 
MidgardCollector *midgard_collector_new(
		MidgardConnection *mgd, const gchar *typename, 
		const gchar *domain, GValue *value)
{
	g_assert(mgd != NULL);
	g_assert(typename != NULL);
	g_assert(domain != NULL);
	g_assert(value != NULL);

	MidgardCollector *self = 
		(MidgardCollector *)g_object_new(MIDGARD_TYPE_COLLECTOR, NULL);
	
	self->priv->klass =
		MIDGARD_OBJECT_GET_CLASS_BY_NAME(typename);
	
	 /* Initialize private QB instance and set domain as constraint */
	self->priv->builder = midgard_query_builder_new(mgd, typename);
	midgard_query_builder_add_constraint(self->priv->builder,
			domain,
			"=", value);

	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_CLASS(self->priv->klass);
	MidgardCoreQueryConstraint *constraint = midgard_core_query_constraint_new();
	midgard_core_query_constraint_set_builder(constraint, self->priv->builder);
	midgard_core_query_constraint_set_class(constraint, klass);

	if(!midgard_core_query_constraint_parse_property(&constraint, klass, domain)) {
		
		g_object_unref(constraint);
		
		/* FIXME */
		if(value) {
			
			g_value_unset(value);
			g_free(value);
		}
		
		g_object_unref(self);
		
		return NULL;
	}
	
	g_object_unref(constraint);

        self->priv->typename = (const gchar *) g_strdup(typename);
	self->priv->type = g_type_from_name(typename);
	self->priv->domain = (const gchar*) g_strdup(domain);
	self->priv->domain_value = value;
	self->priv->keyname = NULL;
	self->priv->keyname_value = NULL;
	self->priv->mgd = mgd;
	
	g_value_unset(value);
	g_free(value);
	
	self->priv->values = NULL;
	g_datalist_init(&self->priv->datalist);

	return self;
}

/** 
 * midgard_collector_set_key_property:
 * @self: #MidgardCollector instance
 * @key: property name which must be a key
 * @value: optional value of key's constraint
 * 
 * If @value is explicitly set to NULL , then all key property name's records
 * are selected from database and set in internal collector's resultset. 
 * If not, key property name and its value is used as constraint to limit selected
 * records from database. In latter case add_value_property method should be invoked.
 * GValue value passed as third argument is owned by Midgard Collector. If value
 * should be reused, its copy should be passed to constructor.
 * 
 * Cases to return FALSE:
 *
 * <itemizedlist> 
 * <listitem><para>
 * @self #MidgardCollector instance is invalid
 * </para></listitem>
 * <listitem><para>
 * @key property is not registered for the @typename class 
 * </para></listitem>
 * <listitem><para>
 * @value is of invalid type
 * </para></listitem>
 * <listitem><para>
 * @key property is already set for @self #MidgardCollector 
 * </para></listitem>
 * </itemizedlist>
 *
 * Returns: %TRUE if key has been set, %FALSE otherwise 
 */
gboolean midgard_collector_set_key_property(
		MidgardCollector *self, const gchar *key, GValue *value)
{
	g_assert(self != NULL);

	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_CLASS(g_type_class_peek(self->priv->builder->priv->type));
	MidgardCoreQueryConstraint *constraint = midgard_core_query_constraint_new();
	midgard_core_query_constraint_set_builder(constraint, self->priv->builder);
	midgard_core_query_constraint_set_class(constraint, klass);
	GdaConnection *cnc = self->priv->mgd->priv->connection;

	if(!midgard_core_query_constraint_parse_property(&constraint, klass, key)) {
		
		g_object_unref(constraint);
		return FALSE;
	}

	/* Check if property is private */
	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_CLASS (constraint->priv->klass);
	MgdSchemaTypeAttr *type_attr = dbklass->dbpriv->storage_data;
	if (midgard_core_object_property_refuse_private (self->priv->mgd, type_attr, NULL, constraint->priv->propname)) {
	
		g_object_unref(constraint);
		return FALSE;
	}

	gchar *q_table = gda_connection_quote_sql_identifier (cnc, constraint->priv->current->table);
	gchar *q_field = gda_connection_quote_sql_identifier (cnc, constraint->priv->current->field);
	gchar *sql_field = g_strconcat(q_table, ".", q_field, " AS midgard_collector_key", NULL);
	g_free (q_table);
	g_free (q_field);
	
	g_object_unref(constraint);

	self->priv->values = g_list_prepend(self->priv->values,
			sql_field);
	
	self->priv->keyname = (const gchar*) g_strdup(key);
	
	/* add constraint if the value is set*/
	if(value){
		midgard_query_builder_add_constraint(
				self->priv->builder,
				key,
				"=", value);
		self->priv->keyname_value = value;
	}
	
	return TRUE;
}

/**
 * midgard_collector_add_value_property:
 * @self: #MidgardCollector instance
 * @value: property name
 *
 * Cases to return FALSE:
 * <itemizedlist>
 * <listitem><para>
 * @self, #MidgardCollector object is invalid
 * </para></listitem>
 * <listitem><para>
 * @value property is not registered for the @class, which has been initialized for given #MidgardCollector
 * </para></listitem> 
 * </itemizedlist>
 * 
 * Number of value properties added to Midgard Collector is limited by
 * the number of properties registered for type which has been initialized
 * for the given #MidgardCollector instance.
 * 
 * @See: midgard_query_builder_add_constraint() for available property name pattern
 *
 * Returns: %TRUE if named value property has been added, %FALSE otherwise
 */ 
gboolean midgard_collector_add_value_property(
		MidgardCollector *self, const gchar *value)
{
	g_assert(self != NULL);
	g_assert(value != NULL);

	if(!self->priv->keyname){
		g_warning("Collector's key is not set. Call set_key_property method");
		return FALSE;
	}
	
	MidgardDBObjectClass *klass = MIDGARD_DBOBJECT_CLASS(g_type_class_peek(self->priv->builder->priv->type));
	MidgardCoreQueryConstraint *constraint = midgard_core_query_constraint_new();
	midgard_core_query_constraint_set_builder(constraint, self->priv->builder);
	midgard_core_query_constraint_set_class(constraint, klass);
	GdaConnection *cnc = self->priv->mgd->priv->connection;

	if(!midgard_core_query_constraint_parse_property(&constraint, klass, value)) {
		
		g_object_unref(constraint);
		return FALSE;
	}
	
	/* Check if property is private */
	MidgardDBObjectClass *dbklass = MIDGARD_DBOBJECT_CLASS (constraint->priv->klass);
	MgdSchemaTypeAttr *type_attr = dbklass->dbpriv->storage_data;
	if (midgard_core_object_property_refuse_private (self->priv->mgd, type_attr, NULL, constraint->priv->propname)) {
	
		g_object_unref(constraint);
		return FALSE;
	}

	gchar *q_table = gda_connection_quote_sql_identifier (cnc, constraint->priv->current->table);
	gchar *q_field = gda_connection_quote_sql_identifier (cnc, constraint->priv->current->field);
	gchar *q_name = gda_connection_quote_sql_identifier (cnc, constraint->priv->pspec->name);
	gchar *sql_field = g_strconcat (q_table, ".", q_field, " AS ", q_name, NULL);
	g_free (q_table);
	g_free (q_field);
	g_free (q_name);

	g_object_unref(constraint);
	self->priv->values =
		g_list_prepend(self->priv->values, sql_field);
	
	return TRUE;
}

/**
 * midgard_collector_set:
 * @self: #MidgardCollector instance
 * @key: key name for which @subkey&@value pair should be set
 * @subkey: property name which is a subkey
 * @value: value for given @subkey
 *  
 * Cases to return FALSE:
 * <itemizedlist>
 * <listitem><para>
 * @self, #MidgardCollector instance is invalid
 * </para></listitem>
 * <listitem><para>
 * @subkey property name is not registered for collestor's class
 * </para></listitem>
 * <listitem><para>
 * @value is of invalid type
 * </para></listitem>
 * </itemizedlist>
 * 
 * If the key is already added to MidgardCollector then its value 
 * (as subkey&value pair) is destroyed and new one is set. 
 * In other case new key and its subkey&value pair is added  to collector.
 * 
 * Key used in this function is a value returned ( or set ) for collector's key.
 * Keys are collection of values returned from property fields.  
 * Subkey is an explicit  property name.
 *
 * GValue @value argument is owned by MidgardCollector.
 * If value should be reused, its copy should be passed as method argument.
 * 
 * Returns: %TRUE, if key's value has been set, %FALSE otherwise
 */
gboolean 
midgard_collector_set (MidgardCollector *self, const gchar *key, const gchar *subkey, GValue *value)
{
	g_assert(self != NULL);
	GQuark keyquark = g_quark_from_string(key);
	GData *valueslist;
	GQuark subkeyquark;

	if(subkey == NULL){

		/* This is workaround. Far from being ellegant.
		 * datalist foreach doesn't call user function 
		 * if datalist's data isw NULL.
		 * Even if key quark is set*/
		subkeyquark = g_quark_from_string("0");
		valueslist = (GData *) g_datalist_id_get_data(
				&self->priv->datalist,
				subkeyquark);
		
		if(valueslist == NULL)
			g_datalist_init(&valueslist);
		
		GValue *val = g_new0(GValue, 1);
		g_value_init(val, G_TYPE_UINT);
		g_value_set_uint(val, 0);
		g_datalist_id_set_data_full (
				&valueslist,
				subkeyquark,
				val,
				_unset_subkey_value);

		g_datalist_id_set_data_full (
				&self->priv->datalist,
				keyquark,
				(gpointer) valueslist,
				__unset_subkey_valuelist);

		return TRUE;
	}

	const gchar *nick = 
		_collector_find_class_property(self, subkey);
	
	if(!nick)
		return FALSE;

	subkeyquark = g_quark_from_string(subkey);
	valueslist = (GData *) g_datalist_id_get_data(
			&self->priv->datalist,
			keyquark);
		
	if(valueslist == NULL)
		g_datalist_init(&valueslist);

	g_datalist_id_set_data_full (
			&valueslist,
			subkeyquark, 
			(gpointer) value,
			_unset_subkey_value);

	g_datalist_id_set_data_full (
			&self->priv->datalist,
			keyquark,
			(gpointer) valueslist,
			__unset_subkey_valuelist);

	return TRUE;
}

/**
 * midgard_collector_get:
 * @self: #MidgardCollector instance
 * @key: name of the key to look for
 * 
 * GData keys ( collector's subkeys ) are inserted to GData as
 * Quarks , so you must call g_quark_to_string if you need to get strings 
 * ( e.g. implementing hash table for language bindings ).
 *
 * Returns: #GData for the given key or %NULL if key is not found in collection
 */ 
GData*
midgard_collector_get (MidgardCollector *self, const gchar *key)
{
	g_assert(self);

	if(!self->priv->keyname){
		g_warning("Collector's key is not set. Call set_key_property method");
		return FALSE;
	}

	return (GData *) g_datalist_id_get_data (
			&self->priv->datalist,
			g_quark_from_string(key));
}

/**
 * midgard_collector_get_subkey:
 * @self: #MidgardCollector instance
 * @key: name of the key 
 * @subkey: name of key's subkey to look for
 * 
 * Returns: subkey's #GValue value or %NULL if not found
 */
GValue*
midgard_collector_get_subkey (MidgardCollector *self, const gchar *key, const gchar *subkey)
{
	g_assert(self);

	const gchar *nick;
	nick = _collector_find_class_property(self, subkey);
	if(!nick)
		return NULL;

	if(&self->priv->datalist == NULL){
		g_warning("Collector's key set with NULL value");
		return NULL;
	}

	GData *valueslist = g_datalist_id_get_data (
			&self->priv->datalist,
			g_quark_from_string(key));
		
	if(!valueslist) {
		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
				"midgard_collector: No subkeys associated with the given '%s' key",
				key);
		return NULL;
	}

	GValue *value =  g_datalist_id_get_data (
			&valueslist, 
			g_quark_from_string(subkey));

	return value;
}

static void __merge_datalist (	GQuark key_id,
				gpointer data,
				gpointer user_data)
{
	GValue *value = (GValue *) data;
	GData *datalist = (GData *) user_data;
	GValue *new_value = g_new0(GValue, 1);
	g_value_init(new_value, G_VALUE_TYPE(value));
	g_value_copy(value, new_value);

	g_datalist_id_set_data_full (
			&datalist,
			key_id,
			(gpointer) user_data,
			_unset_subkey_value);
}

/**
 * midgard_collector_merge:
 * @self: #MidgardCollector instance
 * @mc: #MidgardCollector instance 
 * @overwrite: whether overwrite collector's keys
 *
 *
 * If third overwrite parameter is set as TRUE then all keys which exists 
 * in @self and @mc collector's instance will be oberwritten in @self colection
 * instance. If set as FALSE , only those keys will be added, which do not exist
 * in @self collection and exist in @mc collection.
 * 
 * Cases to return FALSE:
 * <itemizedlist>
 * <listitem><para>
 * Second argument is not valid #MidgardCollector 
 * </para></listitem>
 * <listitem><para>
 * @mc, #MidgardCollector has no keys collection
 * </para></listitem>
 * </itemizedlist>
 * 
 * Returns: %TRUE if collections has been merged, %FALSE otherwise
 */ 
gboolean 
midgard_collector_merge (MidgardCollector *self, MidgardCollector *mc, gboolean overwrite)
{
	g_assert(self);
	g_assert(mc);

	if(!MIDGARD_COLLECTOR(mc)){
		g_warning("Second argument is not an instance of midgard_collector class!");
		return FALSE;
	}

	guint i = 0;
	gchar **keys = 
		midgard_collector_list_keys(mc);
	GData *self_datalist, *mc_datalist;

	if(!keys) return FALSE;

	while(keys[i] != NULL) {
		
		self_datalist = 
			midgard_collector_get(self, keys[i]);
		if((self_datalist && overwrite) || 
				(self_datalist == NULL)){
			
			mc_datalist = 
				midgard_collector_get(mc, keys[i]);

			if(!self_datalist) 
				g_datalist_init(&self_datalist);

			g_datalist_foreach(
					&mc_datalist,
					__merge_datalist,
					self_datalist);

			g_datalist_id_set_data_full (
					&self->priv->datalist,  
					0,
					NULL, 
					_unset_subkey_value);
			}	
		i++;
	}
	g_free((gchar *)keys);
	return TRUE;
}

typedef struct _dle _dle;

struct _dle{
	guint elements;	
};

static void __count_datalist_elements (	GQuark key_id,
					gpointer data,
					gpointer user_data)
{
	_dle **le = (_dle**) user_data;
	(*le)->elements++;
}

static void __get_collection_keys (	GQuark key_id,
					gpointer data,
					gpointer user_data)
{
	GList **list = user_data;
	*list = g_list_prepend(*list,
			(gchar *)g_quark_to_string(key_id));
}

/**
 * midgard_collector_list_key:
 * @self: #MidgardCollector instance
 * 
 * Returned array of string is newly created array with pointers to each string 
 * in array. It should be freed without any need to free each string.
 * g_free (instead of g_strfreev) should be used to free returned list.
 * 
 * Returns: %NULL terminated array of strings or %NULL
 */
gchar**
midgard_collector_list_keys (MidgardCollector *self)
{	
	g_assert(self);

	guint list_size = 0 , i = 0;
	_dle *le = g_new(_dle, 1);
	le->elements = 0;

	g_datalist_foreach (&self->priv->datalist,
			__count_datalist_elements,
			&le);

	if(le->elements == 0) {
		g_free(le);
		return NULL;
	}

	list_size = le->elements;
	g_free(le);
	
	gchar **keys = g_new(gchar *, list_size+1);
	GList *list = NULL;	

	g_datalist_foreach (&self->priv->datalist,
			__get_collection_keys,
			&list);

	if(list) {
		
		/* list is already reversed 
		 * it's done by prepending lists in __get_collection_keys */
		GList *l = NULL;
		for(l = list; l != NULL; l = l->next) {
			keys[i] = l->data;
			i++;
		}
		keys[i] = NULL;
		g_list_free(list);
		return keys;
	}
	
	g_free(keys);
	return NULL;
}

/**
 * midgard_collector_remove_key:
 * @self: #MidgardCollector instance
 * @key: name of the key in collector's collection
 * 
 * Removes key and associated value from the given #MidgardCollector instance. 
 * 
 * Returns: %TRUE if key (and its value) has been removed from collection, %FALSE otherwise  
 */
gboolean midgard_collector_remove_key(
		MidgardCollector *self, const gchar *key)
{
	g_assert(self != NULL);
	g_assert(key != NULL);

	GQuark keyquark = g_quark_from_string(key);

	GData *valueslist = g_datalist_id_get_data (
			&self->priv->datalist,
			keyquark);
	if(!valueslist)
		return FALSE;

	g_datalist_clear(&valueslist);
	g_datalist_id_remove_data(&self->priv->datalist, keyquark); 

	return TRUE;
}

/* Parent methods re-implementation */

gboolean midgard_collector_add_constraint(
		MidgardCollector *self, const gchar *name, 
		const gchar *op, const GValue *value)
{
	g_assert(self);
	return MIDGARD_QUERY_BUILDER_GET_CLASS(
			self->priv->builder)->add_constraint(
				self->priv->builder, name,
				op, value);
}

gboolean midgard_collector_add_constraint_with_property(
		MidgardCollector *self, const gchar *property_a, 
		const gchar *op, const gchar *property_b)
{
	g_assert(self != NULL);
	g_assert(property_a != NULL);
	g_assert(property_b != NULL);
	g_assert(op != NULL);
	return MIDGARD_QUERY_BUILDER_GET_CLASS(
			self->priv->builder)->add_constraint_with_property(
				self->priv->builder, 
				property_a, op, property_b);
}

gboolean midgard_collector_begin_group(
		MidgardCollector *self, const gchar *type)
{
	g_assert(self);
	return MIDGARD_QUERY_BUILDER_GET_CLASS(
			self->priv->builder)->begin_group(
				self->priv->builder, type);
}

gboolean midgard_collector_end_group(
		MidgardCollector *self)
{
	g_assert(self);
	return MIDGARD_QUERY_BUILDER_GET_CLASS(
			self->priv->builder)->end_group(
				self->priv->builder);
}

gboolean midgard_collector_add_order(
		MidgardCollector *self,
		const gchar *name, const gchar *dir)
{
	g_assert(self);
	return MIDGARD_QUERY_BUILDER_GET_CLASS(
			self->priv->builder)->add_order(
				self->priv->builder, name, dir);
}

void midgard_collector_set_offset(
		MidgardCollector *self, guint offset)
{
	g_assert(self != NULL);
	MIDGARD_QUERY_BUILDER_GET_CLASS(
			self->priv->builder)->set_offset(
				self->priv->builder, offset);
}

void midgard_collector_set_limit(
		MidgardCollector *self, guint limit)
{
	g_assert(self);
	MIDGARD_QUERY_BUILDER_GET_CLASS(
			self->priv->builder)->set_limit(
				self->priv->builder, limit);
}

void midgard_collector_count(
		MidgardCollector *self)
{
	g_assert(self);
	return;
}

gboolean midgard_collector_include_deleted(MidgardCollector *self)
{
	g_assert(self != NULL);
	MIDGARD_QUERY_BUILDER_GET_CLASS(
			self->priv->builder)->include_deleted(
				self->priv->builder);

	return TRUE;
}

/*
static void __set_value(GValue *val, gchar *field_value)
{
	switch(G_VALUE_TYPE(val)){
		
		case G_TYPE_STRING:
			g_value_set_string(val, (gchar *)field_value);
			break;
		
		case G_TYPE_UINT:
			g_value_set_uint(val, atoi(field_value));
			break;
		
		case G_TYPE_INT:
			g_value_set_int(val, atoi(field_value));
			break;
		
		case G_TYPE_FLOAT:
			g_value_set_float(val, g_ascii_strtod(field_value, NULL));
			break;
		
		case G_TYPE_BOOLEAN:
			g_value_set_boolean(val, atoi(field_value));
			break;
	}
}
*/

/**
 * midgard_collector_execute;
 * @self: #MidgardCollector instance
 * 
 * Executes SQL query and set internal keys&values collection.
 *
 * Overwritten #MidgardQueryBuilder execute method.
 * Unlike QB's execute method this one returns boolean value.
 * Resultset is stored inernally by #MidgardCollector instance.
 *
 * Cases to return FALSE:
 * <itemizedlist>
 * <listitem><para>
 * No key is associated, midgard_collector_set_key_property()
 * </para></listitem>
 * <listitem><para>
 * No value property is associated, midgard_collector_add_value_property()
 * </para></listitem>
 * <listitem><para>
 * Database provider returned SQL query syntax error
 * </para></listitem>
 * <listitem><para>
 * No record(s) matched  
 * </para></listitem>
 * </itemizedlist>
 * 
 * Returns: %TRUE in success, %FALSE otherwise
 */ 
gboolean 
midgard_collector_execute (MidgardCollector *self)
{
	g_assert(self);

	if(!self->priv->keyname){
		g_warning("Collector's key is not set. Call set_key_property method");
		return FALSE;
	}

	if(!self->priv->values)
		return FALSE;

        GList *list =
		g_list_reverse(self->priv->values);
	GString *sgs = g_string_new("");
	guint i = 0;
	for( ; list; list = list->next){
		if(i > 0)
			g_string_append(sgs, ", ");
		g_string_append(sgs, list->data);
		i++;
	} 

	gchar *select = g_string_free(sgs, FALSE);
	gchar *sql = midgard_core_qb_get_sql( 
			self->priv->builder, MQB_SELECT_FIELD, 
			select);
	
	if(!sql)
		return FALSE;
	
	GdaDataModel *model = 
		midgard_core_query_get_model(
				self->priv->builder->priv->mgd, sql);
	g_free(sql);
	
	if(!model)
		return FALSE;
	
	gint rows, columns;
	gint ret_rows, ret_fields;	
	const GValue *gda_value;
	const GValue *key_value;
	GValue *ck_value;
	GParamSpec *pspec = NULL;

	ret_rows = gda_data_model_get_n_rows(model);

	for (rows = 0; rows < ret_rows; rows++) {

		ret_fields =  gda_data_model_get_n_columns(model);	

		for (columns = 0; columns < ret_fields; columns++) {
			
			pspec = NULL;
			key_value = 
				midgard_data_model_get_value_at(model, 0, rows);

			gda_value =
				midgard_data_model_get_value_at(model, columns, rows); 
			GValue *new_value = g_new0(GValue, 1);
			const gchar *col_title = gda_data_model_get_column_title (model, columns);
			//g_value_init(new_value, G_VALUE_TYPE(gda_value));
			//g_value_init(new_value, G_TYPE_STRING);

			if (G_VALUE_TYPE(gda_value) == GDA_TYPE_TIMESTAMP) {

				g_value_init(new_value, MGD_TYPE_TIMESTAMP);
				g_value_transform(gda_value, new_value);
			
			} else if (G_VALUE_TYPE (gda_value) == GDA_TYPE_BLOB) {
				gchar *stringified = midgard_core_query_binary_stringify ((GValue*)gda_value);
				g_value_init(new_value, G_TYPE_STRING);
				g_value_take_string(new_value, stringified);
			} else if (G_VALUE_TYPE (gda_value) == G_TYPE_STRING) {		
				g_value_init(new_value, G_TYPE_STRING);
				g_value_take_string(new_value, g_value_dup_string (gda_value));
			} else {
				/* Look for property's spec */
				pspec = g_object_class_find_property (G_OBJECT_CLASS (self->priv->klass), col_title);
				if (pspec) {
					g_value_init(new_value, pspec->value_type);
					if (G_VALUE_TYPE (gda_value) != pspec->value_type)
						g_value_transform(gda_value, new_value);
					else
						g_value_copy(gda_value, new_value);
				/* Try metadata class */
				} else {
					MidgardMetadataClass *mklass =
						(MidgardMetadataClass*) g_type_class_peek(g_type_from_name("MidgardMetadata"));
					pspec = g_object_class_find_property(G_OBJECT_CLASS(mklass), col_title);
					if (pspec) {
						g_value_init(new_value, pspec->value_type);
						if (G_VALUE_TYPE (gda_value) != pspec->value_type)
							g_value_transform(gda_value, new_value);
						else
							g_value_copy(gda_value, new_value);
					}
				}

				/* Try default string type */
				if (!pspec) {
					g_value_init(new_value, G_TYPE_STRING);
					if (G_VALUE_TYPE (gda_value) != G_TYPE_STRING)
						g_value_transform(gda_value, new_value);
					else
						g_value_copy(gda_value, new_value);
				}
			}

			ck_value = g_new0(GValue, 1);
			g_value_init(ck_value, G_TYPE_STRING);

			if(!G_VALUE_HOLDS_STRING(key_value)) {
				
				g_value_transform(key_value, ck_value);
			
			} else {
				
				g_value_copy(key_value, ck_value);
			}

			midgard_collector_set(self,
					g_value_get_string((GValue*)ck_value),
					col_title,
					new_value);
			
			if (ret_fields == 1){

				midgard_collector_set(self,
						g_value_get_string((GValue*)ck_value),
						NULL,
						NULL);

				g_value_unset (new_value);
				g_free (new_value);
			}

			g_value_unset(ck_value);
			//g_value_unset(new_value);
			g_free(ck_value);
		}
	}
	g_object_unref(model);
	return TRUE;	
}

/* GOBJECT ROUTINES */

static void _midgard_collector_instance_init(
		GTypeInstance *instance, gpointer g_class)
{
	MidgardCollector *self = (MidgardCollector *) instance;
	self->priv = g_new(MidgardCollectorPrivate, 1);

	self->priv->domain = NULL;
	self->priv->domain_value = NULL;
	self->priv->typename = NULL;
	self->priv->type = 0;
	self->priv->klass = NULL;
	self->priv->keyname = NULL;
	self->priv->datalist = NULL;
	self->priv->keyname_value = NULL;
	self->priv->builder = NULL;
	self->priv->values = NULL;
}

static void _midgard_collector_finalize(GObject *object)
{
	g_assert(object != NULL);

	MidgardCollector *self = (MidgardCollector *) object;

	if(self->priv->datalist)
		g_datalist_clear(&self->priv->datalist);
	
	g_free((gchar *)self->priv->typename);
	g_free((gchar *)self->priv->domain);
	g_free((gchar *)self->priv->keyname); 
	
	if(self->priv->keyname_value) {
		g_value_unset(self->priv->keyname_value);
		g_free(self->priv->keyname_value);
	}
	
	g_object_unref(self->priv->builder);

	for(GList *l = self->priv->values; l != NULL; l = l->next){
		g_free(l->data);
	}
	if (self->priv->values) {
		g_list_free(self->priv->values);
		self->priv->values = NULL;
	}

	g_free(self->priv);
}

static void _midgard_collector_class_init(
		gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardCollectorClass *klass = MIDGARD_COLLECTOR_CLASS (g_class);
	
	gobject_class->finalize = _midgard_collector_finalize;
	klass->set_key_property = midgard_collector_set_key_property;
	klass->add_value_property = midgard_collector_add_value_property;
	klass->set = midgard_collector_set;
	klass->get = midgard_collector_get;
	klass->get_subkey = midgard_collector_get_subkey;
	klass->merge = midgard_collector_merge;
	klass->list_keys = midgard_collector_list_keys;
	klass->remove_key = midgard_collector_remove_key;	
	klass->add_constraint = midgard_collector_add_constraint;
	klass->begin_group = midgard_collector_begin_group;
	klass->end_group = midgard_collector_end_group;
	klass->add_order = midgard_collector_add_order;
	klass->set_offset = midgard_collector_set_offset;
	klass->set_limit = midgard_collector_set_limit;
	klass->count = midgard_collector_count;
	klass->execute = midgard_collector_execute;
}

GType midgard_collector_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof (MidgardCollectorClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_collector_class_init,
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardCollector),
			0,              /* n_preallocs */
			(GInstanceInitFunc) _midgard_collector_instance_init /* instance_init */
		};	
		type = g_type_register_static (MIDGARD_TYPE_QUERY_BUILDER, "MidgardCollector", &info, 0);
	}
	return type;
}
