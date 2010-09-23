NULL = 

noinst_LTLIBRARIES = libmidgardcore.la

AM_CFLAGS = -I$(top_srcdir)/src @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@

libmidgardcore_la_SOURCES = \
			    midgard_cr_core_config.c \
			    midgard_cr_core_sqlstoragemanager.c \
			    midgard_cr_core_storage_sql.c \
			    midgard_cr_core_type.c \
			    midgard_cr_core_timestamp.c \
			    midgard_cr_core_schema_builder.c \
			    $(NULL)

libmidgardcore_la_LDFLAGS = -module -export-dynamic
libmidgardcore_la_LIBADD = @MIDGARD_LIBS@

EXTRA_DIST = \
	     MidgardCRCore.vapi
