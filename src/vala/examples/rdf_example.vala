/* ============================================ */
/* = WARNING: This is not a real example, YET = */
/* ============================================ */

/*
<rdf:RDF 
	xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
	xmlns:owl="http://www.w3.org/2002/07/owl#" 
	xmlns:foaf="http://xmlns.com/foaf/0.1/"> 
	
	<owl:Thing rdf:about="http://www.midgard-project.org/">
		<foaf:topic rdf:resource="http://www.midgard-project.org/rdf/topics/content_repository" />
	</owl:Thing>

	<foaf:Person rdf:about="http://www.midgard-project.org/people/vali">
		<foaf:myersBriggs>INTJ</foaf:myersBriggs>
		<foaf:name>Vali</foaf:name>
		<foaf:currentProject rdf:resource="http://www.midgard-project.org/" />
		<foaf:homepage>http://www.midgard-project.org/people/vali</foaf:homepage>
	</foaf:Person>

</rdf:RDF>
*/

using MidgardCR;

void profiler_callback_start (SQLProfiler profiler)
{
        profiler.start ();
}

void profiler_callback_end (SQLProfiler profiler)
{
        profiler.end ();
        GLib.print ("SQL QUERY: %s (%.04f) \n", profiler.command, profiler.time);
}

void main()
{
	GLib.Log.set_always_fatal (GLib.LogLevelFlags.LEVEL_WARNING);
	var storage_manager = getStorageManager();
	var content_manager = storage_manager.content_manager;

	/* Set names and uris for NamespaceManager */
	content_manager.namespace_manager.create_uri ("foaf", "http://xmlns.com/foaf/0.1/");

	/* Store data */
	var mgd = new RDFGenericObject ("owl:Thing");
	mgd.identifier = "http://www.midgard-project.org/";

	mgd.set_property_value ("http://xmlns.com/foaf/0.1/topic", "http://www.midgard-project.org/rdf/topics/content_repository");
	try {
		content_manager.create (mgd);
	} catch (StorageContentManagerError e) {
		GLib.error (e.message);
	}

	var rdf_vali = new RDFGenericObject ("http://xmlns.com/foaf/0.1/Person");
	rdf_vali.identifier = "http://www.midgard-project.org/people/vali";

	rdf_vali.set_property_literal ("foaf:currentProject", "http://www.midgard-project.org/");
	rdf_vali.set_property_value ("foaf:myersBriggs",    "INTJ");
	rdf_vali.set_property_value ("foaf:name",           "Vali");
	rdf_vali.set_property_value ("foaf:homepage",       "http://www.midgard-project.org/people/vali");
	
	try {
		content_manager.create (rdf_vali);	
	} catch (StorageContentManagerError e) {
		GLib.error (e.message);
	}
        
	/* Query Data */
	var query = content_manager.query_manager.create_query_select ("foaf:Person");
	query.set_constraint (new SQLQueryConstraint(
		new QueryProperty ("http://xmlns.com/foaf/0.1/currentProject", null),
		"=",
		QueryValue.create_with_value ("http://www.midgard-project.org/"),
		null
	));

	query.validate();
	query.execute();

	GLib.print ("These people work on midgard!\n");
	foreach (Storable object in query.list_objects ()) {
		RDFGenericObject rdf_obj = (RDFGenericObject) object;
		var name = rdf_obj.get_property_value ("foaf:name");
		GLib.print("\t%s\n", name != null ? (string) name : "Empty name");
	}
}

/* ================= */
/* = Helpers below = */
/* ================= */

RDFSQLStorageManager getStorageManager()
{
	Config config = new Config();
	config.dbtype = "SQLite";
	config.dbname = "ExampleDB";
	config.dbdir = "./";

	/* connect */
	var storage_manager = new RDFSQLStorageManager ("rdf_test_manager", config);
	storage_manager.open ();
	storage_manager.initialize_storage();

	/* Register models in builder and validate models */
	var model_manager = storage_manager.model_manager;
	var builder = new MidgardCR.ObjectBuilder ();

	foreach (ObjectModel model in model_manager.list_object_models())
		builder.register_model (model);
	builder.execute ();

	/* Connect profiler callbacks to all StorageManager signal emissions */
        SQLProfiler profiler = (SQLProfiler) storage_manager.profiler;
        profiler.enable (true);
        storage_manager.operation_start.connect (() => {
                profiler_callback_start (profiler);
        });
        storage_manager.operation_end.connect (() => {
                profiler_callback_end (profiler);
        }); 


	return storage_manager;
}
