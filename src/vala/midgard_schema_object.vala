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

	public interface Storable : GLib.Object {

		/* signals */
		public abstract signal void create ();
		public abstract signal void created ();
		public abstract signal void update ();
		public abstract signal void updated ();
		public abstract signal void remove ();
		public abstract signal void removed ();
	}

	public abstract class Timestamp : GLib.Object {

	}

	public abstract class Metadata : GLib.Object, Storable {

		/* proprties */
		public abstract string parent { get; construct; }	
		public abstract uint action { get; } /* is it needed ? */
		public abstract Timestamp created { get; }
		public abstract Timestamp revised { get; }
	}

	public abstract class RepositoryObject : GLib.Object, Storable {

		/* internal properties */
		internal string _guid = null;
		internal int _id = 0;
		internal Metadata _metadata = null;	
		internal GLib.List<string> _ns_properties_list = null;
		internal GLib.List<GLib.Value?> _ns_values_list = null;

		/* properties */
		public string guid { 
			get { return this._guid; }
		}

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
