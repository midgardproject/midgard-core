using MidgardCR;

void midgardcr_test_add_rdf_sql_storage_manager () {

	Test.add_func ("/RDFSQLStorageManager/constructor", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
	});

	Test.add_func ("/RDFSQLStorageManager/property/content_manager", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
		var content_manager = storage_manager.content_manager;
		assert (content_manager != null);
		assert (content_manager is RDFSQLContentManager);
	});

	Test.add_func ("/RDFSQLStorageManager/property/model_manager", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
		var model_manager = storage_manager.model_manager;
		assert (model_manager != null);
		assert (model_manager is RDFSQLModelManager);
	});

	Test.add_func ("/RDFSQLStorageManager/property/object_manager", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
		var object_manager = storage_manager.object_manager;
		assert (object_manager != null);
		assert (object_manager is RDFSQLObjectManager);
	});

	Test.add_func ("/RDFSQLStorageManager/property/profiler", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
		var profiler = storage_manager.profiler;
		assert (profiler != null);
		assert (profiler is SQLProfiler);
	});

	Test.add_func ("/RDFSQLStorageManager/property/transaction", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
		var transaction = storage_manager.transaction;
		assert (transaction != null);
		assert (transaction is SQLTransaction);
	});

	Test.add_func ("/RDFSQLStorageManager/property/workspace_manager", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLStorageManager/open", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
		try {
			storage_manager.open ();
		} catch (StorageManagerError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLStorageManager/close", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
		try {
			storage_manager.open ();
		} catch (StorageManagerError e) {
			warning (e.message);
		}
		try {
			storage_manager.close ();
		} catch (StorageManagerError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLStorageManager/initialize_storage", () => {
		var storage_manager = get_storage_manager ();
		assert (storage_manager != null);	
		try {
			storage_manager.open ();
		} catch (StorageManagerError e) {
			warning (e.message);
		}
		try {
			storage_manager.initialize_storage ();
		} catch (StorageManagerError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLStorageManager/fork", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLStorageManager/clone", () => {
		assert (MISS_IMPL == null);
	});
}

