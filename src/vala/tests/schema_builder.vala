using MidgardCR;

void midgardcr_test_add_schema_builder_tests () {

	/* constructor */
	Test.add_func ("/SchemaBuilder/constructor", () => {
		MidgardCR.SchemaBuilder builder = new MidgardCR.SchemaBuilder ();
		assert (builder != null);
	});

	/* register model */
	Test.add_func ("/SchemaBuilder/register_model", () => {
		MidgardCR.SchemaBuilder builder = new MidgardCR.SchemaBuilder ();
		assert (builder != null);

		MidgardCR.SchemaModel model = new MidgardCR.SchemaModel (DEFAULT_CLASSNAME);
                assert (model != null);
                assert (model.name == DEFAULT_CLASSNAME);

                /* SUCCESS */
                MidgardCR.SchemaModelProperty prop_model = new MidgardCR.SchemaModelProperty (DEFAULT_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model);

		/* SUCCESS */
		try {
			builder.register_model (model);		
		} catch (MidgardCR.ValidationError e) {
			GLib.warning (e.message);
		} 

		/* FAIL */
		try {
			builder.register_model (model);
		} catch (MidgardCR.ValidationError e) {
			if (!(e is MidgardCR.ValidationError.NAME_DUPLICATED))
				GLib.warning (e.message);
		}	
	});

	/* execute */
	Test.add_func ("/SchemaBuilder/execute", () => {
		MidgardCR.SchemaBuilder builder = new MidgardCR.SchemaBuilder ();
		assert (builder != null);

		MidgardCR.SchemaModel model = new MidgardCR.SchemaModel (DEFAULT_CLASSNAME);
                assert (model != null);
                assert (model.name == DEFAULT_CLASSNAME);

                /* SUCCESS */
                MidgardCR.SchemaModelProperty prop_model = new MidgardCR.SchemaModelProperty (DEFAULT_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model);

		/* SUCCESS */
		try {
			builder.register_model (model);		
		} catch (MidgardCR.ValidationError e) {
			GLib.warning (e.message);
		} 

		/* FAIL */
		try {
			builder.register_model (model);
		} catch (MidgardCR.ValidationError e) {
			if (!(e is MidgardCR.ValidationError.NAME_DUPLICATED))
				GLib.warning (e.message);
		}	

		/* SUCCESS */
		try {
			builder.execute ();
		} catch (GLib.Error e) {
			GLib.warning (e.message);
		}	
	});
}

