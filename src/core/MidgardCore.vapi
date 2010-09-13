
[CCode (lower_case_cprefix = "midgard_core_", cheader_filename = "midgard_core.h")]

namespace MidgardCore {
	[Compact]
	public class Config {
		public static bool read_file (Midgard.Config config, string name, bool user) throws GLib.KeyFileError, GLib.FileError;
	}
}
