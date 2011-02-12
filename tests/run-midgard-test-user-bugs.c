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

#include "midgard_test.h"

static MidgardConnection *mgd_global = NULL;

static void 
midgard_test_user_setup (MidgardUserTest *mut, gconstpointer data) 
{ 
	MidgardUser *user = MIDGARD_USER (data); 
	mut->mgd = mgd_global; 
	mut->user = user; 
}

static void 
midgard_test_user_setup_foo (MidgardUserTest *mut, gconstpointer data) 
{ 
	return;
}

static void 
midgard_test_user_teardown_foo (MidgardUserTest *mut, gconstpointer data)
{
	return;
}

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);	
	
	midgard_init();

	g_test_add("/midgard_user/bugs/73", MidgardUserTest, NULL, midgard_test_user_setup_foo,  
			midgard_test_user_bugs_73, midgard_test_user_teardown_foo);

	return g_test_run();
}
