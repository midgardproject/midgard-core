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

namespace MidgardCR
{
	public class QueryProperty : GLib.Object, QueryValueHolder {

		/* internal properties */
		internal string _property_name = null;
		internal SQLQueryStorage _storage = null;
		internal GLib.Object? _core_query_holder = null;
	
		/* properties */
		public string propertyname {
			/* FIXME, set and get using underlying core object */ 
			get { return this._property_name; }
			set { this._property_name = value; }
		}

		public SQLQueryStorage storage { 
			/* FIXME, set and get using underlying core object */
			get { return this._storage; }
			set { this._storage = value; }
		}

		/* constructor */
		public QueryProperty (string property, SQLQueryStorage? storage) {
			Object (propertyname: property, storage: storage);
			MidgardCRCore.QueryStorage _core_storage = storage != null ? storage._core_query_storage as MidgardCRCore.QueryStorage : null;
			this._core_query_holder = new MidgardCRCore.QueryProperty (property, _core_storage);	
		}	

		/* static constructor */
		public static QueryProperty create_property (string property, SQLQueryStorage? storage) {
			return new QueryProperty (property, storage);
		}

		public GLib.Value get_value () { return this._property_name; }
		public void set_value (GLib.Value value) { this._property_name = (string) value; }
	}
}
