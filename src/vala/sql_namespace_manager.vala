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

	public class SQLNamespaceManager : GLib.Object, NamespaceManager {

		internal string[] _names = null;
		internal string[] _uris = null;
		internal SQLStorageManager _storage_manager = null;

		/* constructor */
		public SQLNamespaceManager (SQLStorageManager manager) {
			Object ();
			this._storage_manager = manager;
		}

		/* methods */
		public bool create_uri (string uri, string name) throws NamespaceManagerError {
			return false;
		}

		public  unowned string[]? list_names () {
			if (this._names == null)
				return null;
			return this._names;
		}

		public unowned string? get_uri_by_name (string name) {
			uint i = 0;
			foreach (unowned string n in this._names) {
				if (n == name)
					break;
				i++;
			}
			return this._uris[i];
		}

		public unowned string? get_name_by_uri (string uri) {
			return null;
		}

		public bool name_exists () {
			return false;
		}

		public bool uri_exists () {
			return false;
		}
	}
}
