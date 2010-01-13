#!/bin/sh

MIDGARD_TEST_DB="midgard_test_database_provider"

echo "sudo mysqladmin create ${MIDGARD_TEST_DB}"
sudo mysql -e "CREATE DATABASE ${MIDGARD_TEST_DB} CHARACTER SET utf8";
# Grant all privileges" 
echo "GRANT all ON ${MIDGARD_TEST_DB}.*  to 'midgard'@'localhost' identified by 'midgard'";
sudo mysql -e "GRANT all ON ${MIDGARD_TEST_DB}.*  to 'midgard'@'localhost' identified by 'midgard'";
sudo mysql -e " FLUSH PRIVILEGES";

# Clean files
rm -f midgard-test-database-provider.html

valgrind -v --log-file=db-provider.log ./run-midgard-test-database-provider 

#gtester -k -o midgard-test-database-provider.xml ./run-midgard-test-database-provider 
#gtester-report midgard-test-database-provider.xml > midgard-test-database-provider.html

echo "Droping database ${MIDGARD_TEST_DB}"
sudo mysqladmin -f drop ${MIDGARD_TEST_DB}

#echo "Removing SQLite database file"
#rm ~/.midgard2/data/midgard_test_database_provider.db
