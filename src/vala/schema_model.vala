
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

		/* private properties */
		
		private Model[] _models = null;

		/* internal properties */
		
		internal uint _id = 0;

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
		public Model? parent { get; set; }

		/**
		 * The name of parent model.
		 */
		public string parentname {
			get { 
				if (this.parent == null)
					return "SchemaObject";

				return this.parent.name;
			}
		}

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
			model.parent = this;
			return this; 
		}

		/**
		 * Get model by given name
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
				throw new MidgardCR.ValidationError.REFERENCE_INVALID ("No models associated.");

			/* TODO */
			/* Check if name does not containt invalid characters */

			string[] names = new string[0];
			foreach (MidgardCR.Model model in this._models) {	
				foreach (string name in names) {
					if (name == model.name)
						throw new MidgardCR.ValidationError.NAME_DUPLICATED ("Duplicated model name found");
				}
				names += model.name;
			}	
		}	
	}

	/**
	 * SchemaModelProperty defines property registered for class.
	 */ 
	public class SchemaModelProperty : GLib.Object, Model, ModelProperty {

		/* private properties */

		private Model[] _models = null;
		private string _typename = null;
		private GLib.Type _gtype = 0;

		/* internal properties */

		internal uint _id = 0;

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
		 * Name of the class, property is installed.	 
		 */
		public string? classname {
			get {
				if (this.parent == null)
					return null;
				return this.parent.name;
			}
		}

		/**
		 * Marks property as private 
		 */
		public bool @private { get; set; }

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
		 * Creates new SchemaModelProperty for given property name
		 *
		 * @param name property name
		 * @param type the type of the property 
		 * @param dvalue default property's value 
		 */
		public SchemaModelProperty (string name, string type, string dvalue) {
			Object (name: name, valuetypename: type, valuedefault: dvalue);
			this._set_gtype_from_name ();	
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
		 * Check if SchemaModelProperty is valid
		 *
		 * If it's valid, this method silently returns.
		 * In other case, error is thrown.
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
	
			/* Invalid object associated as model */
			if (this._models != null && (!(this._models[0] is SchemaModelProperty)))
				throw new MidgardCR.ValidationError.TYPE_INVALID ("Associated model is not a SchemaModelProperty instance");

			/* Associated model has no parent defined */
			if (this._models != null && this._models[0].parent == null)
				throw new MidgardCR.ValidationError.REFERENCE_INVALID ("Null parent defined for associated model");
		}

		public void execute () { 
			return; 
		}		
	}
}
