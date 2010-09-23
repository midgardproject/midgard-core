
const string MISS_IMPL = "MISSING IMPLEMENTATION ";
const string DEFAULT_CLASSNAME = "CRFoo";
const string DEFAULT_PROPERTY_NAME = "title_property";

void main (string[] args) {
	Test.init (ref args);
	midgardcr_test_add_config_tests ();
	midgardcr_test_add_sql_storage_manager_tests ();
	midgardcr_test_add_schema_model_tests ();
	midgardcr_test_add_schema_model_property_tests ();
	midgardcr_test_add_schema_builder_tests ();
	Test.run ();
}
