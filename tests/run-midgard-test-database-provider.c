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

/* Check underlying database provider escaping problems and possible sql injections */

#include "midgard_test_database_provider.h"

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);

	/* MySQL */
	g_test_add_func("/midgard_database_provider/mysql_escape", midgard_test_database_provider_mysql_escape);
	g_test_add_func("/midgard_database_provider/mysql_double_escape", midgard_test_database_provider_mysql_double_escape);
	g_test_add_func("/midgard_database_provider/mysql_complex_escape", midgard_test_database_provider_mysql_complex_escape);

	/* SQLite */
	g_test_add_func("/midgard_database_provider/sqlite_escape", midgard_test_database_provider_sqlite_escape);
	g_test_add_func("/midgard_database_provider/sqlite_double_escape", midgard_test_database_provider_sqlite_double_escape);
	g_test_add_func("/midgard_database_provider/sqlite_complex_escape", midgard_test_database_provider_sqlite_complex_escape);

	midgard_init();

	return g_test_run();
}
