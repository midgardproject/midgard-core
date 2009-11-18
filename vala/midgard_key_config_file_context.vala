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

	public class KeyConfigFileContext : KeyConfigContext {
	
		public KeyConfigFileContext (string context_path) throws KeyConfigContextError {
			if (!GLib.Path.is_absolute (context_path)) {
				throw new KeyConfigContextError.PATH_IS_NOT_ABSOLUTE
						("Expected absolute path. %s given", context_path);
			}	

			base(context_path);
		}

		private string[]? set_cfgs (string path, int i) {
			GLib.Dir dir = null;

			/* TODO */
			/* is_absolute */
	
			try {
				dir = GLib.Dir.open(path);
			}
			catch ( GLib.FileError e ) {
			
				GLib.warning ("Can not open context path %s. %s", path, e.message);
			}	

			string name = null;
			string[] cfgs = new string[0];

			while ((name = dir.read_name()) != null) {

				string abspath = GLib.Path.build_filename (path, name);

				/* FIXME, it doesn't add all files */
				if (GLib.FileUtils.test (abspath, GLib.FileTest.IS_DIR)) {

					var children = set_cfgs (abspath, i);
					foreach (weak string child in children) 
						cfgs += child;

				} else {
					/* Add only midgard configuration file *.mcfg */
					if (abspath.has_suffix (Midgard.CONFIG_EXTENSION)) {
						var path_wo_suffix = abspath.substring(0, abspath.len()-Midgard.CONFIG_EXTENSION.len());
						try {
							var regex = new GLib.Regex(GLib.Regex.escape_string(context_path));
							cfgs += regex.replace_literal (path_wo_suffix, -1, 0, "");
						} catch {
							GLib.warning("Weird path '%s', not added.", abspath);
						}
						i++;
					}
				}
			}
		
			return cfgs;
		}

		/**
		 * midgard_key_config_file_context_list_key_config:
		 *
		 * Returns:
		 */
		public override string[]? list_key_config() {
			return this.set_cfgs (context_path, 0);
		}
	}
}
