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
	 * RDFMapperProperty provides mappings (relations) between RDFS property and property registered 
	 * for {@link RepositoryObject} derived class. 
	 *
	 * {@see RDFMapperObject} 
	 *
	 * For example, RDFS property is 'foaf:name' and we need to store its data using 'firstname' property
	 * registered for 'Person' repository object.
	 * 
	 * {{{
	 *   var mapper_object = new RDFMapperObject ('foaf:Person', 'Person'); 
	 *   var mapper_property = new RDFMapperProperty ('foaf:name', 'username');
	 *   mapper_object.add_model (mapper_property);
	 * }}}
	 *
	 */ 
	public class RDFMapperProperty : ObjectPropertyModel {
		
		/* private properties */

		private string _property = null;

		/* public properties */
		public string? propertyname {
			get {
				return this._property;
			}
			set {
				this._property = value;
			}	
		}

		/**
		 * Constructor
		 * 
		 * Creates new RDFMapperProperty object for given RDFS and repository object's property names.
		 * 
		 * @param rdfs_property name of the RDFS property
		 * @param property_name {@link RepositoryObject} derived class' property name
		 */
		public RDFMapperProperty (string rdfs_property, string property_name) {
			Object (name: rdfs_property, propertyname: property_name);
		}
	}
}
