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

	/**
	 * SQLStorageContentManager's purpose is to create, update, or delete object's data
	 * in underlying SQL database. Any {@link Storable} object is volatile data, and once 
	 * created with content manager becomes persistant.
	 */	 
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
			this._storage_manager.operation_start ();
			this._storage_manager.operation_end ();
			return false;
		}	

		/**
		 * Creates object's record in SQL table.
		 * 
		 * @param object Storable object
		 *
		 * This method requires valid {@link SQLTableModel}, available from {@link SQLStorageModelManager}.
		 * If such is found, object's properties values will be stored in table, which name, 
		 * is declared as table model's location.
		 *
		 * If given object is {@link RepositoryObject}, valid SQL INSERT prepared statament is generated for 
		 * object's class and stored as permanent statement available for every class instance.
		 */
		public virtual void create (Storable object) throws StorageContentManagerError {
			string name = object.get_type().name();
			SQLTableModel table_model = 
				((SQLStorageModelManager)this._storage_manager.model_manager).get_table_model_by_name (name);
			if (table_model == null)
				throw new StorageContentManagerError.MODEL_INVALID ("SQL table model not available for %s class", name);
			ObjectModel object_model = 
				((SQLStorageModelManager)this._storage_manager.model_manager).get_object_model_by_name (name);
			this._storage_manager.operation_start ();
			MidgardCRCore.SQLStorageContentManager.storable_insert (object, this._storage_manager, object_model, table_model);
			this._storage_manager.operation_end ();
	}	

		/**
		 * Updates object's record in SQL table.
		 * 
		 * @param object Storable object
		 *
		 * This method requires valid {@link SQLTableModel}, available from {@link SQLStorageModelManager}.
		 * If such is found, object's properties values will be stored in table, which name, 
		 * is declared as table model's location.
		 *
		 * If given object is {@link RepositoryObject}, valid SQL UPDATE prepared statament is generated for 
		 * object's class and stored as permanent statement available for every class instance.
		 */
		public virtual void update (Storable object) throws StorageContentManagerError {
			string name = object.get_type().name();
			SQLTableModel table_model = 
				((SQLStorageModelManager)this._storage_manager.model_manager).get_table_model_by_name (name);
			if (table_model == null)
				throw new StorageContentManagerError.MODEL_INVALID ("SQL table model not available for %s class", name);
			ObjectModel object_model = 
				((SQLStorageModelManager)this._storage_manager.model_manager).get_object_model_by_name (name);
			this._storage_manager.operation_start ();
			MidgardCRCore.SQLStorageContentManager.storable_update (object, this._storage_manager, object_model, table_model);
			this._storage_manager.operation_end ();
		}		

		/**
		 * Saves object's record in SQL table
		 *
		 * @param object Storable object
		 *
		 * This method requires valid {@link SQLTableModel}, available from {@link SQLStorageModelManager}.
		 * If such is found, object's properties values will be stored in table, which name, 
		 * is declared as table model's location.
		 *
		 * This method is slower than create () one and equal to update ().
		 * It's very important to note that this method tries to update object's record first, 
		 * and in case of false update performs create operation.
		 * Of course, create operation is executed only (and only) if update fails due to 
		 * non exisiting record identified by object's guid. 
		 * In case of other failure, particular exception is thrown. 	
		 *
		 */
		public virtual void save (Storable object) throws StorageContentManagerError {
			string name = object.get_type().name();
			SQLTableModel table_model = 
				((SQLStorageModelManager)this._storage_manager.model_manager).get_table_model_by_name (name);
			if (table_model == null)
				throw new StorageContentManagerError.MODEL_INVALID ("SQL table model not available for %s class", name);
			ObjectModel object_model = 
				((SQLStorageModelManager)this._storage_manager.model_manager).get_object_model_by_name (name);
			try {
				this._storage_manager.operation_start ();
				MidgardCRCore.SQLStorageContentManager.storable_update (object, this._storage_manager, object_model, table_model);			
				this._storage_manager.operation_end ();
			} catch (StorageContentManagerError e) {
				if (e is StorageContentManagerError.OBJECT_NOT_EXISTS) {
					this._storage_manager.operation_start ();
					MidgardCRCore.SQLStorageContentManager.storable_insert (object, this._storage_manager, object_model, table_model);
					this._storage_manager.operation_end ();
				}
			}
		}		

		public virtual void purge (Storable object) throws StorageContentManagerError {
			string name = object.get_type().name();
			SQLTableModel table_model = 
				((SQLStorageModelManager)this._storage_manager.model_manager).get_table_model_by_name (name);
			if (table_model == null)
				throw new StorageContentManagerError.MODEL_INVALID ("SQL table model not available for %s class", name);
			ObjectModel object_model = 
				((SQLStorageModelManager)this._storage_manager.model_manager).get_object_model_by_name (name);
			this._storage_manager.operation_start ();
			MidgardCRCore.SQLStorageContentManager.storable_purge (object, this._storage_manager, object_model, table_model);
			this._storage_manager.operation_end ();

		}	
	
		public virtual void remove (Storable object) throws StorageContentManagerError {
			this._storage_manager.operation_start ();
			this.purge (object);
			this._storage_manager.operation_end ();
		}		

		public unowned QueryManager get_query_manager () {
			return null;
		}
	}
}
