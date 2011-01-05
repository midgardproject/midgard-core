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
	 * RDFMapperObject provides mappings (relations) between {@link RepositoryObject} derived class 
	 * and RDFS' one.
	 *
	 * The main purpose is to find (quickly) repository class using given RDFS one, and let application
	 * to work with repository objects instead of (unknown in some cases) RDF triples. RDFMapperObject shall 
	 * be always associated with {@link RDFSQLModelManager} and thus available for all RDF implementations,
	 * including Query, Storage or Content managers.  
	 *
	 * For example, RDFS class is 'foaf:Person' and we need to store its data using 'Person' repository object.
	 * 
	 * {{{
	 *   RDFMapperObject mapper_object = new RDFMapperObject ('foaf:Person', 'Person'); 
	 * }}}
	 *
	 * Once this model is registered with model manager, it'll be reused by all RDF managers. If such RDF class is
	 * found in onthology, and its corresponding {@link RDFGenericObject} is initialized, its data won't be stored
	 * in default RDF object's storage, but instead, will be stored in the storage defined for 'Person' class. 
	 * Also, when such mapping is done, any instance of this class can be created using either 'foaf:Person' or 'Person'
	 * class name.
	 */ 
	public class RDFMapperObject : ObjectModel {
		
		/* private properties */

		private string _classname = null;

		/* public properties */

		public string? classname {
			get {
				return this._classname;
			}
			set {
				this._classname = value;
			}	
		}

		/**
		 * Constructor
		 * 
		 * Creates new RDFMapperObject object for given RDFS and repository class names.
		 * 
		 * @param rdfs_class name of the RDFS class 
		 * @param repository_class {@link RepositoryObject} derived class name
		 */
		public RDFMapperObject (string rdfs_class, string repository_class) {
			Object (name: rdfs_class, classname: repository_class);
		}
	}
}
