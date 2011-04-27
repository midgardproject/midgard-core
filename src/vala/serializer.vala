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
 * Copyright (C) 2011 Piotr Pokora <piotrek.pokora@gmail.com>
 */

namespace MidgardCR {

	/**
	 * Serialize (and unserialize) {@link RepositoryObject}
	 */
	public interface Serializer : GLib.Object {
	
		/**
		 * Serialize object to string
		 */
		public abstract string? serialize (RepositoryObject object) throws ValidationError;

		/**
		 * Unserialize object from given string 
		 */
		public abstract RepositoryObject? unserialize (string buffer) throws ValidationError;		
	}
}
