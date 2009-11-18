
#ifndef MIDGARD_TEST_METADATA_H
#define MIDGARD_TEST_METADATA_H

#include "midgard_test.h"

void midgard_test_metadata_check_new(MgdObject *object);
void midgard_test_metadata_check_create(MgdObject *object);
void midgard_test_metadata_check_created(MgdObject *object);
void midgard_test_metadata_check_update(MgdObject *object);
void midgard_test_metadata_check_updated(MgdObject *object);
void midgard_test_metadata_check_delete(MgdObject *object);

void midgard_test_metadata_check_person_references(MgdObject *object, const gchar *guid, ...);
void midgard_test_metadata_check_datetime_properties(MgdObject *object, const gchar *datetime, ...);

#endif /* MIDGARD_TEST_METADATA_H */

