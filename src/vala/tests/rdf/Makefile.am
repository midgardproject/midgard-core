NULL=

AM_CFLAGS = \
	    $(GLIB_CFLAGS) \
	    $(LIBVALA_CFLAGS) \
	    $(MIDGARDCR_CFLAGS) \
	    $(NULL)

bin_PROGRAMS = \
	       midgardcr_test_rdf \
	       $(NULL)

BUILT_SOURCES = \
		midgardcr_test_rdf.vala.stamp \
		$(NULL)

midgardcr_test_rdf_VALASOURCES = \
			     midgardcr_test_rdf.vala \
			     rdf_sql_storage_manager.vala \
			     rdf_sql_namespace_manager.vala \
			     rdf_sql_model_manager.vala \
			     rdf_sql_content_manager.vala \
			     rdf_sql_object_manager.vala \
			     rdf_sql_query_storage.vala \
			     rdf_sql_query_select.vala \
			     $(NULL)

midgardcr_test_rdf_SOURCES = \
			 $(midgardcr_test_rdf_VALASOURCES:.vala=.c) \
			 $(NULL)

midgardcr_test_rdf_LDADD = \
		       $(GLIB_LIBS) \
		       $(LIBVALA_LIBS) \
		       $(MIDGARDCR_LIBS) \
		       $(NULL)

midgardcr_test_rdf.vala.stamp: $(midgardcr_test_rdf_VALASOURCES)
	$(VALAC) -C --save-temps --pkg MidgardCR-3.0 $^
	touch $@

CLEANFILES = \
	     $(midgardcr_test_rdf_VALASOURCES:.vala=.c) \
	     midgardcr_test_rdf.vala.stamp \
	     $(NULL)
