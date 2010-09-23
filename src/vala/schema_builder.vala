
namespace MidgardCR {

	public errordomain SchemaBuilderError {
		NAME_EXISTS
	}

	public class SchemaBuilder : GLib.Object, Executable {

		/* private properties */
		private SchemaModel[] _models = null;

		/* private methods */
		private void _validate_model (Model model) throws ValidationError {
			model.is_valid ();
			Model[] models = model.list_models ();
			if (models == null)
				return;

			foreach (Model model in models)
				this._validate_model (model);
		}

		/* public methods */
		public void register_model (SchemaModel model) throws SchemaBuilderError, ValidationError { 
			GLib.Type typeid = GLib.Type.from_name (model.name);
			if (typeid > 0)
				throw new ValidationError.NAME_DUPLICATED ("%s already registered in GType system", model.name);
			
			this._validate_model (model);

			/* Delay class registration if inheritance or reference requires this */

			/* TODO, create class entry with GParamSpec array before registering new class */
		}

		public void register_storage_models (StorageManager manager) throws SchemaBuilderError, ValidationError { }
		public Storable? factory (StorageManager storage, string classname) throws SchemaBuilderError, ValidationError { return null; }
		public SchemaModel? get_schema_model (string classname) { return null; }
		
		public bool execute () { 
			MidgardCRCore.SchemaBuilder.register_types (this);
			return false; 
		}
	}
}
