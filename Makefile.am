QUIET_GEN = $(Q:@=@echo ' GEN '$@;)

SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

lib_LTLIBRARIES = src/vala/libmidgardcr.la

if MIDGARD_DBUS_SUPPORT
dbus_pkgdatadir = $(DBUS_CONF_DIR)/dbus-1/system.d
dbus_pkgdata_DATA = midgard_dbus.conf
endif

nobase_dist_sysconf_DATA = conf.d/midgard.conf.example

dist_pkgdata_DATA = \
		    midgard_auth_types.xml \
	    	    midgard_dbus.conf \
	    	    conf.d/midgard.conf.example 

midgard_public_h = src/core/midgard3.h

EXTRA_DIST =  \
      	      midgard3.pc.in \
      	      m4/introspection.m4

midgard3_public_h = src/core/midgard3.h

src_vala_libmidgardcr_la_LIBADD = src/core/libmidgardcore.la

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

src_vala_libmidgardcr_la_CPPFLAGS = \
		      	      -I$(top_srcdir)/src -DG_LOG_DOMAIN=\"MidgardCR\"

AM_CFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/core @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@ 

src_vala_libmidgardcr_la_LDFLAGS = -version-info 2021:0:11 $(no_undefined)

src_vala_libmidgardcr_la_public_SOURCES = \
				    src/midgard.c 
 
pkgdatadir = $(datadir)/midgard3
pkglibdir = $(libdir)/midgard3
libmidgardincludedir = $(includedir)/midgard3/midgard
libmidgardinclude_HEADERS = $(midgard_public_h) src/midgard.h 

src_vala_libmidgardcr_la_SOURCES = $(src_vala_libmidgardcr_la_public_SOURCES)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = midgard3.pc

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
INTROSPECTION_CFLAGS += $(AM_CFLAGS)
introspection_sources = $(src_vala_libmidgardcr_la_public_SOURCES) $(midgard3_public_h)
MidgardCR-3.0.gir: $(lib_LTLIBRARIES)
MidgardCR_3_0_gir_INCLUDES = GObject-2.0 libxml2-2.0 Gda-4.0 
MidgardCR_3_0_gir_CFLAGS = $(AM_CFLAGS) `pkg-config --cflags libxml-2.0`
MidgardCR_3_0_gir_LIBS = $(lib_LTLIBRARIES) 
MidgardCR_3_0_gir_FILES = $(addprefix $(srcdir)/,$(introspection_sources))
MidgardCR_3_0_gir_PACKAGES = gobject-2.0 libxml-2.0 libgda-4.0
INTROSPECTION_GIRS = MidgardCR-3.0.gir
INTROSPECTION_LIBS += -lxml2
girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)
CLEANFILES = $(dist_gir_DATA) $(typelib_DATA)
endif

dist-hook:
	mkdir -p $(distdir)/src/vala
	mkdir -p $(distdir)/src/core
	mkdir -p $(distdir)/conf.d
	mkdir -p $(distdir)/m4
	cp conf.d/midgard.conf.example $(distdir)/conf.d
	cp configure.in $(distdir)/
	cp Makefile.am $(distdir)/
	cp src/Makefile.am $(distdir)/src/
	cp src/midgard.c $(distdir)/src/
	cp src/midgard.h $(distdir)/src/
	cp src/core/Makefile.am $(distdir)/src/core/
	cp src/core/*.c $(distdir)/src/core/
	cp src/core/*.h $(distdir)/src/core/
	cp src/vala/*.vala $(distdir)/src/vala/
	cp src/vala/Makefile.am $(distdir)/src/vala/

doc:
	doxygen

.PHONY: doc
