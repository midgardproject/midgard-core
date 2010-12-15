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

	public class RDFSQLContentManager : SQLContentManager {

		/* private properties */
		private RDFSQLQueryManager _rdf_query_manager = null;	

		public override unowned QueryManager query_manager {
			get { 
				if (this._rdf_query_manager == null)
					this._rdf_query_manager = new RDFSQLQueryManager ((RDFSQLStorageManager) this.storage_manager);
				return (QueryManager) this._rdf_query_manager;
			}
		}

		/* constructor */
		public 	RDFSQLContentManager (SQLStorageManager manager) {
			base (manager);
		}

		public override bool exists (Storable object) {
			return false;
		}	

		public override void create (Storable object) throws StorageContentManagerError {

			if (!(object is RDFGenericObject))
				throw new StorageContentManagerError.OBJECT_INVALID ("Expected RDFGenericObject");

			var rdf_object = (RDFGenericObject) object;
			foreach (RepositoryObject triple in rdf_object.list_triples ()) {
				base.create (triple);
			}
		}	
	
		public override void update (Storable object) throws StorageContentManagerError {
			this.save (object);
		}		

		public override void save (Storable object) throws StorageContentManagerError {

			if (!(object is RDFGenericObject))
				throw new StorageContentManagerError.OBJECT_INVALID ("Expected RDFGenericObject");

			var rdf_object = (RDFGenericObject) object;
			foreach (RepositoryObject triple in rdf_object.list_triples ()) {
				base.save (triple);
			}
		}		

		public override void purge (Storable object) throws StorageContentManagerError {

			if (!(object is RDFGenericObject))
				throw new StorageContentManagerError.OBJECT_INVALID ("Expected RDFGenericObject");

			var rdf_object = (RDFGenericObject) object;
			foreach (RepositoryObject triple in rdf_object.list_triples ()) {
				base.purge (triple);
			}
		}	
	
		public override void remove (Storable object) throws StorageContentManagerError {
			return;
		}		
	}
}
