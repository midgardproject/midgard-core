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
	 * SQLTableModel describes relation between class and it's table in SQL database.
	 * It's name property holds the name of the class, and location holds the name of the table.
	 *
	 * Property and column name mapping is done with {@link SQLColumnModel}. 
	 * 
	 * {@link SQLModelManager} provides access to all available schema and SQL storage models.
	 * 
	 */ 
	public class SQLTableModel: GLib.Object, Executable, StorageExecutor, Model, StorageModel {
		
		/* private properties */

		private Model[] _models = null;
		private bool _create_table = false;
		private bool _update_table = false;
		private bool _drop_table = false;
		private ObjectModel _object_model = null;
		private string[] _queries = null;

		/* internal properties */

		internal uint _id = 0;
		internal SQLStorageManager _storage_manager = null;
		internal SQLModelManager _model_manager = null;
	
		/* public properties */
		
		/** 
		 * SQL database' table name, to store data for objects of the class
		 * this model is initialized for.
		 */ 
		public string location { get; set;}	

		/**
                 * Holds the name of the class, StorageModel is initialized for.
                 */
                public string name { get; set; }
                /**
                 * Holds the namespace of model's class
                 */
                public string? @namespace { get; set; }
                /**
                 * Parent model of the model.
                 */
                public Model? parent { get; set; }
		/**
		 * Holds reference to {@link SQLStorageManager} object 
		 */
		public unowned StorageManager storagemanager { 	
			construct { this._storage_manager = (SQLStorageManager)value; }
		}			

		/**
		 * Constructor
		 * 
		 * Creates new SQLTableModel object for given class name and location.
		 *
		 * Created SQLTableModel object describes database table (location) for the class
		 * it is initialized for. For example, data of 'MyContacts' class objects should be 
		 * stored in table 'my_contacts_tbl'. In such case, 'MyContacts' should be classname, 
		 * and 'my_contacts_tbl' should be location.
		 * 
		 * @param classname name of the class 
		 * @param location table name
		 */
		public SQLTableModel (SQLStorageManager manager, string classname, string location) {
			Object (storagemanager: manager, name: classname, location: location);
			/* Silent valac during compile time */
			this._id = 0;
			this._object_model = null;
		}

		public unowned StorageManager get_storagemanager () {
			return this._storage_manager;
		}
	
		/**
		 * Add new property model 
		 *
		 * Any model added to SQLTableModel one, should be an instance
		 * of {@link SQLColumnModel}. In any other case, model 
		 * will be marked as invalid, during validation process.  
		 *
		 * @param model {@link Model} to add
		 *
		 * @return {@link Model} instance (self reference)
		 */	
		public Model add_model (Model model) { 
			this._models += model;			
			model.parent = this;
			return this; 
		}

		/**
		 * Get {@link SQLColumnModel} model by given name.
		 * 
		 * @param name {@link Model} name to look for
		 *
		 * @return {@link Model} if found, null otherwise
		 */
		public unowned Model? get_model_by_name (string name) { 
			if (this._models == null)
				return null; 

			foreach (unowned Model model in this._models) {
				if (model.name == name)
					return model;
			}
			
			return null;
		}

		/**
		 * List all associated models
		 *
		 * @return array of {@link Model} objects, or null if none associated
		 */
		public unowned Model[]? list_models () { 
			return this._models; 
		}
		
		/**
		 * Check whether model is valid
		 *
		 * Conditions to mark model as invalid:
		 * * models with duplicated names
		 * * empty models
		 *
		 */
 		public void is_valid () throws ValidationError { 
			if (this._models == null)
				throw new MidgardCR.ValidationError.REFERENCE_INVALID ("No property models associated.");

			/* TODO */
			/* Check if name does not containt invalid characters */

			string[] names = new string[0];
			foreach (MidgardCR.Model model in this._models) {
				/* TODO, check if it's ObjectModel or StorageModel */	
				if (!(model is SQLColumnModel))
					throw new ValidationError.TYPE_INVALID ("Invalid '%s' model associated with  SQLTableModel. Expected SQLColumnModel", model.get_type().name());
				foreach (string name in names) {
					if (name == model.name)
						throw new MidgardCR.ValidationError.NAME_DUPLICATED ("SQLTableModel: duplicated '%s' model found in '%s' model", model.name, this.name);
				}
				names += model.name;
			}	
		}					

		private void _emit_execution_start () {
			if (this._model_manager != null)
				this._model_manager.execution_start();
		}

		private void _emit_execution_end () {
			if (this._model_manager != null)
				this._model_manager.execution_end();
		}

		/**
		 * Create new {@link SQLColumnModel}
		 */
		public SQLColumnModel create_model_property (string name, string location, string type) {
			SQLColumnModel model = new SQLColumnModel ( (SQLStorageManager)this.get_storagemanager (), name, location, type);
			model.parent = this;
			model.execution_start.connect (this._emit_execution_start);
                        model.execution_end.connect (this._emit_execution_end);
			return model;
		}

		/* StorageExecutor methods */
		public bool exists () {
			return false;
		} 
		
                public void prepare_create () throws ValidationError {
			this.is_valid ();
			this._create_table = true;
			/* Prepare table data to create */
			this._queries += MidgardCRCore.StorageSQL.create_query_insert (this, 
				this._storage_manager._storage_model_object_model, this._storage_manager._storage_model_storage_model);
			/* Prepare columns to create */
			foreach (MidgardCR.Model model in this._models) {	
				((StorageExecutor)model).prepare_create ();
			}
		}

                public void prepare_update () throws ValidationError {
			this.is_valid ();
			this._update_table = true;
			/* No query to update table */			
			/* Prepare columns to update */
			foreach (MidgardCR.Model model in this._models) 
				((StorageExecutor)model).prepare_update ();			
		}

                public void prepare_save () throws ValidationError {
			this.is_valid ();
			this._create_table = true;
			this._update_table = true;
			/* Prepare columns to update and create if do not exist */
			foreach (MidgardCR.Model model in this._models) {
				((StorageExecutor)model).prepare_create ();
				((StorageExecutor)model).prepare_save ();
			}			
		}

                public void prepare_remove () throws ValidationError {
			this.is_valid ();
			this._drop_table = true;	
			/* Ignore prepare_remove call for properties, we'll drop table anyway */	
		}

                public void prepare_purge () throws ValidationError {
			this.is_valid ();
			this._drop_table = true;
			/* Ignore prepare_remove call for properties, we'll drop table anyway */	
		}

		/* Executable methods */
		public void execute () throws ExecutableError { 
			/* emit 'execution_start' signal */
			execution_start ();
			if (this._create_table)
				MidgardCRCore.SQLStorageManager.table_create (this._storage_manager, this);

			if (this._drop_table)
				MidgardCRCore.SQLStorageManager.table_remove (this._storage_manager, this);

			foreach (weak string query in this._queries) {
				MidgardCRCore.SQLStorageManager.query_execute (this._storage_manager, query);
			}

			foreach (MidgardCR.Model model in this._models) {
				((StorageExecutor)model).execute ();			
			}

			this._create_table = false;
			this._update_table = false;
			this._drop_table = false;
			this._queries = null;
			/* emit 'execution_end' signal */
			execution_end ();
		}	

		/**
		 * {@inheritDoc}
		 * Associated or parent models are not copied to model's copy.
		 * Created copy is initialized for the same {@link StorageManager}.
		 *
		 * Properties copied:
		 *
		 *  * name
		 *  * location
		 *  * namespace
		 *
		 * @return new SQLTableModel
		 */
		public Model? copy () {
			var copy = new SQLTableModel ((SQLStorageManager) this.get_storagemanager (), this.name, this.location);
			copy.namespace = this.namespace;
			return (Model) copy;
		}	
	}
}
