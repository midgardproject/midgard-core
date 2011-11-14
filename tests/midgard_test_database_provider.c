/* 
 * Copyright (C) 2010 Piotr Pokora <piotrek.pokora@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "midgard_test_database_provider.h" 
#include "midgard_test.h"

#define CONFIG_DB_PROVIDER_NAME "midgard_test_database_provider"
#define TEST_CLASS_NAME "midgard_snippet"
#define PROPERTY_UP "snippetdir"

static MidgardConnection *mgd_global = NULL;
static MidgardConfig *config_global = NULL;

static void
midgard_test_database_provider_new_user_config (const gchar *provider)
{
	MidgardConfig *config = midgard_config_new();
	gboolean saved = FALSE;
	gboolean read = FALSE;

	g_object_set (config, "database", CONFIG_DB_PROVIDER_NAME, NULL);
	g_object_set (config, "dbtype", provider, NULL);
	g_object_set (config, "dbuser", "midgard_test", NULL);
	g_object_set (config, "dbpass", "midgard_test", NULL);

	// g_object_set (config, "loglevel", "debug", NULL); 
	// g_object_set (config, "logfilename", "/tmp/midgard-test-database-provider.log", NULL); 

	saved = midgard_config_save_file (config, CONFIG_DB_PROVIDER_NAME, TRUE, NULL);
	g_assert (saved == TRUE);
        g_object_unref (config);
	config_global = midgard_config_new();
	read = midgard_config_read_file (config_global, CONFIG_DB_PROVIDER_NAME, TRUE, NULL);
	g_assert (read == TRUE);
}

static void 
midgard_test_database_provider_connection_init (const gchar *provider)
{
	mgd_global = midgard_connection_new();
	midgard_test_database_provider_new_user_config (provider);

	gboolean connected = midgard_connection_open_config (mgd_global, config_global);
	if (!connected) {
		printf("ERROR: %s\n", midgard_connection_get_error_string(mgd_global));
		g_assert (connected != FALSE);
	}
	
	//midgard_connection_set_loglevel (mgd_global, "debug", NULL);

	gboolean base_storage_created = midgard_storage_create_base_storage (mgd_global);
	/* Ugly workaround for 'constraint failed' warning thrown from SQLite provider */
	if (!g_str_equal (provider, "SQLite"))
		g_assert(base_storage_created != FALSE);

	MidgardObjectClass *klass = MIDGARD_OBJECT_GET_CLASS_BY_NAME(TEST_CLASS_NAME);
	gboolean class_table_created = midgard_storage_create (mgd_global, G_OBJECT_CLASS_NAME(klass));
	g_assert(class_table_created == TRUE);

	//midgard_connection_set_loglevel (mgd_global, "debug", NULL);
}

#define VARCHAR_PROPERTY_ESCAPE_VALUE "VARCHAR \' "
#define LONGTEXT_PROPERTY_ESCAPE_VALUE "LONGTEXT \' "

#define VARCHAR_PROPERTY_DOUBLE_ESCAPE_VALUE "VARCHAR \\ \'  "
#define LONGTEXT_PROPERTY_DOUBLE_ESCAPE_VALUE "LONGTEXT \\ \'  "

#define VARCHAR_PROPERTY_COMPLEX_ESCAPE_VALUE "VARCHAR \' IS THE NAME"
#define LONGTEXT_PROPERTY_COMPLEX_ESCAPE_VALUE "LONGTEXT \' IS THE CONTENT <?php $title=''; echo \"<?xml version=\\\"1.0\\\" encoding=\\\"utf-8\\\" ?>\\n?>"

#define VARCHAR_PROPERTY_UTF8_VALUE "Źdźbło Ędward Ącki"
#define LONGTEXT_PROPERTY_UTF8_VALUE "öä κόσμε"

static void 
_midgard_test_database_provider_escape (const gchar *provider)
{
	midgard_test_database_provider_connection_init (provider);

	/* Initial instance */
	MidgardObject *page = midgard_object_new (mgd_global, TEST_CLASS_NAME, NULL);
	g_assert (page != NULL);

	g_object_set (G_OBJECT (page), 
			"name", VARCHAR_PROPERTY_ESCAPE_VALUE,
			"code", LONGTEXT_PROPERTY_ESCAPE_VALUE,
			PROPERTY_UP, 1,
			NULL);

	gboolean created = midgard_object_create (page);
	MIDGARD_TEST_ERROR_OK (mgd_global);
	g_assert (created != FALSE);

	GValue guid_value = {0, };
	g_value_init (&guid_value, G_TYPE_STRING);
	g_object_get_property (G_OBJECT (page), "guid", &guid_value);

	/* Copy instance */
	MidgardObject *copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* compare values */
	gchar *initial_name;
	gchar *initial_content;
	gchar *copy_name;
	gchar *copy_content;

	g_object_get (G_OBJECT (page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, "\\ ");
	g_assert_cmpstr (copy_content, !=, "\\ ");
	g_assert_cmpstr (copy_name, !=, "\\\\ ");
	g_assert_cmpstr (copy_content, !=, "\\\\ ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_ESCAPE_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_ESCAPE_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_ESCAPE_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_ESCAPE_VALUE);

	g_object_unref (copy_page);

	/* Update initial instance */
	gboolean updated = midgard_object_update (page);
	g_assert (updated != FALSE);

	/* Get copy */
	copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	/* Get properties once again */
	g_object_get (G_OBJECT (page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Compare again */
	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, "\\ ");
	g_assert_cmpstr (copy_content, !=, "\\ ");
	g_assert_cmpstr (copy_name, !=, "\\\\ ");
	g_assert_cmpstr (copy_content, !=, "\\\\ ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_ESCAPE_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_ESCAPE_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_ESCAPE_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_ESCAPE_VALUE);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	g_value_unset (&guid_value);
	g_object_unref (page);
	g_object_unref (copy_page);
	g_object_unref (mgd_global);
	g_object_unref (config_global);
}

static void 
_midgard_test_database_provider_double_escape (const gchar *provider)
{
	midgard_test_database_provider_connection_init (provider);

	/* Initial instance */
	MidgardObject *page = midgard_object_new (mgd_global, TEST_CLASS_NAME, NULL);
	g_assert (page != NULL);	

	g_object_set (G_OBJECT (page), 
			"name", VARCHAR_PROPERTY_DOUBLE_ESCAPE_VALUE,
			"code", LONGTEXT_PROPERTY_DOUBLE_ESCAPE_VALUE,
			PROPERTY_UP, 1,
			NULL);

	gboolean created = midgard_object_create (page);
	MIDGARD_TEST_ERROR_OK (mgd_global);
	g_assert (created != FALSE);

	GValue guid_value = {0, };
	g_value_init (&guid_value, G_TYPE_STRING);
	g_object_get_property (G_OBJECT (page), "guid", &guid_value);

	/* Copy instance */
	MidgardObject *copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* compare values */
	gchar *initial_name;
	gchar *initial_content;
	gchar *copy_name;
	gchar *copy_content;

	g_object_get (G_OBJECT (page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, "\\ ");
	g_assert_cmpstr (copy_content, !=, "\\ ");
	g_assert_cmpstr (copy_name, !=, "\\\\ ");
	g_assert_cmpstr (copy_content, !=, "\\\\ ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_DOUBLE_ESCAPE_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_DOUBLE_ESCAPE_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_DOUBLE_ESCAPE_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_DOUBLE_ESCAPE_VALUE);

	g_object_unref (copy_page);

	/* Update initial instance */
	gboolean updated = midgard_object_update (page);
	g_assert (updated != FALSE);

	/* Get copy */
	copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	/* Get properties once again */
	g_object_get (G_OBJECT (page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Compare again */
	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, "\\ ");
	g_assert_cmpstr (copy_content, !=, "\\ ");
	g_assert_cmpstr (copy_name, !=, "\\\\ ");
	g_assert_cmpstr (copy_content, !=, "\\\\ ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_DOUBLE_ESCAPE_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_DOUBLE_ESCAPE_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_DOUBLE_ESCAPE_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_DOUBLE_ESCAPE_VALUE);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	g_value_unset (&guid_value);
	g_object_unref (page);
	g_object_unref (copy_page);
	g_object_unref (mgd_global);
	g_object_unref (config_global);
}

static void 
_midgard_test_database_provider_complex_escape (const gchar *provider)
{
	midgard_test_database_provider_connection_init (provider);

	/* Initial instance */
	MidgardObject *page = midgard_object_new (mgd_global, TEST_CLASS_NAME, NULL);
	g_assert (page != NULL);

	g_object_set (G_OBJECT (page), 
			"name", VARCHAR_PROPERTY_COMPLEX_ESCAPE_VALUE,
			"code", LONGTEXT_PROPERTY_COMPLEX_ESCAPE_VALUE,
			PROPERTY_UP, 1,
			NULL);

	gboolean created = midgard_object_create (page);
	MIDGARD_TEST_ERROR_OK (mgd_global);
	g_assert (created != FALSE);

	GValue guid_value = {0, };
	g_value_init (&guid_value, G_TYPE_STRING);
	g_object_get_property (G_OBJECT (page), "guid", &guid_value);

	/* Copy instance */
	MidgardObject *copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* compare values */
	gchar *initial_name;
	gchar *initial_content;
	gchar *copy_name;
	gchar *copy_content;

	g_object_get (G_OBJECT (page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, "\\ ");
	g_assert_cmpstr (copy_content, !=, "\\ ");
	g_assert_cmpstr (copy_name, !=, "\\\\ ");
	g_assert_cmpstr (copy_content, !=, "\\\\ ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_COMPLEX_ESCAPE_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_COMPLEX_ESCAPE_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_COMPLEX_ESCAPE_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_COMPLEX_ESCAPE_VALUE);

	g_object_unref (copy_page);

	/* Update initial instance */
	gboolean updated = midgard_object_update (page);
	g_assert (updated != FALSE);

	/* Get copy */
	copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	/* Get properties once again */
	g_object_get (G_OBJECT (page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Compare again */
	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, "\\ ");
	g_assert_cmpstr (copy_content, !=, "\\ ");
	g_assert_cmpstr (copy_name, !=, "\\\\ ");
	g_assert_cmpstr (copy_content, !=, "\\\\ ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_COMPLEX_ESCAPE_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_COMPLEX_ESCAPE_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_COMPLEX_ESCAPE_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_COMPLEX_ESCAPE_VALUE);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	g_value_unset (&guid_value);
	g_object_unref (page);
	g_object_unref (copy_page);
	g_object_unref (mgd_global);
	g_object_unref (config_global);
}

static void 
_midgard_test_database_provider_sql_injection_drop_table (const gchar *provider)
{
	midgard_test_database_provider_connection_init (provider);

	MidgardQueryStorage *storage = midgard_query_storage_new (TEST_CLASS_NAME);
	MidgardQuerySelect *select = midgard_query_select_new (mgd_global, storage);

	MidgardQueryConstraintGroup *group = midgard_query_constraint_group_new ();

	MidgardQueryProperty *mqp = midgard_query_property_new ("name", NULL);
	GValue strval = {0, };
	g_value_init (&strval, G_TYPE_STRING);
	g_value_set_string (&strval, "fake); DROP TABLE snippet;");
	MidgardQueryValue *mqv = midgard_query_value_create_with_value ((const GValue*) &strval);
	g_value_unset (&strval);
	MidgardQueryConstraint *mqc = midgard_query_constraint_new (mqp, "=", MIDGARD_QUERY_HOLDER (mqv), NULL);
	midgard_query_constraint_group_add_constraint (group, MIDGARD_QUERY_CONSTRAINT_SIMPLE (mqc));

	g_value_init (&strval, G_TYPE_STRING);
	g_value_set_string (&strval, "fake'); DROP TABLE snippet;");
	MidgardQueryValue *mqvA = midgard_query_value_create_with_value ((const GValue*) &strval);
	g_value_unset (&strval);
	MidgardQueryConstraint *mqcA = midgard_query_constraint_new (mqp, "=", MIDGARD_QUERY_HOLDER (mqvA), NULL);
	midgard_query_constraint_group_add_constraint (group, MIDGARD_QUERY_CONSTRAINT_SIMPLE (mqcA));

	g_value_init (&strval, G_TYPE_STRING);
	g_value_set_string (&strval, "fake'; DROP TABLE snippet;");
	MidgardQueryValue *mqvB = midgard_query_value_create_with_value ((const GValue*) &strval);
	g_value_unset (&strval);
	MidgardQueryConstraint *mqcB = midgard_query_constraint_new (mqp, "=", MIDGARD_QUERY_HOLDER (mqvB), NULL);
	midgard_query_constraint_group_add_constraint (group, MIDGARD_QUERY_CONSTRAINT_SIMPLE (mqcB));

	g_value_init (&strval, G_TYPE_STRING);
	g_value_set_string (&strval, "fake; DROP TABLE snippet;");
	MidgardQueryValue *mqvC = midgard_query_value_create_with_value ((const GValue*) &strval);
	g_value_unset (&strval);
	MidgardQueryConstraint *mqcC = midgard_query_constraint_new (mqp, "=", MIDGARD_QUERY_HOLDER (mqvC), NULL);
	midgard_query_constraint_group_add_constraint (group, MIDGARD_QUERY_CONSTRAINT_SIMPLE (mqcC));

	midgard_query_executor_set_constraint (MIDGARD_QUERY_EXECUTOR (select), MIDGARD_QUERY_CONSTRAINT_SIMPLE (group));

	GError *err = NULL;
	midgard_executable_execute (MIDGARD_EXECUTABLE (select), &err);

	g_object_unref (storage);
	g_object_unref (select);
	g_object_unref (mqp);
	g_object_unref (mqv);
	g_object_unref (mqc);
	g_object_unref (mqvA);
	g_object_unref (mqcA);
	g_object_unref (mqvB);
	g_object_unref (mqcB);
	g_object_unref (mqvC);
	g_object_unref (mqcC);
	g_object_unref (group);

	g_assert (err == NULL);
}

void	
midgard_test_database_provider_mysql_escape (void)
{
	_midgard_test_database_provider_escape ("MySQL");
}

void	
midgard_test_database_provider_sqlite_escape (void)
{
	_midgard_test_database_provider_escape ("SQLite");
}

void	
midgard_test_database_provider_mysql_double_escape (void)
{
	_midgard_test_database_provider_double_escape ("MySQL");
}

void	
midgard_test_database_provider_sqlite_double_escape (void)
{
	_midgard_test_database_provider_double_escape ("SQLite");
}

void	
midgard_test_database_provider_mysql_complex_escape (void)
{
	_midgard_test_database_provider_complex_escape ("MySQL");
}

void	
midgard_test_database_provider_sqlite_complex_escape (void)
{
	_midgard_test_database_provider_complex_escape ("SQLite");
}

void	
midgard_test_database_provider_sqlite_sql_injection_drop_table (void)
{
	_midgard_test_database_provider_sql_injection_drop_table ("SQLite");
}

void	
midgard_test_database_provider_mysql_sql_injection_drop_table (void)
{
	_midgard_test_database_provider_sql_injection_drop_table ("MySQL");
}

static void 
_midgard_test_database_provider_utf8_chars (const gchar *provider)
{
	midgard_test_database_provider_connection_init (provider);

	g_assert (g_utf8_validate (VARCHAR_PROPERTY_UTF8_VALUE, -1, NULL) == TRUE);
	g_assert (g_utf8_validate (LONGTEXT_PROPERTY_UTF8_VALUE, -1, NULL) == TRUE);

	/* Initial instance */
	MidgardObject *page = midgard_object_new (mgd_global, TEST_CLASS_NAME, NULL);
	g_assert (page != NULL);

	g_object_set (G_OBJECT (page), 
			"name", VARCHAR_PROPERTY_UTF8_VALUE,
			"code", LONGTEXT_PROPERTY_UTF8_VALUE,
			PROPERTY_UP, 1,
			NULL);

	gboolean created = midgard_object_create (page);
	MIDGARD_TEST_ERROR_OK (mgd_global);
	g_assert (created != FALSE);

	GValue guid_value = {0, };
	g_value_init (&guid_value, G_TYPE_STRING);
	g_object_get_property (G_OBJECT (page), "guid", &guid_value);

	/* Copy instance */
	MidgardObject *copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* compare values */
	gchar *initial_name;
	gchar *initial_content;
	gchar *copy_name;
	gchar *copy_content;

	g_object_get (G_OBJECT (page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, " ");
	g_assert_cmpstr (copy_content, !=, " ");
	g_assert_cmpstr (copy_name, !=, "A ");
	g_assert_cmpstr (copy_content, !=, "A ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_UTF8_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_UTF8_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_UTF8_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_UTF8_VALUE);

	g_object_unref (copy_page);

	/* Update initial instance */
	gboolean updated = midgard_object_update (page);
	g_assert (updated != FALSE);

	/* Get copy */
	copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	/* Get properties once again */
	g_object_get (G_OBJECT (page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Compare again */
	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, " ");
	g_assert_cmpstr (copy_content, !=, " ");
	g_assert_cmpstr (copy_name, !=, "A ");
	g_assert_cmpstr (copy_content, !=, "A ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_UTF8_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_UTF8_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_UTF8_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_UTF8_VALUE);

	/* Update initial instance */
	gboolean second_updated = midgard_object_update (copy_page);
	g_assert (second_updated != FALSE);

	/* Get copy */
	MidgardObject *second_copy_page = midgard_object_new (mgd_global, TEST_CLASS_NAME, &guid_value);
	g_assert (copy_page != NULL);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	/* Get properties once again */
	g_object_get (G_OBJECT (copy_page), 
			"name", &initial_name, 
			"code", &initial_content, 
			NULL);

	g_object_get (G_OBJECT (second_copy_page), 
			"name", &copy_name, 
			"code", &copy_content, 
			NULL);

	/* Compare again */
	/* Check invalid cases */
	g_assert_cmpstr (copy_name, !=, "");
	g_assert_cmpstr (copy_content, !=, "");
	g_assert_cmpstr (copy_name, !=, " ");
	g_assert_cmpstr (copy_content, !=, " ");
	g_assert_cmpstr (copy_name, !=, "A ");
	g_assert_cmpstr (copy_content, !=, "A ");

	/* Check very stupid cases */
	g_assert_cmpstr (copy_name, !=, "Abrakadabra");
	g_assert_cmpstr (copy_content, !=, "Abrakadabra");

	/* Check initial instance */
	g_assert_cmpstr (initial_name, ==, VARCHAR_PROPERTY_UTF8_VALUE);
	g_assert_cmpstr (initial_content, ==, LONGTEXT_PROPERTY_UTF8_VALUE);
	/* Compare both instances */
	g_assert_cmpstr (initial_name, ==, copy_name);
	g_assert_cmpstr (initial_content, ==, copy_content);
	/* Check copy instance */
	g_assert_cmpstr (copy_name, ==, VARCHAR_PROPERTY_UTF8_VALUE);
	g_assert_cmpstr (copy_content, ==, LONGTEXT_PROPERTY_UTF8_VALUE);

	/* clean&clear*/
	g_free (initial_name);
	g_free (initial_content);
	g_free (copy_name);
	g_free (copy_content);

	g_value_unset (&guid_value);
	g_object_unref (page);
	g_object_unref (copy_page);
	g_object_unref (second_copy_page);
	g_object_unref (mgd_global);
	g_object_unref (config_global);
}

void	
midgard_test_database_provider_sqlite_utf8_chars (void)
{
	_midgard_test_database_provider_utf8_chars ("SQLite");
}

void	
midgard_test_database_provider_mysql_utf8_chars (void)
{
	_midgard_test_database_provider_utf8_chars ("MySQL");
}
