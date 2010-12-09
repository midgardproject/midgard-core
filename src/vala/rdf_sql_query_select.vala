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
	public class RDFSQLQuerySelect: SQLQuerySelect {

		/* internal properties */
		internal RDFSQLQueryStorage _rdf_query_storage = null;	
		internal QueryConstraintSimple _original_constraint = null;
		internal QueryConstraintSimple _triple_constraint = null;
	
		/* Constructor */
		public RDFSQLQuerySelect (StorageManager manager, RDFSQLQueryStorage storage) {
			base (manager, (SQLQueryStorage) storage);
			this._rdf_query_storage = storage;
		}
		
		public static new RDFSQLQuerySelect create_query_select (StorageManager manager, RDFSQLQueryStorage storage) {
			return new RDFSQLQuerySelect (manager, storage);
		}

		~RDFSQLQuerySelect () {
			print ("FIXME, destroy implicit constraints");
		}

		public override void set_constraint (QueryConstraintSimple constraint) {
			this._original_constraint = constraint;
		}

		public override unowned QueryConstraintSimple get_constraint () {
			return this._original_constraint;
		}

		public override void validate () throws ValidationError {
			/* TODO */
		}

		public override void execute () throws ExecutableError {
			/* TODO, determine classname */
			print ("CLASS %s \n", this._query_storage.classname);
			this._query_storage._core_query_storage.set ("dbclass", "RDFTripleObject");

			/* Create new constraint group, which will hold all triple related constraints */
			QueryConstraintGroup c_group = new SQLQueryConstraintGroup ("AND");

			/* Default case, so we add implicit classname constraint.
			 * ... AND triple_table.classname = ''...  */
			c_group.add_constraint (new SQLQueryConstraint (
				new QueryProperty ("classname", null),
				"=",
				QueryValue.create_with_value (this._query_storage.classname),
				null)
			);

			/* Get all available constraints */
			QueryConstraintSimple[]? constraints = null;
			var c = this.get_constraint ();
			if (c !=null) {
				constraints += c;
				foreach (QueryConstraintSimple constraint in c.list_constraints ()) {
					constraints += constraint;
				}
			}
			/* Determine new constraints and add them to group */
			foreach (QueryConstraintSimple constraint in constraints) {
				/* Create three constraints for every added one.
				 * We need to check if property_literal or property_value match.
				 * ...AND (
				 *        property_name = constraint.property.propertyname 
				 *        AND
				 * 	 (property_literal = constraint.value.get_value 
				 * 		OR property_value = constraint.value.get_value))	 */
				if (constraint is SQLQueryConstraint) {
					string property_name = ((QueryProperty) ((SQLQueryConstraint)constraint).property).propertyname;
					string property_value = ((QueryValueHolder) ((SQLQueryConstraint)constraint).holder).get_value ().get_string ();
					/* Add 'property' constraint */
					c_group.add_constraint ( 
						new SQLQueryConstraint (
							new QueryProperty ("property", null),
								"=",
								QueryValue.create_with_value (property_name),
								null)
					);
					
					/* Add property_value and property_literal constraints */
					var t_group = new SQLQueryConstraintGroup ("OR");
					t_group.add_constraint ( 
						new SQLQueryConstraint (
							new QueryProperty ("literal", null),
								"=",
								QueryValue.create_with_value (property_value),
								null)
					);
					t_group.add_constraint ( 
						new SQLQueryConstraint (
							new QueryProperty ("value", null),
								"=",
								QueryValue.create_with_value (property_value),
								null)
					);
					c_group.add_constraint (t_group);	
				}
			}

			/* Set implicit constraints, so we can destroy them all */
			this._triple_constraint = c_group;

			base.set_constraint (c_group);
			base.execute ();	
		}

		private RDFGenericObject _find_rdf_object (RDFGenericObject[]? objects, string name, string guid) {
			if (objects == null)
				return null;
			foreach (RDFGenericObject object in objects) {
				if (object.classname == name
					&& object.guid == guid)
					return object;
			}
			return null;
		}

		public override Storable[]? list_objects () {
			Storable[]? objects = base.list_objects ();
			if (objects == null)
				return null;
			/* Initialize new RDFGenericObject instances, from all triple objects
			 * selected from database. Every triple is initialized as standalone 
			 * object and added to specific rdf object if both holds the same 
			 * guid and objectguid properties values */
			RDFGenericObject[]? rdf_objects = null;
			foreach (Storable object in objects) {
				string classname;
				string guid;
				object.get ("classname", out classname, "objectguid", out guid);
				RDFGenericObject rdf_object = this._find_rdf_object (rdf_objects, classname, guid);
				if (rdf_object == null) {
					rdf_object = new RDFGenericObject (classname, guid);
					rdf_objects += rdf_object;
				}
				rdf_object.add_triple ((RepositoryObject)object);
			}
			return (Storable[]) rdf_objects;
		}
	}
}
