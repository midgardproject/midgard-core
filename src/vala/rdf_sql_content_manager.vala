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
		private RDFSQLNamespaceManager _ns_manager = null;

		public override unowned NamespaceManager namespace_manager {
                        get {
                                if (this._ns_manager == null)
                                        this._ns_manager = new RDFSQLNamespaceManager (this);
                                return this._ns_manager;
                        }
                }

		public override unowned QueryManager query_manager {
			get { 
				if (this._rdf_query_manager == null)
					this._rdf_query_manager = new RDFSQLQueryManager ((RDFSQLStorageManager) this.storagemanager);
				return (QueryManager) this._rdf_query_manager;
			}
		}

		/* constructor */
		public 	RDFSQLContentManager (SQLStorageManager manager) {
			base (manager);
		}

		public override bool exists (RepositoryObject object) {
			return false;
		}	

		private void _create_repository_object (RDFGenericObject rdf_object, RepositoryObject object) throws StorageContentManagerError {
			RDFSQLNamespaceManager nsm = (RDFSQLNamespaceManager) this.namespace_manager;
			foreach (RepositoryObject triple in rdf_object.list_triples ()) {
				string pname;
				string _sv = null;
				string _v;
				triple.get ("property", out pname, "value", out _v, "literal", out _sv);
				//string _sv = triple.get_property_literal (pname);
				//Value _v = triple.get_property_value (pname);
				string[] prefix_tokens = nsm.get_prefix_tokens (pname);
				object.set_property_literal (prefix_tokens[1], _sv);
				print ("SET OBJECT, PROP %s (%s), (%s) (%s)[%p] \n", pname, prefix_tokens[1], (string) _v, _sv, _sv);	
			}
		}

		public override void create (RepositoryObject object) throws StorageContentManagerError {

			/* RDFGenericObject object with RDFObjectModel for RepositoryObject available */

			if (!(object is RDFGenericObject))
				throw new StorageContentManagerError.OBJECT_INVALID ("Expected RDFGenericObject");

			/* Decorated repository object */
			RepositoryObject ro = ((RDFGenericObject)object).repository_object;
			if (ro != null) {
				this._create_repository_object ((RDFGenericObject) object, ro);
				return;
			}

			/* RDF generic object with triples */
			var rdf_object = (RDFGenericObject) object;
			foreach (RepositoryObject triple in rdf_object.list_triples ()) {
				base.create (triple);
			}
		}	
	
		public override void update (RepositoryObject object) throws StorageContentManagerError {
			this.save (object);
		}		

		public override void save (RepositoryObject object) throws StorageContentManagerError {

			if (!(object is RDFGenericObject))
				throw new StorageContentManagerError.OBJECT_INVALID ("Expected RDFGenericObject");

			var rdf_object = (RDFGenericObject) object;
			foreach (RepositoryObject triple in rdf_object.list_triples ()) {
				base.save (triple);
			}
		}		

		public override void purge (RepositoryObject object) throws StorageContentManagerError {

			if (!(object is RDFGenericObject))
				throw new StorageContentManagerError.OBJECT_INVALID ("Expected RDFGenericObject");

			var rdf_object = (RDFGenericObject) object;
			foreach (RepositoryObject triple in rdf_object.list_triples ()) {
				base.purge (triple);
			}
		}	
	
		public override void remove (RepositoryObject object) throws StorageContentManagerError {
			return;
		}		
	}
}
