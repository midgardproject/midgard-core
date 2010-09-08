noinst_LTLIBRARIES = libmidgardcore.la

AM_CFLAGS = -I$(top_srcdir)/src @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@

libmidgardcore_la_SOURCES = \
			    midgard_core_config.c

libmidgardcore_la_LDFLAGS = -module -export-dynamic
libmidgardcore_la_LIBADD = @MIDGARD_LIBS@
