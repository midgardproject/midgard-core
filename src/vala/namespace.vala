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

	public errordomain NamespaceManagerError {
		IDENTIFIER_INVALID,
		IDENTIFIER_EXISTS,
		NAME_INVALID,
		NAME_EXISTS
	}

	/**
	 * Namespace manager object provides bidirectional string mappings.
	 * One of the strings is a short name, which can be used as alias or prefix.
	 * The other one, is url, uri, or any other long string which itself is unique identifier.
	 *
	 * The main purpose of namespace manager implementation is to help application 
	 * to store name and unique identifier pairs and make them easily available.
	 * 
	 * For example, namespace manager can be used to store and manage rdf prefixes and uris.
	 */ 
	public interface NamespaceManager : GLib.Object {

		/* methods */

		/**
		 * Creates new uri with given name.
		 *
		 * {{{
		 *   namespace_manager.create_mapping ("foaf", "http://xmlns.com/foaf/0.1/");
		 * }}}
		 *
		 * @param name, short and unique name of the mapping
		 * @param id, full length identifier of the mapping
		 */
		public abstract void create_mapping (string name, string id) throws NamespaceManagerError;

		/**
		 * List all available names managed by manager.
		 *
		 * @return array of names or null if none is created and managed by manager.
		 */
		public abstract unowned string[]? list_names ();
		
		/**
		 *  Get identifier which is mapped with the given name.
		 *
		 * {{{
		 *   namespace_manager.create_mapping ("foaf", "http://xmlns.com/foaf/0.1/");
		 *   string id = namespace_manager.get_identifier_by_name ("foaf");
		 *   // returned id is "http://xmlns.com/foaf/0.1/"
		 * }}}
		 *
		 * @param name, short name of the identifier
		 * @return identifier mapped to the the given name, or null if none is found
		 */
		public abstract unowned string? get_identifier_by_name (string name);

		/**
		 *  Get the name which is mapped with the given identifier.
		 *
		 * {{{
		 *   namespace_manager.create_mapping ("foaf", "http://xmlns.com/foaf/0.1/");
		 *   string name = namespace_manager.get_name_by_identifier ("http://xmlns.com/foaf/0.1/");
		 *   // returned name is "foaf"
		 * }}}
		 *
		 *
		 * @param id, full length identifier 
		 * @return name mapped to identifier, or null if none is found
		 */
		public abstract unowned string? get_name_by_identifier (string id);

		/**
		 * Check whether given name exists.
		 * 
		 * @param name, the name to check 
		 * @return true if given name exists, false otherwise
		 */
		public abstract bool name_exists (string name);

		/**
		 * Check whether given identifier exists.
		 *
		 * @param id, identifier to check 
		 * @return true if given identifier exists, false otherwise
		 */
		public abstract bool identifier_exists (string id);
	}
}
