/* $Id$
 *
 * Copyright (C) 2007 Piotr Pokora <piotrek.pokora@gmail.com>
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
#include <libgda/libgda.h>

const char *midgard_version()
{
	return MIDGARD_LIB_VERSION;
}

void midgard_init() 
{	
	GType type;
	/* g_type_init_with_debug_flags(G_TYPE_DEBUG_OBJECTS | G_TYPE_DEBUG_NONE); */

#ifdef HAVE_LIBGDA_4
	gda_init ();
#else
	gda_init ("midgard-core-gda", midgard_version(), 0, NULL);
#endif

	type = MIDGARD_TYPE_BLOB;
	g_assert(type != 0);
	g_type_class_ref(type);

	type = MIDGARD_TYPE_USER;
	g_assert(type != 0);
	g_type_class_ref(type);

	type = MIDGARD_TYPE_CONNECTION;
	g_assert(type != 0);
	g_type_class_ref(type);

	type = MIDGARD_TYPE_CONFIG;
	g_assert(type != 0);
	g_type_class_ref(type);

	type = MIDGARD_TYPE_COLLECTOR;
	g_assert(type != 0);
	g_type_class_ref(type);

	type = MIDGARD_TYPE_QUERY_BUILDER;
	g_assert(type != 0);
	g_type_class_ref(type);
	
	type = MIDGARD_TYPE_DBOBJECT;
	g_assert(type != 0);
	g_type_class_ref(type);

	type = MIDGARD_TYPE_OBJECT;
	g_assert(type != 0);
	g_type_class_ref(type);

	type = MIDGARD_TYPE_VIEW;
	g_assert(type != 0);
	g_type_class_ref(type);
}

void midgard_close(void)
{
	GObjectClass *klass;	
	
	klass = g_type_class_peek(MIDGARD_TYPE_BLOB);
	if (klass)
		g_type_class_unref(klass);

	klass = g_type_class_peek(MIDGARD_TYPE_USER);
	if (klass)
		g_type_class_unref(klass);
	
	klass = g_type_class_peek(MIDGARD_TYPE_CONNECTION);
	if (klass)
		g_type_class_unref(klass);

	klass = g_type_class_peek(MIDGARD_TYPE_CONFIG);
	if (klass)
		g_type_class_unref(klass);

	klass = g_type_class_peek(MIDGARD_TYPE_COLLECTOR);
	if (klass)
		g_type_class_unref(klass);

	klass = g_type_class_peek(MIDGARD_TYPE_QUERY_BUILDER);
	if (klass)
		g_type_class_unref(klass);

	klass = g_type_class_peek(MIDGARD_TYPE_DBOBJECT);
	if (klass)
		g_type_class_unref(klass);

	klass = g_type_class_peek(MIDGARD_TYPE_OBJECT);
	if (klass)
		g_type_class_unref(klass);

	klass = g_type_class_peek(MIDGARD_TYPE_VIEW);
	if (klass)
		g_type_class_unref(klass);
}
