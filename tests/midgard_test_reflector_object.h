
#ifndef MIDGARD_TEST_REFLECTOR_OBJECT_H
#define MIDGARD_TEST_REFLECTOR_OBJECT_H

#include "midgard_test.h"

typedef struct {
	MidgardConnection *mgd;
	MidgardObject *object;
} MidgardReflectorObjectTest;

void 	midgard_test_reflector_object_is_mixin			(MidgardReflectorObjectTest *mwct, gconstpointer data);
void 	midgard_test_reflector_object_is_interface 		(MidgardReflectorObjectTest *mwct, gconstpointer data);
void 	midgard_test_reflector_object_is_abstract		(MidgardReflectorObjectTest *mwct, gconstpointer data);
void 	midgard_test_reflector_object_list_defined_properties	(MidgardReflectorObjectTest *mwct, gconstpointer data);

#endif /* MIDGARD_TEST_REFLECTOR_OBJECT_H */
