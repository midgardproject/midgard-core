QUIET_GEN = $(Q:@=@echo ' GEN '$@;)

SUBDIRS = src 

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

lib_LTLIBRARIES = src/libmidgardcr.la

nobase_dist_sysconf_DATA = conf.d/midgard.conf.example

dist_pkgdata_DATA = \
	    	    conf.d/midgard.conf.example 

EXTRA_DIST =  \
      	      midgardcr.pc.in \
      	      m4/introspection.m4

midgardcr_public_h = src/core/midgardcr.h

src_libmidgardcr_la_LIBADD = \
			     src/core/libmidgardcore.la \
			     src/vala/libmidgardvala.la

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

src_libmidgardcr_la_CPPFLAGS = \
		      	      -I$(top_srcdir)/src -I$(top_srcdir)/src/core -DG_LOG_DOMAIN=\"MidgardCR\"

AM_CFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/core @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@ 

src_libmidgardcr_la_LDFLAGS = -version-info @MIDGARD_ABI_MAJOR@:0:0 $(no_undefined)

src_libmidgardcr_la_public_SOURCES = \
				    src/midgard.c 

pkgdatadir = $(datadir)/midgardcr
pkglibdir = $(libdir)/midgardcr
libmidgardincludedir = $(includedir)/midgard/midgardcr/
libmidgardinclude_HEADERS = $(midgardcr_public_h) 
midgardcrincludedir = $(includedir)/midgard
midgardcrinclude_HEADERS = src/midgardcr.h 

src_libmidgardcr_la_SOURCES = $(src_libmidgardcr_la_public_SOURCES)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = midgardcr.pc

vapidir = $(datadir)/vala/vapi
vapi_DATA = \
	    src/vala/MidgardCR-3.0.vapi \
	    $(NULL)

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
INTROSPECTION_CFLAGS += $(AM_CFLAGS)
introspection_sources = $(src_libmidgardcr_la_public_SOURCES)
MidgardCR-3.0.gir: $(lib_LTLIBRARIES)
MidgardCR_3_0_gir_INCLUDES = GObject-2.0 Gda-4.0
MidgardCR_3_0_gir_CFLAGS = $(AM_CFLAGS)
MidgardCR_3_0_gir_LIBS = $(lib_LTLIBRARIES) 
MidgardCR_3_0_gir_FILES = $(addprefix $(srcdir)/,$(introspection_sources))
MidgardCR_3_0_gir_PACKAGES = gobject-2.0 libgda-4.0
introspection_file = src/vala/MidgardCR-3.0.gir
girdir = $(datadir)/gir-1.0
gir_DATA = $(introspection_file)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(introspection_file:.gir=.typelib)
CLEANFILES = $(dist_gir_DATA) $(typelibs_DATA)
endif

doc:
	doxygen

.PHONY: doc
