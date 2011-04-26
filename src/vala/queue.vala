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
	 * Simple interface which provides access to data queue.
	 * 
	 * Main idea is to allow create queue for any kind of data and any kind of data type. 
	 * The data which is stored in queue is simple GLib.Value, so it's up to the implementation
	 * or application if there are strings, integers or objects queued.  
	 * 
	 * Any value stored in queue as its element, can be (optionally) identified 
	 * with unique name. Implementation shall quarantee name uniqueness and provide naming convention.  
	 * Multiple elements with null or empty names shall be allowed.
	 */
	public interface Queue : GLib.Object {		

		/**
		 * Add new element to the queue and put it on its end.
		 */
		public abstract bool append_value (Value val, string? name = null);

		/**
		 * Add new element to the queue and put it on its start.
		 */
		public abstract bool prepend_value (Value val, string? name = null);

		/**
		 * In the queue, find an element which is identified by given name
		 */
		public abstract Value get_value_by_name (string name);

		/**
		 * Get number of elements in the queue.
		 */
		public abstract uint get_length ();

		/**
		 * In the queue, check if value at given position exists.
		 */
		public abstract bool value_exists_at (uint position);

		/** 
		 * From the queue, get value which exists at given position.
		 */
		public abstract Value? get_value_at (uint position);

		/** 
		 * In the queue, set value at given position.
		 * It's implementation specific what should be done in the case 
		 * if some value already exists at the same position.
		 */
		public abstract bool set_value_at (Value val, uint position);

		/**
		 * From the queue, get current position. 
		 */
		public abstract uint get_current_position ();

		/**
		 * From the queue, remove value which exists at given position.
		 */
		public abstract bool remove_value (uint position);
	}
}
