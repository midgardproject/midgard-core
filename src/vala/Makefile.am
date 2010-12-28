NULL = 

noinst_LTLIBRARIES = libmidgardvala.la

AM_CFLAGS = \
	    -I$(top_srcdir)/src/core \
	    @MIDGARD_CFLAGS@ \
	    $(GLIB_CFLAGS) \
	    $(GDA_CFLAGS) 

AM_LDFLAGS = @MIDGARD_LIBS@

midgardcr_header = $(top_srcdir)/src/core/midgardcr.h
midgardcr_vapi_file = MidgardCR-3.0.vapi

libmidgardvala_la_SOURCES = \
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
			  sql_content_manager.vala \
			  sql_model_manager.vala \
			  sql_table_model.vala \
			  sql_column_model.vala \
			  sql_query_storage.vala \
			  sql_query_constraint.vala \
			  sql_query_constraint_group.vala \
			  sql_query_select.vala \
			  sql_namespace_manager.vala \
			  sql_query_manager.vala \
			  sql_transaction.vala \
			  rdf_generic_object.vala \
			  rdf_storage_manager.vala \
			  rdf_sql_storage_manager.vala \
			  rdf_sql_content_manager.vala \
			  rdf_sql_query_select.vala \
			  rdf_sql_query_storage.vala \
			  rdf_sql_query_manager.vala \
			  rdf_sql_namespace_manager.vala \
			  $(NULL)
			  
libmidgardvala_la_LDFLAGS = -module -export-dynamic
libmidgardvala_la_LIBADD = @MIDGARD_LIBS@ 

EXTRA_DIST = \
	     $(midgardcr_vapi_file) \
	     examples/autogen.sh \
	     examples/configure.in \
	     examples/Makefile.am \
	     examples/example1.vala \
	     examples/example2.vala \
	     examples/example3.vala \
	     examples/rdf_example.vala \
	     $(NULL)

VALAFLAGS = --vapi=$(midgardcr_vapi_file) --vapidir=$(top_srcdir)/src/core --pkg=MidgardCRCore  -H $(midgardcr_header) 

CLEANFILES = \
	     $(midgardcr_vapi_file) \
	     $(midgardcr_header) \
	     libmidgardvala_la_vala.stamp \
	     $(libmidgardvala_la_SOURCES:.vala=.c)
