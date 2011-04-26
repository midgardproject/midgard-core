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

	public errordomain ValidationError {
		NAME_INVALID,
		NAME_DUPLICATED,
		ELEMENT_INVALID,
		TYPE_INVALID,
		KEY_INVALID,
		VALUE_INVALID,
		REFERENCE_INVALID,
		PARENT_INVALID,
		LOCATION_INVALID,
		INTERNAL
	}	

	/**
	 * Simple validation interface.
	 *
	 * Provides basic methods to validate instance and check if the one is valid.
	 */
	public interface Validable {

		/**
		 * Check whether instance is valid.
		 *
		 * Implementation shall mark instance as invalid initially, and as valid one
		 * in case if validate didn't throw exception.
		 * 
		 * Returns true on success, false otherwise.
		 */
		public abstract bool is_valid ();

		/**
		 * Validates instance.
		 * 
		 * Implementation shall mark instance as valid in case of no error found.
		 */ 
		public abstract void validate () throws ValidationError;	
	}
}
