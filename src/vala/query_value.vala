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
	public class QueryValue : GLib.Object, QueryValueHolder {
		
		/* internal properties */
		internal GLib.Object _core_query_holder = null;

		private void _create_core_value () {
			if (this._core_query_holder == null)
				this._core_query_holder = new MidgardCRCore.QueryValue ();
			return;
		}

		/* static constructor */
		public static QueryValue create_with_value (GLib.Value value) {
			QueryValue qv = new QueryValue ();
			qv._create_core_value ();
			qv.set_value (value);
			return qv;
		}

		public GLib.Value get_value () { 
			this._create_core_value ();
			GLib.Value val = "";
			((MidgardCRCore.QueryValue)this._core_query_holder).get_value (ref val);
			return val;
		}

		public void set_value (GLib.Value value) { 
			this._create_core_value ();
			((MidgardCRCore.QueryValue)this._core_query_holder).set_value (value);
		}
	}
}
