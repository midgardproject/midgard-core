
const string MISS_IMPL = "MISSING IMPLEMENTATION ";
const string DEFAULT_CLASSNAME = "CRFoo";
const string DEFAULT_TABLENAME = "CRFoo_table";
const string TITLE_PROPERTY_NAME = "title_property";
const string TITLE_COLUMN = "title_col";
const string ABSTRACT_PROPERTY_NAME = "abstract_property";
const string ABSTRACT_COLUMN = "abstract_col";

const string PERSON_CLASS_NAME = "Person";
const string PERSON_TABLE_NAME = "person";
const string FIRSTNAME = "firstname";
const string LASTNAME = "lastname";

const string ACTIVITY_CLASS_NAME = "Activity";
const string ACTIVITY_TABLE_NAME = "midgard_activity";
const string ACTOR = "actor";
const string VERB = "verb";
const string TARGET = "target";
const string SUMMARY = "summary";
const string APPLICATION = "application";

void main (string[] args) {
	Test.init (ref args);
	midgardcr_test_add_config_tests ();
	midgardcr_test_add_sql_storage_manager_tests ();
	midgardcr_test_add_object_model_tests ();
	midgardcr_test_add_object_model_property_tests ();
	midgardcr_test_add_schema_builder_tests ();
	midgardcr_test_add_sql_storage_model_manager_tests ();
	Test.run ();
}
