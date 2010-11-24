NULL = 

noinst_LTLIBRARIES = libmidgardcore.la

AM_CFLAGS = -I$(top_srcdir)/src @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@

libmidgardcore_la_SOURCES = \
			    midgard_cr_core_config.c \
			    midgard_cr_core_config.h \
			    midgard_cr_core_sqlstoragemanager.c \
			    midgard_cr_core_sqlstoragemanager.h \
			    midgard_cr_core_storage_sql.c \
			    midgard_cr_core_storage_sql.h \
			    midgard_cr_core_type.c \
			    midgard_cr_core_type.h \
			    midgard_cr_core_timestamp.c \
			    midgard_cr_core_timestamp.h \
			    midgard_cr_core_object_builder.c \
			    midgard_cr_core_object_builder.h \
			    midgard_cr_core_schema_object.c \
			    midgard_cr_core_schema_object.h \
			    midgard_cr_core_query_constraint.c \
			    midgard_cr_core_query_constraint.h \
			    midgard_cr_core_query_constraint_group.c \
			    midgard_cr_core_query_constraint_group.h \
			    midgard_cr_core_query_constraint_simple.c \
			    midgard_cr_core_query_constraint_simple.h \
			    midgard_cr_core_query_executor.c \
			    midgard_cr_core_query_executor.h \
			    midgard_cr_core_query_holder.c \
			    midgard_cr_core_query_holder.h \
			    midgard_cr_core_query_private.c \
			    midgard_cr_core_query_private.h \
			    midgard_cr_core_query_property.c \
			    midgard_cr_core_query_property.h \
			    midgard_cr_core_query_select.c \
			    midgard_cr_core_query_select.h \
			    midgard_cr_core_query_storage.c \
			    midgard_cr_core_query_storage.h \
			    midgard_cr_core_query_value.c \
			    midgard_cr_core_query_value.h \
			    midgard_cr_core_sql_storage_content_manager.c \
			    midgard_cr_core_sql_storage_content_manager.h \
			    midgard_cr_core_sql_storage_model_manager.c \
			    midgard_cr_core_sql_storage_model_manager.h \
			    midgard_cr_core_guid.c \
			    midgard_cr_core_guid.h \
			    midgard_cr_core_uuid.c \
			    midgard_cr_core_uuid.h \
			    midgard_cr_core_md5.c \
			    midgard_cr_core_md5.h \
			    $(NULL)

libmidgardcore_la_LDFLAGS = -module -export-dynamic
libmidgardcore_la_LIBADD = @MIDGARD_LIBS@

EXTRA_DIST = \
	     MidgardCRCore.vapi
