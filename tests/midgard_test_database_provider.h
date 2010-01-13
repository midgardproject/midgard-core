
#ifndef MIDGARD_TEST_DATABASE_PROVIDER_H
#define MIDGARD_TEST_DATABASE_PROVIDER_H

#include <midgard/midgard.h>

void	midgard_test_database_provider_mysql_escape		(void);
void	midgard_test_database_provider_sqlite_escape		(void);
void	midgard_test_database_provider_mysql_double_escape	(void);
void	midgard_test_database_provider_sqlite_double_escape	(void);
void	midgard_test_database_provider_mysql_complex_escape	(void);
void	midgard_test_database_provider_sqlite_complex_escape	(void);

void	midgard_test_database_sql_injection 			(void);

#endif /* MIDGARD_TEST_DATABASE_PROVIDER_H */
