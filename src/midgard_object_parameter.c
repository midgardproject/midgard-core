/* 
 * Copyright (C) 2006 Piotr Pokora <piotr.pokora@infoglob.pl>
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

#include "src/midgard_core_object.h"
#include "midgard_object.h"
#include "midgard_collector.h"
#include "midgard_error.h"
#include "guid.h"
#include "midgard_core_object_parameter.h"
#include "midgard_blob.h"

static MidgardCollector *__create_domain_collector(MidgardConnection *mgd, const gchar *domain)
{
	/* Initialize collector for object and its domain */
	GValue *domain_value = g_new0(GValue, 1);
	g_value_init(domain_value, G_TYPE_STRING);
	g_value_set_string(domain_value, domain);
	
	MidgardCollector *mc =
		midgard_collector_new(mgd,
				"midgard_parameter",
				"domain", domain_value);
	
	midgard_collector_set_key_property(mc, "name", NULL);
	
	/* Add value */
	midgard_collector_add_value_property(mc, "value");

	return mc;
}

static MidgardCollector *__get_parameters_collector(MidgardObject *object, 
		const gchar *domain)
{	
	if(!object->priv->_params) {
		object->priv->_params = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
		g_hash_table_insert(object->priv->_params, (gpointer) g_strdup(domain), NULL);
		return NULL;
	}
	
	return g_hash_table_lookup(object->priv->_params, domain);
}

static void __register_domain_collector(
		MidgardObject *object, const gchar *domain, MidgardCollector *mc)
{		
	if(object->priv->_params)
		g_hash_table_insert(object->priv->_params, (gpointer) g_strdup(domain), mc);
}

static gboolean __is_guid_valid(MidgardObject *self)
{
	if(MGD_OBJECT_GUID (self) == NULL) {

		midgard_set_error(MGD_OBJECT_CNC (self),
				MGD_GENERIC_ERROR,
				MGD_ERR_INVALID_PROPERTY_VALUE,
				" Guid property is NULL for %s. ", 
				G_OBJECT_TYPE_NAME(self));
		g_warning("%s", MGD_OBJECT_CNC (self)->err->message); 
		g_clear_error(&MGD_OBJECT_CNC (self)->err);
		return FALSE;
	}

	if(!midgard_is_guid(MGD_OBJECT_GUID (self))) {

		midgard_set_error(MGD_OBJECT_CNC (self),
				MGD_GENERIC_ERROR,
				MGD_ERR_INVALID_PROPERTY_VALUE,
				" Guid-property of %s has invalid value.", 
				G_OBJECT_TYPE_NAME(self));
		g_warning("%s", MGD_OBJECT_CNC (self)->err->message);
		g_clear_error(&MGD_OBJECT_CNC (self)->err);
		return FALSE;
	}

	return TRUE;
}

/**
 * midgard_object_get_paramater:
 * @self: #MidgardObject instance
 * @domain: parameter's domain string
 * @name: parameter's name string
 *
 * NULL is returned if parameter record with given domain and name is not found.
 * Returned GValue is owned by midgard-core and shouldn't be freed.
 *
 * Returns: GValue which holds value for domain&name pair
 */
const GValue *midgard_object_get_parameter(MidgardObject *self,
		const gchar *domain, const gchar *name)
{
	g_assert(self);

	if(!__is_guid_valid(self))
		return NULL;

	GValue *ret_value;
	MidgardCollector *domain_collector = 
		__get_parameters_collector(self, domain);	

	if(domain_collector == NULL){
		
		MidgardCollector *mc = 
			__create_domain_collector(MGD_OBJECT_CNC (self), domain);

		/* Limit records to these with parent guid */
		GValue guid_value = {0, };
		g_value_init(&guid_value, G_TYPE_STRING);
		g_value_set_string(&guid_value, MGD_OBJECT_GUID (self));
	
		midgard_collector_add_constraint(mc,
				"parentguid", "=", &guid_value);
		g_value_unset(&guid_value);

		midgard_collector_execute(mc);
		
		/* prepend collector to object's parameter list 
		 * we will free it in MidgardObject destructor */
		self->priv->parameters = 
			g_slist_prepend(self->priv->parameters, mc);

		ret_value = 
			midgard_collector_get_subkey(mc, name, "value");

		__register_domain_collector(self, domain, mc);

		return (const GValue*)ret_value;
	
	} else {
		
		ret_value = 
			midgard_collector_get_subkey(
					domain_collector, name, "value");
		return (const GValue *)ret_value;
	}

	return NULL;
}

/**
 * midgard_object_set_parameter:
 * @self: #MidgardObject instance
 * @domain: parameter's domain string
 * @name: parameter's name string
 * @value: a GValue value which should be set for domain&name pair
 *
 * Creates object's parameter object if it doesn't exists, updates otherwise.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 */ 
gboolean 
midgard_object_set_parameter (MidgardObject *self, const gchar *domain, const gchar *name, GValue *value) 
{
	g_return_val_if_fail (self != NULL, FALSE);

	if(!__is_guid_valid(self))
		return FALSE;
		
	MidgardObject *param;
	const gchar *value_string =
		g_value_get_string(value);
	gboolean delete_parameter = FALSE;
	gboolean do_return_true = FALSE;

	if(g_str_equal(value_string, ""))
		delete_parameter = TRUE;

	const GValue *get_value = 
		midgard_object_get_parameter(self,
				domain, name);

	MidgardCollector *domain_collector =
		__get_parameters_collector(self, domain);

	if(!domain_collector) {

		domain_collector = __create_domain_collector(MGD_OBJECT_CNC (self), domain);
		__register_domain_collector(self, domain, domain_collector);
	}

	/* This is the case when set_parameter is invoked
	 * before any get_parameter */
	if(get_value == NULL && delete_parameter) {
		MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (self), MGD_ERR_NOT_EXISTS);
		return FALSE;
	}

	/* Parameter doesn't exist. We have to create it */
	if(get_value == NULL && !delete_parameter){
		
		param = midgard_object_new(MGD_OBJECT_CNC (self), "midgard_parameter", NULL);
		g_object_set(param, 
				"domain", domain, 
				"name", name,
				"parentguid", MGD_OBJECT_GUID (self),
				NULL);
			
		g_object_set_property(G_OBJECT(param), "value", value);

		if(midgard_object_create(param)) {
		
			if(domain_collector) {
				if(!midgard_collector_set(domain_collector,
							name, "value", value)){
					g_warning("Failed to update parameter's cache"); 
				}
			}
			
			g_object_unref(param);
			return TRUE;

		} else {
			/* Error should be already set by create */
			g_object_unref(param);
			return FALSE;
		}

	
		/* Parameter exists. if value is '' we delete it.
		 * In any other case we update it */
	} else {
		
		MidgardQueryBuilder *builder = 
			midgard_query_builder_new(MGD_OBJECT_CNC (self),
					"midgard_parameter");
		GValue gval = {0,};
		g_value_init(&gval, G_TYPE_STRING);
		g_value_set_string(&gval, MGD_OBJECT_GUID (self));
		midgard_query_builder_add_constraint(builder,
				"parentguid", "=", &gval);
		g_value_unset(&gval);

		g_value_init(&gval, G_TYPE_STRING);
		g_value_set_string(&gval, domain);
		midgard_query_builder_add_constraint(builder,
				"domain", "=", &gval);
		g_value_unset(&gval);

		g_value_init(&gval, G_TYPE_STRING);
		g_value_set_string(&gval, name);
		midgard_query_builder_add_constraint(builder,
				"name", "=", &gval);
		g_value_unset(&gval);

		guint n_objects;
		GObject **ret_object =
			midgard_query_builder_execute(builder, &n_objects);
	
		g_object_unref(builder);
		if(!ret_object){		
			return FALSE;
		}

		if(delete_parameter){

			if(midgard_object_delete(
						MIDGARD_OBJECT(ret_object[0]))) {
				midgard_collector_remove_key(
						domain_collector,
						name);
				do_return_true = TRUE;				
			}
			
		} else {
			
			g_object_set(ret_object[0], "value", value_string, NULL);
			if(midgard_object_update(
						MIDGARD_OBJECT(ret_object[0]))) {
				
				if(domain_collector) {
					midgard_collector_set(domain_collector,
							name, "value", value);
				}

				do_return_true = TRUE;
			}
		}
		
		g_object_unref(ret_object[0]);
		g_free(ret_object);
		if(do_return_true)
			return TRUE;

	}

	return FALSE;
}

/**
 * midgard_object_list_parameters: 
 * @self: a #MidgardObject self instance
 * @domain: optional paramaters' domain
 * 
 * Returned objects are midgard_parameter class. Parameter objects are 
 * fetched from database unconditionally if domain i sexplicitly set to NULL. 
 * That is, only those which parent guid property matches object's guid. 
 *
 * Returned array should be freed when no longer needed.
 * 
 * Returns: Newly allocated and NULL terminated array of midgard_parameter objects. 
 */
MidgardObject **midgard_object_list_parameters(MidgardObject *self, const gchar *domain)
{
	g_return_val_if_fail(self != NULL, NULL);
	g_return_val_if_fail(MGD_OBJECT_GUID (self) != NULL, NULL);
	g_return_val_if_fail(MGD_OBJECT_CNC (self) != NULL, NULL);

	MidgardObject **objects = NULL;

	if(domain == NULL) {

		objects = midgard_core_object_parameters_list(
				MGD_OBJECT_CNC (self), "midgard_parameter", MGD_OBJECT_GUID (self));
		
		return objects;
	}

	GParameter *parameters = g_new0(GParameter, 1);
	GValue dval = {0, };
	g_value_init(&dval, G_TYPE_STRING);
	g_value_set_string(&dval, domain);

	parameters[0].name = "domain";
	parameters[0].value = dval;

	objects = midgard_core_object_parameters_find(
			MGD_OBJECT_CNC (self), "midgard_parameter", 
			MGD_OBJECT_GUID (self), 1, (const GParameter*) parameters);

	g_value_unset(&dval);
	g_free(parameters);

	return objects;
}

/**
 * midgard_object_delete_parameters:
 * @self: #MidgardObject instance
 * @n_params: number of properties
 * @parameters: properties list
 *
 * Delete object's parameter(s) which match given properties' values.
 * Properties list in @parameters is optional. All object's parameters are 
 * deleted ( if exist ) if @parameters is explicitly set to %NULL.
 *
 * Returns: %TRUE on success, %FALSE if at least one of the parameters could not be deleted
 */
gboolean midgard_object_delete_parameters(MidgardObject *self, 
		guint n_params, const GParameter *parameters)
{
	g_assert(self != NULL);

	if(!MGD_OBJECT_GUID (self)) {
		
		g_warning("Object is not fetched from database. Empty guid");
	}

	return midgard_core_object_parameters_delete(
			MGD_OBJECT_CNC (self), "midgard_parameter", 
			MGD_OBJECT_GUID (self), n_params, parameters);
}

/**
 * midgard_object_purge_parameters:
 * @self: #MidgardObject instance
 * @n_params: number of properties
 * @parameters: properties list
 *
 * Purge object's parameter(s) which match given properties' values.
 * Properties list in @parameters is optional. All object's parameters are 
 * purged ( if exist ) if @parameters is explicitly set to %NULL.
 *
 * Returns: %TRUE on success, %FALSE if at least one of the parameters could not be purged
 */
gboolean midgard_object_purge_parameters(MidgardObject *self, 
		guint n_params, const GParameter *parameters)
{
	g_assert(self != NULL);

	if(!MGD_OBJECT_GUID (self)) {
		
		g_warning("Object is not fetched from database. Empty guid");
	}

	return midgard_core_object_parameters_purge(
			MGD_OBJECT_CNC (self), "midgard_parameter", 
			MGD_OBJECT_GUID (self), n_params, parameters);
}

/**
 * midgard_object_find_parameters:
 * @self: #MidgardObject instance
 * @n_params: number of properties
 * @parameters: properties list
 *
 * Find object's parameter(s) with matching given properties.
 * @parameters argument is optional. All object's parameters are 
 * returned ( if exist ) if @parameters is explicitly set to %NULL.
*
 * Returns: newly created, NULL terminated array of #MidgardObject ( midgard_parameter class ) or %NULL on failure
 */
MidgardObject **midgard_object_find_parameters(MidgardObject *self, 
		guint n_params, const GParameter *parameters)
{
	g_assert(self != NULL);

	if(!MGD_OBJECT_GUID (self)) {
		
		g_warning("Object is not fetched from database. Empty guid");
	}

	return midgard_core_object_parameters_find(
			MGD_OBJECT_CNC (self), "midgard_parameter", 
			MGD_OBJECT_GUID (self), n_params, parameters);
}

/**
 * midgard_object_has_parameters:
 * @self: #MidgardObject instance
 *
 * Returns: %TRUE if object has paramateres, %FALSE otherwise.
 */ 
gboolean midgard_object_has_parameters(MidgardObject *self)
{
	g_assert(self != NULL);

	return midgard_core_object_has_parameters(
			MGD_OBJECT_CNC(self), "midgard_parameter", MGD_OBJECT_GUID(self));
}

/* Internal routines */

MidgardObject **midgard_core_object_parameters_list(
		MidgardConnection *mgd, const gchar *class_name, const gchar *guid)
{
	g_assert(class_name != NULL);
	g_assert(guid != NULL);

	MidgardQueryBuilder *builder =
		midgard_query_builder_new(mgd, class_name);

	if(!builder)
		return NULL;

	GValue gval = {0, };
	g_value_init(&gval, G_TYPE_STRING);
	g_value_set_string(&gval, guid);
	midgard_query_builder_add_constraint(builder,
			"parentguid", "=", &gval);
	g_value_unset(&gval);

	guint n_objects;	
	GObject **objects =
		midgard_query_builder_execute(builder, &n_objects);

	g_object_unref(builder);

	return (MidgardObject **)objects;
}

MidgardObject *midgard_core_object_parameters_create(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters)
{
	g_assert(mgd != NULL);
	g_assert(class_name != NULL);
	g_assert(guid != NULL);
	g_assert(n_params > 0);

	guint i;
	GParamSpec *pspec = NULL;
	GValue pval;
	const gchar *prop_name;

	MidgardObjectClass *klass = 
		MIDGARD_OBJECT_GET_CLASS_BY_NAME(class_name);

	MidgardObject *object = midgard_object_new(mgd, class_name, NULL);

	/* Check if properties in parameters are registered for given class */
	for ( i = 0; i < n_params; i++) {
	
		prop_name = parameters[i].name;
		pval = parameters[i].value;

		pspec = g_object_class_find_property(G_OBJECT_CLASS(klass), prop_name);

		if(!pspec) {
			
			MIDGARD_ERRNO_SET(mgd, MGD_ERR_INVALID_PROPERTY);
			g_warning("Property '%s' not registered for '%s' class", 
					parameters[i].name, class_name);
			g_object_unref(object);
			return NULL;
		}

		g_object_set_property(G_OBJECT(object), prop_name, &pval);
	}
	
	/* Set parentguid so we definitely define parameter or attachment*/
	g_object_set(object, "parentguid", guid, NULL);

	if(!midgard_object_create(object)) {
		
		g_object_unref(object);
		/* error code is set by create method */
		return NULL;
	}

	return object;
}

static GObject **__fetch_parameter_objects(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters)
{
	MidgardQueryBuilder *builder =
		midgard_query_builder_new(mgd, class_name);
	
	/* Error is set in builder constructor */
	if(!builder)
		return FALSE;
	
	GValue gval = {0, };
	g_value_init(&gval, G_TYPE_STRING);
	g_value_set_string(&gval, guid);
	
	if(!midgard_query_builder_add_constraint(builder, "parentguid", "=", &gval)) {
		
		g_value_unset(&gval);
		g_object_unref(builder);
		return FALSE;
	}
	
	g_value_unset(&gval);

	guint i;
	const gchar *prop_name;
	GValue pval;
	
	for ( i = 0; i < n_params; i++) {
		
		prop_name = parameters[i].name;
		pval = parameters[i].value;
		
		if(!midgard_query_builder_add_constraint(builder,
					prop_name, "=", &pval)) {
			
			/* error is set by add_constraint */
			g_object_unref(builder);
			return FALSE;
		}
	}

	guint n_objects;	
	GObject **objects = midgard_query_builder_execute(builder, &n_objects);
	g_object_unref(builder);
	
	return objects;
}

gboolean midgard_core_object_parameters_delete(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters)
{
	g_assert(mgd != NULL);
	g_assert(class_name != NULL);
	g_assert(guid != NULL);
	
	GObject **objects = __fetch_parameter_objects(
			mgd, class_name, guid, n_params, parameters);

	/* nothing to delete */
	if(!objects)
		return FALSE;
	
	gboolean rv = FALSE;
	guint i = 0;
	
	while (objects[i] != NULL ) {
	
		if(midgard_object_delete(MIDGARD_OBJECT(objects[i])))
			rv = TRUE;

		g_object_unref(objects[i]);
		i++;
	}

	g_free(objects);
	return rv;
}

gboolean midgard_core_object_parameters_purge(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters)
{
	g_assert(mgd != NULL);
	g_assert(class_name != NULL);
	g_assert(guid != NULL);
	
	GObject **objects = __fetch_parameter_objects(
			mgd, class_name, guid, n_params, parameters);

	/* nothing to purge */
	if(!objects)
		return FALSE;
	
	gboolean rv = FALSE;
	guint i = 0;
	
	while (objects[i] != NULL ) {
	
		if(midgard_object_purge(MIDGARD_OBJECT(objects[i])))
			rv = TRUE;

		g_object_unref(objects[i]);
		i++;
	}

	g_free(objects);
	return rv;
}

gboolean midgard_core_object_parameters_purge_with_blob(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters)
{
	g_assert(mgd != NULL);
	g_assert(class_name != NULL);
	g_assert(guid != NULL);
	
	GObject **objects = __fetch_parameter_objects(
			mgd, class_name, guid, n_params, parameters);

	/* nothing to purge */
	if(!objects)
		return FALSE;
	
	gboolean rv = FALSE;
	guint i = 0;
	MidgardBlob *blob = NULL;
	
	while (objects[i] != NULL ) {

		/* ! IMPORTANT !
		 * Blob on filesystem must be deleted first.
		 * In any other case, if something goes wrong, we won't be able 
		 * to check which file is orphaned.
		 * If file is removed and record's deletion failed, on is able 
		 * to check file with blob's file_exists method */
		blob = midgard_blob_new(MIDGARD_OBJECT(objects[i]), NULL);
		
		if(!blob) {
			g_warning("Can not handle blob for given attachment");
			continue;
		}

		if(midgard_blob_exists(blob))
			midgard_blob_remove_file(blob);
		
		midgard_object_purge(MIDGARD_OBJECT(objects[i]));

		g_object_unref(blob);
		g_object_unref(objects[i]);
		i++;
	}

	g_free(objects);
	return rv;
}

MidgardQueryBuilder *midgard_core_object_parameter_query_builder(
		MidgardConnection *mgd, const gchar *class_name, const gchar *guid)
{
	g_assert(mgd != NULL);
	g_assert(class_name != NULL);
	g_assert(guid != NULL);

	MidgardQueryBuilder *builder = 
		midgard_query_builder_new(mgd, class_name);

	/* error is set in builder constructor */
	if(!builder)
		return NULL;

	GValue gval = {0, };
	g_value_init(&gval, G_TYPE_STRING);
	g_value_set_string(&gval, guid);
	
	if(!midgard_query_builder_add_constraint(builder, "parentguid", "=", &gval)) {
		
		g_value_unset(&gval);
		g_object_unref(builder);
		return NULL;
	}

	g_value_unset(&gval);

	return builder;
}

MidgardObject **midgard_core_object_parameters_find(
		MidgardConnection *mgd, const gchar *class_name,
		const gchar *guid, guint n_params, const GParameter *parameters)
{
	g_assert(mgd != NULL);
	g_assert(class_name != NULL);
	g_assert(guid != NULL);

	if(!parameters)
		return NULL;
	
	if(parameters && n_params == 0) {
		g_warning("Expected at least 1 parameter. Parameters pointer is not NULL");
		return NULL;
	}

	MidgardQueryBuilder *builder =
		midgard_core_object_parameter_query_builder(mgd, class_name, guid);

	/* error is set in builder constructor */
	if(!builder)
		return NULL;
	
	guint i;
	const gchar *prop_name;
	GValue pval;

	for ( i = 0; i < n_params; i++) {

		prop_name = parameters[i].name;
		pval = parameters[i].value;

		if(!midgard_query_builder_add_constraint(builder, 
					prop_name, "=", &pval)) {

			/* error is set by add_constraint */
			g_object_unref(builder);
			return NULL;
		}
	}

	guint n_objects;
	GObject **objects = midgard_query_builder_execute(builder, &n_objects);
	g_object_unref(builder);

	return (MidgardObject **)objects;
}

gboolean midgard_core_object_has_parameters(
		MidgardConnection *mgd, const gchar *class_name, const gchar *guid)
{
	g_assert(mgd != NULL);
	g_assert(class_name != NULL);
	g_assert(guid != NULL);

	MidgardQueryBuilder *builder =
		midgard_core_object_parameter_query_builder(mgd, class_name, guid);

	/* error is set in builder constructor */
	if(!builder)
		return FALSE;

	midgard_query_builder_set_limit(builder, 1);
	
	gboolean rv = TRUE;
	guint i = midgard_query_builder_count(builder);

	if(i == 0)
		rv = FALSE;

	g_object_unref(builder);

	return rv;
}	
