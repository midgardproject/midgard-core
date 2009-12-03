#!/bin/sh

MIDGARD_TEST_DB="midgard_test"
MIDAGRD_TEST_DB_USER="midgard_test"
MIDGARD_TEST_DB_PASS="midgard_test"

./autogen.sh

echo "sudo mysqladmin create midgard_test"
sudo mysql -e "CREATE DATABASE ${MIDGARD_TEST_DB} CHARACTER SET utf8";
# Grant all privileges" 
echo "GRANT all ON ${MIDGARD_TEST_DB}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
sudo mysql -e "GRANT all ON ${MIDGARD_TEST_DB}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
sudo mysql -e " FLUSH PRIVILEGES";

MIDGARD_TEST_DB_REPLICATOR_IMPORT="midgard_test_replicator_import"

echo "sudo mysqladmin create midgard_test_replicator_import"
sudo mysql -e "CREATE DATABASE ${MIDGARD_TEST_DB_REPLICATOR_IMPORT} CHARACTER SET utf8";
# Grant all privileges" 
echo "GRANT all ON ${MIDGARD_TEST_DB_REPLICATOR_IMPORT}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
sudo mysql -e "GRANT all ON ${MIDGARD_TEST_DB_REPLICATOR_IMPORT}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
sudo mysql -e " FLUSH PRIVILEGES";

# Clean files
rm -f midgard-test.html

gtester -k -o midgard-test.xml ./run-midgard-test-config ./run-midgard-test-connection ./run-midgard-test-object ./run-midgard-test-tree ./run-midgard-test-replicator ./run-midgard-test-replicator-import ./run-midgard-test-user
gtester-report midgard-test.xml > midgard-test.html

# Those below might be enabled and used if one needs report file per test program 
#gtester -k -o midgard-test-config.xml ./run-midgard-test-config
#gtester-report midgard-test-config.xml > midgard-test.html
#gtester-report midgard-test-config.xml > run-midgard-test-config.hml

#gtester -k -o midgard-test-object.xml ./run-midgard-test-object
#gtester-report midgard-test-object.xml >> midgard-test.html
#gtester-report midgard-test-object.xml > run-midgard-test-object.html

echo "Droping database midgard_test"
#sudo mysqladmin -f drop midgard_test

echo "Droping database midgard_test_replicator_import"
#sudo mysqladmin -f drop midgard_test_replicator_import

