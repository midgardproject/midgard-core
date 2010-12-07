/* 
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
 * 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
 */

namespace MidgardCR
{
	public class RDFSQLQuerySelect: SQLQuerySelect {

		/* internal properties */
		internal RDFSQLQueryStorage _rdf_query_storage = null;	
	
		/* Constructor */
		public RDFSQLQuerySelect (StorageManager manager, RDFSQLQueryStorage storage) {
			base (manager, (SQLQueryStorage) storage);
		}
		
		public static new RDFSQLQuerySelect create_query_select (StorageManager manager, RDFSQLQueryStorage storage) {
			return new RDFSQLQuerySelect (manager, storage);
		}
	}
}
