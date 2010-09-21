using MidgardCR;

const string DEFAULT_CLASSNAME = "CRFoo";

void midgardcr_test_add_schema_model_tests () {

	/* constructor */
	Test.add_func ("/SchemaModel/constructor", () => {
		MidgardCR.SchemaModel model = new MidgardCR.SchemaModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);
	});

	/* add model */
	Test.add_func ("/SchemaModel/add_model", () => {
		MidgardCR.SchemaModel model = new MidgardCR.SchemaModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);

		/* SUCCESS */
		MidgardCR.SchemaModelProperty prop_model = new MidgardCR.SchemaModelProperty ("title", "string", "");
		assert (prop_model != null);
		
		model.add_model (prop_model);
	});

	/* get model by name */
	Test.add_func ("/SchemaModel/get_model_by_name", () => {	
		MidgardCR.SchemaModel model = new MidgardCR.SchemaModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);
		string title = "title_property";

		/* SUCCESS */
		MidgardCR.SchemaModelProperty prop_model = new MidgardCR.SchemaModelProperty (title, "string", "");
		assert (prop_model != null);
		
		model.add_model (prop_model);

		/* FAIL */
		MidgardCR.SchemaModelProperty not_found_model = model.get_model_by_name ("notexists") as SchemaModelProperty;
		assert (not_found_model == null);

		/* SUCCESS */
		MidgardCR.SchemaModelProperty found_model = model.get_model_by_name (title) as SchemaModelProperty;
		assert (found_model != null);
		assert (found_model.name == title);
	});

	Test.add_func ("/SchemaModel/list_models", () => {
		MidgardCR.SchemaModel model = new MidgardCR.SchemaModel (DEFAULT_CLASSNAME);
		assert (model != null);
		assert (model.name == DEFAULT_CLASSNAME);
		string title = "title_property";
		string foo = "foo_property";
		
		/* FAIL */
		MidgardCR.Model[] empty_models = model.list_models ();
		assert (empty_models == null);

		/* SUCCESS */
		MidgardCR.SchemaModelProperty prop_a_model = new MidgardCR.SchemaModelProperty (title, "string", "");
		assert (prop_a_model != null);
	
		/* SUCCESS */
		MidgardCR.SchemaModelProperty prop_b_model = new MidgardCR.SchemaModelProperty (foo, "string", "");
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

	Test.add_func ("/SchemaModel/get_reflector", () => {
		GLib.print (MISS_IMPL);
	});

	Test.add_func ("/SchemaModel/is_valid", () => {
		GLib.print (MISS_IMPL);
	});
}

