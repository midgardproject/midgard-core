#!/bin/sh

MIDGARD_TEST_DB="midgard_test_database_provider"

echo "sudo mysqladmin create midgard_test"
sudo mysql -e "CREATE DATABASE ${MIDGARD_TEST_DB} CHARACTER SET utf8";
# Grant all privileges" 
echo "GRANT all ON ${MIDGARD_TEST_DB}.*  to '${MIDGARD_TEST_DB}'@'localhost' identified by '${MIDGARD_TEST_DB}'";
sudo mysql -e "GRANT all ON ${MIDGARD_TEST_DB}.*  to '${MIDGARD_TEST_DB}'@'localhost' identified by '${MIDGARD_TEST_DB}'";
sudo mysql -e " FLUSH PRIVILEGES";

# Clean files
rm -f midgard-test-database-provider.html

gtester -k -o midgard-test-database-provider.xml ./run-midgard-test-database-provider 
gtester-report midgard-test-database-provider.xml > midgard-test-database-provider.html

echo "Droping database midgard_test"
sudo mysqladmin -f drop ${MIDGARD_TEST_DB}
