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

namespace MidgardCR {

	public class RDFSQLQueryManager : SQLQueryManager {
		
		/* private properties */
		private RDFSQLStorageManager _storage_manager = null;

		/* public properties */
		public new unowned StorageManager storage_manager {
			get {
				return this._storage_manager;
			}
			construct {
				this._storage_manager = (RDFSQLStorageManager) value; 
			}
		}

		/* constructor */
		public RDFSQLQueryManager (RDFSQLStorageManager storage) {
			Object (storage_manager: storage);
		}

		public override QuerySelect create_query_select (string classname) {
			return new RDFSQLQuerySelect (this.storage_manager, new RDFSQLQueryStorage (classname));
		}
	}
}
