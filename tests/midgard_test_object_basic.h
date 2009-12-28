
#ifndef MIDGARD_TEST_OBJECT_BASIC_H
#define MIDGARD_TEST_OBJECT_BASIC_H

#include "midgard_test.h"
#include "midgard_test_object.h"

/* helpers */
MidgardObject *midgard_test_object_basic_new(MidgardConnection *mgd, const gchar *name, GValue *value);
MidgardObject *midgard_test_object_basic_new_by_guid(MidgardConnection *mgd, const gchar *name, const gchar *guid);

/* tests */
void midgard_test_object_basic_create(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_update(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_delete(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_purge(MidgardObjectTest *mot, gconstpointer data);

void midgard_test_object_basic_lock(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_unlock(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_approve(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_object_basic_unapprove(MidgardObjectTest *mot, gconstpointer data);

/* gboolean midgard_test_object_basic_update(MidgardObject *object); */
/* gboolean midgard_test_object_basic_delete(MidgardObject *object); */
/* gboolean midgard_test_object_basic_purge(MidgardObject *object); */
void midgard_test_object_basic_check_properties(MidgardObject *object);

void midgard_test_object_basic_run(void);
void midgard_test_object_basic_create_run(void);
void midgard_test_object_basic_update_run(void);

#endif /* MIDGARD_TEST_OBJECT_BASIC_H */
