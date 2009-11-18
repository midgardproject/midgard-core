/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
 *
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
 */

using GLib;

namespace Midgard {
	
	public abstract class KeyConfig : Object {

		protected string path = null;
		protected Midgard.KeyConfigContext context;	
	
		public KeyConfig (Midgard.KeyConfigContext keyctx, string path) {
		
			this.context = keyctx;
			this.path = path;
		}

		public virtual void set_value (string group, string key, string value) {

			return;
		}	

		public virtual string? get_value (string group, string key) {

			string value = null;
			return value;
		}	

		public virtual void set_comment (string group, string key, string comment) {
		
			return;
		}
	
		public virtual string? get_comment (string group, string key) {
	
			string comment = null;	
			return comment;
		}

		public virtual string[]? list_groups () {
			return null;
		}	
		
		public virtual bool group_exists (string name) {

			return false;
		}

		public virtual bool delete_group (string name) {
		
			return false;
		}

		public virtual bool store () {

			return false;
		}

		public virtual bool load_from_data (string data) {
	
			return false;
		}

		public virtual string? to_data () {
		
			return null;
		}
	}
}
