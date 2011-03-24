
#ifndef MIDGARD_TEST_CONNECTION_H
#define MIDGARD_TEST_CONNECTION_H

#include "midgard_test_config.h"

MidgardConnection *midgard_test_connection_open_user_config(const gchar *name, MidgardConfig **config);

void 	midgard_test_connection_run			(void);
void 	midgard_test_connection_open 			(void);
void 	midgard_test_connection_open_config 		(void);
void	midgard_test_connection_close			(void);
void 	midgard_test_connection_set_loglevel 		(void);
void 	midgard_test_connection_get_error 		(void);
void 	midgard_test_connection_get_error_string 	(void);
void 	midgard_test_connection_get_user 		(void);
void 	midgard_test_connection_copy 			(void);
void 	midgard_test_connection_enable_workspace	(void);
void 	midgard_test_connection_get_workspace		(void);
void 	midgard_test_connection_set_workspace		(void);
void 	midgard_test_connection_signals_auth_changed 	(void);
void 	midgard_test_connection_signals_error 		(void);
void 	midgard_test_connection_signals_lost_provider	(void);
void	midgard_test_connection_signals_connected	(void);
void	midgard_test_connection_signals_disconnected	(void);

#endif /* MIDGARD_TEST_CONNECTION_H */
