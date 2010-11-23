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
	       $(NULL)

BUILT_SOURCES = \
		example1.vala.stamp \
		example2.vala.stamp \
		example3.vala.stamp \
		$(NULL)

example1_VALASOURCES = example1.vala
example2_VALASOURCES = example2.vala
example3_VALASOURCES = example3.vala

example1_SOURCES = \
		   $(example1_VALASOURCES:.vala=.c) \
		   $(NULL)

example2_SOURCES = \
		   $(example2_VALASOURCES:.vala=.c) \
		   $(NULL)

example3_SOURCES = \
		   $(example3_VALASOURCES:.vala=.c) \
		   $(NULL)

example1_LDADD = \
		 $(GLIB_LIBS) \
		 $(LIBVALA_LIBS) \
		 $(MIDGARDCR_LIBS) \
		 $(NULL)
example2_LDADD = \
		 $(GLIB_LIBS) \
		 $(LIBVALA_LIBS) \
		 $(MIDGARDCR_LIBS) \
		 $(NULL)

example3_LDADD = \
		 $(GLIB_LIBS) \
		 $(LIBVALA_LIBS) \
		 $(MIDGARDCR_LIBS) \
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
