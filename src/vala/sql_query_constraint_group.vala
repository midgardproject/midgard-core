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
	public class SQLQueryConstraintGroup : GLib.Object, QueryConstraintSimple, QueryConstraintGroup {

		/* internal properties */
		internal string _group_type = null;
		internal GLib.Object? _core_query_constraint = null;

		/* properties */
		public string grouptype { 
			get { return this._group_type; }
			set { this._group_type = value; }
		}

		/* constructor */
		public SQLQueryConstraintGroup (string group_type) {
			Object (grouptype: group_type);

			this._core_query_constraint = (MidgardCRCore.QueryConstraintSimple) new MidgardCRCore.QueryConstraintGroup ();
			((this as SQLQueryConstraintGroup)._core_query_constraint as MidgardCRCore.QueryConstraintGroup)
				.set_group_type (group_type);
		}	

		/* methods */
		public static SQLQueryConstraintGroup create_constraint_group (string group_type) {
			return new SQLQueryConstraintGroup (group_type);
		}

		public static SQLQueryConstraintGroup create_with_constraints (string group_type, QueryConstraintSimple[] constraints) {
			SQLQueryConstraintGroup constraint = new SQLQueryConstraintGroup (group_type);
			foreach (QueryConstraintSimple c in constraints) {
				constraint.add_constraint (c);
			} 
			return constraint;
		}

		public void add_constraint (QueryConstraintSimple constraint) {
			/* This condition is for safe typecasting. We can not type cast to constraint simple
			 * interface, due to core query objects held by particular constraints */
			if (constraint is QueryConstraint)
				((this as SQLQueryConstraintGroup).
					_core_query_constraint as MidgardCRCore.QueryConstraintGroup).
						add_constraint (((constraint as SQLQueryConstraint)._core_query_constraint as MidgardCRCore.QueryConstraintSimple));
			else if (constraint is QueryConstraintGroup)
				((this as SQLQueryConstraintGroup).
					_core_query_constraint as MidgardCRCore.QueryConstraintGroup).
						add_constraint (((constraint as SQLQueryConstraintGroup)._core_query_constraint as MidgardCRCore.QueryConstraintSimple));

		}

		public unowned QueryConstraintSimple[]? list_constraints () {
			return ((this as SQLQueryConstraintGroup)._core_query_constraint as SQLQueryConstraint).list_constraints ();
		}
	}
}
