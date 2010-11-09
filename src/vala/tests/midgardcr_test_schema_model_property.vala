using MidgardCR;

void midgardcr_test_add_object_model_property_tests () {

	/* constructor */
	Test.add_func ("/ObjectPropertyModel/constructor", () => {
		MidgardCR.ObjectPropertyModel model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);
	});

	/* add model */
	Test.add_func ("/ObjectPropertyModel/add_model", () => {
		MidgardCR.ObjectPropertyModel model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_model = new MidgardCR.ObjectPropertyModel ("title", "string", "");
		assert (prop_model != null);
		
		MidgardCR.ObjectPropertyModel new_model = model.add_model (prop_model) as MidgardCR.ObjectPropertyModel;
		assert (new_model == model);

		/* FAIL */
		bool valid = true;
		try {
			new_model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			/* Do not print any warning */
			valid = false;
		}
		assert (valid == false);
	});

	/* get model by name */
	Test.add_func ("/ObjectPropertyModel/get_model_by_name", () => {	
		MidgardCR.ObjectPropertyModel model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_model = new MidgardCR.ObjectPropertyModel ("title", "string", "");
		assert (prop_model != null);
		
		model.add_model (prop_model);

		/* FAIL */
		MidgardCR.ObjectPropertyModel not_found_model = model.get_model_by_name ("notexists") as ObjectPropertyModel;
		assert (not_found_model == null);

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel found_model = model.get_model_by_name ("title") as ObjectPropertyModel;
		assert (found_model != null);
		assert (found_model.name == "title");
	});

	Test.add_func ("/ObjectPropertyModel/list_models", () => {
		MidgardCR.ObjectPropertyModel model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);
		
		/* FAIL */
		MidgardCR.Model[] empty_models = model.list_models ();
		assert (empty_models == null);

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_a_model = new MidgardCR.ObjectPropertyModel ("title", "string", "");
		assert (prop_a_model != null);
		
		model.add_model (prop_a_model);
		
		/* SUCCESS */
		MidgardCR.Model[] models = model.list_models ();
		assert (models != null);
		assert (models.length == 1);
		assert (models[0].name == "title"); 
	});

	Test.add_func ("/ObjectPropertyModel/get_reflector", () => {
		GLib.print (MISS_IMPL);
	});

	Test.add_func ("/ObjectPropertyModel/is_valid", () => {
		MidgardCR.ObjectPropertyModel model = new MidgardCR.ObjectPropertyModel (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);

		/* FAIL */
		bool invalid_model = true;
		try {
			model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			invalid_model = false;
		}
		assert (invalid_model == true);

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_a_model = new MidgardCR.ObjectPropertyModel ("title", "string", "");
		assert (prop_a_model != null);

		MidgardCR.ObjectModel cmodel = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
                assert (cmodel != null);
                assert (cmodel.name == DEFAULT_CLASSNAME);
		prop_a_model.parent = cmodel;

		model.add_model (prop_a_model);
		bool valid_model = true;
		try {
			model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			valid_model = false;
		}
		assert (valid_model == true);	

		/* FAIL */
		MidgardCR.ObjectPropertyModel prop_c_model = new MidgardCR.ObjectPropertyModel ("foo", "string", "");
		model.add_model (prop_c_model);
		assert (prop_c_model != null);

		bool invalid_model_many = false;
		try {
			model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			invalid_model_many = true;
		}
		assert (invalid_model_many == true); 
	});
}

