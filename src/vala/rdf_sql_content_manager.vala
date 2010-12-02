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

	public class RDFSQLContentManager : SQLStorageContentManager {
	
		/* constructor */
		public 	RDFSQLContentManager (SQLStorageManager manager) {
			base (manager);
		}

		public override bool exists (Storable object) {
			return false;
		}	

		public override void create (Storable object) throws StorageContentManagerError
		{
			//base.create (object);

			if (object is RDFGenericObject) {
				var builder = new ObjectBuilder ();

				var rdf_object = (RDFGenericObject) object;
				foreach (string name in rdf_object.list_all_properties()) {
					/* TODO: convert property-name to canonical form */
					var rdf_prop = builder.factory ("RDFTripleObject") as RepositoryObject;
					rdf_prop.set(
						"objectguid", rdf_object.guid,
						"identifier", rdf_object.identifier,
						"classname",  rdf_object.classname,
						"property",   name,
						"literal",    (string) rdf_object.get_property_literal(name),
						"value",      (string) rdf_object.get_property_value(name)
					);
					base.create (rdf_prop);
				}
			}
		}	
	
		public override void update (Storable object) throws StorageContentManagerError {
			return;
		}		

		public override void save (Storable object) throws StorageContentManagerError {
			return;
		}		

		public override void remove (Storable object) throws StorageContentManagerError {
			return;
		}	
	
		public override void purge (Storable object) throws StorageContentManagerError {
			return;
		}		

		/* public unowned QueryManager get_query_manager () {
			return null;
		} */
	}
}
