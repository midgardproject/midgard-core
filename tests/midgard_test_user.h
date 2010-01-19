
#ifndef MIDGARD_TEST_USER_H
#define MIDGARD_TEST_USER_H

#include "midgard_test.h"

typedef struct {
	MidgardUser *user;
	MidgardConnection *mgd;
} MidgardUserTest;

void 	midgard_test_user_init		(MidgardUserTest *mut, gconstpointer data);
void 	midgard_test_user_create	(MidgardUserTest *mut, gconstpointer data);
void 	midgard_test_user_update	(MidgardUserTest *mut, gconstpointer data);
void 	midgard_test_user_delete	(MidgardUserTest *mut, gconstpointer data);
void 	midgard_test_user_get		(MidgardUserTest *mut, gconstpointer data);
void 	midgard_test_user_query		(MidgardUserTest *mut, gconstpointer data);
void	midgard_test_user_login		(MidgardUserTest *mut, gconstpointer data);
void	midgard_test_user_logout	(MidgardUserTest *mut, gconstpointer data);

#endif /* MIDGARD_TEST_USER_H */
