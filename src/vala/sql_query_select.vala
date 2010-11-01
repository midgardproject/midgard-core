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
	public class SQLQuerySelect: GLib.Object, Executable, QueryExecutor, QuerySelect {

		/* internal properties */
		internal StorageManager _storage_manager = null;
		internal QueryStorage _query_storage = null;	
		internal QueryConstraintSimple _constraint = null;
		internal uint _limit = 0;
		internal uint _offset = 0;
		internal uint _results_count = 0;
	
		/* public properties */
		public StorageManager storagemanager { 
			get { return this._storage_manager; }
			construct { this._storage_manager = value; }
		}
		
		public QueryStorage storage {
			get { return this._query_storage; }
			construct { this._query_storage = value; }
		}

		/* Constructor */
		public SQLQuerySelect (StorageManager manager, QueryStorage storage) {
			Object (storagemanager: manager, storage: storage);
		}

		public void set_constraint (QueryConstraintSimple constraint) {
			this._constraint = constraint;
		}

		public void set_limit (uint limit) {
			this._limit = limit;
		}

		public void set_offset (uint offset) {
			this._offset = offset;
		}

		public void add_order (QueryProperty property, string type) {
			/* TODO */
			return;
		}

		public uint get_results_count () {
			return this._results_count;
		}

		public void validate () throws ValidationError {
			/* TODO */
		}

		public void execute () throws ExecutableError {
			/* TODO */
		}

		public void add_join (string type, QueryProperty left_property, QueryProperty right_property) {
			/* TODO */
		}
		
		public Storable[]? list_objects () {
			/* TODO */
			return null;
		}

		public void toggle_read_only (bool toggle) {
			/* TODO */
		}
	}
}
