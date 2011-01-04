/*
 In this example:
 - Create configuration and open connection using StorageManager
 - Create ObjectModel models for Activity and Person classes
 - Create TableModel and ColumnModel models for classes
 - Create tables and columns for classes in SQLite database
 - Store information about classes and their tables in database
*/ 

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
	config.dbdir = "./";

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
		.add_model (new ObjectPropertyModel ("firstname", "string", ""))
		.add_model (new ObjectPropertyModel ("lastname", "string", ""));

	/* Create new SQL TableModel which defines 'Person' class table and all required columns */
	/* Every 'Person' object's data will be stored in 'person' table */
	SQLTableModel person_sm = new SQLTableModel (storage_manager, "Person", "person");
	/* Add two columns to 'person' table: 'firstname', 'lastname' */
	person_sm
		.add_model (new SQLColumnModel (storage_manager, "firstname", "firstname", "string"))
		.add_model (new SQLColumnModel (storage_manager, "lastname", "lastname", "string"));

	ObjectModel actor_ref_model = new ObjectModelReference ("ReferenceObject");
	actor_ref_model
		.add_model (new ObjectPropertyInt ("id", "0"))
		.add_model (new ObjectPropertyString ("guid", ""));

	/* Create reference to 'Person' object for 'actor' property which is of 'ReferenceObject' type. 
	 * In other words, 'actor' property holds 'ReferenceObject' instance, which holds identifiers of
	 * 'Person' object associated with 'Activity' one */
	var am_actor = new ObjectPropertyReference ("actor", person_model, (MidgardCR.ObjectModelReference) actor_ref_model); 

	/* Define 'Activity' class */
	var activity_model = new ObjectModel ("Activity");
	/* Define properties: 'verb', 'target', 'summary', 'application' */
	activity_model
		.add_model (am_actor)
		.add_model (new ObjectPropertyModel ("verb", "string", ""))
		.add_model (new ObjectPropertyModel ("target", "guid", ""))
		.add_model (new ObjectPropertyModel ("summary", "guid", ""))
		.add_model (new ObjectPropertyModel ("application", "string", ""));

	/* Create new SQL StorageModel which defines 'Activity' class table and all required columns */
	/* Add columns to table: 'verb', 'application', 'target', 'summary' and those required by 'actor' */
	var asm_verb = new SQLColumnModel (storage_manager, "verb", "verb", "string");
	asm_verb.index = true;

	var asm_application = new SQLColumnModel (storage_manager, "application", "application", "string");
	asm_application.index = true;

	var actor_model = new SQLColumnModel (storage_manager, "actor", "actor", "object");
	actor_model
		.add_model (new SQLColumnModel (storage_manager, "id", "actor_id", "int"))
		.add_model (new SQLColumnModel (storage_manager, "guid", "actor_guid", "guid"))
		.add_model (new SQLColumnModel (storage_manager, "classname", "actor_classname", "string"));

	/* Activity class requires 'midgard_activity' table */
	var activity_sm = new SQLTableModel (storage_manager, "Activity", "midgard_activity") as SQLTableModel;
	activity_sm
		.add_model (asm_verb)
		.add_model (asm_application)
		.add_model (actor_model)
		.add_model (new SQLColumnModel (storage_manager, "target", "target", "string"))
		.add_model (new SQLColumnModel (storage_manager, "summary", "summary", "string"));

	/* Add Object and Storage models to StorageModelManager */
	model_manager
		.add_model (person_model)
		.add_model (person_sm)
		.add_model (activity_model)
		.add_model (activity_sm);

	/* Connect profiler callbacks to all ModelManager signal emissions */	
	SQLProfiler profiler = (SQLProfiler) storage_manager.profiler;
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

