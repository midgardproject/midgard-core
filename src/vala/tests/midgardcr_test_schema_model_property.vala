using MidgardCR;

void midgardcr_test_add_object_model_property_tests () {

	/* constructor */
	Test.add_func ("/ObjectModelProperty/constructor", () => {
		MidgardCR.ObjectModelProperty model = new MidgardCR.ObjectModelProperty (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);
	});

	/* add model */
	Test.add_func ("/ObjectModelProperty/add_model", () => {
		MidgardCR.ObjectModelProperty model = new MidgardCR.ObjectModelProperty (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);

		/* SUCCESS */
		MidgardCR.ObjectModelProperty prop_model = new MidgardCR.ObjectModelProperty ("title", "string", "");
		assert (prop_model != null);
		
		MidgardCR.ObjectModelProperty new_model = model.add_model (prop_model) as MidgardCR.ObjectModelProperty;
		assert (new_model == model);
	});

	/* get model by name */
	Test.add_func ("/ObjectModelProperty/get_model_by_name", () => {	
		MidgardCR.ObjectModelProperty model = new MidgardCR.ObjectModelProperty (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);

		/* SUCCESS */
		MidgardCR.ObjectModelProperty prop_model = new MidgardCR.ObjectModelProperty ("title", "string", "");
		assert (prop_model != null);
		
		model.add_model (prop_model);

		/* FAIL */
		MidgardCR.ObjectModelProperty not_found_model = model.get_model_by_name ("notexists") as ObjectModelProperty;
		assert (not_found_model == null);

		/* SUCCESS */
		MidgardCR.ObjectModelProperty found_model = model.get_model_by_name ("title") as ObjectModelProperty;
		assert (found_model != null);
		assert (found_model.name == "title");
	});

	Test.add_func ("/ObjectModelProperty/list_models", () => {
		MidgardCR.ObjectModelProperty model = new MidgardCR.ObjectModelProperty (TITLE_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == TITLE_PROPERTY_NAME);
		
		/* FAIL */
		MidgardCR.Model[] empty_models = model.list_models ();
		assert (empty_models == null);

		/* SUCCESS */
		MidgardCR.ObjectModelProperty prop_a_model = new MidgardCR.ObjectModelProperty ("title", "string", "");
		assert (prop_a_model != null);
		
		model.add_model (prop_a_model);
		
		/* SUCCESS */
		MidgardCR.Model[] models = model.list_models ();
		assert (models != null);
		assert (models.length == 1);
		assert (models[0].name == "title"); 
	});

	Test.add_func ("/ObjectModelProperty/get_reflector", () => {
		GLib.print (MISS_IMPL);
	});

	Test.add_func ("/ObjectModelProperty/is_valid", () => {
		MidgardCR.ObjectModelProperty model = new MidgardCR.ObjectModelProperty (TITLE_PROPERTY_NAME, "string", "");
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
		MidgardCR.ObjectModelProperty prop_a_model = new MidgardCR.ObjectModelProperty ("title", "string", "");
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
		MidgardCR.ObjectModelProperty prop_c_model = new MidgardCR.ObjectModelProperty ("foo", "string", "");
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

