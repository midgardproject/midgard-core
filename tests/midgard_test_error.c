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

#include "midgard_test_error.h"
#include "midgard_test_config.h"

void midgard_test_error_ok(MidgardConnection *mgd)
{
	g_assert_cmpstr(midgard_connection_get_error_string(mgd), ==, "MGD_ERR_OK");
	g_assert_cmpint(midgard_connection_get_error(mgd), ==, MGD_ERR_OK);

	return;
}

void midgard_test_error_assert(MidgardConnection *mgd, gint errcode)
{
	g_assert_cmpint(midgard_connection_get_error(mgd), ==, errcode);

	const gchar *errstr = midgard_error_string(MGD_GENERIC_ERROR, errcode);
	g_assert_cmpstr(midgard_connection_get_error_string(mgd), ==, errstr);

	return;
}
