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
		config.dbname = "midgardcr_test";
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
}

