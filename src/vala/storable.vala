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

using GLib;

namespace MidgardCR {

	/**
	 * Basic interface for objects which can be stored with {@link StorageContentManager}
	 */
	public interface Storable : GLib.Object {

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
	}

	/**
 	 * Basic interface to implement objects which hold datetime.
	 */
	public abstract class Timestamp : GLib.Object {

	}

	/**
	 * Basic interface for metadata objects.
	 */
	public abstract class Metadata : GLib.Object, Storable {

		/* proprties */
		public abstract string parent { get; construct; }	
		public abstract uint action { get; } /* is it needed ? */
		public abstract Timestamp created { get; }
		public abstract Timestamp revised { get; }
	}

	/**
	 * Abstract class for objects which can be stored in repository.
	 */
	public abstract class RepositoryObject : GLib.Object, Storable {

		/* internal properties */
		internal string _guid = null;
		internal int _id = 0;
		internal Metadata _metadata = null;	
		internal GLib.List<string> _ns_properties_list = null;
		internal GLib.List<GLib.Value?> _ns_values_list = null;

		/* properties */

		/**
		 * Holds unique guid which identifies object.
		 *
		 * Guid might be set by application, only during construction time.
		 * It's internally set in any other case.
		 */
		public string guid { 
			get { 
				return this._guid; 
			}
			construct {
				this._guid = value;
			}
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

		public Metadata? metadata { 
			get {  return this._metadata; }	
		}

		/* methods */
		public virtual void set_property_value (string name, GLib.Value value) {
			if (this._ns_properties_list == null) {
				this._ns_properties_list = new List<string> ();
				this._ns_values_list = new List<GLib.Value?> ();
			}
			this._ns_properties_list.append (name);
			this._ns_values_list.append (value);
		}

		public virtual GLib.Value? get_property_value (string name) {
			if (this._ns_properties_list == null)
				return null;
		
			int i = 0;
			foreach (string element in this._ns_properties_list) {
				if (element == name)
					return this._ns_values_list.nth_data (i);
				i++;
			}			
			
			return null;
		}

		public virtual string[]? list_all_properties () {
			if (this._ns_properties_list == null)
				return null;
			
			string[] all_props = null;	
			foreach (string element in this._ns_properties_list) {
				all_props += element;
			}			
			return all_props;	
		}
	}

	public abstract class StorageObject : GLib.Object, Storable {

	}

	public class ReferenceObject : GLib.Object {

		/* properties */
		public string classname { get; construct;}
		public string guid { get; set; }
		public int id { get; set; }
	}
}
