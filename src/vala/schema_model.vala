
namespace MidgardCR {

	/**
	 * SchemaModel defines classes which can be registered in GType system.
	 *
	 * It's not related to any storage, so any class can be registered during runtime
	 * without any need to store its definition in underlying storage.
	 *
	 * If required, class' name and properties (as the class definition with introspection data)
	 * might be stored in storage through StorageModel implementation.
	 */	
	public class SchemaModel : GLib.Object, Model {

		private Model[] _models = null;

		/* public properties */
		/**
		 * Holds the name of the class, SchemaModel is created for
		 */
		public string name { get; set; }
		/**
		 * Holds the namespace of model's class
 		 */
		public string @namespace { get; set; }
		/**
		 * Parent model of an instance.
		 */
		public Model parent { get; set; }

		/**
		 * Creates new SchemaModel for given classname
		 * 
		 * @param name classname to create model for
		 */
		public SchemaModel (string name) {
			Object (name: name);
		}

		/**
		 * Associate new model 
		 *
		 * @param model {@link Model} to add
		 *
		 * @return {@link Model} instance (self reference)
		 */	
		public Model add_model (Model model) { 
			this._models += model;			
			return this; 
		}

		/**
		 * Get model by given name
		 * 
		 * @param name {@link Model} name to look for
		 *
		 * @return {@link Model} if found, null otherwise
		 */
		public Model? get_model_by_name (string name) { 
			if (this._models == null)
				return null; 

			foreach (Model model in this._models) {
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
		public Model[]? list_models () { 
			return this._models; 
		}
		
		/**
		 * Get model's reflector 
		 *
		 * @return {@link ModelReflector} of given {@link Model}
		 */
		public ModelReflector get_reflector () { 
			return null; 
		} 

		/**
		 * Check whether model is valid
		 *
		 * @return true if model is valid, false otherwise
		 */
 		public bool is_valid () { 
			return false; 
		}	
	}

	/**
	 * SchemaModelProperty defines property registered for class.
	 */ 
	public class SchemaModelProperty : GLib.Object, ModelProperty, Model, Executable {

		/* private properties */

		private Model[] _models = null;
		private Model[] _parent_models = null;

		/* public properties */
	
		/**
		 * Holds the name of property for which model is created
		 */
		public string name { get; set; }
		
		/**
		 * The type of the property.
		 * Acceptable name of any type registered in GType system ('string', 'int', 'bool'...).
		 */ 
		public string value_typename { get; set; }
		
		/**
		 * Gtype of the property
		 */
		public GLib.Type value_gtype { get; set; }

		/**
		 * Default value of property for newly created objects
		 */
		public GLib.Value value_default { get; set; }

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
		public Model parent { get; set; }

		/**
		 * Marks property as private 
		 */
		public bool @private { get; set; }

		/**
		 * Creates new SchemaModelProperty for given property name
		 *
		 * @param name property name
		 * @param type the type of the property 
		 * @param value default property's value 
		 */
		public SchemaModelProperty (string name, string type, string value) {
			Object (name: name, value_typename: type, value_default: value);
		} 

		public Model add_model (Model model) { 
			this._models += model;
			return this; 
		}

		public Model? get_model_by_name (string name) { return null; }
 		public Model add_parent_model (Model model) { return this; }
 		public Model? get_parent_model () { return null; }
		public Model[]? list_models () { return null; }
		public ModelReflector get_reflector () { return null; } 
		public bool is_valid () { return false; }
		public bool execute () { return true; }	
		public void set_value_gtype (GLib.Type type) { return; }	
	}

	public errordomain SchemaBuilderError {
		NAME_EXISTS
	}

	public class SchemaBuilder : GLib.Object, Executable {

		/* methods */
		public void register_model (SchemaModel model) throws SchemaBuilderError, ValidationError { }
		public void register_storage_models (StorageManager manager) throws SchemaBuilderError, ValidationError { }
		public Storable? factory (StorageManager storage, string classname) throws SchemaBuilderError, ValidationError { return null; }
		public SchemaModel? get_schema_model (string classname) { return null; }
		
		public bool execute () { return false; }
	}
}
