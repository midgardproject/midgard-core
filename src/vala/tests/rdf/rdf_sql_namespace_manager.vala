using MidgardCR;

RDFSQLNamespaceManager global_namespace_manager = null;

RDFSQLNamespaceManager? get_namespace_manager () {
	if (global_namespace_manager != null)
		return global_namespace_manager;
	return null;	
}

void midgardcr_test_add_rdf_sql_namespace_manager () {

	Test.add_func ("/RDFSQLNamespaceManager/constructor", () => {
		RDFSQLStorageManager global_storage_manager = get_storage_manager ();
		global_namespace_manager = new RDFSQLNamespaceManager ((RDFSQLContentManager) global_storage_manager.content_manager);
		assert (global_namespace_manager != null);
	});

	Test.add_func ("/RDFSQLNamespaceManager/create_mapping", () => {
		assert (global_namespace_manager != null);

		try {
			global_namespace_manager.create_mapping (RDF_FOAF_PREFIX, RDF_FOAF_URI);
        		global_namespace_manager.create_mapping (RDF_OWL_PREFIX, RDF_OWL_URI);
		} catch (NamespaceManagerError e) {
			warning (e.message);
		}
	});

	Test.add_func ("/RDFSQLNamespaceManager/get_identifier_by_name", () => {
		assert (global_namespace_manager != null);

		string identifier = global_namespace_manager.get_identifier_by_name (RDF_FOAF_PREFIX);
		assert (identifier == RDF_FOAF_URI);
		identifier = global_namespace_manager.get_identifier_by_name (RDF_OWL_PREFIX);
		assert (identifier == RDF_OWL_URI);

		/* FAIL */	
		identifier = global_namespace_manager.get_identifier_by_name ("");
		assert (identifier == null);
		identifier = global_namespace_manager.get_identifier_by_name ("Fotomodello");
		assert (identifier == null);

	});

	Test.add_func ("/RDFSQLNamespaceManager/get_name_by_identifier", () => {
		assert (global_namespace_manager != null);

		string name = global_namespace_manager.get_name_by_identifier (RDF_FOAF_URI);
		assert (name == RDF_FOAF_PREFIX);
		name = global_namespace_manager.get_name_by_identifier (RDF_OWL_URI);
		assert (name == RDF_OWL_PREFIX);

		/* FAIL */	
		name = global_namespace_manager.get_name_by_identifier ("");
		assert (name == null);
		name = global_namespace_manager.get_name_by_identifier ("Fotomodello");
		assert (name == null);
	});

	Test.add_func ("/RDFSQLNamespaceManager/identifier_exists", () => {
		assert (global_namespace_manager != null);

		bool identifier_exists = global_namespace_manager.identifier_exists (RDF_FOAF_URI);
		assert (identifier_exists == true);
		identifier_exists = global_namespace_manager.identifier_exists (RDF_OWL_URI);
		assert (identifier_exists == true);
		
		/* FAIL */
		identifier_exists = global_namespace_manager.identifier_exists ("");
		assert (identifier_exists == false);

		identifier_exists = global_namespace_manager.identifier_exists ("/a/b/c");
		assert (identifier_exists == false);
	});

	Test.add_func ("/RDFSQLNamespaceManager/list_names", () => {
		assert (global_namespace_manager != null);

		string[] names = global_namespace_manager.list_names ();
		assert (names != null);

		assert (RDF_FOAF_PREFIX in names);
		assert (RDF_OWL_PREFIX in names);

		/* FAIL */
		assert (!("" in names));
		assert (!(RDF_OWL_URI in names));
		assert (!(RDF_FOAF_URI in names));
	});

	Test.add_func ("/RDFSQLNamespaceManager/name_exists", () => {
		assert (global_namespace_manager != null);

		bool name_exists = global_namespace_manager.name_exists (RDF_OWL_PREFIX);
		assert (name_exists == true);
		name_exists = global_namespace_manager.name_exists (RDF_OWL_PREFIX);
		assert (name_exists == true);

		/* FAIL */
		name_exists = global_namespace_manager.name_exists ("");
		assert (name_exists == false);
		name_exists = global_namespace_manager.name_exists (RDF_OWL_URI);
		assert (name_exists == false);
 		name_exists = global_namespace_manager.name_exists (RDF_FOAF_URI);
		assert (name_exists == false); 
	});

	Test.add_func ("/RDFSQLNamespaceManager/is_prefix", () => {
		assert (global_namespace_manager != null);

		bool is_prefix = global_namespace_manager.is_prefix (RDF_OWL_PREFIX);
		assert (is_prefix == true);
		is_prefix = global_namespace_manager.is_prefix (RDF_FOAF_PREFIX);
		assert (is_prefix == true);

		/* FAIL */
		is_prefix = global_namespace_manager.is_prefix ("");
		assert (is_prefix == false);
		is_prefix = global_namespace_manager.is_prefix (RDF_OWL_URI);
		assert (is_prefix == false);
 		is_prefix = global_namespace_manager.is_prefix (RDF_FOAF_URI);
		assert (is_prefix == false); 
	});

	Test.add_func ("/RDFSQLNamespaceManager/get_prefix_tokens", () => {
		assert (global_namespace_manager != null);

		string[] tokens = global_namespace_manager.get_prefix_tokens (RDF_OWL_PREFIX_THING);
		assert (tokens != null);
		assert (tokens[0] == RDF_OWL_PREFIX);
		assert (tokens[1] == "Thing"); /* FIXME */
		tokens = null;

		tokens = global_namespace_manager.get_prefix_tokens (RDF_OWL_URI_THING);
		assert (tokens != null);
		assert (tokens[0] == RDF_OWL_PREFIX);
		assert (tokens[1] == "Thing"); /* FIXME */
		tokens = null;

		tokens = global_namespace_manager.get_prefix_tokens (RDF_FOAF_PREFIX_TOPIC);
		assert (tokens != null);
		assert (tokens[0] == RDF_FOAF_PREFIX);
		assert (tokens[1] == "topic"); /* FIXME */
		tokens = null;

		tokens = global_namespace_manager.get_prefix_tokens (RDF_FOAF_URI_TOPIC);
		assert (tokens != null);
		assert (tokens[0] == RDF_FOAF_PREFIX);
		assert (tokens[1] == "topic"); /* FIXME */
		tokens = null;

		/* FAIL */
		tokens = global_namespace_manager.get_prefix_tokens ("");
		assert (tokens != null);
		assert (tokens[0] == null);
		assert (tokens[1] == null); 
		tokens = null;
	});

	Test.add_func ("/RDFSQLNamespaceManager/get_prefix", () => {
		assert (global_namespace_manager != null);

		string prefix = global_namespace_manager.get_prefix (RDF_OWL_PREFIX);
		assert (prefix == RDF_OWL_PREFIX);
		prefix = global_namespace_manager.get_prefix (RDF_OWL_URI);
		assert (prefix == RDF_OWL_PREFIX);
		prefix = global_namespace_manager.get_prefix (RDF_OWL_PREFIX_THING);
		assert (prefix == RDF_OWL_PREFIX);
		prefix = global_namespace_manager.get_prefix (RDF_OWL_URI_THING);
		assert (prefix == RDF_OWL_PREFIX);

		prefix = global_namespace_manager.get_prefix (RDF_FOAF_PREFIX);
		assert (prefix == RDF_FOAF_PREFIX);
		prefix = global_namespace_manager.get_prefix (RDF_FOAF_URI);
		assert (prefix == RDF_FOAF_PREFIX);
		prefix = global_namespace_manager.get_prefix (RDF_FOAF_PREFIX_TOPIC);
		assert (prefix == RDF_FOAF_PREFIX);
		prefix = global_namespace_manager.get_prefix (RDF_FOAF_URI_TOPIC);
		assert (prefix == RDF_FOAF_PREFIX);

		/* FAIL */
		prefix = global_namespace_manager.get_prefix ("");
		assert (prefix == null);
	});

	Test.add_func ("/RDFSQLNamespaceManager/get_prefix_with_statement", () => {
		assert (global_namespace_manager != null);

		string stmt = global_namespace_manager.get_prefix_with_statement (RDF_OWL_PREFIX_THING);
		assert (stmt == RDF_OWL_PREFIX_THING);
		stmt = global_namespace_manager.get_prefix_with_statement (RDF_OWL_URI_THING);
		assert (stmt == RDF_OWL_PREFIX_THING);
	
		stmt = global_namespace_manager.get_prefix_with_statement (RDF_FOAF_PREFIX_TOPIC);
		assert (stmt == RDF_FOAF_PREFIX_TOPIC);
		stmt = global_namespace_manager.get_prefix_with_statement (RDF_FOAF_URI_TOPIC);
		assert (stmt == RDF_FOAF_PREFIX_TOPIC);

		/* FAIL */
		stmt = global_namespace_manager.get_prefix_with_statement (RDF_OWL_PREFIX);
		assert (stmt == null);
		stmt = global_namespace_manager.get_prefix_with_statement (RDF_OWL_URI);
		assert (stmt == null);
	});

	Test.add_func ("/RDFSQLNamespaceManager/is_uri", () => {
		assert (global_namespace_manager != null);

		bool is_uri = global_namespace_manager.is_uri (RDF_OWL_URI);
		assert (is_uri == true);	
		is_uri = global_namespace_manager.is_uri (RDF_FOAF_URI);
		assert (is_uri == true);	
	
		/* FAIL */	
		is_uri = global_namespace_manager.is_uri (RDF_FOAF_PREFIX);
		assert (is_uri == false);	
		is_uri = global_namespace_manager.is_uri (RDF_OWL_PREFIX);
		assert (is_uri == false);	
		is_uri = global_namespace_manager.is_uri ("");
		assert (is_uri == false);	
	});

	Test.add_func ("/RDFSQLNamespaceManager/get_uri_tokens", () => {
		assert (global_namespace_manager != null);

		string[] tokens = global_namespace_manager.get_uri_tokens (RDF_OWL_PREFIX_THING);
		assert (tokens != null);
		assert (tokens[0] == RDF_OWL_URI);
		assert (tokens[1] == "Thing"); /* FIXME */
		tokens = null;

		tokens = global_namespace_manager.get_uri_tokens (RDF_OWL_URI_THING);
		assert (tokens != null);
		assert (tokens[0] == RDF_OWL_URI);
		assert (tokens[1] == "Thing"); /* FIXME */
		tokens = null;

		tokens = global_namespace_manager.get_uri_tokens (RDF_FOAF_PREFIX_TOPIC);
		assert (tokens != null);
		assert (tokens[0] == RDF_FOAF_URI);
		assert (tokens[1] == "topic"); /* FIXME */
		tokens = null;

		tokens = global_namespace_manager.get_uri_tokens (RDF_FOAF_URI_TOPIC);
		assert (tokens != null);
		assert (tokens[0] == RDF_FOAF_URI);
		assert (tokens[1] == "topic"); /* FIXME */
		tokens = null;

		/* FAIL */
		tokens = global_namespace_manager.get_uri_tokens ("");
		assert (tokens != null);
		assert (tokens[0] == null);
		assert (tokens[1] == null); 
		tokens = null;
	});

	Test.add_func ("/RDFSQLNamespaceManager/get_uri", () => {
		assert (global_namespace_manager != null);

		string uri = global_namespace_manager.get_uri (RDF_OWL_PREFIX);
		assert (uri == RDF_OWL_URI);
		uri = global_namespace_manager.get_uri (RDF_OWL_URI);
		assert (uri == RDF_OWL_URI);
		uri = global_namespace_manager.get_uri (RDF_OWL_PREFIX_THING);
		assert (uri == RDF_OWL_URI);
		uri = global_namespace_manager.get_uri (RDF_OWL_URI_THING);
		assert (uri == RDF_OWL_URI);

		uri = global_namespace_manager.get_uri (RDF_FOAF_PREFIX);
		assert (uri == RDF_FOAF_URI);
		uri = global_namespace_manager.get_uri (RDF_FOAF_URI);
		assert (uri == RDF_FOAF_URI);
		uri = global_namespace_manager.get_uri (RDF_FOAF_PREFIX_TOPIC);
		assert (uri == RDF_FOAF_URI);
		uri = global_namespace_manager.get_uri (RDF_FOAF_URI_TOPIC);
		assert (uri == RDF_FOAF_URI);

		/* FAIL */
		uri = global_namespace_manager.get_uri ("");
		assert (uri == null);
	});

	Test.add_func ("/RDFSQLNamespaceManager/get_uri_with_statement", () => {
		assert (global_namespace_manager != null);

		string stmt = global_namespace_manager.get_uri_with_statement (RDF_OWL_PREFIX_THING);
		assert (stmt == RDF_OWL_URI_THING);
		stmt = global_namespace_manager.get_uri_with_statement (RDF_OWL_URI_THING);
		assert (stmt == RDF_OWL_URI_THING);
	
		stmt = global_namespace_manager.get_uri_with_statement (RDF_FOAF_PREFIX_TOPIC);
		assert (stmt == RDF_FOAF_URI_TOPIC);
		stmt = global_namespace_manager.get_uri_with_statement (RDF_FOAF_URI_TOPIC);
		assert (stmt == RDF_FOAF_URI_TOPIC);

		/* FAIL */
		stmt = global_namespace_manager.get_uri_with_statement (RDF_OWL_PREFIX);
		assert (stmt == null);
		stmt = global_namespace_manager.get_uri_with_statement (RDF_OWL_URI);
		assert (stmt == null);	
	});
}
