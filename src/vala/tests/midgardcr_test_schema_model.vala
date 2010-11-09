using MidgardCR;

void midgardcr_test_add_object_model_tests () {

	/* constructor */
	Test.add_func ("/ObjectModel/constructor", () => {
		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);
	});

	/* add model */
	Test.add_func ("/ObjectModel/add_model", () => {
		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_model = new MidgardCR.ObjectPropertyModel ("title", "string", "");
		assert (prop_model != null);
		
		model.add_model (prop_model);
	});

	/* get model by name */
	Test.add_func ("/ObjectModel/get_model_by_name", () => {	
		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);	

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_model = new MidgardCR.ObjectPropertyModel ("title", "string", "");
		assert (prop_model != null);
		
		model.add_model (prop_model);

		/* FAIL */
		MidgardCR.ObjectPropertyModel not_found_model = model.get_model_by_name ("notexists") as ObjectPropertyModel;
		assert (not_found_model == null);

		/* SUCCESS */
		unowned MidgardCR.ObjectPropertyModel found_model = model.get_model_by_name ("title") as ObjectPropertyModel;
		assert (found_model != null);
		assert (found_model.name == "title");
	});

	Test.add_func ("/ObjectModel/list_models", () => {
		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);
		string title = "title_property";
		string foo = "foo_property";
		
		/* FAIL */
		MidgardCR.Model[] empty_models = model.list_models ();
		assert (empty_models == null);

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_a_model = new MidgardCR.ObjectPropertyModel (title, "string", "");
		assert (prop_a_model != null);
	
		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_b_model = new MidgardCR.ObjectPropertyModel (foo, "string", "");
		assert (prop_b_model != null);
	
		model.add_model (prop_a_model).add_model (prop_b_model);
		
		/* SUCCESS */
		MidgardCR.Model[] models = model.list_models ();
		assert (models != null);
		assert (models.length == 2);
		if (models[0].name == title)
			assert (models[1].name == foo); 
		if (models[0].name == foo)
			assert (models[1].name == title); 
	});

	Test.add_func ("/ObjectModel/get_reflector", () => {
		GLib.print (MISS_IMPL);
	});

	Test.add_func ("/ObjectModel/is_valid", () => {
		MidgardCR.ObjectModel model = new MidgardCR.ObjectModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);
		string title = "title_property";
		string foo = "foo_property";
		
		/* FAIL */
		bool invalid_model = false;
		try {
			model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			invalid_model = true;
		}
		assert(invalid_model == true);

		/* SUCCESS */
		MidgardCR.ObjectPropertyModel prop_a_model = new MidgardCR.ObjectPropertyModel (title, "string", "");
		assert (prop_a_model != null);
		MidgardCR.ObjectPropertyModel prop_b_model = new MidgardCR.ObjectPropertyModel (foo, "string", "");
		assert (prop_b_model != null);

		model.add_model (prop_a_model).add_model (prop_b_model);
		bool valid_model = true;
		try {
			model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			valid_model = false;
		}
		assert (valid_model == true);	

		/* FAIL */
		MidgardCR.ObjectPropertyModel prop_c_model = new MidgardCR.ObjectPropertyModel (foo, "string", "");
		model.add_model (prop_c_model);
		assert (prop_c_model != null);

		bool invalid_model_duplicate = false;
		try {
			model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			invalid_model_duplicate = true;
		}
		assert (invalid_model_duplicate == true); 
	});
}

