using MidgardCR;

RepositoryObject global_repository_object = null;
RDFSQLContentManager global_content_manager = null;

void midgardcr_test_add_rdf_sql_content_manager () {

	Test.add_func ("/RDFSQLContentManager/constructor", () => {
		global_storage_manager = get_storage_manager ();
		assert (global_storage_manager != null);

		global_content_manager = new RDFSQLContentManager ((SQLStorageManager) global_storage_manager);
		assert (global_content_manager != null);
	});

	Test.add_func ("/RDFSQLContentManager/property/namespace_manager", () => {
		assert (global_content_manager != null);
		
		RDFSQLNamespaceManager ns_manager = (RDFSQLNamespaceManager) global_content_manager.namespace_manager;
		assert (ns_manager != null);
	});

	Test.add_func ("/RDFSQLContentManager/property/query_manager", () => {	
		assert (global_content_manager != null);
		
		RDFSQLQueryManager query_manager = (RDFSQLQueryManager) global_content_manager.query_manager;
		assert (query_manager != null);
	});

	Test.add_func ("/RDFSQLContentManager/create", () => {
		assert (global_content_manager != null);

		try {
			global_repository_object = global_storage_manager.object_manager.factory (RDF_OWL_PREFIX_THING, null);
		} catch (ObjectManagerError e) {
			warning (e.message);
		} catch (ValidationError e) {
			warning (e.message);
		}
		assert (global_repository_object != null);
		assert (global_repository_object is RepositoryObject);
		assert (global_repository_object is RepositoryObject);
		assert (!(global_repository_object is RDFGenericObject));

		global_repository_object.set ("topic", "The test");
		global_content_manager.create ((RepositoryObject) global_repository_object);
	});

	Test.add_func ("/RDFSQLContentManager/exists", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLContentManager/update", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLContentManager/save", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLContentManager/remove", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLContentManager/purge", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLContentManager/get_storage_manager", () => {
		assert (MISS_IMPL == null);
	});
}

