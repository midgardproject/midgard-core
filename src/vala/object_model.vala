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

	/**
	 * ObjectModel defines classes which can be registered in GType system.
	 *
	 * It's not related to any storage, so any class can be registered during runtime
	 * without any need to store its definition in underlying storage.
	 *
	 * If required, class' name and properties (as the class definition with introspection data)
	 * might be stored in storage through StorageModel implementation.
	 */	
	public class ObjectModel : GLib.Object, Model {

		/* private properties */
		
		private Model[] _models = null;

		/* internal properties */
		
		internal uint _id = 0;

		/* public properties */

		/**
		 * Holds the name of the class, ObjectModel is created for
		 */
		public string name { get; set; }
		/**
		 * Holds the namespace of model's class
 		 */
		public string? @namespace { get; set; }
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
		 * Creates new ObjectModel for given classname
		 * 
		 * @param name classname to create model for
		 */
		public ObjectModel (string name) {
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
			if (model is ObjectPropertyModel)
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

			/* Check duplicates */
			string[] names = new string[0];
			foreach (MidgardCR.Model model in this._models) {	
				foreach (string name in names) {
					if (name == model.name)
						throw new MidgardCR.ValidationError.NAME_DUPLICATED ("ObjectModel: duplicated %s model found in %s model", model.name, this.name);
				}
				names += model.name;
			}

			/* Validate models */
			foreach (MidgardCR.Model model in this._models) {
				model.is_valid ();
			}	
		}

		/**
		 * {@inheritDoc}
		 *
		 * Associated models or parent model are not copied to model's copy.
		 * @return new ObjectModel with name and namespace copied properties
		 */
		public Model? copy () {
			var copy = new ObjectModel (this.name);
			copy.namespace = this.namespace;

			return (Model) copy;
		}	
	}

	/** 
	 * ObjectModelReference defines class which holds identifiers to other object.
	 * 
	 * Class registered with ObjectModelReference is {@link ReferenceObject} derived one.
	 * The purpose of the reference class is to provide ability to hold reference to other object
	 * indirectly, via reference object which holds identifiers to other one.
	 * 
	 * For example, ReferenceObject hold identifiers of other object via 'id' and 'guid' property. 
	 */	
	public class ObjectModelReference : ObjectModel {
				
		/**
		 * Create new ObjectModelReference instance
		 *
		 * Creates reference model for given classname, which must be registered as
		 * {@link ReferenceObject} derived one.
		 *
		 * @param name name of the class 
		 */
		public ObjectModelReference (string name) {
			/* Add 'MidgardCR' prefix as C namespace */
			string real_classname = null;
			if ("MidgardCR" in name)
				real_classname = name;
			else 
				real_classname = "MidgardCR" + name;
			base (real_classname);
		}

		/**
		 * Static constructor.
		 */ 
		public static ObjectModelReference create_model (string classname) {
			return new ObjectModelReference (classname);
		}

		public new void is_valid () throws ValidationError {
			GLib.Type this_type = GLib.Type.from_name (this.name);
			GLib.Type ref_type = GLib.Type.from_name ("ReferenceObject");
			if (this_type.is_a (ref_type) == false)
				throw new MidgardCR.ValidationError.TYPE_INVALID 
					("'%s' classname associated with ObjectModelReference doesn't inherit 'ReferenceObject' class", this.name);		
		}
	}
}
