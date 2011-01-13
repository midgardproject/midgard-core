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

		/** 
		 * Initizalize new rdf {@link NamespaceManager} for given content manager.
		 */
		public RDFSQLNamespaceManager (RDFSQLContentManager manager) {
			Object ();
			this._content_manager = manager;
		}

		/**
		 * Check whether string is known prefix
		 *
		 * Checks, if the given string is a valid prefix, or holds valid prefix.
		 * For example, these both strings are valid for this method: 'foaf' and 'foaf:name'.
		 */
		public bool is_prefix (string id) {
			if (this.get_prefix (id) == null)
				return false;
			return true;
		}

		/**
		 * Get known prefix form string
		 */
		public string? get_prefix (string id) {
			/* registered name */
			if (this.name_exists (id) == true)
				return id.dup ();

			/* uri */
			if ("/" in id)	
				return null;

			/* prefix */
			if (":" in id) {
				string[] spltd = id.split (":");
				if (this.name_exists (spltd[0]) == true)
					return spltd[0].dup ();	
			}
			return null;
		}

		/**
		 * Check whether string is known uri
		 */
		public bool is_uri (string id) {
			if (this.get_uri (id) == null)
				return false;
			return true;		
		}	

		/** 
		 * Get known uri from string 
		 */
		public string? get_uri (string id) {
			/* registered identifier */
			if (this.identifier_exists (id) == true)
				return id.dup ();
			
			/* uri '#' terminating */
			if ("#" in id) {
				string[] spltd = id.split ("#");
				if (spltd[0] != null)
					if (this.identifier_exists (spltd[0]));
						return spltd[0] + "#";
			}

			/* prefix or unknown string */
			if (!("/" in id))
				return null;

			/* check uri */
			string rs = id.rstr ("/");
			string uri = id.substring (0, (id.length - rs.length) + 1);
			if (this.identifier_exists (uri) == true)
				return uri;
			
			return null;			
		}

		/**
		 * Determines whether given string is identifier or its name, and returns
		 * valid name or identifier.
		 * 
		 * If given string is valid uri, returns valid prefixed name.
		 * If given string is valid namespace's prefix, returns valid uri.
		 * 
		 * For example, for given 'foaf:Person' string, 'http:\/\/xmlns.com\/foaf\/0.1\/Person' is returned,
		 *
		 * for 'http:////xmlns.com\/foaf\/0.1\/Person' string, returns 'foaf:Person'.
		 * 
		 * @param id, string to decode
		 * 
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
