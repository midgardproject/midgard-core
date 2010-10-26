/*
 In this example:
 - Create configuration and open connection to database.
 - Register all classes for which, information in database exists.
 - Create new 'Activity' instance
 - Create database record for an object
*/

using MidgardCR;

void main () {
	
	SQLStorageManager storage_manager = null;

        /* Create new Config which sets SQL database type and its name */
        Config config = new Config ();
        config.dbtype = "SQLite";
        config.dbname = "ExampleDB";
        config.dbdir = "./";

        /* Create new, named StorageManager for given Config instance */
        try {
                storage_manager = new SQLStorageManager ("test_manager", config);
        } catch (StorageManagerError e) {
                GLib.warning ("Failed to initialize new SQLStorageManager");
        }

        /* Open connection to underlying SQL database */
        try {
                storage_manager.open ();
        } catch (StorageManagerError e) {
                GLib.warning (e.message);
        }

	/* Initialize ObjectBuilder which is responsible to register classes from models */
	var builder = new ObjectBuilder ();	

	/* Get all available ObjectModel models from model manager */
        ObjectModel[] object_models = storage_manager.model_manager.list_object_models ();
	
	/* Register models in builder and validate models */
	try {
		foreach (ObjectModel model in object_models) 
			builder.register_model (model);
	} catch (ValidationError e) {
		GLib.error (e.message);
	}

	/* Finally, build classes */
	try {
		builder.execute ();
	} catch (ExecutableError e) {
		GLib.error (e.message);
	}

	var content_manager = new SQLStorageContentManager (storage_manager);

	Storable person = builder.factory ("Activity");
	person.set (
		"verb", "http://activitystrea.ms/schema/1.0/post", 
		"application", "MidgardCR",
		"summary", "Initialy created");

	content_manager.create (person);
}
