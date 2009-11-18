
#ifndef MIDGARD_TEST_PROPERTY_REFLECTOR_H
#define MIDGARD_TEST_PROPERTY_REFLECTOR_H

#include "midgard_test.h"
#include "midgard_test_object.h"

/* tests */
void midgard_test_property_reflector_new(MgdObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_midgard_type(MgdObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_is_link(MgdObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_is_linked(MgdObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_link_class(MgdObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_link_name(MgdObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_link_target(MgdObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_description(MgdObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_is_multilang(MgdObjectTest *mot, gconstpointer data);

#endif /* MIDGARD_TEST_PROPERTY_REFLECTOR_H */
