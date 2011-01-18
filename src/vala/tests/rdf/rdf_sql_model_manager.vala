using MidgardCR;

RDFSQLModelManager get_model_manager () {
	RDFSQLStorageManager storage_manager = get_storage_manager ();
	var model_manager = new RDFSQLModelManager (storage_manager);
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
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/get_model_by_name", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/is_valid", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/prepare_create", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/prepare_update", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/prepare_save", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/prepare_remove", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/prepare_purge", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/execute", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/list_models_by_type", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/list_model_types", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/get_type_model_by_name", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLModelManager/list_models", () => {
		assert (MISS_IMPL == null);
	});
}

