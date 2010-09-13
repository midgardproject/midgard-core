
[CCode (lower_case_cprefix = "midgard_core_", cheader_filename = "midgard_core.h")]

namespace MidgardCore {
	[Compact]
	public class Config {
		public static bool read_file (Midgard.Config config, string name, bool user) throws GLib.KeyFileError, GLib.FileError;
		public static bool read_file_at_path (Midgard.Config config, string path) throws GLib.KeyFileError, GLib.FileError;
		public static bool read_data (Midgard.Config config, string data) throws GLib.KeyFileError;
		public static string[]? list_files (bool user) throws GLib.KeyFileError;
		public static bool save_file (Midgard.Config config, string name, bool user) throws GLib.KeyFileError, GLib.FileError;
		public static bool save_file_at_path (Midgard.Config config, string path) throws GLib.KeyFileError, GLib.FileError;
		public static bool create_blobdir (Midgard.Config config) throws GLib.KeyFileError, GLib.FileError;
	}
}
