using GLib;


namespace MidgardCR
{
	public interface QueryConstraintSimple : GLib.Object {

		public abstract QueryConstraintSimple[]? list_constraints ();
	}

	public interface QueryStorage : GLib.Object {
		
		/* properties */
		public abstract string classname { get; set; }
	}

	public interface QueryConstraint : QueryConstraintSimple {

		/* properties */
		public abstract QueryValueHolder holder { get; set; }
		public abstract QueryProperty property { get; set; }
		public abstract QueryStorage storage { get; set; }
		public abstract string operator { get; set; }
	}

	public interface QueryConstraintGroup : QueryConstraintSimple {

		/* properties */
		public abstract string grouptype { get; set; }

		public abstract string get_group_type ();
		public abstract void set_group_type (string name);
		public abstract void add_constraint (QueryConstraintSimple constraint);
	}

	public interface QueryValueHolder : GLib.Object {
		
		public abstract GLib.Value get_value ();
		public abstract void set_value (GLib.Value value); 
	}	

	public class QueryValue : GLib.Object, QueryValueHolder {

		private GLib.Value _value;
			
		public GLib.Value get_value () { return this._value; }
		public void set_value (GLib.Value value) { this._value = value; }
	
	}

	public class QueryProperty : GLib.Object, QueryValueHolder {

		private GLib.Value _value;	
	
		/* properties */
		string propertyname { get; set; }
		QueryStorage storage { get; set; }

		public GLib.Value get_value () { return this._value; }
		public void set_value (GLib.Value value) { this._value = value; }
	}

	public interface QueryExecutor : Executable {

		public abstract void set_constraint (QueryConstraintSimple constraint);
		public abstract void set_limit (uint limit);
		public abstract void set_offset (uint offset);
		public abstract void add_order (QueryProperty property, string type);
		public abstract uint get_results_count ();
		public abstract void validate () throws ValidationError;
	}

	public interface QuerySelect : QueryExecutor {
		
		public abstract void add_join (string type, QueryProperty left_property, QueryProperty right_property);		
		public abstract Storable[]? list_objects ();
		public abstract void toggle_read_only (bool toggle);
	}

	public interface QueryData : QueryExecutor {
		
		public abstract void add_join (string type, QueryProperty left_property, QueryProperty right_property);	
		public abstract void collect_property (QueryProperty property, QueryStorage storage);	
		public abstract void list_data ();
	}
}
