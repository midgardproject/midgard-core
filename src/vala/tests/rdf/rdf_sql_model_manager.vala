using MidgardCR;

RDFSQLModelManager global_model_manager = null;
RDFSQLStorageManager global_storage_manager = null;

RDFSQLModelManager get_model_manager () {
	if (global_model_manager != null)
		return global_model_manager;
	RDFSQLStorageManager global_storage_manager = get_storage_manager ();
	try {
		global_storage_manager.open ();
	} catch (StorageManagerError e) {
		warning (e.message);
	}
	var model_manager = new RDFSQLModelManager (global_storage_manager);
	return model_manager;
}

void midgardcr_test_add_rdf_sql_model_manager () {

	Test.add_func ("/RDFSQLModelManager/constructor", () => {
		var model_manager = get_model_manager ();
		assert (model_manager != null);
		assert (model_manager is RDFSQLModelManager);	
	});

	Test.add_func ("/RDFSQLModelManager/property/parent", () => {
		var model_manager = get_model_manager ();
		assert (model_manager != null);
		assert (model_manager.parent == null);
		assert (!(model_manager.parent is Model));
	});

	Test.add_func ("/RDFSQLModelManager/property/namespace", () => {
		string ns = "model_manager_namespace";
		var model_manager = get_model_manager ();
		assert (model_manager != null);
		assert (model_manager.namespace == null);
		model_manager.namespace = ns;
		assert (model_manager.namespace != null);
		assert (model_manager.namespace == ns);
	});

	Test.add_func ("/RDFSQLModelManager/property/name", () => {
		string name = "model_manager_name";
		var model_manager = get_model_manager ();
		assert (model_manager != null);
		assert (model_manager.name == null);
		model_manager.name = name;
		assert (model_manager.name != null);
		assert (model_manager.name == name);
	});

	Test.add_func ("/RDFSQLModelManager/property/namespace_manager", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/property/storagemanager", () => {
		RDFSQLStorageManager storage_manager = get_storage_manager ();
		var model_manager = new RDFSQLModelManager (storage_manager);
		assert (model_manager != null);
		assert (model_manager.storagemanager != null);
		assert (model_manager.storagemanager is RDFSQLStorageManager);
		assert ((RDFSQLStorageManager)model_manager.storagemanager == (RDFSQLStorageManager)storage_manager);
	});
	
	Test.add_func ("/RDFSQLModelManager/add_model", () => {
		global_model_manager = get_model_manager ();
		assert (global_model_manager != null);
		var storage_manager = (SQLStorageManager) global_model_manager.storagemanager; 

		/* Add RDFMapperObject model and its property */
		var rdf_model = new RDFMapperObject (RDF_OWL_PREFIX_THING, OWLTHING_CLASS_NAME);
		assert (rdf_model != null);
	        var rdf_model_ref = rdf_model.add_model (new RDFMapperProperty (RDF_FOAF_PREFIX_TOPIC, OWLTHING_TOPIC_PROPERTY));
		assert ((RDFMapperObject)rdf_model == (RDFMapperObject)rdf_model_ref);
		var global_model_ref = global_model_manager.add_model (rdf_model);
		assert ((RDFSQLModelManager)global_model_ref == (RDFSQLModelManager)global_model_manager);

		/* Add RepositoryObject model */
		ObjectModel owl_thing_model = new ObjectModel (OWLTHING_CLASS_NAME);
		owl_thing_model.add_model (new ObjectPropertyModel (OWLTHING_TOPIC_PROPERTY, "string", ""));
		global_model_ref = global_model_manager.add_model (owl_thing_model);	
		assert ((RDFSQLModelManager)global_model_ref == (RDFSQLModelManager)global_model_manager);

		/* Add SQLTable and column models */
		SQLTableModel owl_thing_tm = new SQLTableModel (storage_manager, OWLTHING_CLASS_NAME, OWLTHING_TABLE_NAME);
	        owl_thing_tm.add_model (new SQLColumnModel (storage_manager, OWLTHING_TOPIC_PROPERTY, OWLTHING_TOPIC_PROPERTY, "string"));
        	global_model_ref = global_model_manager.add_model (owl_thing_tm);
		assert ((RDFSQLModelManager)global_model_ref == (RDFSQLModelManager)global_model_manager);
	});

	Test.add_func ("/RDFSQLModelManager/get_model_by_name", () => {
		assert (global_model_manager != null);
		var rdf_mapper_model = global_model_manager.get_model_by_name (RDF_OWL_PREFIX_THING);
		assert (rdf_mapper_model != null);
		/* FAIL */
		/* RDFSQLModelManager gets by name RDF mapper only */
		var owl_thing_model = global_model_manager.get_model_by_name (OWLTHING_CLASS_NAME);
		assert (owl_thing_model == null);
		var not_exist_model = global_model_manager.get_model_by_name ("NOTEXIST");
		assert (not_exist_model == null); 	
	});

	Test.add_func ("/RDFSQLModelManager/list_models_by_type", () => {
		assert (global_model_manager != null);
		Model[] rdf_models = global_model_manager.list_models_by_type ("RDFMapperObject");
		assert (rdf_models != null);

		Model[] object_models = global_model_manager.list_models_by_type ("ObjectModel");
		assert (object_models != null);	

		Model[] storage_models = global_model_manager.list_models_by_type ("SQLTableModel");
		assert (storage_models != null);	

		/* FAIL */
		Model[] m_models = global_model_manager.list_models_by_type ("FotoModello");
		assert (m_models == null);

		Model[] e_models = global_model_manager.list_models_by_type ("");
		assert (e_models == null);	
	});

	Test.add_func ("/RDFSQLModelManager/list_model_types", () => {
		assert (global_model_manager != null);

		string[] types = global_model_manager.list_model_types ();
		assert ("RDFMapperObject" in types);	
		assert ("ObjectModel" in types);
		assert ("SQLTableModel" in types);
		
		/* FAIL */
		assert (!("FotoModello" in types));
		assert (!("" in types));
	});

	Test.add_func ("/RDFSQLModelManager/get_type_model_by_name", () => {
		assert (global_model_manager != null);

		var model = global_model_manager.get_type_model_by_name ("ObjectModel", OWLTHING_CLASS_NAME);
		assert (model != null);

		model = global_model_manager.get_type_model_by_name ("SQLTableModel", OWLTHING_CLASS_NAME);
		assert (model != null);

		model = global_model_manager.get_type_model_by_name ("RDFMapperObject", RDF_OWL_PREFIX_THING);
		assert (model != null);

		/* FAIL */
		model = global_model_manager.get_type_model_by_name ("FotoModello", OWLTHING_CLASS_NAME);
		assert (model == null);

		model = global_model_manager.get_type_model_by_name ("SQLTableModel", "");
		assert (model == null);

		model = global_model_manager.get_type_model_by_name ("RDFMapperObject", OWLTHING_CLASS_NAME);
		assert (model == null);
	});

	Test.add_func ("/RDFSQLModelManager/list_models", () => {
		assert (global_model_manager != null);
		
		Model[] models = global_model_manager.list_models ();
		assert (models.length != 0);	

		string[] names = {OWLTHING_CLASS_NAME, RDF_OWL_PREFIX_THING};
		foreach (Model m in models) {
			assert (m.name.dup () in names); /* name duplication is done to satisfy vala compiler, remove it once that doesn't complain */
			
			/* FAIL */
			assert (m.name != "");
			assert (m.name != null);
		}
	});

	Test.add_func ("/RDFSQLModelManager/is_valid", () => {
		assert (global_model_manager != null);
		
		try {
			global_model_manager.is_valid ();
		} catch (ValidationError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLModelManager/prepare_create", () => {
		assert (global_model_manager != null);

		try {
			global_model_manager.prepare_create ();
		} catch (ValidationError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLModelManager/prepare_update", () => {
		assert (global_model_manager != null);

		try {
			global_model_manager.prepare_update ();
		} catch (ValidationError e) {
			if (e is ValidationError.NAME_INVALID) {
				/* do nothing atm */
			} else {
				warning (e.message);
			}
		}
	});

	Test.add_func ("/RDFSQLModelManager/prepare_save", () => {
		assert (global_model_manager != null);

		try {
			global_model_manager.prepare_save ();
		} catch (ValidationError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLModelManager/prepare_remove", () => {
		assert (global_model_manager != null);

		try {
			global_model_manager.prepare_remove ();
		} catch (ValidationError e) {
			if (e is ValidationError.NAME_INVALID) {
				/* do nothing atm */
			} else {
				warning (e.message);
			}
		}
	});

	Test.add_func ("/RDFSQLModelManager/prepare_purge", () => {
		assert (global_model_manager != null);

		try {
			global_model_manager.prepare_purge ();
		} catch (ValidationError e) {
			if (e is ValidationError.NAME_INVALID) {
				/* do nothing atm */
			} else {
				warning (e.message);
			}
		}
	});

	Test.add_func ("/RDFSQLModelManager/execute", () => {
		assert (global_model_manager != null);
		
		global_model_manager.execute ();
	});

	Test.add_func ("/RDFSQLModelManager/prepare_update", () => {
		assert (global_model_manager != null);

		try {
			global_model_manager.prepare_update ();
			warning (MISS_IMPL);
		} catch (ValidationError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLModelManager/execute", () => {
		assert (global_model_manager != null);
		
		global_model_manager.execute ();
	});

	Test.add_func ("/RDFSQLModelManager/prepare_remove", () => {
		assert (global_model_manager != null);

		try {
			global_model_manager.prepare_remove ();
			warning (MISS_IMPL);
		} catch (ValidationError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLModelManager/execute", () => {
		assert (global_model_manager != null);
		
		global_model_manager.execute ();
	});
}

