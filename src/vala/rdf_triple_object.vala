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

	public class RDFTripleObject : GLib.Object, Storable {

		/* internal properties */
		internal string _guid = null;
		internal string _identifier = null;
		internal string _classname = null;
		internal int _id = 0;
		internal HashTable<string, GLib.Value?> _ns_values_hash = null;
		internal HashTable<string, string> _ns_literals_hash = null;

		/* properties */
		public string guid {
			get { return this._guid; }
		}

		public string identifier {
			get {
				if (this._identifier == null) {
					this._identifier = "midgard:guid:" + this.guid;
				}

				return this._identifier;
			}
			set { this._identifier = value; } /* TODO: do we need to "denamespace" this? */
		}

		public string classname {
			get { return this._classname; }
			construct { this._classname = value; }
		}

		public uint id {
			get { return this._id; }
		}

		/* constructor */
		public RDFTripleObject (string classname) {
			Object (classname: classname);	
		}
		
		construct {
			this._ns_values_hash = new HashTable <string, GLib.Value?>(str_hash, str_equal);
			this._ns_literals_hash = new HashTable <string, string>(str_hash, str_equal);
		}			

		/* methods */
		public virtual void set_property_value (string name, GLib.Value value) {
			this._ns_values_hash.insert (name,value);
		}

		public virtual void set_property_literal (string name, string value) {
			this._ns_literals_hash.insert (name,value);
		}

		public virtual GLib.Value? get_property_value (string name) {
			return this._ns_values_hash.lookup (name);
		}

		public virtual GLib.Value? get_property_literal (string name) {
			return this._ns_literals_hash.lookup (name);
		}

		public virtual string[]? list_all_properties () {
			HashTable <weak string, string> tmphash = new HashTable<weak string, string> (str_hash, str_equal);
			foreach (string element in this._ns_values_hash.get_keys ()) {
				tmphash.insert (element, null);
			}	
			
			foreach (string element in this._ns_literals_hash.get_keys ()) {
				tmphash.insert (element, null);
			}	

			string[] propnames = null;
			foreach (string el in tmphash.get_keys ())
				propnames += el;

			return propnames;	
		}
	}

}
