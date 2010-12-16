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
	 * ObjectPropertyModel defines property registered for class.
	 */ 
	public class ObjectPropertyModel : GLib.Object, Model, ModelProperty {

		/* private properties */

		private Model[] _models = null;
		private string _typename = null;
		private GLib.Type _gtype = 0;

		/* internal properties */

		internal uint _id = 0;
		internal bool _isref = false;
		internal string _reference = null;
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
		 * Property's namespace. Set if required, in any case namespace can be null;
		 */ 
		public string? @namespace { get; set; }

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
		 * {@inheritDoc}
		 */
		public bool isref {
			get { return this._isref; }
		}

		/**
		 * {@inheritDoc}
		 */
		public string? reference {
			get { return this._reference; }
		}

		/**
		 * Name of the class, if property is a reference.
		 */
		public string? refname {
			get { return this._refname; }
		}

		/**
		 * Name of the property, if property is a reference.
		 */
		public string? reftarget {
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
		 * Creates new ObjectPropertyModel for given property name
		 *
		 * @param name property name
		 * @param type the type of the property 
		 * @param dvalue default property's value 
		 */
		public ObjectPropertyModel (string name, string type, string dvalue) {
			Object (name: name, valuetypename: type, valuedefault: dvalue);
			this._id = 0; /* silent valac */
			this._set_gtype_from_name ();	
		} 
		
		/**
		 * {@inheritDoc}
		 * 
		 * Only one model should be added to ObjectPropertyModel.
		 *
		 * It should be either:
		 *
		 *  A. Model {@link ObjectModel}
		 *    a. property should be object type
		 *    a. isref value is set to true
		 *    a. refname is set to added model's name
		 *    a. reftarget is null
		 *  A. Model {@link ObjectPropertyModel}
		 *    a. property should be string or integer
		 *    a. isref value is set to true
		 *    a. refname value is set to parentname of given model
		 *    a. reftarget is set to added model's name
		 *
		 * Model is invalid, if added one, is neither {@link ObjectModel} or ObjectPropertyModel
		 */
		public virtual Model add_model (Model model) {
			if (model is ObjectModel) {
				this._isref = true;
				this._refname = model.name;
			} 
			if (model is ObjectPropertyModel) {
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
		 * @return {@link ObjectModel} or {@link ObjectPropertyModel} model if given name
		 * matches the first added model's one. There's no lookup by name done, as multiple 
		 * models are invalid in case of property model 
		 */
		public virtual unowned Model? get_model_by_name (string name) {
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
		 *    a. associated ObjectPropertyModel model has no parent model
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
			if (this._models != null && (!(this._models[0] is ObjectPropertyModel))) {
				if (!(this._models[0] is ObjectModel))
					throw new MidgardCR.ValidationError.TYPE_INVALID ("Invalid, associated %s model", this._models[0].get_type().name());
			}

			/* Associated model has no parent defined */
			if (this._models != null && this._models[0].parent == null)
				if (this._models[0] is ObjectPropertyModel)
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
		 * @return new ObjectPropertyModel with copied properties: 
		 */
		public Model? copy () {
			var copy = new ObjectPropertyModel (this.name, this.valuetypename, this.valuedefault);
			copy.description = this.description;
			copy.namespace = this.namespace;
			copy.private = this.private;
			
			return (Model) copy;
		}  
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyString is a property of an object which holds string type value.
	 */ 
	public class ObjectPropertyString : ObjectPropertyModel {
		
		/**
		 * Creates new ObjectPropertyModel of string type
		 *
		 * @param name property name
		 * @param dvalue default property's value 
		 * @param namespace namespace of the property
		 */
		public ObjectPropertyString (string name, string dvalue, string? namespace = null) {
			Object (valuetypename: "string", name: name, valuedefault: dvalue, @namespace: namespace);
		} 
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyText is a property of an object which holds text type value.
	 */ 
	public class ObjectPropertyText : ObjectPropertyModel {
		
		/**
		 * Creates new ObjectPropertyModel of text type
		 *
		 * @param name property name
		 * @param dvalue default property's value 
		 * @param namespace namespace of the property
		 */
		public ObjectPropertyText (string name, string dvalue, string? namespace = null) {
			Object (valuetypename: "text", name: name, valuedefault: dvalue, @namespace: namespace);
		} 
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyGuid is a property of an object which holds GUID type value.
	 */ 
	public class ObjectPropertyGUID : ObjectPropertyModel {
		
		/**
		 * Creates new ObjectPropertyModel of guid type
		 *
		 * @param name property name
		 * @param dvalue default property's value 
		 * @param namespace namespace of the property
		 */
		public ObjectPropertyGUID (string name, string dvalue, string? namespace = null) {
			Object (valuetypename: "guid", name: name, valuedefault: dvalue, @namespace: namespace);
		} 
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyString is a property of an object which holds UUID type value.
	 */ 
	public class ObjectPropertyUUID : ObjectPropertyModel {
		
		/**
		 * Creates new ObjectPropertyModel of UUID type
		 *
		 * @param name property name
		 * @param dvalue default property's value 
		 * @param namespace namespace of the property
		 */
		public ObjectPropertyUUID (string name, string dvalue, string? namespace = null) {
			Object (valuetypename: "uuid", name: name, valuedefault: dvalue, @namespace: namespace);
		} 
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyDatetime is a property of an object which holds datetime type value.
	 */ 
	public class ObjectPropertyDatetime : ObjectPropertyModel {
		
		/**
		 * Creates new ObjectPropertyModel of datetime type
		 *
		 * @param name property name
		 * @param dvalue default property's value 
		 * @param namespace namespace of the property
		 */
		public ObjectPropertyDatetime (string name, string dvalue, string? namespace = null) {
			Object (valuetypename: "datetime", name: name, valuedefault: dvalue, @namespace: namespace);
		} 
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyInt is a property of an object which holds integer type value.
	 */ 
	public class ObjectPropertyInt : ObjectPropertyModel {
		
		/**
		 * Creates new ObjectPropertyModel of integer type
		 *
		 * @param name property name
		 * @param dvalue default property's value 
		 * @param namespace namespace of the property
		 */
		public ObjectPropertyInt (string name, string dvalue, string? namespace = null) {
			Object (valuetypename: "int", name: name, valuedefault: dvalue, @namespace: namespace);
		} 
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyBool is a property of an object which holds boolean type value.
	 */ 
	public class ObjectPropertyBool : ObjectPropertyModel {
		
		/**
		 * Creates new ObjectPropertyModel of boolean type
		 *
		 * @param name property name
		 * @param dvalue default property's value 
		 * @param namespace namespace of the property
		 */
		public ObjectPropertyBool (string name, string dvalue, string? namespace = null) {
			Object (valuetypename: "bool", name: name, valuedefault: dvalue, @namespace: namespace);
		} 
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyFloat is a property of an object which holds float type value.
	 */ 
	public class ObjectPropertyFloat : ObjectPropertyModel {
		
		/**
		 * Creates new ObjectPropertyModel of float type
		 *
		 * @param name property name
		 * @param dvalue default property's value 
		 * @param namespace namespace of the property
		 */
		public ObjectPropertyFloat (string name, string dvalue, string? namespace = null) {
			Object (valuetypename: "float", name: name, valuedefault: dvalue, @namespace: namespace);
		} 
	}

	/**
	 * {@inheritDoc}
	 *
	 * ObjectPropertyReference is a property of an object which holds object type value.
	 * 
	 * Object type held by this property should be of 'ReferenceObject' (or derived) type.
	 * The purpose of this property is to hold reference to other {@link RepositoryObject} object 
	 * by holding referenced object identifiers (like id or guid value).
	 */ 
	public class ObjectPropertyReference : ObjectPropertyModel {

		//internal string _ref_classname = null;	
	
		/**
		 * Creates new ObjectPropertyModel of object type
		 *
		 * @param name property name
		 * @param object_model {@link ObjectModel} which defines the class a property holds a reference 
		 * @param model_reference {@link ObjectModelReference} 
		 */
		public ObjectPropertyReference (string name, ObjectModel object_model, ObjectModelReference model_reference) {
			Object (valuetypename: "object", name: name);	
			if (model_reference != null) {
				this._reference = model_reference.name;
				model_reference.add_model (object_model);
			}
			this.add_model ((Model)model_reference);
			this._isref = true;
			this._refname = object_model.name;
		}

		/**
		 * Static constructor.
		 */
		public static ObjectPropertyReference create_model (
			string name, ObjectModel object_model, ObjectModelReference model_reference) { 	
			return new ObjectPropertyReference (name, object_model, model_reference);
		}
	}
}
