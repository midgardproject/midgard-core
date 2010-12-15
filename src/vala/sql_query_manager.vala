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

	public class SQLQueryManager : GLib.Object, QueryManager {
		
		/* private properties */
		private SQLStorageManager _storage_manager = null;

		/* public properties */
		public unowned StorageManager storage_manager {
			get {
				return this._storage_manager;
			}
			construct {
				this._storage_manager = (SQLStorageManager) value; 
			}
		}

		/* constructor */
		public SQLQueryManager (SQLStorageManager storage) {
			Object (storage_manager: storage);
		}

		public QuerySelect create_query_select (string classname) {
			return new SQLQuerySelect (this.storage_manager, new SQLQueryStorage (classname));
		}

		public unowned StorageManager get_storage_manager () {
			return this.storage_manager;
		}
	}
}
