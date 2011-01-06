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
 * Copyright (C) 2010, 2011 Piotr Pokora <piotrek.pokora@gmail.com>
 */

using GLib;

namespace MidgardCR
{
	/**
	 * Basic interface for abstract constraints. 
	 */
	public interface QueryConstraintSimple : GLib.Object {
		
		/**
		 * List all constraints added to instance.
		 */		
		public abstract unowned QueryConstraintSimple[]? list_constraints ();
	}

	/**
	 * Basic interface to propagate class' storage.
	 *
	 */ 
	public interface QueryStorage : GLib.Object {
		
		/* properties */
		
		/**
		 * Returns classname for which QueryStorage has been initialized for.
		 */
		public abstract string classname { get; set; }
	}

	/**
	 * Basic interface for query single constraints.
	 */
	public interface QueryConstraint : QueryConstraintSimple {

		/* properties */
		public abstract QueryValueHolder holder { get; set; }
		public abstract QueryProperty property { get; set; }
		public abstract QueryStorage storage { get; set; }
		public abstract string operator { get; set; }
	}

	/**
	 * Basic interface for constraint which can group other constraints.
	 */
	public interface QueryConstraintGroup : QueryConstraintSimple {

		/* properties */
		public abstract string grouptype { get; set; }

		public abstract void add_constraint (QueryConstraintSimple constraint);
	}

	/**
	 * Basic interface which hold constraint's value.
	 */
	public interface QueryValueHolder : GLib.Object {
		
		public abstract GLib.Value get_value ();
		public abstract void set_value (GLib.Value value); 
	}	

	/**
	 * Basic interface for query execution.
	 */ 
	public interface QueryExecutor : Executable {

		public abstract void set_constraint (QueryConstraintSimple constraint);
		public abstract void set_limit (uint limit);
		public abstract void set_offset (uint offset);
		public abstract void add_order (QueryProperty property, string type);
		public abstract uint get_results_count ();
		public abstract void validate () throws ValidationError;
	}

	/**
	 * Basic interface for queries which fetch objects from storage
	 */
	public interface QuerySelect : QueryExecutor {
		
		public abstract void add_join (string type, QueryProperty left_property, QueryProperty right_property);		
		public abstract Storable[]? list_objects ();
		public abstract void toggle_read_only (bool toggle);
	}
	
	/**
	 * Basic interface for queries which fetch data from storage.
	 */
	public interface QueryData : QueryExecutor {
		
		public abstract void add_join (string type, QueryProperty left_property, QueryProperty right_property);	
		public abstract void collect_property (QueryProperty property, QueryStorage storage);	
		public abstract void list_data ();
	}
}
