using MidgardCR;

void midgardcr_test_add_rdf_sql_query_storage () {

	Test.add_func ("/RDFSQLQueryStorage/constructor", () => {
		assert (MISS_IMPL == null);
	});

	Test.add_func ("/RDFSQLQueryStorage/property/classname", () => {
		assert (MISS_IMPL == null);
	});
}

