
using GLib;

namespace Midgard {

	public interface Profiler : GLib.Object {

		public abstract void enable (bool toggle);
		public abstract void start ();
		public abstract void end ();
		public abstract int get_time ();
		public abstract string get_execution_command ();
	}
}
