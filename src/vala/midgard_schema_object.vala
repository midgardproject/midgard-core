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

		/* properties */
		public string guid { 
			get { return this._guid; }
		}

		public uint id { 
			get { return this._id; }
		}

		public Metadata? metadata { 
			get {  return null; }	
		}

		/* methods */
		public abstract void set_property_value (string name, GLib.Value value);
		public abstract GLib.Value get_property_value (string name);
		public abstract string[]? list_all_properties ();
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
