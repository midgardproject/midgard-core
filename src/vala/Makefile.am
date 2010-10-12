NULL = 

noinst_LTLIBRARIES = libmidgardvala.la

AM_CFLAGS = -I$(top_srcdir)/src/core -I$(top_srcdir)/src @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@

BUILT_SOURCES = libmidgardvala.vala.stamp

midgard3_header = $(top_srcdir)/src/core/midgard3.h

libmidgardvala_la_VALASOURCES = \
			  midgard_config.vala \
			  executable.vala \
			  midgard_namespace.vala \
			  profiler.vala \
			  sql_profiler.vala \
			  midgard_query.vala \
			  midgard_schema_model_reflector.vala \
			  model.vala \
			  object_model.vala \
			  schema_builder.vala \
			  midgard_schema_object.vala \
			  midgard_storage_manager_sql.vala \
			  midgard_storage.vala \
			  midgard_workspace.vala \
			  sql_storage_model_manager.vala \
			  sql_storage_model.vala \
			  sql_storage_model_property.vala \
			  $(NULL)

libmidgardvala_la_SOURCES = \
			  libmidgardvala.vala.stamp \
  			  $(libmidgardvala_la_VALASOURCES:.vala=.c)

libmidgardvala_la_LDFLAGS = -module -export-dynamic
libmidgardvala_la_LIBADD = @MIDGARD_LIBS@ 

libmidgardvala.vala.stamp: $(libmidgardvala_la_VALASOURCES)
	$(VALAC) $(VALAFLAGS)  --vapi=MidgardCR-3.0.vapi --use-header $(top_srcdir)/src/core/MidgardCRCore.vapi --vapidir=$(top_srcdir)/src/core -H $(midgard3_header) -C $^
	touch $@

EXTRA_DIST = \
	     $(libmidgardvala_la_VALASOURCES) 

CLEANFILES = \
	     libmidgardvala.vala.stamp \
	     $(libmidgardvala_la_VALASOURCES:.vala=.c) 	

dist-hook:
	cp *.vala $(distdir)
	cp Makefile.am $(distdir)
	cp Makefile.in $(distdir)
	cp MidgardCR-3.0.vapi $(distdir)

distdir: dist-hook
