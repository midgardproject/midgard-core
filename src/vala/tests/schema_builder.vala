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
                MidgardCR.SchemaModelProperty prop_model = new MidgardCR.SchemaModelProperty (TITLE_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model);

		/* SUCCESS */
		try {
			builder.register_model (model);		
		} catch (MidgardCR.ValidationError e) {
			GLib.warning (e.message);
		} catch (MidgardCR.SchemaBuilderError e) {
			GLib.warning (e.message);
		} 

		/* FAIL */
		try {
			builder.register_model (model);
		} catch (MidgardCR.ValidationError e) {
			if (!(e is MidgardCR.ValidationError.NAME_DUPLICATED))
				GLib.warning (e.message);
		} catch (MidgardCR.SchemaBuilderError e) {
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
                MidgardCR.SchemaModelProperty prop_model = new MidgardCR.SchemaModelProperty (TITLE_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model);

		/* SUCCESS */
		try {
			builder.register_model (model);		
		} catch (MidgardCR.ValidationError e) {
			GLib.warning (e.message);
		} catch (MidgardCR.SchemaBuilderError e) {
			GLib.warning (e.message);
		} 

		/* FAIL */
		try {
			builder.register_model (model);
		} catch (MidgardCR.ValidationError e) {
			if (!(e is MidgardCR.ValidationError.NAME_DUPLICATED))
				GLib.warning (e.message);
		} catch (MidgardCR.SchemaBuilderError e) {
			GLib.warning (e.message);
		} 	

		/* SUCCESS */
		try {
			builder.execute ();
		} catch (MidgardCR.ExecutableError e) {
			GLib.warning (e.message);
		}
		
		GLib.Type typeid = GLib.Type.from_name (DEFAULT_CLASSNAME);
		assert (typeid != 0);
	
		GLib.Object o = GLib.Object.new (typeid);
		GLib.Type otype = GLib.Type.from_instance (o);

		/* Created object must be an instance of our default class */
		assert (otype.name () == DEFAULT_CLASSNAME);

		GLib.ParamSpec[] pspecs = o.get_class ().list_properties ();
		/* There are 4 properties registered */
		assert (pspecs.length == 4);

		/* Check if our default property is installed for default class */
		GLib.ParamSpec pspec = o.get_class ().find_property (TITLE_PROPERTY_NAME);
		assert (pspec != null);

		/* Check if property setter and getter works correctly */
		string property_value = "Answer me These questions three, Ere the other side he see";
		o.set ("title-property", property_value);
		string tp;
		o.get ("title-property", out tp);
		assert (tp != null);
		assert (tp != "");
		assert (tp == property_value);
	});

	/* factory */
	Test.add_func ("/SchemaBuilder/factory", () => {
		GLib.print (MISS_IMPL);
	});

}

