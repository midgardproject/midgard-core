/* 
 * Copyright (C) 2008 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_core_object.h"
#include "midgard_object.h"
#include "midgard_core_object_parameter.h"
#include "midgard_error.h"

/**
 * midgard_object_list_attachments: 
 * @self: a #MidgardObject self instance 
 * 
 * Returned objects are midgard_attachment class. Attachments objects are 
 * fetched from database unconditionally. 
 * That is, only those which parent guid property matches object's guid. 
 *
 * Returned array should be freed when no longer needed.
 * 
 * Returns: Newly allocated and NULL terminated array of midgard_attachment objects. 
 */
MidgardObject **midgard_object_list_attachments(MidgardObject *self)
{
	g_return_val_if_fail(self != NULL, NULL);
	g_return_val_if_fail(MGD_OBJECT_GUID (self) != NULL, NULL);
	g_return_val_if_fail(MGD_OBJECT_CNC (self) != NULL, NULL);

	MidgardObject **objects = NULL;

	objects = midgard_core_object_parameters_list(
			MGD_OBJECT_CNC (self), "midgard_attachment", MGD_OBJECT_GUID (self));
		
	return objects;
}

/**
 * midgard_object_create_attachment:
 * @self: #MidgardObject instance
 * @name: name for attachment
 * @title: its title
 * @mimetype: and mimetype
 *
 * Creates object's attachment using given properties.
 * Any property may be explicitly set to NULL.
 *
 * Returns: newly created #MidgardObject of midgard_attachment class or %NULL on failure
 */
MidgardObject *midgard_object_create_attachment(MidgardObject *self, 
		const gchar *name, const gchar *title, const gchar *mimetype)
{
	g_return_val_if_fail(self != NULL, NULL);

	if(!MGD_OBJECT_GUID (self)) {
		g_warning("Object is not fetched from database. Empty guid");
		return NULL;
	}

	guint n_params, i = 0;

	/* Check if there's duplicate name */
	if(name != NULL && *name != '\0') {
		
		MidgardQueryBuilder *builder = 
			midgard_query_builder_new(MGD_OBJECT_CNC (self), "midgard_attachment");
		
		if(!builder)
			return NULL;

		GValue pval = {0, };
		g_value_init(&pval, G_TYPE_STRING);
		g_value_set_string(&pval, MGD_OBJECT_GUID (self));
		midgard_query_builder_add_constraint(builder, "parentguid", "=", &pval);
		g_value_unset(&pval);
		g_value_init(&pval, G_TYPE_STRING);
		g_value_set_string(&pval, name);
		midgard_query_builder_add_constraint(builder, "name", "=", &pval);
		g_value_unset(&pval);

		i = midgard_query_builder_count(builder);
		g_object_unref(builder);

		if(i > 0) {
			
			MIDGARD_ERRNO_SET(MGD_OBJECT_CNC (self), MGD_ERR_OBJECT_NAME_EXISTS);
			return NULL;
		}
	}

	/* create parameters */
	i = 0;
	if(name) i++;
	if(title) i++;
	if(mimetype) i++;

	n_params = i;

	GValue nval = {0, };
	GValue tval = {0, };
	GValue mval = {0, };

	GParameter *parameters = NULL;

	/* TODO, implement  parameters from va_list function if needed */

	if(i > 0) {
		
		parameters = g_new0(GParameter, i);
		
		if(name) {
			i--;
			parameters[i].name = "name";
			g_value_init(&nval, G_TYPE_STRING);
			g_value_set_string(&nval, name);
			parameters[i].value = nval;
		}

		if(title) {
			i--;
			parameters[i].name = "title";
			g_value_init(&tval, G_TYPE_STRING);
			g_value_set_string(&tval, title);
			parameters[i].value = tval;
		}

		if(mimetype) {
			i--;
			parameters[i].name = "mimetype";
			g_value_init(&mval, G_TYPE_STRING);
			g_value_set_string(&mval, mimetype);
			parameters[i].value = mval;
		}
	}

	MidgardObject *att = 
		midgard_core_object_parameters_create(MGD_OBJECT_CNC (self), 
				"midgard_attachment", MGD_OBJECT_GUID (self), 
				n_params, parameters);
	
	for(i = 0; i < n_params; i++) {	
		g_value_unset(&parameters[i].value);
	}

	g_free(parameters);

	return att;
}

/**
 * midgard_object_delete_attachments:
 * @self: #MidgardObject instance
 * @n_params: number of properties
 * @parameters: properties list
 *
 * Delete object's attachments(s) which match given properties' values.
 * Properties list in @parameters is optional. All object's attachments are 
 * deleted ( if exist ) if @parameters is explicitly set to %NULL.
 *
 * Returns: %TRUE on success, %FALSE if at least one of the attachment could not be deleted
 */
gboolean midgard_object_delete_attachments(MidgardObject *self, 
		guint n_params, const GParameter *parameters)
{
	g_assert(self != NULL);

	if(!MGD_OBJECT_GUID (self)) {
		
		g_warning("Object is not fetched from database. Empty guid");
	}

	return midgard_core_object_parameters_delete(
			MGD_OBJECT_CNC (self), "midgard_attachment", 
			MGD_OBJECT_GUID (self), n_params, parameters);
}


/**
 * midgard_object_purge_attachments:
 * @self: #MidgardObject instance
 * @delete_blob: whether blob should be deleted as well
 * @n_params: number of properties
 * @parameters: properties list
 *
 * Purge object's attachments(s) which match given properties' values.
 * Properties list in @parameters is optional. All object's attachments are 
 * purged ( if exist ) if @parameters is explicitly set to %NULL.
 *
 * @delete_blob should be set to %TRUE if midgard_attachment holds a reference 
 * to blob located on filesystem ( it should be set to %TRUE by default ).
 * However, if midgard_attachment is created for blobs sharing and file should not 
 * be deleted, @delete_blob should be set to %FALSE.
 *
 * There's no way to determine if midgard_attachment is sharing blob, so aplication 
 * itelf is responsible to create such own logic.
 *
 * Returns: %TRUE on success, %FALSE if at least one of the attachment could not be purged
 */
gboolean midgard_object_purge_attachments(MidgardObject *self, gboolean delete_blob,
		guint n_params, const GParameter *parameters)
{
	g_assert(self != NULL);

	if(!MGD_OBJECT_GUID (self)) {
		
		g_warning("Object is not fetched from database. Empty guid");
	}

	gboolean rv = FALSE;

	if(delete_blob) {
	
		rv = midgard_core_object_parameters_purge_with_blob(
				MGD_OBJECT_CNC (self), "midgard_attachment", 
				MGD_OBJECT_GUID (self), n_params, parameters);
	
	} else {
		
		rv = midgard_core_object_parameters_purge(
				MGD_OBJECT_CNC (self), "midgard_attachment",
				MGD_OBJECT_GUID (self), n_params, parameters);
	}

	return rv;
}

/**
 * midgard_object_find_attachments:
 * @self: #MidgardObject instance
 * @n_params: number of properties
 * @parameters: properties list
 *
 * Find object's attachment(s) with matching given properties.
 * @parameters argument is optional. All object's attachments are 
 * returned ( if exist ) if @parameters is explicitly set to %NULL.
*
 * Returns: newly created, NULL terminated array of #MidgardObject ( midgard_attachment class ) or %NULL on failure
 */
MidgardObject **midgard_object_find_attachments(MidgardObject *self, 
		guint n_params, const GParameter *parameters)
{
	g_assert(self != NULL);

	if(!MGD_OBJECT_GUID (self)) {
		
		g_warning("Object is not fetched from database. Empty guid");
	}

	return midgard_core_object_parameters_find(
			MGD_OBJECT_CNC (self), "midgard_attachment", 
			MGD_OBJECT_GUID (self), n_params, parameters);
}

/**
 * midgard_object_has_attachments:
 * @self: #MidgardObject instance
 * 
 * Returns: %TRUE if object has attachments, %FALSE otherwise.
 */
gboolean midgard_object_has_attachments(MidgardObject *self)
{
	g_assert(self != NULL);
	
	return midgard_core_object_has_parameters(
			MGD_OBJECT_CNC(self), "midgard_attachment", MGD_OBJECT_GUID(self));
}

