
#include "midgard_test_types_and_extending.h"

void 	
midgard_test_types_and_extending_base_abstract (MidgardBaseAbstractTest *mwct, gconstpointer data)
{
	/* Check if type is abstract */
	g_assert (G_TYPE_IS_ABSTRACT (MIDGARD_TYPE_BASE_ABSTRACT));

	GObjectClass *klass = g_type_class_ref (MIDGARD_TYPE_BASE_ABSTRACT);
	g_assert (klass != NULL);

	/* No properties registered */
	guint n_prop;
	GParamSpec **pspecs = g_object_class_list_properties (klass, &n_prop);
	g_assert_cmpint (n_prop, ==, 0);

	g_free(pspecs);

	g_type_class_unref (klass);
}

void 	
midgard_test_types_and_extending_base_abstract_derived_type	(MidgardBaseAbstractTest *mwct, gconstpointer data)
{
	GType abstract_type = g_type_from_name ("AbstractTypeWithInterfaces");
	g_assert_cmpint (abstract_type, !=, G_TYPE_NONE);

	GObjectClass *klass = g_type_class_ref (abstract_type);
	g_assert (klass != NULL);

	/* Check if type is abstract */
	g_assert (G_TYPE_IS_ABSTRACT (abstract_type));

	/* Find registered property */
	GParamSpec *primary_type_spec = g_object_class_find_property (klass, "jcr-primaryType");
	g_assert (primary_type_spec != NULL);

	GParamSpec *mixin_spec = g_object_class_find_property (klass, "jcr-mixinTypes");
	g_assert (mixin_spec != NULL);

	/* Check parent type */
	GType base_abstract_type = g_type_parent (abstract_type);
	g_assert_cmpuint (base_abstract_type, ==, MIDGARD_TYPE_BASE_ABSTRACT);

	g_assert_cmpuint (base_abstract_type, !=, G_TYPE_OBJECT);
	g_assert_cmpuint (base_abstract_type, !=, MIDGARD_TYPE_DBOBJECT);

	g_type_class_unref (klass);
}

void 	
midgard_test_types_and_extending_base_abstract_derived_type_with_interfaces (MidgardBaseAbstractTest *mwct, gconstpointer data)
{
	GType nt_base_type = g_type_from_name ("nt_unstructured");
	g_assert_cmpint (nt_base_type, !=, G_TYPE_NONE);

	GObjectClass *klass = g_type_class_ref (nt_base_type);
	g_assert (klass != NULL);

	/* Check if type is abstract */
	g_assert (G_TYPE_IS_ABSTRACT (nt_base_type) == FALSE);

	/* Find registered property */
	GParamSpec *primary_type_spec = g_object_class_find_property (klass, "jcr-primaryType");
	g_assert (primary_type_spec != NULL);

	GParamSpec *mixin_spec = g_object_class_find_property (klass, "jcr-mixinTypes");
	g_assert (mixin_spec != NULL);

	/* Check parent type */
	GType nt_unstructured_parent_type = g_type_parent (nt_base_type);
	GType nt_base = g_type_from_name ("nt_base");
	g_assert_cmpstr (g_type_name(nt_unstructured_parent_type), ==, g_type_name (nt_base));
	g_assert_cmpuint (nt_unstructured_parent_type, ==, nt_base);

	g_assert_cmpuint (nt_unstructured_parent_type, !=, G_TYPE_OBJECT);
	g_assert_cmpuint (nt_unstructured_parent_type, !=, MIDGARD_TYPE_DBOBJECT);

	g_type_class_unref (klass);
}

void 	
midgard_test_types_and_extending_interfaces (MidgardBaseAbstractTest *mwct, gconstpointer data)
{

}

void 	
midgard_test_types_and_extending_dbobject_with_interfaces (MidgardBaseAbstractTest *mwct, gconstpointer data)
{

}


