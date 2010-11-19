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
	public class SQLQueryConstraint : GLib.Object, QueryConstraintSimple, QueryConstraint {

		/* internal properties */
		internal QueryValueHolder _holder = null;
		internal QueryProperty _property = null;
		internal SQLQueryStorage _storage = null;
		internal GLib.Object _core_query_constraint = null;
		internal string _op = null;	

		/* properties */
		public QueryValueHolder holder { 
			get { return this._holder; }
			set { this._holder = value; }
		}

		public QueryProperty property { 
			get { return this._property; }
			set { this._property = value; }
		}

		public QueryStorage storage { 
			get { return (QueryStorage) this._storage; } 
			set { this._storage = (SQLQueryStorage) value; }
		}

		public string operator { 
			get { return this._op; }
			set { this._op = value; }
		}

		/* constructor */
		public SQLQueryConstraint (QueryProperty property, string op, QueryValueHolder holder, SQLQueryStorage? storage) {
			Object (property: property, operator: op, holder: holder);
			if (storage != null)
				this.storage = storage;
			MidgardCRCore.QueryStorage _core_storage = storage != null ? storage._core_query_storage as MidgardCRCore.QueryStorage: null;
			this._core_query_constraint = 
				new MidgardCRCore.QueryConstraint (
					property._core_query_holder as MidgardCRCore.QueryProperty,
					op,
					((holder as MidgardCR.QueryValue)._core_query_holder as MidgardCRCore.QueryValue),
					_core_storage);
		}	

		public static SQLQueryConstraint create_constraint (
			QueryProperty property, string op, QueryValueHolder holder, SQLQueryStorage? storage){
			return new SQLQueryConstraint (property, op, holder, storage);
		}

		/* methods */
		public QueryConstraintSimple[]? list_constraints () {
			return null;
		}
	}
}
