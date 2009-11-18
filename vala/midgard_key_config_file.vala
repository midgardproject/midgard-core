/* 
 * Copyright (C) 2009 Piotr Pokora <piotrek.pokora@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

using GLib;

namespace Midgard {
 const string CONFIG_EXTENSION = ".mcfg";

	public class KeyConfigFile : KeyConfig {

		private GLib.KeyFile key_file = null;
		private string file_path = null;
		private bool file_exists = false;

		public KeyConfigFile (Midgard.KeyConfigFileContext keyctx, string path) throws KeyConfigContextError {
			base(keyctx, path);
			if (path.has_suffix (Midgard.CONFIG_EXTENSION)) {
		           file_path = GLib.Path.build_filename (keyctx.context_path, path);
			} else {
				file_path = GLib.Path.build_filename (keyctx.context_path, path, Midgard.CONFIG_EXTENSION);
			}
	
			if (!GLib.Path.is_absolute (file_path)) {
				throw new KeyConfigContextError.PATH_IS_NOT_ABSOLUTE
				            ("Absolute paths are not accepted while %s is passed.", file_path);
			}

			key_file = new GLib.KeyFile();

			if (GLib.FileUtils.test (file_path, GLib.FileTest.IS_REGULAR)) {
				file_exists = true;
				try {
					key_file.load_from_file (file_path, GLib.KeyFileFlags.KEEP_TRANSLATIONS);
				} catch (GLib.Error e) {
					file_exists = false;
				}
			}
		}

		public override void set_value (string group, string key, string value) {
			key_file.set_string (group, key, value);
		}	

		public override string? get_value (string group, string key) {
			try {
				if (key_file.has_key (group, key)) {
					return key_file.get_string (group, key);
				}
			} catch (GLib.KeyFileError e) {
				return null;
			}
			return null;
		}

		public override void set_comment (string group, string key, string comment) {
			try {
				key_file.set_comment (group, key, comment);
			} catch (GLib.Error e) {
				GLib.warning("Failed to set comment string for key %s, group %s. Error is %s", key, group, e.message);
			}
		}
	
		public override string? get_comment (string group, string key) {
			try {
				return key_file.get_comment (group, key);
			} catch  {
				return null;
			}
		}

		public override string[]? list_groups () {
			return key_file.get_groups ();
		}	
		
		public override bool group_exists (string name) {
			return key_file.has_group (name);
		}

		public override bool delete_group (string name) {
			try {
				key_file.remove_group (name);
				return true;
			}	catch (GLib.Error e) {
				GLib.warning ("Failed to delete group %s. Error is %s", name, e.message);
			} 
			return false;
		}

		public override bool store () {
			/* Create directories if do not exist yet */
			if (!file_exists) {
				string basedir = GLib.Path.get_dirname (file_path);
				if (!GLib.FileUtils.test (basedir, GLib.FileTest.IS_DIR)) {
					int created = GLib.DirUtils.create_with_parents (basedir, 0750);
					if (created == -1)	return false;
				}
			}
			try {
				return GLib.FileUtils.set_contents (file_path, key_file.to_data());
			} catch {
				return false;
			}
		}

		public override bool load_from_data (string data) {
			if (file_exists) return false;
			try {
				return key_file.load_from_data (data, data.length, GLib.KeyFileFlags.KEEP_TRANSLATIONS);
			} catch {
				return false;
			}
		}

		public override string? to_data () {
			if (!file_exists) return null;
			return key_file.to_data ();
		}
	}
}
