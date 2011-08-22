/* 
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_base_abstract.h"

/* MIDGARD_BASE_ABSTRACT ABSTRACT CLASS ROUTINES */

static GObjectClass *__parent_class= NULL;

static void 
_midgard_base_abstract_class_init (gpointer g_class, gpointer g_class_data)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (g_class);
	MidgardBaseAbstractClass *klass = MIDGARD_BASE_ABSTRACT_CLASS (g_class);
	__parent_class = g_type_class_peek_parent (g_class);
	
	gobject_class->dispose = __parent_class->dispose;
	gobject_class->finalize = __parent_class->finalize;
	gobject_class->set_property = NULL;
	gobject_class->get_property = NULL;
}

GType 
midgard_base_abstract_get_type (void)
{
       	static GType type = 0;
 
	if (type == 0) {
 		static const GTypeInfo info = {
			sizeof (MidgardBaseAbstractClass),
			NULL,           /* base_init */
			NULL,           /* base_finalize */
			(GClassInitFunc) _midgard_base_abstract_class_init,	
			NULL,           /* class_finalize */
			NULL,           /* class_data */
			sizeof (MidgardBaseAbstract),
			0,              /* n_preallocs */
			NULL		/* instance_init */
		};

		type = g_type_register_static (G_TYPE_OBJECT, "MidgardBaseAbstract", &info, G_TYPE_FLAG_ABSTRACT);
	}

	return type;
}

