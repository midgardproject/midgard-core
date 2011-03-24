#!/bin/sh

MIDGARD_TEST_DB="midgard_test"
MIDGARD_TEST_DB_REPLICATOR_IMPORT="midgard_test_replicator_import"
MIDGARD_TEST_DB_PROVIDER="midgard_test_database_provider"

MIDAGRD_TEST_DB_USER="midgard_test"
MIDGARD_TEST_DB_PASS="midgard_test"

# ./autogen.sh

if [ "$MIDGARD_TEST_DATABASE_TYPE" != "SQLite" ]
then
    echo "Preparing temporary database for generic tests…"
    echo "-> sudo mysqladmin create midgard_test"
    sudo mysql -e "CREATE DATABASE ${MIDGARD_TEST_DB} CHARACTER SET utf8";
    echo "-> GRANT all ON ${MIDGARD_TEST_DB}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
    sudo mysql -e "GRANT all ON ${MIDGARD_TEST_DB}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
    sudo mysql -e " FLUSH PRIVILEGES";

    echo "Preparing temporary database for provider test…"
    echo "-> sudo mysqladmin create midgard_test_database_provider"
    sudo mysql -e "CREATE DATABASE ${MIDGARD_TEST_DB_PROVIDER} CHARACTER SET utf8";
    echo "-> GRANT all ON ${MIDGARD_TEST_DB_PROVIDER}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
    sudo mysql -e "GRANT all ON ${MIDGARD_TEST_DB_PROVIDER}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
    sudo mysql -e " FLUSH PRIVILEGES";

    echo "-> sudo mysqladmin create midgard_test_replicator_import"
    sudo mysql -e "CREATE DATABASE ${MIDGARD_TEST_DB_REPLICATOR_IMPORT} CHARACTER SET utf8";
    echo "-> GRANT all ON ${MIDGARD_TEST_DB_REPLICATOR_IMPORT}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
    sudo mysql -e "GRANT all ON ${MIDGARD_TEST_DB_REPLICATOR_IMPORT}.*  to '${MIDGARD_TEST_DB_USER}'@'localhost' identified by '${MIDGARD_TEST_DB_PASS}'";
    sudo mysql -e " FLUSH PRIVILEGES";
fi

# Clean files
rm -f midgard-test.html

echo "\nRunning tests…\n"
gtester -k -o midgard-test.xml \
	./run-midgard-test-config \
	./run-midgard-test-connection \
	./run-midgard-test-object \
	./run-midgard-test-tree \
	./run-midgard-test-replicator \
	./run-midgard-test-user \
	./run-midgard-test-user-bugs \
	./run-midgard-test-workspace-context \
	./run-midgard-test-workspace \
	./run-midgard-test-object-workspace \
	./run-midgard-test-database-provider \
	# ./run-midgard-test-replicator-import


echo "\nCleanup…"
if [ "$MIDGARD_TEST_DATABASE_TYPE" == "SQLite" ]
then
    echo "-> Deleting sqlite database file"
    rm -f ~/.midgard2/data/${MIDGARD_TEST_DB}.db
else
    echo "-> Droping database midgard_test"
    sudo mysqladmin -f drop midgard_test > /dev/null 2> /dev/null

    echo "-> Droping database midgard_test_replicator_import"
    sudo mysqladmin -f drop midgard_test_replicator_import > /dev/null 2> /dev/null

    echo "-> Droping database midgard_test_database_provider"
    sudo mysqladmin -f drop midgard_test_database_provider > /dev/null 2> /dev/null
fi

echo "\nGenerating report… "
gtester-report midgard-test.xml > midgard-test.html
echo "-> DONE.    see midgard-test.html"


# Those below might be enabled and used if one needs report file per test program 
#gtester -k -o midgard-test-config.xml ./run-midgard-test-config
#gtester-report midgard-test-config.xml > midgard-test.html
#gtester-report midgard-test-config.xml > run-midgard-test-config.hml

#gtester -k -o midgard-test-object.xml ./run-midgard-test-object
#gtester-report midgard-test-object.xml >> midgard-test.html
#gtester-report midgard-test-object.xml > run-midgard-test-object.html

