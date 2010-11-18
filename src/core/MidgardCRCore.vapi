
[CCode (lower_case_cprefix = "midgard_cr_core_", cheader_filename = "midgard_cr_core.h")]

namespace MidgardCRCore {
	[Compact]
	public class Config {
		public static bool read_file (MidgardCR.Config config, string name, bool user) throws GLib.KeyFileError, GLib.FileError;
		public static bool read_file_at_path (MidgardCR.Config config, string path) throws GLib.KeyFileError, GLib.FileError;
		public static bool read_data (MidgardCR.Config config, string data) throws GLib.KeyFileError;
		public static string[]? list_files (bool user) throws GLib.KeyFileError;
		public static bool save_file (MidgardCR.Config config, string name, bool user) throws GLib.KeyFileError, GLib.FileError;
		public static bool save_file_at_path (MidgardCR.Config config, string path) throws GLib.KeyFileError, GLib.FileError;
		public static bool create_blobdir (MidgardCR.Config config) throws GLib.KeyFileError, GLib.FileError;
		public static string get_default_confdir ();
		public static void set_dbtype_id (MidgardCR.Config config, string dbtype);
	}

	public class StorageSQL {
		public static string create_query_insert_columns (GLib.Object object, MidgardCR.ObjectModel schema, MidgardCR.StorageModel model);
		public static string create_query_insert_values (GLib.Object object, MidgardCR.ObjectModel schema, MidgardCR.StorageModel model);
		public static string create_query_insert (GLib.Object object, MidgardCR.ObjectModel schema, MidgardCR.StorageModel model);
	}
	
	public class SQLStorageManager {
		public static bool open (MidgardCR.SQLStorageManager storage_mgr) throws MidgardCR.StorageManagerError;
		public static bool close (MidgardCR.SQLStorageManager storage_mgr) throws MidgardCR.StorageManagerError;
		public static bool initialize_storage (MidgardCR.SQLStorageManager storage_mgr) throws MidgardCR.StorageManagerError;
		public static bool table_exists (MidgardCR.SQLStorageManager manager, MidgardCR.SQLTableModel storage_model);
		public static void table_create (MidgardCR.SQLStorageManager manager, MidgardCR.SQLTableModel storage_model) throws MidgardCR.ExecutableError;
		public static void table_remove (MidgardCR.SQLStorageManager manager, MidgardCR.SQLTableModel storage_model) throws MidgardCR.ExecutableError;
		public static bool column_exists (MidgardCR.SQLStorageManager manager, MidgardCR.SQLColumnModel model);
		public static bool column_create (MidgardCR.SQLStorageManager manager, MidgardCR.SQLColumnModel model) throws MidgardCR.ExecutableError;
		public static bool column_update (MidgardCR.SQLStorageManager manager, MidgardCR.SQLColumnModel model) throws MidgardCR.ExecutableError;
		public static bool column_remove (MidgardCR.SQLStorageManager manager, MidgardCR.SQLColumnModel model) throws MidgardCR.ExecutableError;
		public static int query_execute (MidgardCR.SQLStorageManager manager, string query) throws MidgardCR.ExecutableError;
		public static void load_models (MidgardCR.SQLStorageManager storage_mgr) throws MidgardCR.StorageManagerError;
}

	public class SQLStorageModelManager {
		public static void prepare_create (MidgardCR.SQLStorageModelManager manager, MidgardCR.Model model) throws MidgardCR.ValidationError;
		public static void execute (MidgardCR.SQLStorageModelManager manager) throws MidgardCR.ExecutableError;
	}

	public class SQLStorageContentManager {
		public static void storable_insert (MidgardCR.Storable storable, MidgardCR.SQLStorageManager manager, MidgardCR.ObjectModel object_model, MidgardCR.SQLTableModel table_model) throws MidgardCR.StorageContentManagerError;
	}

	public class ObjectBuilder {
		public static void register_types (MidgardCR.ObjectBuilder builder) throws GLib.Error;
	}

	/* Query classes */
	public class QueryStorage : GLib.Object {
		[CCode (has_construct_function = false)]
		public QueryStorage (string classname); 
	}

	public interface QueryHolder : GLib.Object {
		public void get_value (GLib.Value value);
		public bool set_value (GLib.Value value);
	}

	public class QueryProperty : GLib.Object, MidgardCRCore.QueryHolder {
		[CCode (has_construct_function = false)]
		public QueryProperty (string property, MidgardCRCore.QueryStorage? storage);
		[NoAccessorMethod]
		public string property { owned get; set; }
		[NoAccessorMethod]
		public MidgardCRCore.QueryStorage storage { owned get; set; }
	}

	public class QueryValue : GLib.Object, MidgardCRCore.QueryHolder {
		[CCode (has_construct_function = false)]
		public QueryValue ();
		public static unowned MidgardCRCore.QueryValue create_with_value (GLib.Value value);
		//[NoWrapper]
		//public virtual void get_value (GLib.Value value);
		//[NoWrapper]
		//public virtual unowned MidgardCRCore.QueryValue set_value (GLib.Value value);
	}

	public interface QueryConstraintSimple : GLib.Object {
		public unowned QueryConstraintSimple[]? list_constraints ();
	}

	public class QueryConstraint : GLib.Object, MidgardCRCore.QueryConstraintSimple {
		[CCode (has_construct_function = false)]
		public QueryConstraint (MidgardCRCore.QueryProperty property, string op, MidgardCRCore.QueryHolder holder, MidgardCRCore.QueryStorage? storage);
	}

	public class QueryExecutor: GLib.Object {
		public virtual bool execute() throws MidgardCR.ExecutableError;
		public virtual bool set_constraint (MidgardCRCore.QueryConstraintSimple constraint);
	}

	public class QuerySelect : MidgardCRCore.QueryExecutor {
		public static MidgardCRCore.QuerySelect create_static (MidgardCR.StorageManager manager, MidgardCRCore.QueryStorage storage);
		public MidgardCR.Storable[]? list_objects ();
	}
}
