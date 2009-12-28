
#ifndef MIDGARD_TEST_METADATA_H
#define MIDGARD_TEST_METADATA_H

#include "midgard_test.h"

void midgard_test_metadata_check_new(MidgardObject *object);
void midgard_test_metadata_check_create(MidgardObject *object);
void midgard_test_metadata_check_created(MidgardObject *object);
void midgard_test_metadata_check_update(MidgardObject *object);
void midgard_test_metadata_check_updated(MidgardObject *object);
void midgard_test_metadata_check_delete(MidgardObject *object);

void midgard_test_metadata_check_person_references(MidgardObject *object, const gchar *guid, ...);
void midgard_test_metadata_check_datetime_properties(MidgardObject *object, const gchar *datetime, ...);

#endif /* MIDGARD_TEST_METADATA_H */

