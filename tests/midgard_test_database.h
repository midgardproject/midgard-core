
#ifndef MIDGARD_TEST_DATABASE_H
#define MIDGARD_TEST_DATABASE_H

#include "midgard_test.h"

gboolean midgard_test_database_create(MidgardConnection *mgd);
gboolean midgard_test_database_update(MidgardConnection *mgd);

void midgard_test_database_run_create(void);
void midgard_test_database_run_update(void);

#endif /* MIDGARD_TEST_DATABASE_H */
