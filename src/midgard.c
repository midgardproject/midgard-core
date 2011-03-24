/* $Id$
 *
 * Copyright (C) 2007, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#ifndef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "midgard-core"
#endif

#include "midgard_object.h"
#include "midgard_user.h"
#include "midgard_blob.h"
#include "midgard_collector.h"
#include "midgard_view.h"
#include "midgard_timestamp.h"
#include "midgard_metadata.h"
#include "midgard_workspace.h"

#include <locale.h>

#include <libgda/libgda.h>

const char *
midgard_version()
{
	return MIDGARD_LIB_VERSION;
}

/* A helper funtion which is registered as string to float 
 * convertion one. GGtype system doesn't support such conversion,
 * and what is more, such convesrion must be done using plain C locale
 * for SQL safety. */
static void 
__transform_string_to_float (const GValue *src_val, GValue *dest_val)
{
	g_assert (src_val != NULL);
	g_assert (dest_val != NULL);

	g_assert (G_VALUE_HOLDS_FLOAT (dest_val));

	gchar *lstring = setlocale (LC_NUMERIC, "0");
	setlocale (LC_NUMERIC, "C");

	const gchar *string_float = g_value_get_string (src_val);

	if (!string_float) {

		g_value_set_float (dest_val, 0.00);
	
	} else {
		
		g_value_set_float (dest_val, atof (string_float));
	}

	setlocale (LC_ALL, lstring);

	return;
}

/* A helper function whch transforms string to boolean */
static void 
__transform_string_to_boolean (const GValue *src_val, GValue *dest_val)
{
	g_assert (src_val != NULL);
	g_assert (dest_val != NULL);

	g_assert (G_VALUE_HOLDS_BOOLEAN (dest_val));

	const gchar *_str = g_value_get_string (src_val);

	if (!_str || (_str && *_str == '\0')) {
		g_value_set_boolean (dest_val, FALSE);
		return;
	}

    	/* Stringified boolean */
	if (g_str_equal (_str, "TRUE")) {
		g_value_set_boolean (dest_val, TRUE);
		return;
	}

	if (g_str_equal (_str, "true")) {
		g_value_set_boolean (dest_val, TRUE);
		return;
	}

	if (g_str_equal (_str, "FALSE")) {
		g_value_set_boolean (dest_val, FALSE);
		return;
	}

	if (g_str_equal (_str, "false")) {
		g_value_set_boolean (dest_val, FALSE);
		return;
	}

	gint i = atoi (_str);

	if (i > 0)
		g_value_set_boolean (dest_val, TRUE);
	else
		g_value_set_boolean (dest_val, FALSE);

	return;
}

void 
midgard_init() 
{	
	GType type;
	/* g_type_init_with_debug_flags(G_TYPE_DEBUG_OBJECTS | G_TYPE_DEBUG_NONE); */

#ifdef HAVE_LIBGDA_4
	gda_init ();
#else
	gda_init ("midgard-core-gda", midgard_version(), 0, NULL);
#endif

	type = MIDGARD_TYPE_BLOB;
	g_assert (type != 0);
	g_type_class_ref (type);

	type = MIDGARD_TYPE_USER;
	g_assert (type != 0);
	g_type_class_ref (type);

	type = MIDGARD_TYPE_CONNECTION;
	g_assert (type != 0);
	g_type_class_ref (type);

	type = MIDGARD_TYPE_CONFIG;
	g_assert (type != 0);
	g_type_class_ref (type);

	type = MIDGARD_TYPE_COLLECTOR;
	g_assert (type != 0);
	g_type_class_ref (type);

	type = MIDGARD_TYPE_QUERY_BUILDER;
	g_assert (type != 0);
	g_type_class_ref (type);
	
	type = MIDGARD_TYPE_DBOBJECT;
	g_assert (type != 0);
	g_type_class_ref (type);

	type = MIDGARD_TYPE_OBJECT;
	g_assert (type != 0);
	g_type_class_ref (type);

	type = MIDGARD_TYPE_VIEW;
	g_assert (type != 0);
	g_type_class_ref (type);

	type = MIDGARD_TYPE_METADATA;
	g_assert (type != 0);
	g_type_class_ref (type);
	/* Initialize MidgardMetadataClass */
	MidgardMetadata *m = g_object_new (MIDGARD_TYPE_METADATA, NULL);
	g_object_unref (m);

	type = MIDGARD_TYPE_WORKSPACE;
	g_assert (type != 0);
	g_type_class_ref (type);

	/* Register transform function explicitly, we need own routine */
	g_value_register_transform_func (G_TYPE_STRING, G_TYPE_FLOAT, __transform_string_to_float);
  	g_value_register_transform_func (G_TYPE_STRING, G_TYPE_BOOLEAN, __transform_string_to_boolean);
}

void 
midgard_close(void)
{
	GObjectClass *klass;	
	
	klass = g_type_class_peek (MIDGARD_TYPE_BLOB);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_USER);
	if (klass)
		g_type_class_unref (klass);
	
	klass = g_type_class_peek (MIDGARD_TYPE_CONNECTION);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_CONFIG);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_COLLECTOR);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_QUERY_BUILDER);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_DBOBJECT);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_OBJECT);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_VIEW);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_METADATA);
	if (klass)
		g_type_class_unref (klass);

	klass = g_type_class_peek (MIDGARD_TYPE_WORKSPACE);
	if (klass)
		g_type_class_unref (klass);
}
