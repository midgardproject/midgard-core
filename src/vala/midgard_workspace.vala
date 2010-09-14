
using GLib;

namespace Midgard {

	errordomain WorkspaceStorageError {
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
	}
}
