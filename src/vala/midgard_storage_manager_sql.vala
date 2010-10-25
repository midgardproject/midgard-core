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

using GLib;

namespace MidgardCR {

	public class SQLStorageManager : GLib.Object, StorageManager {

		/* private properties */
		private string _name = "";
		private Config _config = null;
		private StorageContentManager _content_manager = null;
		private Transaction _transaction = null;
		private StorageWorkspaceManager _workspace_manager = null;
		private SQLStorageModelManager _model_manager = null;

		/* internal properties */
		internal SQLProfiler _profiler = null;
		internal GLib.Object _cnc = null; 
		internal GLib.Object _parser = null;
		internal ObjectModel[] _object_models = null;
		internal StorageModel[] _storage_models = null;
		/* SQLTableModel, SQLColumnModel */
		internal ObjectModel _storage_model_object_model = null;
		internal StorageModel _storage_model_storage_model = null;
		internal ObjectModel _storage_model_property_object_model = null;
		internal StorageModel _storage_model_property_storage_model = null;
		/* ObjectModel, ObjectModelProperty */
		internal ObjectModel _object_model_object_model = null;
		internal StorageModel _object_model_storage_model = null;
		internal ObjectModel _object_model_property_object_model = null;
		internal StorageModel _object_model_property_storage_model = null;

		/* public properties */
		public string name { 
			get { return this._name; }
			construct { this._name = value; }
		}
		
		public Config config { 
			get { return this._config; }
			construct { this._config = value; }
		}

		public StorageContentManager content_manager {
			get { 
				/*if (this._content_manager == null)
					this._content_manager = StorageContentManager (); */
				return this._content_manager; 
			} 
		}

		public Profiler profiler {
			get { 
				if (this._profiler == null)
					this._profiler = new MidgardCR.SQLProfiler (); 
				return (MidgardCR.Profiler) this._profiler;
			}
		}
			
		public Transaction transaction {
			get { return _transaction; }
		}
		
		public StorageWorkspaceManager workspace_manager {
			get { return _workspace_manager; }
		}

		public unowned StorageModelManager model_manager {
			get { 
				if (this._model_manager == null) {
					this._model_manager = new MidgardCR.SQLStorageModelManager ();	
					this._model_manager._storage_manager = this;
					this._model_manager._object_models = this._object_models;
					this._model_manager._storage_models = this._storage_models;
				}
				return (MidgardCR.StorageModelManager)this._model_manager; 
			}
		}	

		/**
		 * Constructor
		 *
		 * @param name Unique StorageManager name
		 * @param config MidgardCR.Config to associate with manager
                 * 
		 */ 
		public SQLStorageManager (string name, Config config) throws StorageManagerError {
			if (name == "" || name == null)
				throw new MidgardCR.StorageManagerError.NAME_INVALID ("Can not initialize SQLStorageManager with empty name");

			Object (name: name, config: config);

			/* OBJECT MODEL  */
			  /* Initialize ObjectModel for ObjectModel */
                        this._object_model_object_model = new ObjectModel ("MidgardCRObjectModel");
                        this._object_model_object_model.add_model (new ObjectModelProperty ("name", "string", ""));
                        this._object_model_object_model.add_model (new ObjectModelProperty ("parentname", "string", ""));
                        /* Initialize StorageModel for ObjectModel */
                        this._object_model_storage_model = new SQLTableModel (this, this._object_model_object_model.name, "midgard_schema_type");
                        this._object_model_storage_model.add_model (new SQLColumnModel (this, "name", "class_name", "string"));
                        this._object_model_storage_model.add_model (new SQLColumnModel (this, "parentname", "extends", "string"));

			/* Initialize property ObjectModel */
                        this._object_model_property_object_model = new ObjectModel ("MidgardCRObjectModelProperty");
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("name", "string", ""));
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("classname", "string", ""));
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("valuetypename", "string", ""));
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("valuedefault", "string", ""));
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("nick", "string", ""));
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("description", "string", ""));
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("isref", "bool", ""));
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("refname", "string", ""));
                        this._object_model_property_object_model.add_model (new ObjectModelProperty ("reftarget", "string", ""));

             		/* Initialize StorageModelProperty for ObjectModelProperty */
                        this._object_model_property_storage_model = new SQLTableModel (this, this._object_model_property_object_model.name, "midgard_schema_type_properties");
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "name", "property_name", "string"));
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "classname", "class_name", "string"));
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "valuetypename", "gtype_name", "string"));
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "valuedefault", "default_value_string", "string"));
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "nick", "property_nick", "string"));
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "description", "description", "string"));
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "isref", "is_reference", "bool"));
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "refname", "reference_class_name", "string"));
                        this._object_model_property_storage_model.add_model (new SQLColumnModel (this, "reftarget", "reference_property_name", "string"));

			/* STORAGE MODELS  */
			this._storage_model_object_model = new ObjectModel ("MidgardCRSQLTableModel");
			this._storage_model_object_model.add_model (new ObjectModelProperty ("name", "string", ""));
			this._storage_model_object_model.add_model (new ObjectModelProperty ("location", "string", ""));
			this._storage_model_object_model.add_model (new ObjectModelProperty ("description", "text", ""));
			this._storage_model_storage_model = new SQLTableModel(this, "MidgardCRSQLTableModel", "midgard_mapper_type");
                	this._storage_model_storage_model.add_model (new SQLColumnModel (this, "name", "class_name", "string"));
                	this._storage_model_storage_model.add_model (new SQLColumnModel (this, "location", "table_name", "string"));
                	this._storage_model_storage_model.add_model (new SQLColumnModel (this, "description", "description", "text"));
		
			/* SQLColumnModel models */
			this._storage_model_property_object_model = new ObjectModel ("MidgardCRSQLColumnModel");
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("name", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("location", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("valuetypename", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("valuegtype", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("valuedefault", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("index", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("primary", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("description", "text", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("tablename", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("isref", "bool", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("refname", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("reftarget", "string", ""));
			this._storage_model_property_object_model.add_model (new ObjectModelProperty ("propertyof", "string", ""));
			this._storage_model_property_storage_model = new SQLTableModel(this, "MidgardCRSQLColumnModel", "midgard_mapper_columns");
                	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "name", "property_name", "string"));
                	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "location", "column_name", "string"));
	               	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "valuetypename", "gtype_name", "string"));
	               	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "index", "has_index", "bool"));
	               	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "primary", "is_primary", "bool"));
                	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "description", "description", "text"));
			this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "tablename", "table_name", "string"));
                	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "isref", "is_reference", "bool"));
                	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "refname", "reference_table_name", "string"));
                	this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "reftarget", "reference_column_name", "string"));
			this._storage_model_property_storage_model.add_model (new SQLColumnModel (this, "propertyof", "property_of", "string"));


		}
		
		/**
		 * Opens connection to database defined in MidgardCR.Config associated with StorageManager.
		 * 
		 * If, StorageManager opened connection already, silently returns true.
		 */	
		public bool open () throws MidgardCR.StorageManagerError { 
			if (this._cnc != null)
				return true;

			if (MidgardCRCore.SQLStorageManager.open (this)) {
				this.opened (); /* emit 'opened' signal */
				return true;
			}
			
			return false;
		}

		public bool close () throws MidgardCR.StorageManagerError { 
			if (!MidgardCRCore.SQLStorageManager.close (this))
				return false;
				
			this.closed (); /* emit 'closed' signal */
			return true;
		}

		public bool initialize_storage () throws MidgardCR.StorageManagerError {
			if (!MidgardCRCore.SQLStorageManager.initialize_storage (this))
				return false;

			return true;
		}

		public StorageManager fork () { return null; }
		public StorageManager clone () { return null; }
	}
}
