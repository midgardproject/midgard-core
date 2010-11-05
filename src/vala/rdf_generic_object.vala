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
		internal GLib.List<string> _ns_properties_list = null;
		internal GLib.List<GLib.Value?> _ns_values_list = null;

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

		public virtual void set_property_literal (string name, GLib.Value value) {
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

		public virtual GLib.Value? get_property_literal (string name) {
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

}
