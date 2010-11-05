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
			if (model is ObjectModelProperty)
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
						throw new MidgardCR.ValidationError.NAME_DUPLICATED ("ObjectModel: duplicated %s model found in %s model", model.name, this.name);
				}
				names += model.name;
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
	 * ObjectModelProperty defines property registered for class.
	 */ 
	public class ObjectModelProperty : GLib.Object, Model, ModelProperty {

		/* private properties */

		private Model[] _models = null;
		private string _typename = null;
		private GLib.Type _gtype = 0;

		/* internal properties */

		internal uint _id = 0;
		internal bool _isref = false;
		internal string _refname = null;
		internal string _reftarget = null;

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

		/**
		 * Tells whether property of an object is a reference to other object or its property.
		 *
		 * By default it's false and set to true, when another model is added ({@link add_model}).
		 *
		 * In case of {@link ObjectModel}, property is reference and must be the type of object.
		 * In case of ObjectModelProperty, property is reference to given property and must be 
		 * of the same type as added one 
		 */
		public bool isref {
			get { return this._isref; }
		}

		/**
		 * Name of the class, if property is a reference.
		 */
		public string refname {
			get { return this._refname; }
		}

		/**
		 * Name of the property, if property is a reference.
		 */
		public string reftarget {
			get { return this._reftarget; }
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

				case "object":
					this._gtype = typeof (Object);
				break;

				default:
					this._gtype = 0;
					GLib.warning ("ObjectModel: unhandled '%s' value type", this._typename);
				break;
			}	
		}
		
		/**
		 * Creates new ObjectModelProperty for given property name
		 *
		 * @param name property name
		 * @param type the type of the property 
		 * @param dvalue default property's value 
		 */
		public ObjectModelProperty (string name, string type, string dvalue) {
			Object (name: name, valuetypename: type, valuedefault: dvalue);
			this._set_gtype_from_name ();	
		} 
		
		/**
		 * {@inheritDoc}
		 * 
		 * Only one model should be added to ObjectModelProperty.
		 *
		 * It should be either:
		 *
		 *  A. Model {@link ObjectModel}
		 *    a. property should be object type
		 *    a. isref value is set to true
		 *    a. refname is set to added model's name
		 *    a. reftarget is null
		 *  A. Model {@link ObjectModelProperty}
		 *    a. property should be string or integer
		 *    a. isref value is set to true
		 *    a. refname value is set to parentname of given model
		 *    a. reftarget is set to added model's name
		 *
		 * Model is invalid, if added one, is neither {@link ObjectModel} or ObjectModelProperty
		 */
		public Model add_model (Model model) {
			if (model is ObjectModel) {
				this._isref = true;
				this._refname = model.name;
			} 
			if (model is ObjectModelProperty) {
				this._isref = true;
				this._reftarget = model.name;
				if (model.parent != null)
					this._refname = ((ObjectModel)model.parent).parentname;
			}
			this._models += model;
			return this; 
		}

		/**
		 * {@inheritDoc}
		 *
		 * @return {@link ObjectModel} or {@link ObjectModelProperty} model if given name
		 * matches the first added model's one. There's no lookup by name done, as multiple 
		 * models are invalid in case of property model 
		 */
		public unowned Model? get_model_by_name (string name) {
			if (this._models == null)
				return null;
			if (this._models[0].name == name)
				return this._models[0];
 
			return null; 
		}

		/**
		 * {@inheritDoc}
		 */		
		public unowned Model[]? list_models () { 
			return this._models; 
		}

		/** 
		 * {@inheritDoc}
		 *
		 *  A. NAME_INVALID
		 *    a. {@link valuetypename} is null or empty
		 *  A. TYPE_INVALID
		 *    a. {@link valuegtype} is 0
		 *    a. invalid model class associated
		 *  A. REFERENCE_INVALID
		 *    a. more than one model associated
		 *    a. associated ObjectModelProperty model has no parent model
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
			if (this._models != null && (!(this._models[0] is ObjectModelProperty))) {
				if (!(this._models[0] is ObjectModel))
					throw new MidgardCR.ValidationError.TYPE_INVALID ("Invalid, associated %s model", this._models[0].get_type().name());
			}

			/* Associated model has no parent defined */
			if (this._models != null && this._models[0].parent == null)
				if (this._models[0] is ObjectModelProperty)
					throw new MidgardCR.ValidationError.REFERENCE_INVALID ("Null parent defined for associated '%s' model.", this._models[0].name);
		}

		/**
		 * {@inheritDoc}
		 *
		 * Associated models or parent models are not copied to models' copy.
		 *
		 * Copied properties: 
		 *
		 *  * name
		 *  * valuetypename
		 *  * valuedefault
		 *  * description
		 *  * namespace
		 *  * private
		 *
		 * @return new ObjectModelProperty with copied properties: 
		 */
		public Model? copy () {
			var copy = new ObjectModelProperty (this.name, this.valuetypename, this.valuedefault);
			copy.description = this.description;
			copy.namespace = this.namespace;
			copy.private = this.private;
			
			return (Model) copy;
		} 
		 
	}
}
