
#include "midgard_test_config.h"
#include "midgard_test_connection.h"
#include "midgard_test_database.h"
#include "midgard_test_error.h"
#include "midgard_test_metadata.h"
#include "midgard_test_object_basic.h"
#include "midgard_test_object_fetch.h"
#include "midgard_test_user.h"
#include "midgard_test_object_class.h"
#include "midgard_test_property_reflector.h"
#include "midgard_test_replicator.h"
#include "midgard_test_replicator_import.h"
#include "midgard_test_user.h"
#include "midgard_test_schema_object_factory.h"
#include "midgard_test_storage.h"

#define MGD_TEST_SITEGROUP_NAME "midgard_test_sitegroup"
#define MIDGARD_TRAC_TICKET_BASE_URL "http:/""/trac.midgard-project.org/ticket/"

#define _MGD_TEST_OBJECT_SETUP \
static void midgard_test_setup(MidgardObjectTest *mot, gconstpointer data) \
{ \
	MidgardObject *object = (MidgardObject *) data; \
        mot->mgd = mgd_global; \
	mot->object = object; \
}

#define _MGD_TEST_OBJECT_TEARDOWN \
static void midgard_test_teardown(MidgardObjectTest *mot, gconstpointer data) \
{ \
	MidgardObject *object = (MidgardObject *)data; \
	g_object_unref(object); \
}

#define _MGD_TEST_UNREF_MIDGARD \
g_test_add("/midgard_connection/unref", MidgardObjectTest, mgd_global, midgard_test_setup_foo, \
	midgard_test_run_foo, midgard_test_unref_object);

#define _MGD_TEST_UNREF_SCHEMA \
g_test_add("/midgard_schema/unref", MidgardObjectTest, schema, midgard_test_setup, \
	midgard_test_run_foo, midgard_test_unref_object);

#define _MGD_TEST_UNREF_MGDOBJECT(__obj) \
g_test_add("/midgard_object/unref", MidgardObjectTest, __obj, midgard_test_setup, \
	midgard_test_run_foo, midgard_test_unref_object);

#define _MGD_TEST_UNREF_GOBJECT(__obj) \
g_test_add("/gobject/unref", MidgardObjectTest, __obj, midgard_test_setup, \
	midgard_test_run_foo, midgard_test_unref_object);

#define MGD_TEST_OBJECT_NAME "midgard-core-test"

#define _MGD_TEST_MOT(__mot) \
	g_assert(__mot != NULL); \
	MidgardObject *object = __mot->object; \
	g_assert(object != NULL); \
	MidgardConnection *mgd = __mot->mgd; \
	g_assert(mgd != NULL);

void midgard_test_setup_foo(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_teardown_foo(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_run_foo(MidgardObjectTest *mot, gconstpointer data);

void midgard_test_unref_object(MidgardObjectTest *mot, gconstpointer data);

void midgard_test_lock_root_objects(MidgardConnection *mgd, MidgardUser *object);
gchar *midgard_test_get_current_person_guid(MidgardConnection *mgd);
