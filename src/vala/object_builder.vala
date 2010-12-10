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

	public errordomain ObjectBuilderError {
		NAME_EXISTS
	}

	public class ObjectBuilder : GLib.Object, Executable {

		/* private properties */
		internal ObjectModel[] _models = null;
		/* internal ObjectModel[] _delayed_models = null; */

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
		public void register_model (ObjectModel model) throws ObjectBuilderError, ValidationError { 
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

		public void register_storage_models (StorageManager manager) throws ObjectBuilderError, ValidationError { }

		public Storable? factory (string classname) throws ObjectBuilderError, ValidationError { 
			GLib.Type type_id = GLib.Type.from_name (classname);	
			if (type_id == 0)
				throw new ValidationError.NAME_INVALID ("%s is not registered class", classname);
			
			GLib.Type storable_id = GLib.Type.from_name ("MidgardCRStorable");
			if (type_id.is_a (storable_id) == false)
				throw new ValidationError.TYPE_INVALID ("%s is not Storable derived class", classname);

			Storable obj = (Storable) GLib.Object.new (type_id);
			return obj;	 
		}

		public ObjectModel? get_object_model (string classname) { return null; }
		
		public void execute () throws ExecutableError {
			if (this._models == null)
				throw new ExecutableError.COMMAND_INVALID_DATA ("No models associated with builder");
			try {
				MidgardCRCore.ObjectBuilder.register_types (this);
			} catch (Error e) {
				throw new ExecutableError.INTERNAL (e.message);
			}
		}
	}
}
