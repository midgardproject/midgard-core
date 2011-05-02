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

		private void _destroy_constraint (QueryConstraintSimple c) {
			return;
			if (c == null)
				return;
			QueryConstraintSimple[]? constraints = c.list_constraints ();
			return;
			foreach (QueryConstraintSimple constraint in c.list_constraints ()) {
				this._destroy_constraint (constraint);
			}
			c = null;
		}

		~RDFSQLQuerySelect () {	
			this._destroy_constraint (this._triple_constraint);
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

		private string _encode_value (NamespaceManager ns_manager, string val) {
			if ("/" in val) {
				string rs = val.rstr ("/");
				if (rs == null)
					return val;
				/* Add extra 1 for "/" taken into account in rstr() */
				string uri = val.substring (0, (val.length - rs.length) + 1);
				string name = ns_manager.get_name_by_identifier (uri);
				if (name == null)
					return val;
				/* Exclude extra "/" returned from rstr() */
				return name + ":" + rs.substring (1, -1); 
			} 
			else if (":" in val) { 
				/* foaf:Person */
				string[] spltd = val.split (":");
				string uri = ns_manager.get_identifier_by_name (spltd[0]);	
				if (uri == null)
					return val;
				return uri + spltd[1];
			}
			return val.dup ();
		}

		public override void execute () throws ExecutableError {
			/* TODO, determine classname */	
			string _dest_classname = "RDFTripleObject";
			this._query_storage._core_query_storage.set ("dbclass", _dest_classname);
			NamespaceManager ns_manager = this.storagemanager.content_manager.namespace_manager;

			/* Create new constraint group, which will hold all triple related constraints */
			QueryConstraintGroup c_group = new SQLQueryConstraintGroup ("AND");

			/* Default case, so we add implicit classname constraints.
			 * ... AND (triple_table.class_name = '' OR triple.class_name = '' )...  */
			var classname_group = new SQLQueryConstraintGroup ("OR");
			string _classname = _encode_value (ns_manager, this._query_storage.classname);
			classname_group.add_constraint (new SQLQueryConstraint (
				new QueryProperty ("classname", null),
				"=",
				QueryValue.create_with_value (_classname),
				null)
			);
			classname_group.add_constraint (new SQLQueryConstraint (
				new QueryProperty ("classname", null),
				"=",
				QueryValue.create_with_value (this._query_storage.classname),
				null)
			);
			c_group.add_constraint (classname_group);

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
					string encoded_property_name = _encode_value (ns_manager, 
						((QueryProperty) ((SQLQueryConstraint)constraint).property).propertyname);
					string property_name = ((QueryProperty) ((SQLQueryConstraint)constraint).property).propertyname;
					string property_value = ((QueryValueHolder) ((SQLQueryConstraint)constraint).holder).get_value ().get_string ();
					
					/* Create new QueryStorage for join and constraints added */
					var join_storage = new RDFSQLQueryStorage (_dest_classname);

					/* Create implicit join, so we can select all triples with the same objectguid */
					base.add_join ("left", new QueryProperty ("objectguid", null), 
						new QueryProperty ("objectguid", join_storage));			


					/* Add 'property' constraint */
					/* We add both (OR grouped) constraints. One with uri and second one with it's alias */
					var property_name_group = new SQLQueryConstraintGroup ("OR");
					property_name_group.add_constraint ( 
						new SQLQueryConstraint (
							new QueryProperty ("property", join_storage),
								"=",
								QueryValue.create_with_value (property_name),
								null)
					);	
					property_name_group.add_constraint ( 
						new SQLQueryConstraint (
							new QueryProperty ("property", join_storage),
								"=",
								QueryValue.create_with_value (encoded_property_name),
								null)
					);
					c_group.add_constraint (property_name_group);			
	
					/* Add property_value and property_literal constraints */
					var t_group = new SQLQueryConstraintGroup ("OR");
					t_group.add_constraint ( 
						new SQLQueryConstraint (
							new QueryProperty ("literal", join_storage),
								"=",
								QueryValue.create_with_value (property_value),
								null)
					);
					t_group.add_constraint ( 
						new SQLQueryConstraint (
							new QueryProperty ("value", join_storage),
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

		private RDFGenericObject? _find_rdf_object (RDFGenericObject[]? objects, string name, string guid) {
			if (objects == null)
				return null;
			foreach (RDFGenericObject object in objects) {
				if (object.classname == name
					&& object.guid == guid)
					return object;
			}
			return null;
		}

		public override RepositoryObject[]? list_objects () {
			RepositoryObject[]? objects = base.list_objects ();
			if (objects == null)
				return null;
			/* Initialize new RDFGenericObject instances, from all triple objects
			 * selected from database. Every triple is initialized as standalone 
			 * object and added to specific rdf object if both holds the same 
			 * guid and objectguid properties values */
			RDFGenericObject[]? rdf_objects = null;
			foreach (RepositoryObject object in objects) {
				string classname;
				string guid;
				object.get ("classname", out classname, "objectguid", out guid);
				RDFGenericObject rdf_object = this._find_rdf_object (rdf_objects, classname, guid);
				if (rdf_object == null) {
					rdf_object = new RDFGenericObject (classname, null, guid);
					rdf_objects += rdf_object;
				}
				rdf_object.add_triple ((RepositoryObject)object);
			}
			return (RepositoryObject[]) rdf_objects;
		}
	}
}
