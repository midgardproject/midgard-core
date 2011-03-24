## Process this file with automake to produce Makefile.in

NULL = 

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

lib_LTLIBRARIES = \
	src/libmidgard2.la

bin_PROGRAMS = \
	test/midgard2-schema \
	test/midgard2-query

if MIDGARD_DBUS_SUPPORT
dbus_pkgdatadir = $(DBUS_CONF_DIR)/dbus-1/system.d
dbus_pkgdata_DATA = midgard_dbus.conf
endif

test_midgard2_schema_SOURCES = test/midgard-schema.c 
test_midgard2_query_SOURCES = test/midgard-query.c

noinst_PROGRAMS = $(TESTS)

nobase_dist_sysconf_DATA = \
        conf.d/midgard.conf.example

dist_pkgdata_DATA = MidgardObjects.xml \
	midgard_auth_types.xml \
	midgard_dbus.conf \
	conf.d/midgard.conf.example 

schemadir = $(datadir)/midgard2/schema
schema_DATA = schema/README.txt
viewsdir = $(datadir)/midgard2/views
views_DATA = views/README-views.txt

midgard_public_h = \
	src/midgard_config.h \
	src/midgard_config_auto.h \
	src/midgard_dbobject.h \
	src/midgard_defs.h \
	src/midgard_dbus.h \
	src/midgard_executable.h \
	src/midgard_key_config_common.h \
	src/midgard_object.h \
 	src/query_builder.h \
 	src/midgard_metadata.h \
	src/midgard_timestamp.h \
	src/uuid.h \
	src/guid.h \
	src/midgard_datatypes.h \
	src/midgard_type.h \
	src/midgard_quota.h \
	src/midgard_reflection_property.h \
	src/midgard_schema.h \
	src/midgard_connection.h \
	src/midgard_error.h \
	src/midgard_collector.h \
	src/midgard_object_parameter.h \
	src/midgard_object_attachment.h \
	src/midgard_query_constraint.h \
	src/midgard_query_executor.h \
	src/midgard_query_constraint_group.h \
	src/midgard_query_holder.h \
	src/midgard_query_value.h \
	src/midgard_query_property.h \
	src/midgard_query_select.h \
	src/midgard_query_storage.h \
	src/midgard_query_constraint_simple.h \
	src/midgard_reflector_object.h \
	src/midgard_reflector_property.h \
	src/midgard_replicator.h \
	src/midgard_schema_object_factory.h \
	src/midgard_schema_object_tree.h \
	src/midgard_blob.h \
	src/midgard_storage.h \
	src/midgard_transaction.h \
	src/midgard_user.h \
	src/midgard_utils.h \
	src/midgard_validable.h \
	src/midgard_view.h \
	src/midgard_workspace.h \
	src/midgard_workspace_context.h \
	src/midgard_workspace_manager.h \
	src/midgard_workspace_storage.h \
	$(NULL)

man_MANS = man/midgard2-schema.1 \
	man/midgard2-query.1 

EXTRA_DIST =  \
	$(man_MANS) \
	midgard2.pc.in \
	$(schema_DATA) \
	$(views_DATA) \
	m4/introspection.m4

LDADD = src/libmidgard2.la

if PLATFORM_WIN32
no_undefined = -no-undefined
endif

$(PROGRAMS): $(LDADD)

src_libmidgard2_la_CPPFLAGS = \
	-I$(top_srcdir)/src -DG_LOG_DOMAIN=\"midgard-core\"

AM_CFLAGS = -I$(top_srcdir)/src @MIDGARD_CFLAGS@
AM_LDFLAGS = @MIDGARD_LIBS@ 

src_libmidgard2_la_LDFLAGS = -version-info 2015:0:5 $(no_undefined)

test_midgard_schema_CPPFLAGS = \
	 -DG_LOG_DOMAIN=\"midgard-core\"

src_libmidgard2_la_core_SOURCES = \
	src/midgard_core_connection.h \
	src/midgard_core_connection.c \
	src/midgard_core_query.h \
	src/midgard_core_query.c \
	src/mgdschema.c \
	src/midgard_quota.c \
	src/query_constraint.c \
	src/query_constraint.h \
	src/group_constraint.c \
	src/group_constraint.h \
	src/query_group_constraint.h \
	src/query_group_constraint.c \
	src/query_order.c \
	src/query_order.h \
	src/schema.h \
	src/midgard_core_metadata.c \
	src/midgard_core_metadata.h \
	src/midgard_core_object_class.c \
	src/midgard_core_object_class.h \
	src/midgard_core_query_builder.h \
	src/midgard_core_query_builder.c \
	src/midgard_core_object.c \
	src/midgard_core_object.h \
	src/midgard_core_config.c \
	src/midgard_core_config.h \
	src/midgard_core_object_parameter.h \
	src/midgard_core_schema.c \
	src/midgard_core_views.c \
	src/midgard_core_views.h \
	src/midgard_core_xml.h \
	src/midgard_core_xml.c \
	src/midgard_core_workspace.h \
	src/midgard_core_workspace.c \
	$(NULL)

src_libmidgard2_la_public_SOURCES = \
	src/midgard.c \
	src/midgard_key_config.c \
	src/midgard_key_config_context.c \
	src/midgard_key_config_file.c \
	src/midgard_key_config_file_context.c \
	src/midgard_key_config_common.h \
	src/midgard_object.c \
	src/uuid.c \
	src/guid.c \
        src/midgard_type.c \
	src/midgard_config.c \
	src/query_builder.c \
	src/midgard_metadata.c \
	src/midgard_timestamp.c \
	src/midgard_reflection_property.c \
	src/midgard_connection.c \
	src/midgard_error.c \
	src/midgard_collector.c \
	src/midgard_dbus.h \
	src/midgard_dbus.c \
	src/midgard_dbus_interface.h \
	src/midgard_executable.c \
	src/midgard_object_parameter.c \
	src/midgard_object_attachment.c \
	src/midgard_query_constraint.c \
	src/midgard_query_executor.c \
	src/midgard_query_constraint_group.c \
	src/midgard_query_holder.c \
	src/midgard_query_property.c \
	src/midgard_query_select.c \
	src/midgard_query_constraint_simple.c \
	src/midgard_query_storage.c \
	src/midgard_query_value.c \
	src/midgard_reflector_object.c \
	src/midgard_reflector_property.c \
	src/midgard_replicator.c \
	src/midgard_user.c \
	src/midgard_schema_object_factory.c \
	src/midgard_schema_object_factory.h \
	src/midgard_schema_object_tree.c \
	src/midgard_schema_object_tree.h \
	src/midgard_storage.c \
	src/midgard_storage.h \
	src/midgard_dbobject.c \
	src/midgard_blob.c \
	src/midgard_transaction.c \
	src/midgard_md5.c \
	src/midgard_md5.h \
	src/midgard_validable.c \
	src/midgard_view.c \
	src/midgard_workspace.c \
	src/midgard_workspace_context.c \
	src/midgard_workspace_manager.c \
	src/midgard_workspace_storage.c \
	$(NULL)

pkgdatadir = $(datadir)/midgard2
pkglibdir = $(libdir)/midgard2
libmidgardincludedir = $(includedir)/midgard2/midgard
libmidgardinclude_HEADERS = $(midgard_public_h) src/midgard.h 

src_libmidgard2_la_SOURCES = $(src_libmidgard2_la_public_SOURCES) $(src_libmidgard2_la_core_SOURCES)

mgdschemasdir = $(datadir)/midgard2/schema

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = midgard2.pc

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
INTROSPECTION_CFLAGS += $(AM_CFLAGS)
introspection_sources = $(src_libmidgard2_la_public_SOURCES) $(midgard_public_h)
Midgard-10.05.gir: $(lib_LTLIBRARIES)
Midgard_10_05_gir_INCLUDES = GObject-2.0 libxml2-2.0 Gda-4.0 
Midgard_10_05_gir_CFLAGS = $(AM_CFLAGS) `pkg-config --cflags libxml-2.0`
Midgard_10_05_gir_LIBS = $(lib_LTLIBRARIES) 
Midgard_10_05_gir_FILES = $(addprefix $(srcdir)/,$(introspection_sources))
Midgard_10_05_gir_PACKAGES = gobject-2.0 libxml-2.0 libgda-4.0
INTROSPECTION_GIRS = Midgard-10.05.gir
INTROSPECTION_LIBS += -lxml2
girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)
CLEANFILES = $(dist_gir_DATA) $(typelib_DATA)
endif

doc:
	doxygen

.PHONY: doc
