using MidgardCR;

void midgardcr_test_add_config_tests () {

	/* constructor */
	Test.add_func ("/Config/constructor", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
        	assert (config.sharedir == "/usr/local/share");
		assert (config.blobdir == "/var/local/lib/midgard/blobs");
		assert (config.cachedir == "/var/local/cache/midgard");
		assert (config.vardir == "/var/local/");
        	assert (config.dbdir == "");
	});

	/* read file */
   	Test.add_func ("/Config/read_file", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
		try {
			config.read_file ("thereisnosuchfile", true);
		} catch (GLib.FileError e) {
			/* this is ok, do nothing */	
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}

		try {
			config.read_file ("thereisnosuchfile", false);
		} catch (GLib.FileError e) {
			/* this is ok, do nothing */	
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}
    	});

	/* save file */
   	Test.add_func ("/Config/save_file", () => {
		MidgardCR.Config config = new MidgardCR.Config ();
		config.dbtype = "SQLite";
		config.dbname = "midgardcr_test";
		config.dbdir = "./";
		try {
			config.save_file ("MidgardCR_test", true);
		} catch (GLib.FileError e) {
			GLib.warning (e.message);
		} catch (GLib.KeyFileError e) {
			GLib.warning (e.message);
		}
    	});
}

