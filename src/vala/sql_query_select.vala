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
			/* Dummy, to silent valac during compile time. We need those in MidgardCRCore. */
			this._stmt = null;
			this._resultset = null;	
		}
		
		public static SQLQuerySelect create_query_select (StorageManager manager, SQLQueryStorage storage) {
			return new SQLQuerySelect (manager, storage);
		}

		public virtual void set_constraint (QueryConstraintSimple constraint) {
			this._create_core_query_select ();
			/* Keep this condition. We need to make proper typecasting to internal,
			 * core's query constraints. We can not type cast to interface, 
			 * and we should not provide internal methods for constraint classes */
			if (constraint is SQLQueryConstraint)
				(this._core_query_select as MidgardCRCore.QuerySelect)
					.set_constraint ((MidgardCRCore.QueryConstraintSimple) ((SQLQueryConstraint)constraint)._core_query_constraint);
			else if (constraint is SQLQueryConstraintGroup)
				(this._core_query_select as MidgardCRCore.QuerySelect)
					.set_constraint (((MidgardCRCore.QueryConstraintSimple) ((SQLQueryConstraintGroup)constraint)._core_query_constraint));
			this._constraint = constraint;
		}

		public virtual unowned QueryConstraintSimple get_constraint () {
			return this._constraint;
		}

		public void set_limit (uint limit) {
			this._create_core_query_select ();	
			(this._core_query_select as MidgardCRCore.QuerySelect).set_limit (limit);
		}

		public void set_offset (uint offset) {
			this._create_core_query_select ();
			(this._core_query_select as MidgardCRCore.QuerySelect).set_offset (offset);
		}

		public void add_order (QueryProperty property, string type) {
			this._create_core_query_select ();
			(this._core_query_select as MidgardCRCore.QuerySelect).add_order (property._core_query_holder as MidgardCRCore.QueryProperty, type);
	
		}

		public uint get_results_count () {
			this._create_core_query_select ();
			return (this._core_query_select as MidgardCRCore.QuerySelect).get_results_count ();
		}

		public virtual void validate () throws ValidationError {
			this._create_core_query_select ();
			/* TODO */
		}

		public virtual void execute () throws ExecutableError {
			this._storage_manager.operation_start ();
			this._create_core_query_select ();
			((MidgardCRCore.QueryExecutor)this._core_query_select).execute ();
			this._storage_manager.operation_end ();
		}

		public void add_join (string type, QueryProperty left_property, QueryProperty right_property) {
			this._create_core_query_select ();
			(this._core_query_select as MidgardCRCore.QuerySelect).add_join (type, left_property._core_query_holder as MidgardCRCore.QueryProperty, right_property._core_query_holder as MidgardCRCore.QueryProperty);
		}
		
		public virtual Storable[]? list_objects () {
			this._create_core_query_select ();	
			return ((MidgardCRCore.QuerySelect)this._core_query_select).list_objects ();
		}

		public void toggle_read_only (bool toggle) {
			this._create_core_query_select ();
			/* TODO */
		}
	}
}
