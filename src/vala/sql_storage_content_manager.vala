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

	public class SQLStorageContentManager : GLib.Object, StorageContentManager {
	
		/* internal properties */
		internal SQLStorageManager _storage_manager = null;

		/* public properties */
		public StorageManager storagemanager {
			get { return this._storage_manager; }
			construct { this._storage_manager = (SQLStorageManager) value; }
		}

		/* constructor */
		public 	SQLStorageContentManager (SQLStorageManager manager) {
			Object (storagemanager: manager);
		}

		/* public methods */

		public unowned StorageManager get_storage_manager () {
			return this._storage_manager;
		}

		public virtual bool exists (Storable object) {
			return false;
		}	

		public virtual bool create (Storable object) throws StorageContentManagerError {
			string name = object.get_type().name();
			SQLTableModel table_model = ((SQLStorageModelManager)this._storage_manager.model_manager).get_table_model_by_name (name);
			ObjectModel object_model = ((SQLStorageModelManager)this._storage_manager.model_manager).get_object_model_by_name (name);
			MidgardCRCore.SQLStorageContentManager.storable_insert (object, this._storage_manager, object_model, table_model);
			return true;
		}	
	
		public virtual bool update (Storable object) throws StorageContentManagerError {
			return false;
		}		

		public virtual bool save (Storable object) throws StorageContentManagerError {
			return false;
		}		

		public virtual bool remove (Storable object) throws StorageContentManagerError {
			return false;
		}	
	
		public virtual bool purge (Storable object) throws StorageContentManagerError {
			return false;
		}		

		public unowned QueryManager get_query_manager () {
			return null;
		}
	}
}
