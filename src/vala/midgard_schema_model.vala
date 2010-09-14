using GLib;

namespace Midgard {

	errordomain ValidationError {
		NAME_INVALID,
		TYPE_INVALID,
		VALUE_INVALID,
		REFERENCE_INVALID,
		PARENT_INVALID,
		LOCATION_INVALID
	}

	public interface Model : GLib.Object {
		
		/* properties */
		public abstract Model       parent    { get; set; }
		public abstract string      @namespace       { get; set;  }
		public abstract string      name            { get; set;  }

		/* methods */
		public abstract Model add_model (Model model);
		public abstract Model? get_model_by_name (string name);
		public abstract Model[]? list_models ();
		public abstract ModelReflector get_reflector ();
	}

	public interface ModelProperty : Model, Executable {
		
		/* properties */
		public abstract GLib.Type   value_gtype     { get; set; }
		public abstract string      value_typename  { get; set; }
		public abstract GLib.Value  value_default   { get; set; }
		public abstract string      description     { get; set; }
		public abstract bool        is_private      { get; set; }
	}

	public class SchemaModel : GLib.Object, Model {

		public string name {
			get { return "foo"; }
			set { this.name = value; }
		}

		public string @namespace {
			get { return "foo"; }
			set { this.namespace = value; }
		}

		public Model parent {
			get { return this.parent; }
			set { this.parent = value; }
		}

		public string get_name () { return "foo"; }
		public Model add_model (Model model) 
			requires (model is SchemaModel) {
			return this; 
		}
		public Model? get_model_by_name (string name) { return null; }
 		public Model add_parent_model (Model model) { return this; }
 		public Model? get_parent_model () { return null; }
		public Model[]? list_models () { return null; }
		public ModelReflector get_reflector () { return null; } 
 		public bool is_valid () { return false; }	
	}

	public class SchemaModelProperty : GLib.Object, Model {
	
		public string name {
			get { return this.name; }
			set { this.name = value; }
		}

		public string @namespace {
			get { return this.namespace; }
			set { this.namespace = value; }
		}

		public Model parent {
			get { return this.parent; }
			set { this.parent = value; }
		}

		public string get_name () { return "foo"; }
		public Model add_model (Model model) { return this; }
		public Model? get_model_by_name (string name) { return null; }
 		public Model add_parent_model (Model model) { return this; }
 		public Model? get_parent_model () { return null; }
		public Model[]? list_models () { return null; }
		public ModelReflector get_reflector () { return null; } 
		public bool is_valid () { return false; }

		public void set_value_typename (string name) { return; }
		public void set_value_gtype (GLib.Type type) { return; }
		public void set_value_default (GLib.Value value) { return; }
		public void set_private (bool toggle) { return; }
		public void set_description (string description) { return; }
		public bool set_namespace (string name) { return false; }
		public string get_namespace () { return "foo"; }
	}

	errordomain SchemaBuilderError {
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
