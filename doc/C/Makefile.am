## Process this file with automake to produce Makefile.in

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

AUTOMAKE_OPTIONS = 1.6

# The name of the module.
DOC_MODULE=midgard-2.0

# The top-level SGML file.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.sgml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=--deprecated-guards

# Extra options to pass to gtkdoc-scangobj
#SCANGOBJ_OPTIONS=--module=midgard --types=midgard.types

# Extra options to supply to gtkdoc-mkdb.
# # e.g. MKDB_OPTIONS=--sgml-mode --output-format=xml
MKDB_OPTIONS=--sgml-mode --output-format=xml --name-space=midgard --ignore-files="midgard_core_object.c simple_constraint.c" 

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=  ../../src

# Used for dependencies.
HFILE_GLOB=../../src/*.h
CFILE_GLOB=../../src/*.c

# Header files to ignore when scanning
IGNORE_HFILES=config.h midgard_core_object.h simple_constraint.h

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
GTKDOC_CFLAGS = -I$(top_srcdir) \
        -I$(top_builddir) \
	-I /usr/include/glib-2.0 \
        -I $(MIDGARD_CFLAGS) \
	-I ../../
GTKDOC_LIBS =  $(top_builddir)/../src/libmidgard2.la $(MIDGARD_LIBS)
#GTKDOC_LIBS =  -lmidgard $(MIDGARD_LIBS)

# Extra options to supply to gtkdoc-mkdb
#MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files = 

# Images to copy into HTML directory
HTML_IMAGES = midgard2-architecture.png 

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

include $(top_srcdir)/gtk-doc.make

# Other files to distribute
#EXTRA_DIST += ../../src/types.c
