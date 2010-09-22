using MidgardCR;

void midgardcr_test_add_schema_model_property_tests () {

	/* constructor */
	Test.add_func ("/SchemaModelProperty/constructor", () => {
		MidgardCR.SchemaModelProperty model = new MidgardCR.SchemaModelProperty (DEFAULT_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == DEFAULT_PROPERTY_NAME);
	});

	/* add model */
	Test.add_func ("/SchemaModelProperty/add_model", () => {
		MidgardCR.SchemaModelProperty model = new MidgardCR.SchemaModelProperty (DEFAULT_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == DEFAULT_PROPERTY_NAME);

		/* SUCCESS */
		MidgardCR.SchemaModelProperty prop_model = new MidgardCR.SchemaModelProperty ("title", "string", "");
		assert (prop_model != null);
		
		MidgardCR.SchemaModelProperty new_model = model.add_model (prop_model) as MidgardCR.SchemaModelProperty;
		assert (new_model == model);
	});

	/* get model by name */
	Test.add_func ("/SchemaModelProperty/get_model_by_name", () => {	
		MidgardCR.SchemaModelProperty model = new MidgardCR.SchemaModelProperty (DEFAULT_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == DEFAULT_PROPERTY_NAME);

		/* SUCCESS */
		MidgardCR.SchemaModelProperty prop_model = new MidgardCR.SchemaModelProperty ("title", "string", "");
		assert (prop_model != null);
		
		model.add_model (prop_model);

		/* FAIL */
		MidgardCR.SchemaModelProperty not_found_model = model.get_model_by_name ("notexists") as SchemaModelProperty;
		assert (not_found_model == null);

		/* SUCCESS */
		MidgardCR.SchemaModelProperty found_model = model.get_model_by_name ("title") as SchemaModelProperty;
		assert (found_model != null);
		assert (found_model.name == "title");
	});

	Test.add_func ("/SchemaModelProperty/list_models", () => {
		MidgardCR.SchemaModelProperty model = new MidgardCR.SchemaModelProperty (DEFAULT_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == DEFAULT_PROPERTY_NAME);
		
		/* FAIL */
		MidgardCR.Model[] empty_models = model.list_models ();
		assert (empty_models == null);

		/* SUCCESS */
		MidgardCR.SchemaModelProperty prop_a_model = new MidgardCR.SchemaModelProperty ("title", "string", "");
		assert (prop_a_model != null);
		
		model.add_model (prop_a_model);
		
		/* SUCCESS */
		MidgardCR.Model[] models = model.list_models ();
		assert (models != null);
		assert (models.length == 1);
		assert (models[0].name == "title"); 
	});

	Test.add_func ("/SchemaModelProperty/get_reflector", () => {
		GLib.print (MISS_IMPL);
	});

	Test.add_func ("/SchemaModelProperty/is_valid", () => {
		MidgardCR.SchemaModelProperty model = new MidgardCR.SchemaModelProperty (DEFAULT_PROPERTY_NAME, "string", "");
		assert (model != null);
		assert (model.name == DEFAULT_PROPERTY_NAME);

		/* FAIL */
		bool invalid_model = true;
		try {
			model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			invalid_model = false;
		}
		assert (invalid_model == true);

		/* SUCCESS */
		MidgardCR.SchemaModelProperty prop_a_model = new MidgardCR.SchemaModelProperty ("title", "string", "");
		assert (prop_a_model != null);

		model.add_model (prop_a_model);
		bool valid_model = true;
		try {
			model.is_valid ();
		} catch (MidgardCR.ValidationError e) {
			valid_model = false;
		}
		assert (valid_model == true);	

		/* FAIL */
		MidgardCR.SchemaModelProperty prop_c_model = new MidgardCR.SchemaModelProperty ("foo", "string", "");
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

