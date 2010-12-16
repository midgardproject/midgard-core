NULL=

AM_CFLAGS = \
	    $(GLIB_CFLAGS) \
	    $(LIBVALA_CFLAGS) \
	    $(MIDGARDCR_CFLAGS) \
	    $(NULL)

bin_PROGRAMS = \
	       midgardcr_test \
	       $(NULL)

BUILT_SOURCES = \
		midgardcr_test.vala.stamp \
		$(NULL)

midgardcr_test_VALASOURCES = \
			     midgardcr_test.vala \
			     midgardcr_test_config.vala \
			     midgardcr_test_sql_storage_manager.vala \
			     midgardcr_test_schema_model.vala \
			     midgardcr_test_schema_model_property.vala \
			     schema_builder.vala \
			     sql_model_manager.vala \
			     $(NULL)

midgardcr_test_SOURCES = \
		   $(midgardcr_test_VALASOURCES:.vala=.c) \
		   $(NULL)

midgardcr_test_LDADD = \
		 $(GLIB_LIBS) \
		 $(LIBVALA_LIBS) \
		 $(MIDGARDCR_LIBS) \	
		 $(NULL)

midgardcr_test.vala.stamp: $(midgardcr_test_VALASOURCES)
	$(VALAC) -C --save-temps --pkg MidgardCR-3.0 $^
	touch $@

