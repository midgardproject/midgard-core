/* 
 * Copyright (C) 2008, 2009 Piotr Pokora <piotrek.pokora@gmail.com>
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

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);

	g_test_bug_base (MIDGARD_TRAC_TICKET_BASE_URL);

	g_test_add_func("/midgard_connection/open", midgard_test_connection_open);
	g_test_add_func("/midgard_connection/open_config", midgard_test_connection_open_config);
	g_test_add_func("/midgard_connection/close", midgard_test_connection_close);
	g_test_add_func("/midgard_connection/set_loglevel", midgard_test_connection_set_loglevel);
	g_test_add_func("/midgard_connection/get_error", midgard_test_connection_get_error);
	g_test_add_func("/midgard_connection/get_error_string", midgard_test_connection_get_error_string);
	g_test_add_func("/midgard_connection/get_user", midgard_test_connection_get_user);
	g_test_add_func("/midgard_connection/copy", midgard_test_connection_copy);
	g_test_add_func("/midgard_connection/enable_workspace", midgard_test_connection_copy);	
	g_test_add_func("/midgard_connection/set_workspace", midgard_test_connection_copy);
	g_test_add_func("/midgard_connection/get_workspace", midgard_test_connection_copy);
	g_test_add_func("/midgard_connection/signals/auth-changed", midgard_test_connection_signals_auth_changed);
	g_test_add_func("/midgard_connection/signals/connected", midgard_test_connection_signals_connected);
	g_test_add_func("/midgard_connection/signals/disconnected", midgard_test_connection_signals_disconnected);
	g_test_add_func("/midgard_connection/signals/error", midgard_test_connection_signals_error);
	g_test_add_func("/midgard_connection/signals/lost-provider", midgard_test_connection_signals_lost_provider);

	midgard_init();

	return g_test_run();

	midgard_close();
}
