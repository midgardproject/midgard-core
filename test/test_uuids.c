
/* compile:
   cc `pkg-config --cflags --libs midgard-2.0` ./test_uuids.c  -o test-uuids */

#include "../src/midgard_core_object.h"
#include "../src/midgard_core_query.h"
#include "../src/guid.h"
#include "../src/uuid.h"

GString *rep = NULL;
MidgardConnection *mgd;

#define BINARY_TABLE "guids_bin"
#define STRING_TABLE "guids"
#define GUID_FIELD "guid"

static const GValue *_select_guid_at_offset(const gchar *table, guint offset)
{
	GdaCommand *command;
        GdaDataModel *model;
 
	GString *sql = g_string_new("SELECT guid FROM ");
	g_string_append_printf(sql, "%s LIMIT 1 OFFSET %d", table, offset);

        command = gda_command_new (sql->str, GDA_COMMAND_TYPE_SQL, 0);
        model = gda_connection_execute_select_command (mgd->priv->connection, command, NULL, NULL);

        gda_command_free (command);
	g_string_free(sql, TRUE);

	if (!model)
		return NULL;

	gda_data_model_dump (model, stdout);

	return gda_data_model_get_value_at_col_name(model, "guid", 0);
}

static void _create_binary_guid()
{
	GdaParameterList *plist;
	GdaParameter *param;

	GdaDict *dict = gda_dict_new();
	gda_dict_set_connection (dict , mgd->priv->connection);

	const gchar *sql = "INSERT INTO guids_bin (guid) VALUES(##/*name:'guid' type:GdaBinary*/)";

	GdaQuery *query = gda_query_new_from_sql (dict, sql, NULL);
	if (!query)
		g_error("Failed to create new query from string");

	plist = gda_query_get_parameter_list (query);
	param = gda_parameter_list_find_param (plist, "guid");

	gchar *guid = midgard_uuid_new();

	GValue *valb = gda_value_new_from_string ( (const gchar *) guid, GDA_TYPE_BINARY);

	gda_parameter_set_value (param, valb);

	gda_connection_clear_events_list (mgd->priv->connection);

	GError *error = NULL;
	GdaObject *res_obj = gda_query_execute (query, plist, FALSE, &error);
}

static const char hexdigits[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

static void _run_binary_test()
{
	GdaParameterList *plist;
	GdaParameter *param;

	//_create_binary_guid();

	const GValue *guid_value = _select_guid_at_offset(BINARY_TABLE, 14);

	const unsigned char *bguid = (const unsigned char *)g_value_get_string(guid_value);
	gchar *b = NULL;
	guint length = strlen(bguid);
	guint i;

	gchar *_gstr, *gstr;
	gstr = _gstr = g_new(gchar , 34);
	
	for (i = 0; i < length; i++, bguid++) {
	
		unsigned char c = *bguid;
		*(gstr++) = hexdigits[(c>>4)&0xf];
		*(gstr++) = hexdigits[c&0xf];

	}
	*gstr = '\0';


	return;

	if (!guid_value) 
		g_error ("Returned NULL guid value");

	GdaDict *dict = gda_dict_new();
	gda_dict_set_connection (dict , mgd->priv->connection);

	GString *sql = g_string_new ("SELECT guid FROM ");
 	g_string_append_printf (sql, "%s WHERE guid= ##/*name:'guid' type:GdaBinary*/", BINARY_TABLE);

	GdaQuery *query = gda_query_new_from_sql (dict, sql->str, NULL);
	if (!query)
		g_error("Failed to create new query from string");

	plist = gda_query_get_parameter_list (query);

	param = gda_parameter_list_find_param (plist, "guid");

	if (!param)
		g_warning ("Failed to find guid param");

	GValue binval = {0, };
	g_value_init(&binval, GDA_TYPE_BINARY);
	g_value_transform(guid_value, &binval);

        gda_parameter_set_value (param, &binval);

	if (!gda_parameter_is_valid (param))
		g_error ("Guid parameter (with value) is invalid");

	gda_connection_clear_events_list (mgd->priv->connection);

	GError *error = NULL;
	GdaDataModel *model = GDA_DATA_MODEL(gda_query_execute (query, plist, FALSE, &error));

	if (!model)
		g_error("Query returned NULL model");

	if (error)
		g_error ("%s", error->message);

	gda_data_model_dump (model, stdout);

	const GValue *rval = gda_data_model_get_value_at_col_name (model, "guid", 0);
}

int main (int argc, gchar **argv)
{
	g_return_val_if_fail(argc == 2, 1);
	
	gchar *config_file = argv[1];

	midgard_init();

	mgd = midgard_connection_new();

        if(!mgd)
                g_error("Can not initialize midgard connection");

	MidgardConfig *config = g_object_new(MIDGARD_TYPE_CONFIG, NULL);

        GError *error = NULL;
        if(!midgard_config_read_file(config, (const gchar *)config_file, TRUE, &error)) {

                if(error) {
                        g_warning("%s", error->message);
                        g_error_free(error);
                }

                return 1;
        }

        if(!midgard_connection_open_config(mgd, config))
                return 1;

	rep = g_string_new("");

	_run_binary_test();
	//_run_string_tests();

	//_print_report();
}

