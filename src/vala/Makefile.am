noinst_LTLIBRARIES = libmidgardvala.la

AM_CFLAGS = -I$(top_srcdir)/src/core -I$(top_srcdir)/src @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@

BUILT_SOURCES = libmidgardvala.vala.stamp

libmidgardvala_la_VALASOURCES = \
			  midgard_config.vala \
			  midgard_executable.vala \
			  midgard_namespace.vala \
			  midgard_profiler.vala \
			  midgard_query.vala \
			  midgard_schema_model_reflector.vala \
			  midgard_schema_model.vala \
			  midgard_schema_object.vala \
			  midgard_storage_manager_sql.vala \
			  midgard_storage.vala \
			  midgard_workspace.vala

libmidgardvala_la_SOURCES = \
			libmidgardvala.vala.stamp \
			$(libmidgardvala_la_VALASOURCES:.vala=.c)

libmidgardvala_la_LDFLAGS = -module -export-dynamic
libmidgardvala_la_LIBADD = @MIDGARD_LIBS@ 

libmidgardvala.vala.stamp: $(libmidgardvala_la_VALASOURCES)
	$(VALAC) $(VALAFLAGS)  --use-header $(top_srcdir)/src/core/MidgardCore.vapi --vapidir=$(top_srcdir)/src/core -H $(top_srcdir)/src/core/midgard3.h -C $^
	touch $@

EXTRA_DIST = \
	     $(libmidgardvala_la_VALASOURCES) \
	     libmidgardvala.vala.stamp 		

CLEANFILES = \
	     libmidgardvala.vala.stamp \
	     $(libmidgardvala_la_VALASOURCES:.vala=.c) \
	     $(top_srcdir)/src/core/midgard3.h
		
