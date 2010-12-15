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

	public errordomain NamespaceManagerError {
		URI_INVALID,
		URI_EXISTS,
		ALIAS_INVALID,
		ALIAS_EXISTS
	}

	public interface NamespaceManager : GLib.Object {

		/* methods */
		public abstract bool create_uri (string uri, string name) throws NamespaceManagerError;
		public abstract unowned string[]? list_names ();
		public abstract unowned string? get_uri_by_name (string name);
		public abstract unowned string? get_name_by_uri (string uri);
		public abstract bool name_exists ();
		public abstract bool uri_exists ();
	}
}
