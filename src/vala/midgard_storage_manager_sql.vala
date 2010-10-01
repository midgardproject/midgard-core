
using GLib;

namespace MidgardCR {

	public class SQLStorageManager : GLib.Object, StorageManager {

		/* private properties */
		private string _name = "";
		private Config _config = null;
		private StorageContentManager _content_manager = null;
		private Profiler _profiler = null;
		private Transaction _transaction = null;
		private StorageWorkspaceManager _workspace_manager = null;
		private SQLStorageModelManager _model_manager = null;

		/* internal properties */
		internal GLib.Object _cnc = null; 
		internal GLib.Object _parser = null;
		internal SchemaModel[] _schema_models = null;
		internal StorageModel[] _storage_models = null;

		/* public properties */
		public string name { 
			get { return this._name; }
			construct { this._name = value; }
		}
		
		public Config config { 
			get { return this._config; }
			construct { this._config = value; }
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

		public unowned StorageModelManager model_manager {
			get { 
				if (this._model_manager == null) {
					this._model_manager = new MidgardCR.SQLStorageModelManager ();	
					this._model_manager._storage_manager = this;
					this._model_manager._schema_models = this._schema_models;
					this._model_manager._storage_models = this._storage_models;
				}
				return (MidgardCR.StorageModelManager)this._model_manager; 
			}
		}	

		/**
		 * Constructor
		 *
		 * @param name Unique StorageManager name
		 * @param config MidgardCR.Config to associate with manager
                 * 
		 */ 
		public SQLStorageManager (string name, Config config) throws StorageManagerError {
			if (name == "" || name == null)
				throw new MidgardCR.StorageManagerError.NAME_INVALID ("Can not initialize SQLStorageManager with empty name");

			Object (name: name, config: config);
		}
		
		/**
		 * Opens connection to database defined in MidgardCR.Config associated with StorageManager.
		 * 
		 * If, StorageManager opened connection already, silently returns true.
		 */	
		public bool open () throws MidgardCR.StorageManagerError { 
			if (this._cnc != null)
				return true;

			if (MidgardCRCore.SQLStorageManager.open (this)) {
				this.opened (); /* emit 'opened' signal */
				return true;
			}
			
			return false;
		}

		public bool close () throws MidgardCR.StorageManagerError { 
			if (!MidgardCRCore.SQLStorageManager.close (this))
				return false;
				
			this.closed (); /* emit 'closed' signal */
			return true;
		}

		public bool initialize_storage () throws MidgardCR.StorageManagerError {
			if (!MidgardCRCore.SQLStorageManager.initialize_storage (this))
				return false;

			return true;
		}

		public StorageManager fork () { return null; }
		public StorageManager clone () { return null; }
	}
}
