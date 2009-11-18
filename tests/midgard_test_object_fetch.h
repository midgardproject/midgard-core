
#ifndef MIDGARD_TEST_OBJECT_FETCH_H
#define MIDGARD_TEST_OBJECT_FETCH_H

#include "midgard_test.h"
#include "midgard_test_object.h"

gboolean midgard_test_object_fetch_by_id_created(MgdObject *object, guint id);
void midgard_test_object_get_by_id_created(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_get_by_id_updated(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_get_by_id_deleted(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_constructor_id_created(MgdObjectTest *mot, gconstpointer data);

gboolean midgard_test_object_fetch_by_guid_created(MgdObject *object, const gchar *guid);
void midgard_test_object_get_by_guid_created(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_constructor_guid_created(MgdObjectTest *mot, gconstpointer data);

void midgard_test_object_fetch_run(void);

#endif /* MIDGARD_TEST_OBJECT_FETCH_H */
