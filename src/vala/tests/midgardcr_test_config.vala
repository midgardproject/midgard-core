using MidgardCR;

const string DEFAULT_CONFIGURATION = "MidgardCR_test";
const string DEFAULT_CONFIGURATION_PATH = "./MidgardCR_test";

void midgardcr_test_add_config_tests () {

	/* constructor */
	Test.add_func ("/Config/constructor", () => {
		MidgardCR.Config config = new MidgardCR.Config ();

		/* SUCCESS */
		assert (config.sharedir == "/usr/local/share");
		assert (config.blobdir == "/var/local/lib/midgard/blobs");
		assert (config.cachedir == "/var/local/cache/midgard");
		assert (config.vardir == "/var/local/");
		assert (config.dbdir == "");
	});

	/* save configuration */
   	Test.add_func ("/Config/save_configuration", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
		config.dbtype = "SQLite";
		config.dbname = "MidgardCR_TestDatabase";
		config.dbdir = "./";
		bool saved = true;

		/* SUCCESS */
		try {
			saved = config.save_configuration (DEFAULT_CONFIGURATION, true);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}
		assert (saved == true);

		/* FAIL (Access denied) */
		try {
			saved = config.save_configuration (DEFAULT_CONFIGURATION, false);
		} catch (GLib.FileError e) {
			saved = false;		
		} catch (GLib.KeyFileError e) {
			saved = false;
		}
		assert (saved == false);
    	});

	/* read configuration */
   	Test.add_func ("/Config/read_configuration", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
		bool read = false;

		/* FAIL */
		try {
			read = config.read_configuration ("thereisnosuchfile", true);
		} catch (GLib.FileError e) {
			read = false;	
		} catch (GLib.KeyFileError e) {
			read = false;
		}
		assert (read == false);

		/* FAIL */
		try {
			read = config.read_configuration ("thereisnosuchfile", false);
		} catch (GLib.FileError e) {
			read = false;	
		} catch (GLib.KeyFileError e) {
			read = false;
		}
		assert (read == false);

		/* SUCCESS */
		try {
			read = config.read_configuration (DEFAULT_CONFIGURATION, true);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);	
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}
		assert (read == true);

		/* FAIL */
		try {
			read = config.read_configuration (DEFAULT_CONFIGURATION, false);
		} catch (GLib.FileError e) {
			read = false;
		} catch (GLib.KeyFileError e) {
			read = false;
		}
		assert (read == false);
    	});

	/* save configuration at path */
   	Test.add_func ("/Config/save_configuration_at_path", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
		config.dbtype = "SQLite";
		config.dbname = "midgardcr_test";
		config.dbdir = "./";
		bool saved = true;

		/* SUCCESS */
		try {
			saved = config.save_configuration_at_path (DEFAULT_CONFIGURATION_PATH);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}
		assert (saved == true);

		/* FAIL (Access denied) */
		try {
			saved = config.save_configuration_at_path ("/etc/midgard3/hack_file");
		} catch (GLib.FileError e) {
			saved = false;
		} catch (GLib.KeyFileError e) {
			saved = false;
		}
		assert (saved == false);
    	});

	/* read configuration at path */
   	Test.add_func ("/Config/read_configuration_at_path", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
		bool read = false;

		/* FAIL */
		try {
			read = config.read_configuration_at_path ("./thereisnosuchfile");
		} catch (GLib.FileError e) {
			/* this is ok, do nothing */	
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}
		assert (read == false);
		
		/* SUCCESS */
		try {
			read = config.read_configuration_at_path (DEFAULT_CONFIGURATION_PATH);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);	
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}
		assert (read == true);

		/* FAIL */
		try {
			read = config.read_configuration_at_path ("./thereisnosuchfile");
		} catch (GLib.FileError e) {
			/* do nothing */
		} catch (GLib.KeyFileError e) {
			/* do nothing */
		}
		assert (read == true);
    	});

	/* list configurations */
   	Test.add_func ("/Config/list_configurations", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
		string[] invalid_configs = null;
		bool found_invalid_config = false;

		/* FAIL (Access denied) */
		try {
			invalid_configs = config.list_configurations (false);
		} catch (GLib.FileError e) {
			invalid_configs = null;
		}

		foreach (weak string configname in invalid_configs) {
			GLib.print ("%s \n", configname);
			if (configname == DEFAULT_CONFIGURATION) {
				found_invalid_config = true;
				break;
			}
		}
		assert (found_invalid_config == false);

		/* SUCCESS */
		string[] valid_configs = null;
		try {
			valid_configs = config.list_configurations (true);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);
		} 
		assert (valid_configs != null);

		bool found_valid_config = false;
		foreach (weak string configname in valid_configs) {
			if (configname == DEFAULT_CONFIGURATION) {
				found_valid_config = true;
				break;
			}
		}
		assert (found_valid_config == true);

    	});

   	Test.add_func ("/Config/copy", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
		MidgardCR.Config copy_config = config.copy ();

		assert (config.authtype == copy_config.authtype);
		assert (config.dbname == copy_config.dbname);	
		assert (config.dbpass == copy_config.dbpass);
		assert (config.dbtype == copy_config.dbtype);
		assert (config.dbuser == copy_config.dbuser);
		assert (config.host == copy_config.host);
		assert (config.dbport == copy_config.dbport);
		assert (config.logfilename == copy_config.logfilename);
		assert (config.loglevel == copy_config.loglevel);
		assert (config.midgardpassword == copy_config.midgardpassword);
		assert (config.midgardusername == copy_config.midgardusername);
		assert (config.sharedir == copy_config.sharedir);
		assert (config.vardir == copy_config.vardir);
		assert (config.blobdir == copy_config.blobdir);
		assert (config.cachedir == copy_config.cachedir);
		assert (config.dbdir == copy_config.dbdir);
		assert (config.tablecreate == copy_config.tablecreate);
		assert (config.tableupdate == copy_config.tableupdate);
		assert (config.testunit == copy_config.testunit); 
    	});
}

