using MidgardCR;

void midgardcr_test_add_schema_builder_tests () {

	/* constructor */
	Test.add_func ("/ObjectManager/constructor", () => {
		MidgardCR.ObjectManager builder = new MidgardCR.ObjectManager ();
		assert (builder != null);
	});

	/* register model */
	Test.add_func ("/ObjectManager/register_type_from_model", () => {
		MidgardCR.ObjectManager builder = new MidgardCR.ObjectManager ();
		assert (builder != null);

		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
                assert (model != null);
                assert (model.name == DEFAULT_CLASSNAME);

                /* SUCCESS */
                MidgardCR.ObjectPropertyModel prop_model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model);

		/* SUCCESS */
		try {
			builder.register_type_from_model (model);		
		} catch (MidgardCR.ValidationError e) {
			GLib.warning (e.message);
		} catch (MidgardCR.ObjectManagerError e) {
			GLib.warning (e.message);
		} 

		/* FAIL */
		try {
			builder.register_type_from_model (model);
		} catch (MidgardCR.ValidationError e) {
			if (!(e is MidgardCR.ValidationError.NAME_DUPLICATED))
				GLib.warning (e.message);
		} catch (MidgardCR.ObjectManagerError e) {
			GLib.warning (e.message);
		}	
	});

	/* execute */
	Test.add_func ("/ObjectManager/execute", () => {
		MidgardCR.ObjectManager builder = new MidgardCR.ObjectManager ();
		assert (builder != null);

		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
                assert (model != null);
                assert (model.name == DEFAULT_CLASSNAME);

                /* SUCCESS */
                MidgardCR.ObjectPropertyModel prop_model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
                assert (prop_model != null);

                model.add_model (prop_model);

		/* SUCCESS */
		try {
			builder.register_type_from_model (model);		
		} catch (MidgardCR.ValidationError e) {
			GLib.warning (e.message);
		} catch (MidgardCR.ObjectManagerError e) {
			GLib.warning (e.message);
		} 

		/* FAIL */
		try {
			builder.register_type_from_model (model);
		} catch (MidgardCR.ValidationError e) {
			if (!(e is MidgardCR.ValidationError.NAME_DUPLICATED))
				GLib.warning (e.message);
		} catch (MidgardCR.ObjectManagerError e) {
			GLib.warning (e.message);
		} 	

		/* SUCCESS */
		try {
			builder.execute ();
		} catch (MidgardCR.ExecutableError e) {
			GLib.warning (e.message);
		}
		
		GLib.Type storable_type = GLib.Type.from_name ("MidgardCRRepositoryObject");
		GLib.Type typeid = GLib.Type.from_name (DEFAULT_CLASSNAME);
		assert (typeid != 0);
		assert (typeid.is_a (storable_type));

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
	Test.add_func ("/ObjectManager/factory", () => {
		MidgardCR.ObjectManager builder = new MidgardCR.ObjectManager ();
		assert (builder != null);
		RepositoryObject obj = null;

		/* SUCCESS */
		try {
			obj = builder.factory (DEFAULT_CLASSNAME);
		} catch (ValidationError e) {
			GLib.warning (e.message);
		} 	 

		GLib.Type type = obj.get_type ();
		GLib.Type repo_object_type = GLib.Type.from_name ("MidgardCRRepositoryObject");
		assert (type == GLib.Type.from_name (DEFAULT_CLASSNAME));
		assert (type.is_a (repo_object_type));

		/* Check if our default property is installed for default class */
		GLib.ParamSpec pspec = obj.get_class ().find_property (TITLE_PROPERTY_NAME);
		assert (pspec != null);
		/* Check metadata property */
		pspec = obj.get_class ().find_property ("metadata");
		assert (pspec != null);
	});
}

