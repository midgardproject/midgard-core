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

void midgardcr_test_add_sql_storage_model_manager_tests () {

	/* constructor */
	Test.add_func ("/SQLStorageModelManager/constructor", () => {
		MidgardCR.SQLStorageModelManager mm = new MidgardCR.SQLStorageModelManager ();
		assert (mm != null);	
	
		MidgardCR.SQLStorageManager storage_manager = null;
		MidgardCR.Config config = new MidgardCR.Config ();	

		try {
			config.read_configuration (DEFAULT_CONFIGURATION, true);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}

		/* SUCCESS */		
		try {
			storage_manager = new MidgardCR.SQLStorageManager ("test_manager", config);
		} catch (StorageManagerError e) {
			GLib.warning ("Failed to initialize new SQLStorageManager");
		}
		assert (storage_manager != null);
		
		MidgardCR.StorageModelManager model_manager = storage_manager.model_manager;
		assert (model_manager != null);
		assert (model_manager is MidgardCR.SQLStorageModelManager);
	});

	Test.add_func ("/SQLStorageModelManager/prepare_create", () => {	
		MidgardCR.SQLStorageManager storage_manager = null;
		MidgardCR.Config config = new MidgardCR.Config ();	

		try {
			config.read_configuration (DEFAULT_CONFIGURATION, true);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}

		/* SUCCESS */		
		try {
			storage_manager = new MidgardCR.SQLStorageManager ("test_manager", config);
		} catch (StorageManagerError e) {
			GLib.warning ("Failed to initialize new SQLStorageManager");
		}
		assert (storage_manager != null);
	
		bool opened = false;
                /* SUCCESS */
                try {
                        opened = storage_manager.open ();
                } catch (StorageManagerError e) {
                        GLib.warning (e.message);
                }
                assert (opened == true);
	
		MidgardCR.StorageModelManager model_manager = storage_manager.model_manager;
		assert (model_manager != null);
		assert (model_manager is MidgardCR.SQLStorageModelManager);

		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
                assert (model != null);
                assert (model.name == DEFAULT_CLASSNAME);

                /* SUCCESS */
                MidgardCR.ObjectModelProperty prop_model = new MidgardCR.ObjectModelProperty (TITLE_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model);

		/* Add Schema model to StorageModelManager */
		model_manager.add_model (model);
	
		/* SUCCESS */
		try {
			model_manager.prepare_create ();
		} catch (ValidationError e) {
			GLib.warning (e.message);
		}	
	});

	Test.add_func ("/SQLStorageModelManager/execute", () => {	
		MidgardCR.SQLStorageManager storage_manager = null;
		MidgardCR.Config config = new MidgardCR.Config ();	

		try {
			config.read_configuration (DEFAULT_CONFIGURATION, true);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}

		/* SUCCESS */		
		try {
			storage_manager = new MidgardCR.SQLStorageManager ("test_manager", config);
		} catch (StorageManagerError e) {
			GLib.warning ("Failed to initialize new SQLStorageManager");
		}
		assert (storage_manager != null);
	
		bool opened = false;
                /* SUCCESS */
                try {
                        opened = storage_manager.open ();
                } catch (StorageManagerError e) {
                        GLib.warning (e.message);
                }
                assert (opened == true);
	
		MidgardCR.StorageModelManager model_manager = storage_manager.model_manager;
		assert (model_manager != null);
		assert (model_manager is MidgardCR.SQLStorageModelManager);

		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
                assert (model != null);
                assert (model.name == DEFAULT_CLASSNAME);

                /* SUCCESS */
                MidgardCR.ObjectModelProperty prop_model = new MidgardCR.ObjectModelProperty (TITLE_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

		/* SUCCESS */
                MidgardCR.ObjectModelProperty prop_model_a = new MidgardCR.ObjectModelProperty (ABSTRACT_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model).add_model (prop_model_a);

		/* SQLStorageModel */
		/* SUCCESS */
		MidgardCR.SQLStorageModel storage_model = model_manager.create_storage_model (model, DEFAULT_TABLENAME) as MidgardCR.SQLStorageModel;
		assert (storage_model != null);
		MidgardCR.SQLStorageModelProperty storage_model_prop = storage_model.create_model_property (TITLE_PROPERTY_NAME, TITLE_COLUMN, "string");
		assert (storage_model_prop != null);
		MidgardCR.SQLStorageModelProperty storage_model_prop_a = storage_model.create_model_property (ABSTRACT_PROPERTY_NAME, ABSTRACT_COLUMN, "string");
		assert (storage_model_prop_a != null);
		storage_model.add_model (storage_model_prop).add_model (storage_model_prop_a);

		/* Add Schema model to StorageModelManager */
		model_manager.add_model (model).add_model (storage_model);

		SQLProfiler profiler = (SQLProfiler) model_manager.storagemanager.profiler;
		profiler.enable (true);
		model_manager.execution_start.connect (() => {
			profiler_callback_start(profiler);
		}); 
		model_manager.execution_end.connect (() => {
			profiler_callback_end(profiler);
		}); 

		/* SUCCESS */
		try {
			model_manager.prepare_create ();
		} catch (ValidationError e) {
			GLib.warning (e.message);
		}

		/* SUCCESS */
		try {
			model_manager.execute ();
		} catch (ExecutableError e) {
			GLib.warning (e.message);
		}	
	});

	Test.add_func ("/SQLStorageModelManager/execute_and_validate", () => {	
		MidgardCR.SQLStorageManager storage_manager = null;
		var config = new MidgardCR.Config ();	

		try {
			config.read_configuration (DEFAULT_CONFIGURATION, true);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}

		/* SUCCESS */		
		try {
			storage_manager = new MidgardCR.SQLStorageManager ("test_manager", config);
		} catch (StorageManagerError e) {
			GLib.warning ("Failed to initialize new SQLStorageManager");
		}
		assert (storage_manager != null);
	
		bool opened = false;
                /* SUCCESS */
                try {
                        opened = storage_manager.open ();
                } catch (StorageManagerError e) {
                        GLib.warning (e.message);
                }
                assert (opened == true);
	
		var model_manager = storage_manager.model_manager;
		assert (model_manager != null);
		assert (model_manager is MidgardCR.SQLStorageModelManager);

		 /* Define Person class */
		/* SUCCESS */
	        var person_model = new ObjectModel ("Person");
		assert (person_model != null);
        	/* Define properties: 'firstname' and 'lastname' */
        	person_model
                	.add_model (new ObjectModelProperty ("firstname", "string", ""))
                	.add_model (new ObjectModelProperty ("lastname", "string", ""));

        	/* Create new SQL StorageModel which defines 'Person' class table and all required columns */
        	/* Every 'Person' object's data will be stored in 'person' table */
		/* SUCCESS */
        	var person_sm = model_manager.create_storage_model (person_model, "person") as SQLStorageModel;
		assert (person_sm != null);
        	/* Add two columns to 'person' table: 'firstname', 'lastname' */
        	person_sm
                	.add_model (person_sm.create_model_property ("firstname", "firstname", "string"))
                	.add_model (person_sm.create_model_property ("lastname", "lastname", "string"));

		/* Define 'Activity' class */
		/* SUCCESS */
		var activity_model = new ObjectModel ("Activity");
		assert (activity_model != null);
		/* 'actor' property is object type, so we 'link' property with 'Person' object */
		/* SUCCESS */
      		var am_actor = new ObjectModelProperty ("actor", "object", "");
		assert (am_actor != null);
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
		/* SUCCESS */
		var activity_sm = model_manager.create_storage_model (activity_model, "midgard_activity") as SQLStorageModel;
		assert (activity_sm != null);
		/* Add columns to table: 'verb', 'application', 'target', 'summary' and those required by 'actor' */
		var asm_verb = activity_sm.create_model_property ("verb", "verb", "string");
		assert (asm_verb != null);
		asm_verb.index = true;

		var asm_application = activity_sm.create_model_property ("application", "application", "string");
		assert (asm_application != null);
		asm_application.index = true;

		var actor_model = activity_sm.create_model_property ("actor", "actor", "object");
		assert (actor_model != null);
		
		/* Adds additional columns to store info for reference object */
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

		SQLProfiler profiler = (SQLProfiler) model_manager.storagemanager.profiler;
		profiler.enable (true);
		model_manager.execution_start.connect (() => {
			profiler_callback_start(profiler);
		}); 
		model_manager.execution_end.connect (() => {
			profiler_callback_end(profiler);
		}); 

		/* SUCCESS */
		try {
			model_manager.prepare_create ();
		} catch (ValidationError e) {
			GLib.warning (e.message);
		}

		/* SUCCESS */
		try {
			model_manager.execute ();
		} catch (ExecutableError e) {
			GLib.warning (e.message);
		}	

		/* VALIDATE */

		/* SUCCESS */	
		MidgardCR.SQLStorageManager s_mngr = null;	
		try {
			s_mngr = new MidgardCR.SQLStorageManager ("test_manager_the_second", config);
		} catch (StorageManagerError e) {
			GLib.warning ("Failed to initialize new SQLStorageManager");
		}
		assert (s_mngr != null);
	
		opened = false;
                /* SUCCESS */
                try {
                        opened = s_mngr.open ();
                } catch (StorageManagerError e) {
                        GLib.warning (e.message);
                }
                assert (opened == true);

		/* Check loaded models, they must be equal to the ones we added to model manager */

		var model_mngr = s_mngr.model_manager;
                assert (model_mngr != null);
                assert (model_mngr is MidgardCR.SQLStorageModelManager);

		/* SUCCESS */
		unowned ObjectModel[]? object_models = model_mngr.list_object_models ();
		assert (object_models != null);
		assert (object_models.length != 2);
		StorageModel[]? storage_models = model_mngr.list_storage_models ();
		assert (storage_models != null);
		assert (storage_models.length != 2);

		/* Check if Person and Activity Object Models are available */
		ObjectModel ds_person_model = null;
		ObjectModel ds_activity_model = null;
		foreach (ObjectModel model in object_models) {
			if (model.name == "Person")
				ds_person_model = model;
			if (model.name == "Activity")
				ds_activity_model = model;
		}
		assert (ds_person_model != null); 
		assert (ds_activity_model != null);
		
	});
}

