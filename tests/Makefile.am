## Process this file with automake to produce Makefile.in

bin_PROGRAMS = 
	
nobase_CPPFLAGS = \
	-I$(top_srcdir)/src -DG_LOG_DOMAIN=\"midgard-core\"

AM_CFLAGS = -Wall @MIDGARD_CFLAGS@  
AM_LDFLAGS = @MIDGARD_LIBS@

TEST_PROGS = 
noinst_PROGRAMS  = $(TEST_PROGS)

nobase_SOURCES = \
	midgard_test.c \
	midgard_test_config.c \
	midgard_test_connection.c \
	midgard_test_database.c \
	midgard_test_error.c \
	midgard_test_metadata.c \
	midgard_test_object_basic.c \
	midgard_test_object_fetch.c \
	midgard_test_object_tree.c \
	midgard_test_object_class.c \
	midgard_test_property_reflector.c \
	midgard_test_replicator.c \
	midgard_test_replicator_import.c \
	midgard_test_query_builder.c \
	midgard_test_user.c

#midgard_test_SOURCES = midgard_test.c $(nobase_SOURCES)

TEST_PROGS	+= run-midgard-test-config
bin_PROGRAMS	+= run-midgard-test-config
run_midgard_test_config_SOURCES = run-midgard-test-config.c $(nobase_SOURCES)
run_midgard_test_config_CPPFLAGS = $(nobase_CPPFLAGS)

TEST_PROGS	+= run-midgard-test-connection
bin_PROGRAMS	+= run-midgard-test-connection
run_midgard_test_connection_SOURCES = run-midgard-test-connection.c $(nobase_SOURCES)
run_midgard_test_connection_CPPFLAGS = $(nobase_CPPFLAGS)

TEST_PROGS	+= run-midgard-test-object
bin_PROGRAMS	+= run-midgard-test-object
run_midgard_test_object_SOURCES = run-midgard-test-object.c $(nobase_SOURCES)
run_midgard_test_object_CPPFLAGS = $(nobase_CPPFLAGS)

TEST_PROGS	+= run-midgard-test-replicator
bin_PROGRAMS	+= run-midgard-test-replicator
run_midgard_test_replicator_SOURCES = run-midgard-test-replicator.c $(nobase_SOURCES)
run_midgard_test_replicator_CPPFLAGS = $(nobase_CPPFLAGS)

#TEST_PROGS	+= run-midgard-test-replicator-import
#bin_PROGRAMS	+= run-midgard-test-replicator-import
#run_midgard_test_replicator_import_SOURCES = run-midgard-test-replicator-import.c $(nobase_SOURCES)
#run_midgard_test_replicator_import_CPPFLAGS = $(nobase_CPPFLAGS)

TEST_PROGS	+= run-midgard-test-tree
bin_PROGRAMS	+= run-midgard-test-tree
run_midgard_test_tree_SOURCES = run-midgard-test-tree.c $(nobase_SOURCES)
run_midgard_test_tree_CPPFLAGS = $(nobase_CPPFLAGS)

TEST_PROGS	+= run-midgard-test-user
bin_PROGRAMS	+= run-midgard-test-user
run_midgard_test_user_SOURCES = run-midgard-test-user.c $(nobase_SOURCES)
run_midgard_test_user_CPPFLAGS = $(nobase_CPPFLAGS)

doc:
	doxygen

.PHONY: doc
