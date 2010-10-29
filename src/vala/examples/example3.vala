/*
 In this example:
 - Create configuration and open connection to database.
 - Register 'Movie' class without storage
 - Register all classes for which, information in database exists.
 - Create new 'Movie' instance and set namespaced properties
 - Create database record for movie
*/

using MidgardCR;

void main () {
	
	RDFSQLStorageManager storage_manager = null;

        /* Create new Config which sets SQL database type and its name */
        Config config = new Config ();
        config.dbtype = "SQLite";
        config.dbname = "ExampleDB";
        config.dbdir = "./";

        /* Create new, named RDFSQLStorageManager for given Config instance */
        try {
                storage_manager = new RDFSQLStorageManager ("rdf_test_manager", config);
        } catch (StorageManagerError e) {
                GLib.warning ("Failed to initialize new SQLStorageManager");
        }

        /* Open connection to underlying SQL database */
        try {
                storage_manager.open ();
        } catch (StorageManagerError e) {
                GLib.warning (e.message);
        }

	try {
		storage_manager.initialize_storage ();
	} catch (StorageManagerError e) {
		GLib.warning (e.message);
	}

	/* Create model for 'Movie' object */
	var movie_model = new ObjectModel ("Movie");
	var director_model = new ObjectModelProperty ("director", "string", "");
	director_model.namespace = "dc";
	var title_model = new ObjectModelProperty ("title", "string", "");
	title_model.namespace = "foaf";
	var price_model = new ObjectModelProperty ("price", "string", "");
	price_model.namespace = "ns";
	movie_model.add_model (director_model).add_model (title_model).add_model (price_model);

	storage_manager.model_manager.add_model (movie_model);

	try {
		storage_manager.model_manager.prepare_create ();
	} catch (ValidationError e) {
		GLib.warning ("Failed to prepare create operation. %s", e.message);
	}

	try {
		storage_manager.model_manager.execute ();
	} catch (ExecutableError e) {
		GLib.warning ("Execution failed. %s", e.message);
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

	var movie = builder.factory ("Movie") as RepositoryObject;
	GLib.print ("New movie identified by %s \n", movie.guid);
	movie.set_property_value ("foaf:title", "The Book of Eli");
	movie.set_property_value ("dc:director", "Hughes");
	movie.set_property_value ("ns:price", 42);
	
	content_manager.create (movie);
}
