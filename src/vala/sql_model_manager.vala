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

	public class SQLModelManager : GLib.Object, Model, Executable, StorageExecutor, StorageModelManager {

		/* internal properties */
		internal unowned SQLStorageManager _storage_manager = null;
		internal NamespaceManager _ns_manager = null;
		internal unowned StorageModel[] _storage_models = null;
		internal unowned ObjectModel[] _object_models = null;
		internal Model[] _models = null;
		internal Model[] _models_registered = null;
		internal string[] _queries = null;

		/* private properties */
		private Model[]? _tmp_models = null;

		/* Model properties */
		/**
		 * SQLModelManager doesn't hold parent model.
		 */	
		public Model? parent { 
			get { return null; }
			set { return; }
		}
		
		/**
		 * Namespace of the manager (if required)
		 */
                public string? @namespace { get; set;  }

		/**
		 * Name of the manager (if required)
		 */
                public string name { get; set;  }

		/* properties */
		/**
		 * {@link NamespaceManager} of the manager
		 */
		public NamespaceManager namespace_manager { 
			get { return this._ns_manager; }
		}

		/**
		 * {@link StorageManager}
		 */
		public StorageManager storagemanager   { 
			get { return (StorageManager)this._storage_manager; }
			construct { this._storage_manager = (SQLStorageManager) value; } 
		} 		
	
		/* Model methods */
		/**
                 * Associate new model 
                 *
                 * @param model {@link Model} to add
                 *
                 * @return {@link Model} instance (self reference)
                 */
		public virtual Model add_model (Model model) {
			this._models += model;
			return this;
		}
				
		/**
                 * Get model by given name. 
		 * SQLModelManager holds {@link ObjectModel} and {@link StorageModel} models,
		 * so accepted name by the one of Schema or Storage model.
	         * 
                 * @param name {@link Model} name to look for
                 *
                 * @return {@link Model} if found, null otherwise
                 */
                public virtual unowned Model? get_model_by_name (string name) {
			unowned Model model = this._find_model_by_name (this._models, name);
			if (model == null)
				model = this._find_model_by_name (this._models_registered, name);
			return model;
		}

		private unowned Model? _find_model_by_name (Model[] models, string name) {
			if (models == null)
				return null;

			foreach (unowned Model model in models) {
				if (model.name == name)
					return model;
			}
			return null;
		}

		private void _find_object_references_rec (Model model) {
			Model[]? property_models = model.list_models ();
			foreach (unowned Model p_model in property_models) {
				this._find_object_references_rec (p_model);
				if (p_model is ObjectModel) {
					if (this._find_model_by_name (this._models, p_model.name) == null)
						this._tmp_models += (Model)p_model;
				}
			}
		}

		private void _find_references () {
			foreach (Model model in this._models) {
				if (model is ObjectModel)
					this._find_object_references_rec (model);
				this._tmp_models += model;
			}
			this._models = this._tmp_models;
			this._tmp_models = null;
		}

		/**
		 * Perform all checks required to mark instance as valid.
		 */
                public virtual void is_valid () throws ValidationError {
			if (((MidgardCR.SQLStorageManager)this._storage_manager)._cnc == null)
				throw new MidgardCR.ValidationError.INTERNAL ("StorageManager not connected to any database"); 		
		}

		/* StorageExecutor methods */

		public bool exists () {
			return false;
		}

		private void _add_parent_model_properties (StorageModel model) {
			if (model.parent != null)
				return; /* TODO, add parent class storage models */
	
			/* There's no parent model defined so fallback to RepositoryObject storage model */
			StorageModel rosmodel = this._storage_manager._storage_model_repository_object;
			if (rosmodel == null)
				return;
			foreach (Model pmodel in rosmodel.list_models ()) {
				if (pmodel is SQLColumnModel)	
					model.add_model (pmodel.copy ());
			}
		}

		/**
		 * Prepares create operation for all associated models.
		 * Valid SQL query (or prepared statement) is generated 
		 * in this method, so every model added after this method 
		 * call will be ignored and none of its data will be included in 
		 * executed query.
		 */
		public virtual void prepare_create () throws ValidationError {
			this.is_valid ();
			this._find_references ();

			/* Validate models */
			foreach (Model model in this._models) {
				unowned Model model_found = this._find_model_by_name ((Model[])this._object_models, model.name);
				if (model_found != null)
					throw new MidgardCR.ValidationError.NAME_DUPLICATED ("%s class already exists in ObjectModel table", model.name);
			}

			/* Prepare create for every StorageExecutor derived */
			foreach (Model model in this._models) {
				if (model is StorageExecutor) {	
					this._add_parent_model_properties ((StorageModel) model); 			 	
					((StorageExecutor)model).prepare_create ();
				} else if (model is ObjectModel) {
					/* FIXME, Any ReferenceObject should not be added to model manager */
					if (model is ObjectModelReference)
						continue;
					string query = MidgardCRCore.StorageSQL.create_query_insert (
						model, 
						this._storage_manager._object_model_object_model, 
						this._storage_manager._object_model_storage_model);
					this._queries += query;	
					Model[] property_models = model.list_models();
					foreach (Model property_model in property_models) {
						/* Ignore reference object */
						if (property_model is ObjectModel)
							continue;
						/* prepare insert query for every property */
						query = MidgardCRCore.StorageSQL.create_query_insert (
							property_model, 
							this._storage_manager._object_model_property_object_model, 
							this._storage_manager._object_model_property_storage_model);
						this._queries += query;
					}
				}
			}
		}

		private void _model_check_in_storage () throws ValidationError {
			bool found = false;
			string invalid_name = "";
			foreach (Model model in this._models) {
				unowned Model model_found = this._find_model_by_name ((Model[])this._object_models, model.name);
				if (model_found == null)
					model_found = this._find_model_by_name ((Model[])this._storage_models, model.name);
				if (model_found != null 
					&& (((MidgardCR.ObjectModel) model_found)._id == ((MidgardCR.ObjectModel) model)._id)) {
					found = true;
					invalid_name = model.name;
					break;
				}
			}
			if (!found)
				throw new MidgardCR.ValidationError.NAME_INVALID ("No entry in schema or storage table found for given %s ", invalid_name); 
		}

		/**
		 * Prepares update operation.
		 */
                public virtual void prepare_update () throws ValidationError {
			this.is_valid ();
			this._model_check_in_storage ();
		}

		/**
		 * Prepares save operation. 
		 */
                public virtual void prepare_save () throws ValidationError {
			this.is_valid ();
			this._model_check_in_storage ();
		}

		/**
		 * Prepares remove operation.
		 */
                public virtual void prepare_remove () throws ValidationError {
			this.is_valid ();
			this._model_check_in_storage ();
		}

		/**
		 * Prepares purge operation.
		 */
                public virtual void prepare_purge () throws ValidationError {
			this.is_valid ();
			this._model_check_in_storage ();
		}

		/* Executable methods */
		/**
		 * Perform prepared operations.
		 *
		 * Executes SQL query (or queries) generated in prepare methods.
		 * There is no need to invoke execute per every prepare method.
		 * Any prepared query (or prepared statement) is kept and executed
		 * in this method, so there's no limit how many SQL queries might be 
		 * executed for underlying SQL storage engine.
		 */
		public virtual void execute () throws ExecutableError {		
			/* Execute command for every StorageExecutor derived object */
			foreach (Model model in this._models) {
				if (model is StorageExecutor) { 	
					((StorageExecutor)model).execute ();
				}
			}
			/* Store info about classes and properties */
                        foreach (weak string query in this._queries) {
                                execution_start ();
                                MidgardCRCore.SQLStorageManager.query_execute (this._storage_manager, query);
                                execution_end ();
                        }
			this._models_registered = this._models;
			this._models = null;
			this._queries = null;
			try {
				MidgardCRCore.SQLStorageManager.load_models (this._storage_manager);
				this._object_models = this._storage_manager._object_models;
                                this._storage_models = this._storage_manager._storage_models;
			} catch (StorageManagerError e) {
				throw new ExecutableError.INTERNAL ("%s", e.message);
			}
		}

		/* methods */

		/**
		 * {@inheritDoc} 
		 */
		public virtual unowned Model[]? list_models_by_type (string type) {
			if (type == "ObjectModel")
				return (Model[]) this._object_models;
			if (type == "StorageModel")
				return (Model[]) this._storage_models;
			return null;
		}
	
		/**
		 * {@inheritDoc}
		 */
		public virtual string[]? list_model_types () {
			string[] _types = null;
			_types += "ObjectModel";
			_types += "StorageModel";
			return _types;
		}
	
		private unowned SQLTableModel? get_table_model_by_name (string name) {
			if (this._storage_models == null)
				return null;
			foreach (StorageModel model in this._storage_models) {
				if (model.name == name)
					return (SQLTableModel)model;
			}
			return null;
		}

		private unowned ObjectModel? get_object_model_by_name (string name) {
			if (this._storage_models == null)
				return null;
			foreach (unowned ObjectModel model in this._object_models) {
				if (model.name == name)
					return model;
			}
			return null;
		}		

		public virtual unowned Model? get_type_model_by_name (string type, string name) {
			if (type == "ObjectModel")
				return this.get_object_model_by_name (name);
			if (type == "StorageModel")
				return this.get_table_model_by_name (name);
			return null;
		}

		/**
		 * List all models associated with with an instance.
		 * @see list_models_by_type
		 *
		 * @return array of models or null
		 */
		public virtual unowned Model[]? list_models () {
			return this._models;
		}

		/**
		 * SQLModelManager can not be copied.
		 * @return null
		 */
		public virtual Model? copy () {
			return null;
		}

		/**
		 * {@inheritDoc}
		 */
		public unowned StorageManager get_storage_manager () {
			return this.storagemanager;
		}
	}
}
