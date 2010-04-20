
#ifndef MIDGARD_TEST_SCHEMA_OBJECT_FACTORY
#define MIDGARD_TEST_SCHEMA_OBJECT_FACTORY

#include "midgard_test.h"

void	midgard_test_schema_object_factory_get_object_by_guid	(MidgardObjectTest *mot, gconstpointer data);
void	midgard_test_schema_object_factory_get_object_by_path	(MidgardObjectTest *mot, gconstpointer data);
void	midgard_test_schema_object_factory_object_undelete	(MidgardObjectTest *mot, gconstpointer data);

void	midgard_test_schema_object_factory_get_object_by_guid_deleted (MidgardObjectTest *mot, gconstpointer data);

#endif /* MIDGARD_TEST_SCHEMA_OBJECT_FACTORY */

