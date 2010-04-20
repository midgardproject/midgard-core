#include "midgard_test.h"

static MidgardConnection *mgd_global = NULL;

_MGD_TEST_OBJECT_SETUP
_MGD_TEST_OBJECT_TEARDOWN

int main (int argc, char *argv[])
{
	g_test_init (&argc, &argv, NULL);
	
	midgard_init();

	/* FIXME, it should be fixed with API... */
	MidgardSchema *schema = g_object_new (MIDGARD_TYPE_SCHEMA, NULL);
	midgard_schema_init (schema, NULL);
	midgard_schema_read_dir (schema, NULL);

	guint n_types, i;
	const gchar *typename;
	gchar *testname;
	GType *all_types = g_type_children (MIDGARD_TYPE_OBJECT, &n_types);

	MidgardConfig *config = midgard_config_new ();
	g_object_set (G_OBJECT (config), 
			"dbtype", "SQLite",
			"database", "midgard_test_schema_object_factory", 
			NULL);
	mgd_global = midgard_connection_new ();
	gboolean connection_opened = midgard_connection_open_config (mgd_global, config);

	/* Create storage for this test */
	midgard_test_storage_create_test_storage (mgd_global);

	MidgardObject *object = NULL;

	for (i = 0; i < n_types; i++) {

		typename = g_type_name(all_types[i]);	
		
		if (g_str_equal(typename, "midgard_attachment")
				|| g_str_equal(typename, "midgard_parameter"))
			continue;

		/* FIXME, ignore dependent objects for a while*/
		if (midgard_reflector_object_get_property_parent (typename))
			continue;

		object = midgard_test_object_basic_new (mgd_global, typename, NULL);
		g_assert (object != NULL);
		gboolean object_created = midgard_object_create (object);
		MIDGARD_TEST_ERROR_OK(mgd_global);
		g_assert (object_created != FALSE);

		testname = g_strconcat ("/midgard_schema_object_factory/", typename, "/get_by_guid", NULL);
		g_test_add (testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_schema_object_factory_get_object_by_guid, midgard_test_teardown_foo);
		g_free (testname);

		testname = g_strconcat ("/midgard_schema_object_factory/", typename, "/get_by_path", NULL);
		g_test_add (testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_schema_object_factory_get_object_by_path, midgard_test_teardown_foo);
		g_free (testname);

		testname = g_strconcat ("/midgard_schema_object_factory/", typename, "/undelete", NULL);
		g_test_add (testname, MidgardObjectTest, object, midgard_test_setup,  
				midgard_test_schema_object_factory_object_undelete, midgard_test_teardown_foo);
		g_free (testname);
	}

	g_free(all_types);

	/* Finalize */	
	_MGD_TEST_UNREF_SCHEMA
	_MGD_TEST_UNREF_MIDGARD

	return g_test_run();
}
