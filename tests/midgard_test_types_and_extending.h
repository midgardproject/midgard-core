
#ifndef MIDGARD_TEST_BASE_ABSTRACT_H
#define MIDGARD_TEST_BASE_ABSTRACT_H

#include "midgard_test.h"

typedef struct {
	MidgardConnection *mgd;
	MidgardObject *object;
} MidgardBaseAbstractTest;

void 	midgard_test_types_and_extending_base_abstract					(MidgardBaseAbstractTest *mwct, gconstpointer data);
void 	midgard_test_types_and_extending_base_abstract_derived_type			(MidgardBaseAbstractTest *mwct, gconstpointer data);
void 	midgard_test_types_and_extending_base_abstract_derived_type_with_interfaces	(MidgardBaseAbstractTest *mwct, gconstpointer data);
void 	midgard_test_types_and_extending_interfaces					(MidgardBaseAbstractTest *mwct, gconstpointer data);
void 	midgard_test_types_and_extending_dbobject_with_interfaces			(MidgardBaseAbstractTest *mwct, gconstpointer data);

#endif /* MIDGARD_TEST_BASE_ABSTRACT_H */
