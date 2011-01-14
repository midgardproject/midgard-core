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

	public class RDFSQLObjectManager : ObjectManager {

		/* private properties */

		internal RDFSQLStorageManager _storage_manager = null;
	
		/* public properties */

		public StorageManager storagemanager {
			get { return (StorageManager) this._storage_manager; }
			construct { this._storage_manager = (RDFSQLStorageManager) value; }
		}
	
		/* constructor */

		/**
		 * Create new RDFSQLObjectManager instance associated with RDF SQL storage manager.
		 *
		 * The main approach of this object manager is to optimize rdf object creation.
		 * It creates (with factory method) RDFGenericObject instances, which (depends on available
		 * rdf models and namespace manager) can be proxy to RDF triples or RepositoryObject decorator.
		 * 
		 * Check {@link RDFSQLQuerySelect} if multiple objects of the same type should be fetched from SQL database.
		 */
		public RDFSQLObjectManager (RDFSQLStorageManager storage) {
			Object (storagemanager: storage);	
		}

		/**
		 * RDF objects' factory
		 * 
		 * This method optimizes RDF objects creation. New instances are initialized for gievn classname
		 * which can be {@link RepositoryObject} derived classname, prefix with classname or uri identifying 
		 * particular type. 
		 *
		 * Depending on available {@link RDFMapperObject} models and {@link RDFSQLNamespaceManager}
		 * classname argument could be (for example) :
		 *
		 *  1. Person
		 *  1. foaf:Person
		 *  1. http://xmlns.com/foaf/0.1/Person
		 */
		public override Storable? factory (string classname, string? guid = null) throws ObjectManagerError, ValidationError {
			if (this._storage_manager == null)
				throw new ValidationError.INTERNAL ("No StorageManager associated with objectManager");

			RDFSQLNamespaceManager nsm = (RDFSQLNamespaceManager)this._storage_manager.content_manager.namespace_manager;
			if (nsm == null)
				throw new ValidationError.INTERNAL ("No NamespaceManager associated with ContentManager"); 

			string prefix_stmt = nsm.get_prefix_with_statement (classname);
			string uri_stmt = nsm.get_uri_with_statement (classname);
			/* Check if RDFMapperObject for given classname exists.
			 * If yes, create RepositoryObject mapped to given prefix or uri. */
			RDFSQLModelManager mm = (RDFSQLModelManager) this._storage_manager.model_manager;
			var om = mm.get_model_by_name (prefix_stmt);
			if (om == null)
				om = mm.get_model_by_name (uri_stmt);
			if (om != null) 
				return base.factory (((RDFMapperObject)om).classname, guid);
				
			/* There's no namespace registered, try generic rdf object */
			if (guid == null)
				return (Storable) new RDFGenericObject (classname, guid);		
			
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

			Storable[] objects = q.list_objects ();
			return objects[0];
		}
	}
}
