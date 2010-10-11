
using GLib;

namespace MidgardCR {

	public interface Profiler : GLib.Object {

		/* properties */
		public abstract double time { get; }
		public abstract string command { get; }

		public abstract void enable (bool toggle);
		public abstract void start ();
		public abstract void end ();
	}
}
