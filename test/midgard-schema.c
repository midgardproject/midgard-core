/* 
 * Copyright (C) 2004, 2005, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "src/midgard_error.h"
#include "src/midgard_user.h"
#include "src/midgard_object_parameter.h"
#include "src/midgard_utils.h"
#include "src/midgard_storage.h"

#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "Schema"
#endif

static void _test_me(MidgardConnection *mgd, MidgardObject *object)
{
	/* This is anonymous mode now. 
	 * It should be supported by SG account in config file later.
	 * If you want to add anything here, please add *every* object method 
	 * or function. This is real the best place to use with valgrind */	
	
	const gchar *typename = G_OBJECT_TYPE_NAME(object);

	/* Return when object is midgard_attachment type.
	 * This is an exception. Quota feature calls g_error when location of blob is NULL.
	 * We need g_error in runtime environment, but not now and here */
	if (g_str_equal(typename, "midgard_attachment")) {
		g_message("midgard_attachment ignored");
		return;
	}
	
	/* Test create */
	if (midgard_object_create(object)){
		
		g_message("Object '%s' create : OK", typename);
		
		/* Test parameters */
		GValue *pval = g_new0(GValue, 1);
		g_value_init(pval, G_TYPE_STRING);
		g_value_set_string(pval, "__midgard_test_value");
		gboolean param_test = FALSE;
		param_test = midgard_object_set_parameter(object, 
				"__midgard_test_domain", 
				"__midgard_test_name", 
				pval);
		if (!param_test)
			g_warning("Failed to create test parameter for %s",
					typename);
		
		const GValue *rval = 
			midgard_object_get_parameter(object,
					"__midgard_test_domain",
					"__midgard_test_name");

		if (rval == NULL)
			g_warning("Failed to get test parameter for %s", 
					typename);
		
		GValue *dval = g_new0(GValue, 1);
		g_value_init(dval, G_TYPE_STRING);
		g_value_set_string(pval, "");
		param_test = midgard_object_set_parameter(object,
				"__midgard_test_domain",
				"__midgard_test_name",
				pval);
		
		if (!param_test)
			g_warning("Failed to delete test parameter for %s",
					typename);

		/* Test update */
		if (!midgard_object_update(object)) {
			
			g_warning("Failed to update object %s",	typename);
		
		} else {
			
			g_message("Object '%s' update : OK", typename);
		}
			
		/* Test getting object and basic QB functionality 
		 * Get by id or by guid is a wrapper to QB with particular constraint */
		GValue gval = {0, };
		g_value_init(&gval, G_TYPE_STRING);
		gchar *guid;
		g_object_get(G_OBJECT(object), "guid", &guid, NULL);
		g_value_set_string(&gval, guid);
		MidgardObject *new = midgard_object_new(mgd, typename, &gval);
		g_value_unset(&gval);
		
		if (new == NULL) {
			
			g_warning("Failed to get %s by %s", typename, guid);
		
		} else {
			
			g_object_unref(new);
			g_message("Object '%s' get : OK", typename);
		}
		
		g_free(guid);

		if (midgard_object_delete(object, TRUE)) {

			g_message("Object '%s' delete : OK", typename);

		} else {

			g_warning("Failed to delete '%s'", typename);
		}
	
	} else {
		
		g_warning("Failed to create object %s", typename);
	}
}

int 
main (int argc, char **argv)
{
	if (argc != 2){ 
		g_error("You must pass configuration filename as argument!");
		return(1);
	}
	
	
	gchar *config_file = argv[1];
	
	midgard_init();

	MidgardConnection *mgd = midgard_connection_new();

	if (!mgd)
		g_error("Can not initialize midgard connection");

	/* Just keep it here for testing purpose.
	 * Error is always fatal , but sometimes I need to change it to CRITICAL */
	g_log_set_always_fatal(G_LOG_LEVEL_ERROR);

	midgard_connection_set_loglevel(mgd, "warning", NULL);

	g_log_set_handler(G_LOG_DOMAIN, G_LOG_LEVEL_MASK,
			midgard_error_default_log, (gpointer)mgd);
	g_log_set_handler("midgard-core", G_LOG_LEVEL_MASK,
			midgard_error_default_log, (gpointer)mgd);
	g_log_set_handler("midgard-core-type", G_LOG_LEVEL_MASK,
			midgard_error_default_log, (gpointer)mgd);

	g_message("Using '%s' configuration file", config_file);

	MidgardConfig *config = g_object_new(MIDGARD_TYPE_CONFIG, NULL);

	GError *error = NULL;
	if (!midgard_config_read_file(config, (const gchar *)config_file, TRUE, &error)) {
	
		if (error) {
			g_warning("Can not read configuration from given name. %s", error->message);
			g_error_free(error);
		}

		return 1;
	}

	/* open config using class member method or function  
	if (!MIDGARD_CONNECTION_GET_CLASS(midgard)->open_config(
				midgard, config))
	*/
	if (!midgard_connection_open_config(mgd, config)) 	
		return 1;	
				
	guint n_types, i;
	GType *all_types = g_type_children(MIDGARD_TYPE_OBJECT,	&n_types);

	const gchar *typename;
	gchar *mgdusername = NULL, *mgdpassword = NULL;
	gboolean tablecreate, tableupdate, testunit;
	g_object_get(G_OBJECT(config), 
			"tablecreate", &tablecreate,
			"tableupdate", &tableupdate,
			"testunit", &testunit,
			"midgardusername", &mgdusername, 
			"midgardpassword", &mgdpassword,
			NULL);

	if ((mgdusername != NULL) && (mgdpassword != NULL))
		midgard_user_auth(mgd, mgdusername, mgdpassword, NULL, FALSE);
		
	/* Create basic tables first */
	if (tablecreate)
		midgard_storage_create_base_storage (mgd);

	for (i = 0; i < n_types; i++) {

		typename = g_type_name(all_types[i]);

		if (g_str_equal(typename, "midgard_parameter")) {
			MidgardObject *param = midgard_object_new(mgd, typename, NULL);
			g_object_unref(param);
			g_message("midgard_parameter ignored");
			continue;
		}

		g_message("Type %s", typename);
		MidgardObject *object = midgard_object_new(mgd, typename, NULL);

		/* Create tables */
		if (tablecreate) 
			midgard_storage_create (mgd, typename);

		/* Update tables */
		if (tableupdate) 
			midgard_storage_update (mgd, typename);
			
		/* Test objects */
		if (testunit)
			_test_me(mgd, object);
		
		g_object_unref(object);		
	}

	g_free(all_types);
	g_object_unref(config);
	g_free(mgdusername);
	g_free(mgdpassword);
	g_object_unref(mgd);

	midgard_close();

	return 0;
}
