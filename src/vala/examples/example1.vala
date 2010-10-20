using MidgardCR;

void profiler_callback_start (SQLProfiler profiler)
{
	profiler.start ();
}

void profiler_callback_end (SQLProfiler profiler)
{
	profiler.end ();
	GLib.print ("SQL QUERY: %s (%.04f) \n", profiler.command, profiler.time);
}

void main () {

	SQLStorageManager storage_manager = null;

	/* Create new Config which sets SQL database type and its name */
	Config config = new Config ();	
	config.dbtype = "SQLite";
	config.dbname = "ExampleDB";

	/* Create new, named StorageManager for given Config instance */
	try {
		storage_manager = new SQLStorageManager ("test_manager", config);
	} catch (StorageManagerError e) {
		GLib.warning ("Failed to initialize new SQLStorageManager");
	}
	
	/* Open connection to underlying SQL database and "bootstrap" storage */
	try {
		storage_manager.open ();
		storage_manager.initialize_storage ();
        } catch (StorageManagerError e) {
		GLib.warning (e.message);
        }
        
	/* Get ModelManager which is responsible to create all data required 
	 * by classes and their SQL tables */
	StorageModelManager model_manager = storage_manager.model_manager;

	/* Define Person class */
	ObjectModel person_model = new ObjectModel ("Person");	
	/* Define properties: 'firstname' and 'lastname' */
	person_model
		.add_model (new ObjectModelProperty ("firstname", "string", ""))
		.add_model (new ObjectModelProperty ("lastname", "string", ""));

	/* Create new SQL StorageModel which defines 'Person' class table and all required columns */
	/* Every 'Person' object's data will be stored in 'person' table */
	SQLTableModel person_sm = model_manager.create_storage_model (person_model, "person") as SQLTableModel;
	/* Add two columns to 'person' table: 'firstname', 'lastname' */
	person_sm
		.add_model (person_sm.create_model_property ("firstname", "firstname", "string"))
		.add_model (person_sm.create_model_property ("lastname", "lastname", "string"));

	/* Define 'Activity' class */
	var activity_model = new ObjectModel ("Activity");
	/* 'actor' property is object type, so we 'link' property with 'Person' object */
      	var am_actor = new ObjectModelProperty ("actor", "object", "");
	am_actor.add_model (person_model);
	/* Define properties: 'verb', 'target', 'summary', 'application' */
	activity_model
		.add_model (am_actor)
		.add_model (new ObjectModelProperty ("verb", "string", ""))
		.add_model (new ObjectModelProperty ("target", "guid", ""))
		.add_model (new ObjectModelProperty ("summary", "guid", ""))
		.add_model (new ObjectModelProperty ("application", "string", ""));
	
	/* Create new SQL StorageModel which defines 'Activity' class table and all required columns */
	/* Activity class requires 'midgard_activity' table */
	var activity_sm = model_manager.create_storage_model (activity_model, "midgard_activity") as SQLTableModel;
	/* Add columns to table: 'verb', 'application', 'target', 'summary' and those required by 'actor' */
	var asm_verb = activity_sm.create_model_property ("verb", "verb", "string");
	asm_verb.index = true;
	var asm_application = activity_sm.create_model_property ("application", "application", "string");
	asm_application.index = true;
	var actor_model = activity_sm.create_model_property ("actor", "actor", "object");
	actor_model
		.add_model (activity_sm.create_model_property ("id", "actor_id", "int"))
		.add_model (activity_sm.create_model_property ("guid", "actor_guid", "guid"));
	activity_sm
		.add_model (asm_verb)
		.add_model (asm_application)
		.add_model (actor_model)
		.add_model (activity_sm.create_model_property ("target", "target", "string"))
		.add_model (activity_sm.create_model_property ("summary", "summary", "string"));

	/* Add Object and Storage models to StorageModelManager */
	model_manager
		.add_model (person_model)
		.add_model (person_sm)
		.add_model (activity_model)
		.add_model (activity_sm);

	/* Connect profiler callbacks to all ModelManager signal emissions */	
	SQLProfiler profiler = (SQLProfiler) model_manager.storagemanager.profiler;
	profiler.enable (true);
	model_manager.execution_start.connect (() => {
		profiler_callback_start(profiler);
	}); 
	model_manager.execution_end.connect (() => {
		profiler_callback_end(profiler);
	}); 

	/* Prepare to create table 'person' and 'midgard_activity'.
	   Also store Models data in related tables to use such as metadata */
	try {
		model_manager.prepare_create ();
	} catch (ValidationError e) {
		GLib.error (e.message);
	}

	/* Execute all queries prepared in prepare_create() method */
	try {
		model_manager.execute ();
	} catch (ExecutableError e) {
		GLib.error (e.message);
	}	
}

