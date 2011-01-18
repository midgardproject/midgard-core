
using MidgardCR;

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

RDFSQLStorageManager? get_storage_manager () {
        Config config = new Config ();
        config.dbtype = "SQLite";
        config.dbname = "RDFTest";
        config.dbdir = "./";

        try {
                RDFSQLStorageManager mngr = new RDFSQLStorageManager ("RDFTestManager", config);
                return mngr;
        } catch (StorageManagerError e) {
                return null;
        }
}

void main (string[] args) {
	Test.init (ref args);
	midgardcr_test_add_rdf_sql_storage_manager ();
	midgardcr_test_add_rdf_sql_model_manager ();
	midgardcr_test_add_rdf_sql_namespace_manager ();
	midgardcr_test_add_rdf_sql_content_manager ();
	midgardcr_test_add_rdf_sql_object_manager ();
	midgardcr_test_add_rdf_sql_query_storage ();
	midgardcr_test_add_rdf_sql_query_select ();
	Test.run ();
}
