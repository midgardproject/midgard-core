
using GLib;

namespace Midgard {

	public class SQLStorageManager : GLib.Object, StorageManager {

		private string _name = "";
		private Config _config = null;
		private StorageContentManager _content_manager = null;
		private Profiler _profiler = null;
		private Transaction _transaction = null;
		private StorageWorkspaceManager _workspace_manager = null;
		private StorageModelManager _model_manager = null;

		public string name {
			get { return this._name; }
			construct { this._name = value; }
		}
		
		public Config config {
			get { return _config; }
			construct { _config = value; }
		}	

		public StorageContentManager content_manager {
			get { 
				/*if (this._content_manager == null)
					this._content_manager = StorageContentManager (); */
				return this._content_manager; 
			} 
		}

		public Profiler profiler {
			get { return _profiler; }
		}
			
		public Transaction transaction {
			get { return _transaction; }
		}
		
		public StorageWorkspaceManager workspace_manager {
			get { return _workspace_manager; }
		}

		public StorageModelManager model_manager {
			get { return _model_manager; }
		}

		public bool open () { return false; }
		public bool close () { return false; }

		public StorageManager fork () { return null; }
		public StorageManager clone () { return null; }
	}
}
