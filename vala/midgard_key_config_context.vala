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

errordomain KeyConfigContextError {
	PATH_IS_NOT_ABSOLUTE
}

namespace Midgard {

	public abstract class KeyConfigContext : Object {

		[Description(nick = "Context path", blurb = "Information about context path.")]
		public string? context_path { get; private set; default = null; }
	
		public KeyConfigContext (string? context_path) {
		
			this.context_path = context_path;
		}

		public virtual string[]? list_key_config() {
			return null;
		}
	}
}
