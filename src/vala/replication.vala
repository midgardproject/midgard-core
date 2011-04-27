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

	public errordomain ReplicationError {
		OBJECT_IMPORTED,
		OBJECT_EXPORTED,
		OBJECT_DELETED,
		OBJECT_PURGED,
		OBJECT_NOT_EXISTS,
		INTERNAL
	}	

	/**
	 * Replication queue.
	 * 
	 * Provides access to replication tasks: 
	 * 
	 *  1. Adding content (as objects or serialized data) to queue
	 *  1. Validate added objects or data
	 *  1. Perform import or export
	 *
	 * Queue can also handle serializer object. If implementation and application require replication 
	 * and given data is expected to be always in the same format. For example, if incoming data
	 * is always in JSON format, replication queue can provide default JSON serializer. 
	 *
	 * In any other case, implementation shall depend only on values of object type added to queue.
	 */
	public interface ReplicationQueue : Queue, Executable, Validable {
		
		/**
		 * Serializer used by queue.
		 */ 
		public abstract Serializer? serializer { get; }	
	}

	/**
	 * Replication manager.
	 */
	public abstract class ReplicationManager : GLib.Object {
	
		/**
		 * Create new export queue.
		 */
		public virtual ReplicationQueue? create_queue_export() {
			return null;
		}

		/**
		 * Create new import queue.
		 */
		public virtual ReplicationQueue? create_queue_import() {
			return null;
		}
	}
}
