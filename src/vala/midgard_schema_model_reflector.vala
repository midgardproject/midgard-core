
using GLib;

namespace Midgard {

	public interface ModelReflector : GLib.Object {
	
		public abstract Model model { get; construct; } 

	}
	
	public interface ModelPropertyReflector : ModelReflector {

		/* methods */
		public abstract string get_typename ();
		public abstract GLib.Type get_gtype ();
		public abstract GLib.Value get_default_value ();
		public abstract bool is_private ();
		public abstract string? get_description ();
	}
}
