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
		 */
		public bool is_prefix (string id) {
			string[] names = this.list_names ();
			string _t = id; /* valac requires char instead of const char */
			if (_t in names)
				return true;
			return false;
		}

		public string[]? get_prefix_tokens (string id) {
			/* registered name */
			string[] tokens = new string[2];
			tokens[0] = null;
			tokens[1] = null;
			tokens[2] = null;
			
			/* registered prefix */
			if (this.name_exists (id) == true) {
				tokens[0] = id;
				tokens[1] = null;
				return tokens;
			}

			/* uri */
			if ("/" in id) {
				string[] uri_tokens = this.get_uri_tokens (id);
				if (uri_tokens[0] != null) {
					tokens[0] = this.get_name_by_identifier (uri_tokens[0]);
					tokens[1] = uri_tokens[1];
				}	
				return tokens;
			}

			/* prefix statement */
			if (":" in id) {
				string[] spltd = id.split (":");
				if (this.name_exists (spltd[0]) == true) {
					tokens[0] = spltd[0];
					tokens[1] = spltd[1];	
				}
			}
			return tokens;
		}

		/**
		 * Get known prefix from string
		 */
		public string? get_prefix (string id) {
			string[] prefix = this.get_prefix_tokens (id);
			if (prefix[0] != null)
				return prefix[0];
			return null;
		}

		/**
		 * Get statement with known prefix from string
		 */
		public string? get_prefix_with_statement (string id) {
			string[] prefix = this.get_prefix_tokens (id);
			if (prefix[0] != null && prefix[1] != null)
				return prefix[0] + ":" + prefix[1];
			return null;
		}

		/**
		 * Check whether string is known uri
		 */
		public bool is_uri (string id) {
			string name = this.get_name_by_identifier (id);
			if (name == null)
				return false;
			return true;
		}	

		/** 
		 * Get uri tokens
		 *
		 * 
		 */
		public string[]? get_uri_tokens (string id) {
			string[] tokens = new string[2];
			tokens[0] = null;
			tokens[1] = null;
			tokens[2] = null;
			/* registered identifier */
			if (this.identifier_exists (id) == true) {
				tokens[0] = id;
				return tokens;
			}
			
			/* uri '#' terminating */
			if ("#" in id) {
				string[] spltd = id.split ("#");
				if (spltd[0] != null)
					if (this.identifier_exists (spltd[0] + "#")) {
						tokens[0] = spltd[0] + "#";
						tokens[1] = spltd[1];
					}
				return tokens;
			}

			/* prefix or unknown string, try to get prefix */
			if (!("/" in id)) {
				string[] prefix_tokens = this.get_prefix_tokens (id);
				if (prefix_tokens[0] != null) {
					tokens[0] = this.get_identifier_by_name (prefix_tokens[0]);
					tokens[1] = prefix_tokens[1];		
				}
				return tokens;
			}

			/* check uri */
			string rs = id.rstr ("/");
			string uri = id.substring (0, (id.length - rs.length) + 1);
			if (this.identifier_exists (uri) == true) {
				tokens[0] = uri;
				tokens[1] = rs.substring (1, -1);
			}
			
			return tokens;			
		}

		/** 
		 * Get known uri from string 
		 */
		public string? get_uri (string id) {
			string[] tokens = this.get_uri_tokens (id);
			if (tokens[0] != null)
				return tokens[0];
			return null;
		}

		/**
		 * Get statement with known uri from string
		 */
		public string? get_uri_with_statement (string id) {
			string[] tokens = this.get_uri_tokens (id);
			if (tokens[0] != null && tokens[1] != null)
				return tokens[0] + tokens[1];
			return null;
		}
	}
}
