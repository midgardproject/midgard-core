
namespace MidgardCR {

	public errordomain SchemaBuilderError {
		NAME_EXISTS
	}

	public class SchemaBuilder : GLib.Object, Executable {

		/* private properties */
		internal SchemaModel[] _models = null;
		internal SchemaModel[] _delayed_models = null;

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

			foreach (MidgardCR.Model registered_model in this._models) {
				if (registered_model.name == model.name)
					throw new ValidationError.NAME_DUPLICATED ("%s already registered", model.name);
			}

			/* TODO Delay class registration if inheritance or reference requires this */

			this._models += model;	
		}

		public void register_storage_models (StorageManager manager) throws SchemaBuilderError, ValidationError { }
		public Storable? factory (StorageManager storage, string classname) throws SchemaBuilderError, ValidationError { return null; }
		public SchemaModel? get_schema_model (string classname) { return null; }
		
		public void execute () throws ExecutableError { 
			MidgardCRCore.SchemaBuilder.register_types (this);
		}
	}
}
