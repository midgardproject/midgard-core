
#include "midgard_test.h"

void	
midgard_test_schema_object_factory_get_object_by_guid (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT (mot);

	gchar *guid;
	g_object_get (object, "guid", &guid, NULL);

	/* Check object identified by valid guid */
	MidgardObject *new_object = midgard_schema_object_factory_get_object_by_guid (mgd, guid);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (new_object != NULL);

	/* Check object's type */
	g_assert_cmpstr (G_OBJECT_TYPE_NAME (object), ==, G_OBJECT_TYPE_NAME (new_object));
	g_assert (new_object);
		
	/* Check object which doesn't exist */
	gchar *doesnt_exist_guid = "1df3806dd010df4380611dfa5a371390c3b313f313f";
	new_object = midgard_schema_object_factory_get_object_by_guid (mgd, doesnt_exist_guid);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_NOT_EXISTS);
	g_assert (new_object == NULL);

	/* Check invalid guid case */
	gchar *invalid_guid = "1nv@lid6u1d";
	new_object = midgard_schema_object_factory_get_object_by_guid (mgd, invalid_guid);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_NOT_EXISTS);
	g_assert (new_object == NULL);

	g_free (guid);
}

#define ROOT_NAME "Root Object"
#define CHILD_NAME "Child Object"

void	
midgard_test_schema_object_factory_get_object_by_path (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT (mot);
	const gchar *classname = G_OBJECT_TYPE_NAME (object);

	/* Check simple case with independent type */
	const gchar *parent = midgard_reflector_object_get_property_parent (classname);
	const gchar *up = midgard_reflector_object_get_property_up (classname);

	/* return if object can not be located in some tree */
	if (!parent && !up)
		return;

	/* TODO, build case with parent and child objects */
	if (parent)
		return;

	guint up_id;
	const gchar *unique_property = midgard_reflector_object_get_property_unique (classname);
	
	/* Create '/Root Object' */
	MidgardObject *object_a = midgard_object_new (mgd, classname, NULL);
	g_object_set (object_a, unique_property, ROOT_NAME, NULL);
	gboolean object_a_created = midgard_object_create (object_a);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (object_a_created != FALSE);

	/* Create '/Root Object/Child Object' */
	MidgardObject *object_b = midgard_object_new (mgd, classname, NULL);
	g_object_set (object_b, unique_property, CHILD_NAME, NULL);
	g_object_get (object_a, "id", &up_id, NULL);
	g_object_set (object_b, up, up_id, NULL);
	gboolean object_b_created = midgard_object_create (object_b);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (object_b_created != FALSE);

	/* Get object by '/Root Object/Child Object' path */
	MidgardObject *object_by_path = midgard_schema_object_factory_get_object_by_path (mgd, classname, "/Root Object/Child Object");
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OK);
	g_assert (object_by_path != NULL);

	g_assert (G_OBJECT_TYPE (object_by_path) == G_OBJECT_TYPE (object_b));

	g_object_unref (object_a);
	g_object_unref (object_b);
	g_object_unref (object_by_path);
}

void	
midgard_test_schema_object_factory_object_undelete (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT (mot);
	const gchar *guid = (const gchar *) data;

	/* Delete and undelete given object */
	gboolean object_delete = midgard_object_delete (object);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (object_delete != FALSE);

	gboolean object_undelete = midgard_schema_object_factory_object_undelete (mgd, guid);
	MIDGARD_TEST_ERROR_OK (mgd);
	g_assert (object_undelete != FALSE);

	/* Undelete object which doesn't exist */
	gchar *doesnt_exist_guid = "1df3806dd010df4380611dfa5a371390c3b313f313f";
	gboolean doesnt_exist_guid_undelete = midgard_schema_object_factory_object_undelete (mgd, doesnt_exist_guid);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_NOT_EXISTS);
	g_assert (doesnt_exist_guid_undelete == FALSE);	

	/* Undelete with invalid guid */
	gchar *invalid_guid = "1nv@lid6u1d";
	gboolean invalid_guid_undelete = midgard_schema_object_factory_object_undelete (mgd, invalid_guid);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_NOT_EXISTS);
	g_assert (invalid_guid_undelete == FALSE);
}

/* Particular case tests */
void 
midgard_test_schema_object_factory_get_object_by_guid_deleted (MidgardObjectTest *mot, gconstpointer data)
{
	_MGD_TEST_MOT (mot);

	g_test_bug ("#1513");
	gchar *guid;
	g_object_get (G_OBJECT (object), "guid", &guid, NULL);

	MidgardObject *deleted_object = midgard_schema_object_factory_get_object_by_guid (mgd, (const gchar *)guid);
	MIDGARD_TEST_ERROR_ASSERT (mgd, MGD_ERR_OBJECT_DELETED);
	g_assert (deleted_object == NULL);

	g_free (guid);
}
