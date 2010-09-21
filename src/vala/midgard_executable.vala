
using GLib;

namespace MidgardCR {

	public interface Executable : GLib.Object {

		public abstract bool execute ();
	}
}
