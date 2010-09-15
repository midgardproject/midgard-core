noinst_LTLIBRARIES = libmidgardcr.la

AM_CFLAGS = -I$(top_srcdir)/src/core -I$(top_srcdir)/src @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@

BUILT_SOURCES = libmidgardcr.vala.stamp

midgard3_header = $(top_srcdir)/src/core/midgard3.h

libmidgardcr_la_VALASOURCES = \
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

libmidgardcr_la_SOURCES = \
			libmidgardcr.vala.stamp \
			$(libmidgardcr_la_VALASOURCES:.vala=.c)

libmidgardcr_la_LDFLAGS = -module -export-dynamic
libmidgardcr_la_LIBADD = @MIDGARD_LIBS@ 

libmidgardcr.vala.stamp: $(libmidgardcr_la_VALASOURCES)
	$(VALAC) $(VALAFLAGS)  --use-header $(top_srcdir)/src/core/MidgardCRCore.vapi --vapidir=$(top_srcdir)/src/core -H $(midgard3_header) -C $^
	touch $@

EXTRA_DIST = \
	     $(libmidgardcr_la_VALASOURCES) \
	     libmidgardcr.vala.stamp 		

CLEANFILES = \
	     libmidgardcr.vala.stamp \
	     $(libmidgardcr_la_VALASOURCES:.vala=.c) 	
		
