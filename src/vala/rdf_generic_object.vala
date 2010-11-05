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

	public class RDFGenericObject : GLib.Object, Storable {

		/* internal properties */
		internal string _guid = null;
		internal string _identifier = null;
		internal string _classname = null;
		internal int _id = 0;
		internal GLib.HashMap<string, GLib.Value?> _ns_values_hash = null;
		internal GLib.HashMap<string, string> _ns_literals_hash = null;

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
		}

		public uint id {
			get { return this._id; }
		}

		/* constructor */
		public RDFGenericObject (string classname) {
			this._classname = classname;

			this._ns_values_hash = new HashMap<string, GLib.Value?> ();
			this._ns_literals_hash = new HashMap<string,string> ();
		}

		/* methods */
		public virtual void set_property_value (string name, GLib.Value value) {
			this._ns_values_hash.set (name,value);
		}

		public virtual void set_property_literal (string name, GLib.Value value) {
			this._ns_literals_hash.set (name,value);
		}

		public virtual GLib.Value? get_property_value (string name) {
			return this._ns_values_hash.get(name)
		}

		public virtual GLib.Value? get_property_literal (string name) {
			return this._ns_literals_hash.get(name)
		}

		public virtual string[]? list_all_properties () {
			var v_keys = this._ns_values_hash.keys;
			var l_keys = this._ns_literals_hash.keys;

			HashSet<string> result = new HashSet<string>;

			foreach (string k in v_keys) {
				result.add(k);
			}

			foreach (string k in l_keys) {
				result.add(k);
			}

			var retval = result.to_array();
			delete result;

			return retval;
		}
	}

}
