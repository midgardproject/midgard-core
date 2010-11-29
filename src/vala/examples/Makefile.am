NULL=

AM_CFLAGS = \
	    $(GLIB_CFLAGS) \
	    $(LIBVALA_CFLAGS) \
	    $(MIDGARDCR_CFLAGS) \
	    $(NULL)

bin_PROGRAMS = \
	       example1 \
	       example2 \
	       example3 \
	       rdf_example
	       $(NULL)

BUILT_SOURCES = \
		example1.vala.stamp \
		example2.vala.stamp \
		example3.vala.stamp \
		rdf_example.vala.stamp
		$(NULL)

example1_VALASOURCES = example1.vala
example2_VALASOURCES = example2.vala
example3_VALASOURCES = example3.vala
rdf_example_VALASOURCES = rdf_example.vala

example1_SOURCES = \
		   $(example1_VALASOURCES:.vala=.c) \
		   $(NULL)

example2_SOURCES = \
		   $(example2_VALASOURCES:.vala=.c) \
		   $(NULL)

example3_SOURCES = \
		   $(example3_VALASOURCES:.vala=.c) \
		   $(NULL)

rdf_example_SOURCES = \
		      $(rdf_example_VALASOURCES:.vala=.c) \
		      $(NULL)

examples_LDADD = \
		 $(GLIB_LIBS) \
		 $(LIBVALA_LIBS) \
		 $(MIDGARDCR_LIBS) \
		 $(NULL)

example1_LDADD = \
		 $(examples_LDADD) \
		 $(NULL)
example2_LDADD = \
		 $(examples_LDADD) \
		 $(NULL)

example3_LDADD = \
		 $(examples_LDADD) \
		 $(NULL)

rdf_example_LDADD = \
		 $(examples_LDADD) \
		 $(NULL)

example1.vala.stamp: $(example1_VALASOURCES)
	$(VALAC) --save-temps --pkg MidgardCR-3.0 $^
	touch $@

example2.vala.stamp: $(example2_VALASOURCES)
	$(VALAC) -C --save-temps --pkg MidgardCR-3.0 $^
	touch $@

example3.vala.stamp: $(example3_VALASOURCES)
	$(VALAC) -C --save-temps --pkg MidgardCR-3.0 $^
	touch $@

rdf_example.vala.stamp: $(rdf_example_VALASOURCES)
	$(VALAC) -C --save-temps --pkg MidgardCR-3.0 $^
	touch $@
