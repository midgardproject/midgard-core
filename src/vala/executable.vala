
using GLib;

namespace MidgardCR {

	public errordomain ExecutableError {
		INVALID_DEPENDENCE,
		INTERNAL
	}

	public interface Executable : GLib.Object {

		public abstract void execute () throws ExecutableError;
	}
}
