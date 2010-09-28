
using GLib;

namespace MidgardCR {

	public errordomain ExecutableError {
		DEPENDENCE_INVALID,
		COMMAND_INVALID,
		COMMAND_INVALID_DATA,
		INTERNAL
	}

	public interface Executable : GLib.Object {

		public abstract void execute () throws ExecutableError;
	}
}
