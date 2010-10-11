
namespace MidgardCR {

	/**
	 * Describes table column in SQL database.
	 *
	 * Every SQLStorageModelProperty object describes one column of one table in SQL database.
	 * It's required to create column or alter it in the table described by the parent model.
	 * And most important, SQLStorageModelProperty describes the column in which, value of object's property should be stored.
	 *
	 * Let's suppose, there's 'title' property, of string type, which value shoudl be stored in 
	 * 'title_col' column, and default value for every field should be 'Alice'.
	 * 
	 * {{{
	 *   SQLStorageModelProperty model = new SQLStorageModelProperty ("title", "title_col", "string");
	 *   model.valuedefault = 'Alice';
	 * }}}
	 *
	 */ 
	public class SQLStorageModelProperty : GLib.Object, Executable, StorageExecutor, Model, StorageModel, ModelProperty, StorageModelProperty {

		/* private properties */

		private Model[] _models = null;
		private string _typename = null;
		private GLib.Type _gtype = 0;
		private bool _create_column = false;
		private bool _update_column = false;
		private bool _remove_column = false;
		private string[] _queries = null;

		/* internal properties */

		internal uint _id = 0;
		internal unowned SQLStorageManager _storage_manager = null;

		/* public properties */
	
		/**
		 * Holds the name of property for which model is created
		 */
		public string name { get; set; }
	
		/**
		 * Gtype of the property
		 */
		public GLib.Type valuegtype { 
			get { return this._gtype; } 
		}
	
		/**
		 * The type of the property.
		 * Acceptable name of any type registered in GType system ('string', 'int', 'bool'...).
		 */ 
		public string valuetypename { 
			get { return this._typename; }
			set { 
				this._typename = value;
				this._set_gtype_from_name ();
			}	
		}
		
		/**
		 * Default value of property for newly created objects
		 */
		public string valuedefault { get; set; }

		/**
		 * Description of the property
		 */
		public string description { get; set; }

		/**
		 * Property's namespace. Set if required, in any case namepsace can be null;
		 */ 
		public string @namespace { get; set; }

		/**
		 * Parent model
		 */
		public Model? parent { get; set; }

		/**
		 * Marks property as private 
		 */
		public bool @private { get; set; }

		/* StorageModelProperty properties */

		/**
		 * Name of the column model describes
		 */
		public string location { get; set; }

		/**
		 * Toggle to set index for a column 
		 */
		public bool index { get; set; }

		/**
		 * Toggle to mark column as primary key 
		 */
		public bool primary { get; set; }

		/**
		 * {@link SQLStorageManager}
		 */
		public unowned StorageManager storagemanager {
			get { return (StorageManager)this._storage_manager; }
		}

		private void _set_gtype_from_name () {
			switch (this._typename) {
				case "string":
				case "text":
				case "guid":
				case "uuid":
					this._gtype = typeof (string);
				break;

				case "datetime":
					this._gtype = typeof (MidgardCR.Timestamp);
				break;

				case "uint":
					this._gtype = typeof (uint);
				break;

				case "int":
					this._gtype = typeof (int);
				break;

				case "bool":
				case "boolean":
					this._gtype = typeof (bool);
				break;

				case "float":
					this._gtype = typeof (float);
				break;

				default:
					this._gtype = 0;
					GLib.warning ("Unhandled '%s' value type", this._typename);
				break;
			}	
		}		
	
		/**
		 * Constructor 
		 *
		 * @param name property name
		 * @param location the type of the property 
		 * @param typename default property's value 
		 */
		public SQLStorageModelProperty (string name, string location, string type) {
			Object (name: name, location: location, valuetypename: type);
			this._set_gtype_from_name ();	
		} 
	
		/* destructor */
		~SQLStorageModelProperty () {
			this._id = 0;
		}
			
		public Model add_model (Model model) { 
			this._models += model;
			return this; 
		}

		public unowned Model? get_model_by_name (string name) {
			if (this._models[0].name == name)
				return this._models[0];
 
			return null; 
		}

		public unowned Model[]? list_models () { 
			return this._models; 
		}

		/** 
		 * Check if SQLStorageModelProperty is valid
		 *
		 * In case of invalid model, error is thrown:
		 * 
		 * 1. NAME_INVALID, the type of property is empty
		 * 1. TYPE_INVALID, the GLib.Type of property is invalid
		 * 1. REFERENCE_INVALID, more than one model is added 
		 * 1. REFERENCE_INVALID, null parent model 
		 */
		public void is_valid () throws ValidationError { 
			/* type id or name is empty thus invalid */
			if ((this.valuetypename == null)
				|| (this.valuetypename == ""))
				throw new MidgardCR.ValidationError.NAME_INVALID ("Property's type name is empty"); 
			if (this.valuegtype == 0) {
				throw new MidgardCR.ValidationError.TYPE_INVALID ("Property's type is 0");
			}
				
			/* Invalid number of models associated */
			if (this._models.length > 1)
				throw new MidgardCR.ValidationError.REFERENCE_INVALID ("More than one reference model set");
	
			/* Associated model has no parent defined */
			if (this._models != null && this._models[0].parent == null)
				throw new MidgardCR.ValidationError.REFERENCE_INVALID ("Null parent defined for associated model");
		}

		/* StorageExecutor methods */
		/**
		 * Check if column exists
		 *
		 * @return true if column described by model exists in table, false otherwise
		 */
		public bool exists () {
			return MidgardCRCore.SQLStorageManager.column_exists (this._storage_manager, this);
		} 
		
		/**
		 * Prepare create SQL query
		 */
                public void prepare_create () throws ValidationError {
			this.is_valid ();
			this._create_column = true;
			string query = MidgardCRCore.StorageSQL.create_query_insert (this,
                                this._storage_manager._storage_model_property_object_model, this._storage_manager._storage_model_property_storage_model);
			this._queries += query;
		}

		/**
		 * Prepare table alter SQL query, which updates exisiting column
		 */ 
                public void prepare_update () throws ValidationError {
			this.is_valid ();
			this._update_column = true;
		}
		
		/**
		 * Prepare both: create and update SQL queries
		 */
                public void prepare_save () throws ValidationError {
			this.is_valid ();
			this._create_column = true;
			this._update_column = true;
		}

		/** 
		 * Prepare SQL query to remove column from table
		 */
                public void prepare_remove () throws ValidationError {
			this.is_valid ();
			this._remove_column = true;
		}

		/**
		 * Prepare SQL query to remove column from table
		 */
                public void prepare_purge () throws ValidationError {
			this.is_valid ();
			this._remove_column = true;
		}

		/* Executable methods */
		/**
		 * Executes SQL queries, which model has been prepared for.
		 * Depending on prepare, it may create, update or remove column.
		 * Also executes queries to store information about given model.
		 */
		public void execute () { 
			/* Create column or alter table */
			if (this._create_column) {
				execution_start ();
				MidgardCRCore.SQLStorageManager.column_create (this._storage_manager, this);
				execution_end ();
			}
			if (this._update_column) {
				execution_start ();
				MidgardCRCore.SQLStorageManager.column_update (this._storage_manager, this);
				execution_end ();
			}
			if (this._remove_column) {
				execution_start ();
				MidgardCRCore.SQLStorageManager.column_remove (this._storage_manager, this);
				execution_end ();
			}
			/* Store info about column */
			foreach (weak string query in this._queries) {
				execution_start ();
                                MidgardCRCore.SQLStorageManager.query_execute (this._storage_manager, query);
				execution_end ();
                        }
			this._create_column = false;
			this._update_column = false;
			this._remove_column = false;	
			this._queries = null;	
		}		
	}
}
