
#ifndef MIDGARD_TEST_PROPERTY_REFLECTOR_H
#define MIDGARD_TEST_PROPERTY_REFLECTOR_H

#include "midgard_test.h"
#include "midgard_test_object.h"

/* tests */
void midgard_test_property_reflector_new(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_midgard_type(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_is_link(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_is_linked(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_link_class(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_link_name(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_link_target(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_description(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_property_reflector_check_is_multilang(MidgardObjectTest *mot, gconstpointer data);

#endif /* MIDGARD_TEST_PROPERTY_REFLECTOR_H */
