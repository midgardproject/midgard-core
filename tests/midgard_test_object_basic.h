
#ifndef MIDGARD_TEST_OBJECT_BASIC_H
#define MIDGARD_TEST_OBJECT_BASIC_H

#include "midgard_test.h"
#include "midgard_test_object.h"

/* helpers */
MgdObject *midgard_test_object_basic_new(MidgardConnection *mgd, const gchar *name, GValue *value);
MgdObject *midgard_test_object_basic_new_by_guid(MidgardConnection *mgd, const gchar *name, const gchar *guid);

/* tests */
void midgard_test_object_basic_create(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_update(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_delete(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_purge(MgdObjectTest *mot, gconstpointer data);

void midgard_test_object_basic_lock(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_unlock(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_approve(MgdObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_unapprove(MgdObjectTest *mot, gconstpointer data);

/* gboolean midgard_test_object_basic_update(MgdObject *object); */
/* gboolean midgard_test_object_basic_delete(MgdObject *object); */
/* gboolean midgard_test_object_basic_purge(MgdObject *object); */
void midgard_test_object_basic_check_properties(MgdObject *object);

void midgard_test_object_basic_run(void);
void midgard_test_object_basic_create_run(void);
void midgard_test_object_basic_update_run(void);

#endif /* MIDGARD_TEST_OBJECT_BASIC_H */
