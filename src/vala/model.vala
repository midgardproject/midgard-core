
namespace MidgardCR {

	public errordomain ValidationError {
		NAME_INVALID,
		NAME_DUPLICATED,
		TYPE_INVALID,
		VALUE_INVALID,
		REFERENCE_INVALID,
		PARENT_INVALID,
		LOCATION_INVALID,
		INTERNAL
	}	

	public interface Model : GLib.Object {
		
		/* properties */
		public abstract Model?       parent    { get; set; }
		public abstract string      @namespace       { get; set;  }
		public abstract string      name            { get; set;  }

		/* methods */
		
		/**
		 * Associate new model
		 */
		public abstract Model add_model (Model model);

		/**
		 * Get model by given name.
		 */
		public abstract unowned Model? get_model_by_name (string name);
		
		/**
		 * List all associated models.
		 */
		public abstract unowned Model[]? list_models ();

		/**
		 * Check whether model is valid
		 */
		public abstract void is_valid () throws ValidationError;
	}

	public interface ModelProperty : GLib.Object, Model {
		
		/* properties */
		
		/**
		 * The {@link GLib.Type} of the value.
		 */
		public abstract GLib.Type   valuegtype     { get; }

		/**
		 * Name of the {@link GLib.Type} of the value
		 */
		public abstract string      valuetypename  { get; set; }

		/**
		 * Default of the value
		 */
		public abstract string	    valuedefault   { get; set; }

		/**
		 * Description of the property.
		 */
		public abstract string      description     { get; set; }

		/**
		 * Tells whether property model is private. 
		 */
		public abstract bool        @private      { get; set; }

		/**
		 * Tells whether ModelProperty is reference
		 */
		public abstract bool	isref { get; }

		/**
  		 * The name of reference.
		 */
		public abstract string 	refname { get; }

		/**
		 * The name of reference's target
		 */
		public abstract string 	reftarget { get; }
	}
}
