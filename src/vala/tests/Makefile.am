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

midgardcr_test_VALASOURCES = example1.vala

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

