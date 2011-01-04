/*
 In this example:
 - Create configuration and open connection to database.
 - Register all classes for which, information in database exists.
 - Create new RDFGenericObject for RDF Triple 
 - Create database record for the object
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

	/* Initialize ObjectBuilder which is responsible to register classes from models */
	var builder = new ObjectBuilder ();	

	/* Get all available ObjectModel models from model manager */
        ObjectModel[] object_models = (ObjectModel[]) storage_manager.model_manager.list_models_by_type ("ObjectModel");
	
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

	var content_manager = new RDFSQLContentManager (storage_manager);

	/*
	<http://www.midgard-project.org/people/vali>
		foaf:name "Vali";
    		foaf:homepage "http://www.midgard-project.org/people/vali".
	*/

	var rdf_vali = builder.factory ("RDFGenericObject") as RepositoryObject;
	rdf_vali.set ("classname", "http://xmlns.com/foaf/0.1/person");
	rdf_vali.set ("identifier","http://www.midgard-project.org/people/vali");
	rdf_vali.set_property_value ("foaf:name", "Vali");
	rdf_vali.set_property_value ("foaf:homepage", "http://www.midgard-project.org/people/vali");	
	
	GLib.print ("Vali generic RDF object identified by '%s' GUID \n", rdf_vali.guid);	
	
	content_manager.create (rdf_vali);
}
