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

		public override bool create (Storable object) throws StorageContentManagerError {
			base.create (object);
			if (object is RepositoryObject) {
				string[] props = ((RepositoryObject) object).list_all_properties ();
				var builder = new ObjectBuilder ();
				foreach (string name in props) {
					GLib.Value pval = ((RepositoryObject) object).get_property_value (name);
					/* TODO
					 * Handle Object value type
					 * Check property name if it contains full uri */
					var rdf_prop = builder.factory ("RDFGenericObject") as RepositoryObject;
					string[] ptokens = name.split (":", 2);
					rdf_prop.set (
						"objectguid", ((RepositoryObject)object).guid,
						"property", ptokens[1],
						"alias", ptokens[0],
						"value", (string) pval);
					base.create (rdf_prop);
					//delete rdf_prop;
				}
				//delete (Object)builder; 
			}
			return true;
		}	
	
		public override bool update (Storable object) throws StorageContentManagerError {
			return false;
		}		

		public override bool save (Storable object) throws StorageContentManagerError {
			return false;
		}		

		public override bool remove (Storable object) throws StorageContentManagerError {
			return false;
		}	
	
		public override bool purge (Storable object) throws StorageContentManagerError {
			return false;
		}		

		/* public unowned QueryManager get_query_manager () {
			return null;
		} */
	}
}
