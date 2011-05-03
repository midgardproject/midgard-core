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
 * Copyright (C) 2010, 2011 Piotr Pokora <piotrek.pokora@gmail.com>
 */

using GLib;

namespace MidgardCR {

	/**
 	 * Basic interface to implement objects which hold datetime.
	 */
	public abstract class Timestamp : GLib.Object {

	}

	/**
	 * Basic interface for metadata objects.
	 */
	public abstract class Metadata : GLib.Object {

		/* proprties */
		public abstract string parent { get; construct; }	
		public abstract uint action { get; } /* is it needed ? */
		public abstract Timestamp created { get; }
		public abstract Timestamp revised { get; }
	}

	/**
	 * Abstract class for objects which can be stored in repository.
	 */
	public abstract class RepositoryObject : GLib.Object {

		/* internal properties */
		internal string _guid = null;
		internal int _id = 0;
		internal Metadata _metadata = null;	
		internal string _identifier = null;
		internal StorageManager _storage_manager = null;

		/* properties */

		/**
		 * Holds unique guid which identifies object.
		 *
		 * Guid might be set by application, only during construction time.
		 * It's internally set in any other case.
		 */
		public string guid { 
			get { return this._guid; }
			construct { this._guid = value; }
		}

		/**
		 * Holds unique (per class) id which identifies object.
		 *
		 * Any implementation shall never depend on this property in replicated or distributed
		 * environment. It shall be used during runtime only, to identify objects which are unique
		 * per application and storage. Unlike guid property, which is globally unique. 
		 * 
		 */ 
		public uint id { 
			get { return this._id; }
		}

		/**
		 * Application specific free form string identifier of the object.
		 */
		public string? identifier {
			get { return this._identifier; }
			set { this._identifier = value; }
		}

		public Metadata? metadata { 
			get {  return this._metadata; }	
		}

		/**
		 * Storage manager for which current instance has been initialized.
		 * It's not mandatory to set storage manager.
		 */
		public StorageManager storage_manager {
			get { return this._storage_manager; }
			construct { this._storage_manager = value; }	
		}

		/* signals */

		/**
		 * The signal which shall be emitted before object's creation.
		 */
		public abstract signal void create ();
		
		/**
		 * The signal which shall be emitted after object has been created.
		 */
		public abstract signal void created ();

		/**
		 * The signal which shall be emitted before object's update.
		 */
		public abstract signal void update ();

		/**
		 * The signal which shall be emitted after object has been emitted.
		 */
		public abstract signal void updated ();
	
		/**
		 * The signal which shall be emitted before object's removal.
		 */ 
		public abstract signal void remove ();

		/**
		 * The signal which shall be emitted after object has been removed.
		 */
		public abstract signal void removed ();

		/* methods */

		/**
		 * Set value of the property
		 * 
		 * Simple wrapper for GLib.Object.set_property.
		 * Derived classes shall provide own implementation if required.
		 */
		public virtual void set_property_value (string name, GLib.Value value) {
			this.set_property (name, value);
		}

		/**
		 * Set value of the property using string value
		 * 
		 * Sets stringified value. This method initializes correct Value and tries to 
		 * make types conversion.
		 */
		public virtual void set_property_literal (string name, string literal) {
			ParamSpec pspec = this.get_class ().find_property (name);
			if (pspec == null) {
				warning ("Property '%s' not installed for %s class", name, this.get_type ().name ());
				return;
			}
			Value dest_val = {};
			dest_val.init (pspec.value_type);
			Value sval = {};
			sval.init (typeof (string));
			sval.set_string (literal);
			sval.transform (ref dest_val);
			this.set_property (name, dest_val);
		} 	

		/**
		 * Get stringified value
		 * 
		 * Get value of the property and transform it to string.
		 */
		public virtual string? get_property_literal (string name) {
			ParamSpec pspec = this.get_class ().find_property (name);
			if (pspec == null) {
				warning ("Property '%s' not installed for %s class", name, this.get_type ().name ());
				return null;
			}
			Value val = {};
			val.init (pspec.value_type);
			this.get_property (name, ref val);
			Value sval = {};
			sval.init (typeof (string));
			val.transform (ref sval);
			return sval.get_string ();	
		}	

		/**
		 * Get value of the property
		 *
		 * Simple wrapper for GLib.Object.get_property.
		 * Derived classes shall provide own implementation if required.
		 */
		public virtual GLib.Value? get_property_value (string name) {
			Value val = {};
			this.get_property (name, ref val);
			return val;
		}

		/**
		 * Get an array of GLib.ParamSpec for all properties registered for object's class.
		 * 
		 * By default it's simple wrapper for GLib.Object.list_properties().
		 * Derived class can provide own implementation if only required.
		 */
		public virtual unowned GLib.ParamSpec[]? list_all_properties () {
			return this.get_class ().list_properties ();
		}

		/**
		 * Get an array of names for all properties registered for object's class.
		 * 
		 * By default it's simple wrapper for GLib.Object.list_properties().
		 * Derived class can provide own implementation if only required.
		 */
		public virtual string[]? list_all_properties_names () {
			ParamSpec[] pspecs = this.get_class ().list_properties ();
			string[] props = null;
			foreach (weak ParamSpec p in pspecs) {
				props += p.name;
			}	
			return props;
		}
	}

	public class ReferenceObject : GLib.Object {

		/* properties */
		public string classname { get; construct;}
		public string guid { get; set; }
		public int id { get; set; }
	}
}
