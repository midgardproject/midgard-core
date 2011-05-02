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

	public class SQLObjectManager : ObjectManager {

		/* private properties */

		internal SQLStorageManager _storage_manager = null;
		
		/* constructor */

		/**
		 * Create new SQLObjectManager instance associated with SQL storage manager.
		 *
		 * This manager is responsible to help quickly and easily fetch single objects
		 * from SQL storage, which can be identified by unique guid or uuid.
		 *
		 * Check {@link SQLQuerySelect} if multiple objects of the same type should be fetched from SQL database.
		 */
		public SQLObjectManager (SQLStorageManager storage) {
			Object ();
			this._storage_manager = storage;
		}

		public override RepositoryObject? factory (string classname, string? guid = null) throws ObjectManagerError, ValidationError { 
			GLib.Type type_id = GLib.Type.from_name (classname);	
			if (type_id == 0)
				throw new ValidationError.NAME_INVALID ("%s is not registered class", classname);
			
			GLib.Type storable_id = GLib.Type.from_name ("MidgardCRRepositoryObject");
			if (type_id.is_a (storable_id) == false)
				throw new ValidationError.TYPE_INVALID ("%s is not RepositoryObject derived class", classname);

			if (guid == null) {
				RepositoryObject obj = (RepositoryObject) GLib.Object.new (type_id);
				return obj;	 
			}

			/* Initialize QuerySelect to fetch object's data from SQL table */
			var qst = new SQLQueryStorage (classname);
			var q = new SQLQuerySelect (this._storage_manager, qst);
			q.set_constraint (
				new SQLQueryConstraint (
					new QueryProperty ("guid", null), 
					"=", 
					QueryValue.create_with_value (guid), 
					null)
				);
			try {
				q.execute ();
			} catch (ExecutableError e) {
				throw new ValidationError.INTERNAL (e.message);
			}
			
			/* Throw exception if there's no single object found */
			if (q.resultscount < 1)
				throw new ObjectManagerError.OBJECT_NOT_EXIST ("%s object identified by %s doesn't exist", classname, guid);

			/* Throw exception if there's more then one record with the same guid */
			if (q.resultscount > 1)
				throw new ObjectManagerError.OBJECT_DUPLICATE ("%d objects identified by the same %s guid", q.resultscount, guid);

			RepositoryObject[] objects = q.list_objects ();
			return objects[0];
		}
	}
}
