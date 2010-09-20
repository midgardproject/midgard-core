
const string MISS_IMPL = "MISSING IMPLEMENTATION ";

void main (string[] args) {
	Test.init (ref args);
	midgardcr_test_add_config_tests ();
	midgardcr_test_add_sql_storage_manager_tests ();
	Test.run ();
}
