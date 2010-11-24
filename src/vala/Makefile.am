NULL = 

noinst_LTLIBRARIES = libmidgardvala.la

AM_CFLAGS = -I$(top_srcdir)/src/core  @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@

BUILT_SOURCES = libmidgardvala.vala.stamp

midgardcr_header = $(top_srcdir)/src/core/midgardcr.h

libmidgardvala_la_VALASOURCES = \
			  config.vala \
			  executable.vala \
			  namespace.vala \
			  profiler.vala \
			  sql_profiler.vala \
			  query.vala \
			  midgard_schema_model_reflector.vala \
			  model.vala \
			  object_model.vala \
			  object_property_model.vala \
			  object_builder.vala \
			  storable.vala \
			  sql_storage_manager.vala \
			  storage.vala \
			  workspace.vala \
			  query_property.vala \
			  query_value.vala \
			  sql_storage_content_manager.vala \
			  sql_storage_model_manager.vala \
			  sql_table_model.vala \
			  sql_column_model.vala \
			  sql_query_storage.vala \
			  sql_query_constraint.vala \
			  sql_query_select.vala \
			  rdf_generic_object.vala \
			  rdf_storage_manager.vala \
			  rdf_sql_storage_manager.vala \
			  rdf_sql_content_manager.vala \
			  $(NULL)

libmidgardvala_la_SOURCES = \
			  libmidgardvala.vala.stamp \
  			  $(libmidgardvala_la_VALASOURCES:.vala=.c)

libmidgardvala_la_LDFLAGS = -module -export-dynamic
libmidgardvala_la_LIBADD = @MIDGARD_LIBS@ 

libmidgardvala.vala.stamp: $(libmidgardvala_la_VALASOURCES)
	$(VALAC) $(VALAFLAGS)  --vapi=MidgardCR-3.0.vapi --use-header $(top_srcdir)/src/core/MidgardCRCore.vapi --vapidir=$(top_srcdir)/src/core -H $(midgardcr_header) -C $^
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
