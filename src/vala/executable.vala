
using GLib;

namespace MidgardCR {

	public interface Executable : GLib.Object {

		public abstract void execute () throws GLib.Error;
	}
}
