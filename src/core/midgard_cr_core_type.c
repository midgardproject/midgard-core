/* 
 * Copyright (C) 2005, 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_cr_core_type.h"

/* Register MIDGARD_CR_CORE_TYPE_LONGTEXT */
GType 
midgard_cr_core_longtext_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		GTypeInfo info = {
			0,	       /* class size */
			NULL,          /* base_init */
			NULL,          /* base_finalize */
			NULL,          /* class_init */
			NULL,          /* class_finalize */
			NULL,          /* class_data */
			0,
			0,             /* n_preallocs */
			NULL 	       /* instance_init */
		};
		type = g_type_register_static (G_TYPE_STRING, "MidgardCRLONGTEXT", &info, 0);
	}
	return type;
}

/* Register MIDGARD_CR_CORE_TYPE_GUID */
GType 
midgard_cr_core_guid_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
		GTypeInfo info = {
			0,	       /* class size */
			NULL,          /* base_init */
			NULL,          /* base_finalize */
			NULL,          /* class_init */
			NULL,          /* class_finalize */
			NULL,          /* class_data */
			0,
			0,             /* n_preallocs */
			NULL 	       /* instance_init */
		};
		type = g_type_register_static (G_TYPE_STRING, "MidgardCRGUID", &info, 0);
	}
	return type;
}
