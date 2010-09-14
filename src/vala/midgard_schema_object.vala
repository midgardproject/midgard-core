using GLib;

namespace Midgard {

	public interface Storable : GLib.Object {

		/* signals */
		public abstract signal void create ();
		public abstract signal void created ();
		public abstract signal void update ();
		public abstract signal void updated ();
		public abstract signal void remove ();
		public abstract signal void removed ();
	}

	public abstract class Timestamp : GLib.Object {

	}

	public abstract class Metadata : GLib.Object, Storable {

		/* proprties */
		public abstract string parent { get; construct; }	
		public abstract uint action { get; } /* is it needed ? */
		public abstract Timestamp created { get; }
		public abstract Timestamp revised { get; }
	}

	public abstract class SchemaObject : GLib.Object, Storable {

		/* properties */
		public string guid { 
			get { return ""; }
		}

		public uint id { 
			get { return 0; }
		}

		public Metadata metadata { 
			get {  return null; }	
		}

		/* methods */
		public abstract void set_property_value (string name, GLib.Value value);
		public abstract GLib.Value get_property_value (string name);
		public abstract string[]? list_all_properties ();
	}

	public abstract class DBObject : GLib.Object, Storable {

	}
}
