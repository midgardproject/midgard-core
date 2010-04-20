
#ifndef MIDGARD_TEST_ERROR_H
#define MIDGARD_TEST_ERROR_H

#include "midgard_test_connection.h"

#define MIDGARD_TEST_ERROR_OK(__mgd) \
	g_assert_cmpstr(midgard_connection_get_error_string(__mgd), ==, "MGD_ERR_OK"); \
	g_assert_cmpint(midgard_connection_get_error(__mgd), ==, MGD_ERR_OK);

#define MIDGARD_TEST_ERROR_STRING_ASSERT(__mgd, errcode) \
	g_assert_cmpint(midgard_connection_get_error(mgd), ==, errcode); \
	const gchar *errstr = midgard_error_string(MGD_GENERIC_ERROR, errcode); \
	g_assert_cmpstr(midgard_connection_get_error_string(mgd), ==, errstr);

#define MIDGARD_TEST_ERROR_ASSERT(__mgd, errcode) \
	g_assert_cmpint(midgard_connection_get_error(mgd), ==, errcode); 

void midgard_test_error_ok(MidgardConnection *mgd);
void midgard_test_error_assert(MidgardConnection *mgd, gint errcode);

#endif /* MIDGARD_TEST_ERROR_H */
