noinst_LTLIBRARIES = libmidgard_vala.la

AM_CFLAGS = -I$(top_srcdir)/src/core -I$(top_srcdir)/src @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@

BUILT_SOURCES = libmidgard_vala.vala.stamp

libmidgard_vala_la_VALASOURCES = \
			  midgard_config.vala

libmidgard_vala_la_SOURCES = \
			libmidgard_vala.vala.stamp \
			$(libmidgard_vala_la_VALASOURCES:.vala=.c)

libmidgard_vala.vala.stamp: $(libmidgard_vala_la_VALASOURCES)
	$(VALAC) $(VALAFLAGS)  --use-header $(top_srcdir)/src/core/MidgardCore.vapi --vapidir=$(top_srcdir)/src/core -H $(top_srcdir)/src/core/midgard3.h -C $^
	touch $@

EXTRA_DIST = \
	     $(libmidgard_vala_la_VALASOURCES) \
	     libmidgard_vala.vala.stamp      

MAINTAINERCLEANFILES = \
		       midgard.h \
		       $(libmidgard_vala_la_VALASOURCES:.vala=.c) 
