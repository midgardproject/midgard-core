
namespace MidgardCR {

	public errordomain ValidationError {
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
}
