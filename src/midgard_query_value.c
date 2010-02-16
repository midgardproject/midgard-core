/* 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
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

#include "midgard_query_value.h"

struct _MidgardQueryValue {
	GObject parent;
	GValue value;
};

MidgardQueryValue*
midgard_query_value_new	(const GValue *value)
{
	/* TODO */
	return NULL;
}

void
midgard_query_value_get_value (MidgardQueryValue *self, GValue *value)
{
	/* TODO */
	return;
}

gboolean
midgard_query_value_set_value (MidgardQueryValue *self, const GValue *value)
{
	/* TODO */
	return FALSE;
}

GType	
midgard_query_value_get_type (void)
{

}

