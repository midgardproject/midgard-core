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

	public class RDFSQLNamespaceManager : SQLNamespaceManager {

		private RDFSQLContentManager _content_manager = null;

		/* constructor */
		public RDFSQLNamespaceManager (RDFSQLContentManager manager) {
			Object ();
			this._content_manager = manager;
		}

		/**
		 * Determines whether given string is identifier or its name, and returns
		 * valid name or identifier. 
		 * 
		 * If given string is valid uri, returns valid prefixed name.
		 * If given string is valid namespace's prefix, returns valid uri.
		 * 
		 * For example: for given "foaf:Person" string, returns "http://xmlns.com/foaf/0.1/Person", 
		 * for "http://xmlns.com/foaf/0.1/Person" string, returns "foaf:Person".
		 * 
		 * @param id, string to decode
		 * @return valid prefix or uri, or null if none is found
		 *
		 */
		public string? decode (string id) {
			if ("/" in id) {
                                string rs = id.rstr ("/");
                                if (rs == null)
                                        return null;
                                /* Add extra 1 for "/" taken into account in rstr() */
                                string uri = id.substring (0, (id.length - rs.length) + 1);
                                string name = this.get_name_by_identifier (uri);
                                if (name == null)
                                        return null;
                                /* Exclude extra "/" returned from rstr() */
                                return name + ":" + rs.substring (1, -1);
                        }
                        else if (":" in id) {
                                /* foaf:Person */
                                string[] spltd = id.split (":");
                                string uri = this.get_identifier_by_name (spltd[0]);
                                if (uri == null)
                                        return null;
                                return uri + spltd[1];
                        }
                        return null;
		} 
	}
}
