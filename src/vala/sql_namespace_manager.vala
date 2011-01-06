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
		internal string[] _ids = null;
		private SQLContentManager _content_manager = null;

		/* constructor */
		public SQLNamespaceManager (SQLContentManager manager) {
			Object ();
			this._content_manager = manager;
		}

		/* methods */

		/**
		 * {@inheritDoc}
		 * 
		 * Creates mapping between name and identifier and stores this information in SQL database.  
		 */
		public void create_mapping (string name, string id) throws NamespaceManagerError {
			if (this.identifier_exists (id))
				throw new NamespaceManagerError.IDENTIFIER_EXISTS ("%s identifier already exists", id);
			if (this.name_exists (name))
				throw new NamespaceManagerError.NAME_EXISTS ("%s name already exists", name);	

			this._names += name;
			this._ids += id;

			/* TODO */
			/* Store it in a table */
		}

		/**
		 * {@inheritDoc}
		 */
		public  unowned string[]? list_names () {
			if (this._names == null)
				return null;
			return this._names;
		}

		/**
		 * FIXME inheritDoc (valadoc segfaults)
		 *
		 * Get identifier which is mapped to given name
		 */
		public unowned string? get_identifier_by_name (string name) {
			if (this._names == null)
				return null;
			uint i = 0;
			foreach (unowned string n in this._names) {
				if (n == name)
					break;
				i++;
			}
			return this._ids[i];
		}

		public unowned string? get_name_by_identifier (string id) {
			if (this._ids == null)
				return null;
			uint i = 0;
			foreach (unowned string u in this._ids) {
				if (u == id)
					break;
				i++;
			}
			return this._names[i];
		}

		public bool name_exists (string name) {	
			if (this._names == null)
				return false;
			foreach (unowned string n in this._names) {
				if (n == name)
					return true;
			}
			return false;
		}

		public bool identifier_exists (string id) {
			if (this._ids == null)
				return false;
			foreach (unowned string u in this._ids) {
				if (u == id)
					return true;
			}
			return false;
		}
	}
}
