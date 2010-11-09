using MidgardCR;

void profiler_callback_start (SQLProfiler profiler)
{
	//profiler.start ();
}

void profiler_callback_end (SQLProfiler profiler)
{
	//profiler.end ();
	//GLib.print ("SQL QUERY: %s (%.04f) \n", profiler.command, profiler.time);
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
                MidgardCR.ObjectPropertyModel prop_model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
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
                MidgardCR.ObjectPropertyModel prop_model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

		/* SUCCESS */
                MidgardCR.ObjectPropertyModel prop_model_a = new MidgardCR.ObjectPropertyModel (ABSTRACT_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model).add_model (prop_model_a);

		/* SQLTableModel */
		/* SUCCESS */
		MidgardCR.SQLTableModel storage_model = model_manager.create_storage_model (model, DEFAULT_TABLENAME) as MidgardCR.SQLTableModel;
		assert (storage_model != null);
		MidgardCR.SQLColumnModel storage_model_prop = storage_model.create_model_property (TITLE_PROPERTY_NAME, TITLE_COLUMN, "string");
		assert (storage_model_prop != null);
		MidgardCR.SQLColumnModel storage_model_prop_a = storage_model.create_model_property (ABSTRACT_PROPERTY_NAME, ABSTRACT_COLUMN, "string");
		assert (storage_model_prop_a != null);
		storage_model.add_model (storage_model_prop).add_model (storage_model_prop_a);

		/* Add Schema model to StorageModelManager */
		model_manager.add_model (model).add_model (storage_model);

		/*
		SQLProfiler profiler = (SQLProfiler) model_manager.storagemanager.profiler;
		profiler.enable (true);
		model_manager.execution_start.connect (() => {
			profiler_callback_start(profiler);
		}); 
		model_manager.execution_end.connect (() => {
			profiler_callback_end(profiler);
		}); */

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
	        var person_model = new ObjectModel (PERSON_CLASS_NAME);
		assert (person_model != null);
        	/* Define properties: 'firstname' and 'lastname' */
        	person_model
                	.add_model (new ObjectPropertyModel (FIRSTNAME, "string", ""))
                	.add_model (new ObjectPropertyModel (LASTNAME, "string", ""));

        	/* Create new SQL StorageModel which defines 'Person' class table and all required columns */
        	/* Every 'Person' object's data will be stored in 'person' table */
		/* SUCCESS */
        	var person_sm = model_manager.create_storage_model (person_model, PERSON_TABLE_NAME) as SQLTableModel;
		assert (person_sm != null);
        	/* Add two columns to 'person' table: 'firstname', 'lastname' */
        	person_sm
                	.add_model (person_sm.create_model_property (FIRSTNAME, FIRSTNAME, "string"))
                	.add_model (person_sm.create_model_property (LASTNAME, LASTNAME, "string"));

		/* Define 'Activity' class */
		/* SUCCESS */
		var activity_model = new ObjectModel (ACTIVITY_CLASS_NAME);
		assert (activity_model != null);
		/* 'actor' property is object type, so we 'link' property with 'Person' object */
		/* SUCCESS */
      		var am_ref_actor = new ObjectModelReference ("ReferenceObject");
		assert (am_ref_actor != null);
		var am_actor = new ObjectPropertyReference ("actor", person_model, am_ref_actor);
		/* Define properties: 'verb', 'target', 'summary', 'application' */
		activity_model
			.add_model (am_actor)
			.add_model (new ObjectPropertyModel (VERB, "string", ""))
			.add_model (new ObjectPropertyModel (TARGET, "guid", ""))
			.add_model (new ObjectPropertyModel (SUMMARY, "string", ""))
			.add_model (new ObjectPropertyModel (APPLICATION, "string", ""));
	
		/* Create new SQL StorageModel which defines 'Activity' class table and all required columns */
		/* Activity class requires 'midgard_activity' table */
		/* SUCCESS */
		var activity_sm = model_manager.create_storage_model (activity_model, "midgard_activity") as SQLTableModel;
		assert (activity_sm != null);
		/* Add columns to table: 'verb', 'application', 'target', 'summary' and those required by 'actor' */
		var asm_verb = activity_sm.create_model_property (VERB, VERB, "string");
		assert (asm_verb != null);
		asm_verb.index = true;

		var asm_application = activity_sm.create_model_property (APPLICATION, APPLICATION, "string");
		assert (asm_application != null);
		asm_application.index = true;

		var actor_model = activity_sm.create_model_property (ACTOR, ACTOR, "object");
		assert (actor_model != null);
		
		/* Adds additional columns to store info for reference object */
		actor_model
			.add_model (activity_sm.create_model_property ("id", "actor_id", "int"))
			.add_model (activity_sm.create_model_property ("guid", "actor_guid", "guid"));
		activity_sm
			.add_model (asm_verb)
			.add_model (asm_application)
			.add_model (actor_model)
			.add_model (activity_sm.create_model_property (TARGET, TARGET, "guid"))
			.add_model (activity_sm.create_model_property (SUMMARY, SUMMARY, "text"));

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
		int table_n = 3; /* Default table, person and activity */
		unowned ObjectModel[]? object_models = model_mngr.list_object_models ();
		assert (object_models != null);
		assert (object_models.length == table_n);
		StorageModel[]? storage_models = model_mngr.list_storage_models ();
		assert (storage_models != null);
		assert (storage_models.length == table_n);

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

		/* Check if Person and Activity table models are available */
		SQLTableModel ds_person_table = null;
		SQLTableModel ds_activity_table = null;	
		foreach (StorageModel table_model in storage_models) {
			if (table_model.name == PERSON_CLASS_NAME)
				ds_person_table = (SQLTableModel) table_model;
			if (table_model.name == ACTIVITY_CLASS_NAME)
				ds_activity_table = (SQLTableModel) table_model;
		}
		assert (ds_person_table != null);
		assert (ds_activity_table != null);
		assert (ds_person_table.location == PERSON_TABLE_NAME);
		assert (ds_activity_table.location == ACTIVITY_TABLE_NAME);		

		/* Check Person object model properties */
		var fmodel = (ObjectPropertyModel) ds_person_model.get_model_by_name (FIRSTNAME);
		assert (fmodel != null);
		assert (fmodel.valuegtype == typeof (string));
		assert (fmodel.valuetypename == "string");
		var lmodel = (ObjectPropertyModel) ds_person_model.get_model_by_name (LASTNAME);
		assert (lmodel != null);
		assert (lmodel.valuegtype == typeof (string));
		assert (lmodel.valuetypename == "string");

		/* Check Person columns */
		var fcol = (SQLColumnModel) ds_person_table.get_model_by_name (FIRSTNAME);
		assert (fcol != null);
		assert (fcol.location == FIRSTNAME);
		assert (fcol.valuetypename == "string");
		assert (fcol.valuegtype == typeof (string));
		var lcol = (SQLColumnModel) ds_person_table.get_model_by_name (LASTNAME);
		assert (lcol != null);
		assert (lcol.location == LASTNAME);
		assert (lcol.valuetypename == "string");
		assert (lcol.valuegtype == typeof (string));

		/* Check Activity object model properties */
		var a_actor_model = (ObjectPropertyModel) ds_activity_model.get_model_by_name (ACTOR);
		assert (a_actor_model != null);
		assert (a_actor_model is ObjectPropertyReference);
		assert (a_actor_model.valuegtype == typeof (Object));
		assert (a_actor_model.valuetypename == "object");
		var a_actor_reference_model = (ObjectModel) a_actor_model.get_model_by_name ("ReferenceObject");
		assert (a_actor_reference_model != null);
		assert (a_actor_reference_model is ObjectModelReference);
		assert (a_actor_reference_model.name == "ReferenceObject");	
		var a_verb_model = (ObjectPropertyModel) ds_activity_model.get_model_by_name (VERB);
		assert (a_verb_model != null);
		assert (a_verb_model.valuegtype == typeof (string));
		assert (a_verb_model.valuetypename == "string");
		var a_target_model = (ObjectPropertyModel) ds_activity_model.get_model_by_name (TARGET);
		assert (a_target_model != null);
		assert (a_target_model.valuegtype == typeof (string));
		assert (a_target_model.valuetypename == "guid");
		var a_summary_model = (ObjectPropertyModel) ds_activity_model.get_model_by_name (SUMMARY);
		assert (a_summary_model != null);
		assert (a_summary_model.valuegtype == typeof (string));
		assert (a_summary_model.valuetypename == "string");
		var a_app_model = (ObjectPropertyModel) ds_activity_model.get_model_by_name (APPLICATION);
		assert (a_app_model != null);
		assert (a_app_model.valuegtype == typeof (string));
		assert (a_app_model.valuetypename == "string");

		/* Check Activity columns */
		var a_actor_col = (SQLColumnModel) ds_activity_table.get_model_by_name (ACTOR);
		assert (a_actor_col != null);
		assert (a_actor_col.location == ACTOR);
		assert (a_actor_col.valuetypename == "object");
		Model[] actor_columns = a_actor_col.list_models();
		assert (actor_columns.length == 2); /* id + guid */
		/* Check two columns, id and guid created for reference actor object */
		var a_actor_id_col = (SQLColumnModel) a_actor_col.get_model_by_name ("id");
		assert (a_actor_id_col != null);
		assert (a_actor_id_col.location == "actor_id");
		assert (a_actor_id_col.valuetypename == "int"); 
		var a_actor_guid_col = (SQLColumnModel) a_actor_col.get_model_by_name ("guid");
		assert (a_actor_guid_col != null);
		assert (a_actor_guid_col.location == "actor_guid");	
		assert (a_actor_guid_col.valuetypename == "guid"); 	
		var a_verb_col = (SQLColumnModel) ds_activity_table.get_model_by_name (VERB);
		assert (a_verb_col != null);
		assert (a_verb_col.location == VERB);
		assert (a_verb_col.valuetypename == "string");
		var a_target_col = (SQLColumnModel) ds_activity_table.get_model_by_name (TARGET);
		assert (a_target_col != null);
		assert (a_target_col.location == TARGET);
		assert (a_target_col.valuetypename == "guid");
		var a_summary_col = (SQLColumnModel) ds_activity_table.get_model_by_name (SUMMARY);
		assert (a_summary_col != null);
		assert (a_summary_col.location == SUMMARY);
		assert (a_summary_col.valuetypename == "text");
		var a_app_col = (SQLColumnModel) ds_activity_table.get_model_by_name (APPLICATION);
		assert (a_app_col != null);
		assert (a_app_col.location == APPLICATION);
		assert (a_app_col.valuetypename == "string"); 
	});
}

