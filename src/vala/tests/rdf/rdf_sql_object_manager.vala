using MidgardCR;

void midgardcr_test_add_rdf_sql_object_manager () {

	Test.add_func ("/RDFSQLObjectManager/constructor", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLObjectManager/property/storagemanager", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLObjectManager/register_type_from_model", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLObjectManager/factory", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLObjectManager/execute", () => {
		assert (MISS_IMPL == null);
	});
}
