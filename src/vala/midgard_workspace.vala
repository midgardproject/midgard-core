/* 
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
 * 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
 */

using GLib;

namespace MidgardCR {

	public errordomain WorkspaceStorageError {
	        WORKSPACE_STORAGE_ERROR_NAME_EXISTS,
        	WORKSPACE_STORAGE_ERROR_INVALID_PATH,
        	WORKSPACE_STORAGE_ERROR_OBJECT_NOT_EXISTS,
       		WORKSPACE_STORAGE_ERROR_CONTEXT_VIOLATION
	}

	public interface WorkspaceStorage : GLib.Object {
	
		public abstract string path { get; construct; }
	}

	public class WorkspaceContext : GLib.Object, WorkspaceStorage {

		private string _path;

		public string path {
			get { return this._path; }
			construct { this._path = value; }
		} 

		public string[]? get_workspace_names () { return null; }
		public Workspace? get_workspace_by_name () { return null; } 
		public bool has_workspace (Workspace workspace) { return false; }
	}

	public class Workspace : GLib.Object, WorkspaceStorage {

		private WorkspaceContext _ctx;
		private string _path;
		private Workspace _parent;

		public string path {
			get { return this._path; }
			construct { this._path = value; }
		} 
		public Workspace parent { 
				get { return this._parent; }
				set { this._parent = value; }
		}
		public WorkspaceContext context { 
			get { return this._ctx; }
		}

		public Workspace[]? list_children () { return null; }
		public Workspace? get_by_path () { return null; }
	}

	public class SQLWorkspaceManager : StorageWorkspaceManager, SQLStorageManager {

		public WorkspaceStorage workspace { get; set; }
		public bool workspace_create (WorkspaceStorage workspace) throws WorkspaceStorageError { return false; }
		public bool workspace_exists (WorkspaceStorage workspace) throws WorkspaceStorageError { return false; }

		public SQLWorkspaceManager (string name, Config config) {
			Object (name: name, config: config);
		}

		construct {}	
	}
}
