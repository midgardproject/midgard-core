
#ifndef MIDGARD_TEST_REPLICATOR_IMPORT_H
#define MIDGARD_TEST_REPLICATOR_IMPORT_H

#include "midgard_test.h"
#include "midgard_test_object.h"

/* tests */
void	midgard_test_replicator_import_unserialize		(MgdObjectTest *mot, gconstpointer data);
void	midgard_test_replicator_import_dependent_objects_fail	(MgdObjectTest *mot, gconstpointer data);
void	midgard_test_replicator_import_dependent_objects	(MgdObjectTest *mot, gconstpointer data);
void	midgard_test_replicator_import_independent_objects	(MgdObjectTest *mot, gconstpointer data);

#endif /* MIDGARD_TEST_REPLICATOR_IMPORT_H */
