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
		internal SQLQueryStorage _query_storage = null;	
		internal GLib.Object _core_query_select = null;
		internal QueryConstraintSimple _constraint = null;
		internal uint _n_constraints = 0;
		internal uint _limit = 0;
		internal uint _offset = 0;
		internal uint _results_count = 0;
		internal SList _orders = null;
		internal SList _joins = null;
		internal Object _stmt = null;
		internal Object _resultset = null;
		internal bool _readonly = false;
		
		/* public properties */
		public StorageManager storagemanager { 
			get { return this._storage_manager; }
			construct { this._storage_manager = value; }
		}
		
		public SQLQueryStorage storage {
			get { return this._query_storage; }
			construct { this._query_storage = value; }
		}

		public bool readonly { 
			get { return this._readonly; } 
			set { this._readonly = value; }
		}

		private void _create_core_query_select () {
			if (this._core_query_select == null)
				this._core_query_select = (MidgardCRCore.QuerySelect) MidgardCRCore.QuerySelect.create_static (this._storage_manager, (MidgardCRCore.QueryStorage) this._query_storage._core_query_storage);
		}

		/* Constructor */
		public SQLQuerySelect (StorageManager manager, SQLQueryStorage storage) {
			Object (storagemanager: manager, storage: storage);
			this._create_core_query_select ();
		}
		
		public static SQLQuerySelect create_query_select (StorageManager manager, SQLQueryStorage storage) {
			return new SQLQuerySelect (manager, storage);
		}

		public void set_constraint (QueryConstraintSimple constraint) {
			this._create_core_query_select ();
			(this._core_query_select as MidgardCRCore.QuerySelect).set_constraint ((constraint as MidgardCR.SQLQueryConstraint)._core_query_constraint as MidgardCRCore.QueryConstraint);
			this._constraint = constraint;
		}

		public void set_limit (uint limit) {
			this._create_core_query_select ();
			this._limit = limit;
		}

		public void set_offset (uint offset) {
			this._create_core_query_select ();
			this._offset = offset;
		}

		public void add_order (QueryProperty property, string type) {
			this._create_core_query_select ();
			/* TODO */
			return;
		}

		public uint get_results_count () {
			this._create_core_query_select ();
			return this._results_count;
		}

		public void validate () throws ValidationError {
			this._create_core_query_select ();
			/* TODO */
		}

		public void execute () throws ExecutableError {
			this._create_core_query_select ();
			((MidgardCRCore.QueryExecutor)this._core_query_select).execute ();
		}

		public void add_join (string type, QueryProperty left_property, QueryProperty right_property) {
			this._create_core_query_select ();
			/* TODO */
		}
		
		public Storable[]? list_objects () {
			this._create_core_query_select ();	
			return ((MidgardCRCore.QuerySelect)this._core_query_select).list_objects ();
		}

		public void toggle_read_only (bool toggle) {
			this._create_core_query_select ();
			/* TODO */
		}
	}
}
