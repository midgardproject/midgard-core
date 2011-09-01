
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

	/* Check parent type */
	GType base_abstract_type = g_type_parent (abstract_type);
	g_assert_cmpuint (base_abstract_type, ==, MIDGARD_TYPE_BASE_ABSTRACT);

	g_assert_cmpuint (base_abstract_type, !=, G_TYPE_OBJECT);
	g_assert_cmpuint (base_abstract_type, !=, MIDGARD_TYPE_DBOBJECT);
	g_assert_cmpuint (base_abstract_type, !=, MIDGARD_TYPE_OBJECT);

	g_type_class_unref (klass);
}

#define N_IMPLEMENTED 4
const gchar *abstract_type_implements = "MidgardBaseInterface, FirstTestIface, SecondTestIface, FirstTestMixin";

void 	
midgard_test_types_and_extending_base_abstract_derived_type_with_interfaces (MidgardBaseAbstractTest *mwct, gconstpointer data)
{
	GType abstract_type = g_type_from_name ("AbstractTypeWithInterfaces");
	g_assert_cmpint (abstract_type, !=, G_TYPE_NONE);

	GObjectClass *klass = g_type_class_ref (abstract_type);
	g_assert (klass != NULL);

	/* Check implemented interfaces */
	guint n_types;
	guint i;
	GType *ifaces = g_type_interfaces (abstract_type, &n_types);
	g_assert_cmpuint (n_types, ==, N_IMPLEMENTED);

	for (i = 0; i < n_types; i++) {
		const gchar *implemented_interface_name = g_type_name (ifaces[i]);
		gchar *implemented = g_strstr_len (abstract_type_implements, -1, implemented_interface_name);
		g_assert_cmpstr (implemented, !=, NULL);
	}

	g_free (ifaces);

	/* Check every single interface */
	gboolean implements_base_iface = g_type_is_a (abstract_type, MIDGARD_TYPE_BASE_INTERFACE);
	g_assert (implements_base_iface == TRUE);

	gboolean implements_first_iface = g_type_is_a (abstract_type, g_type_from_name ("FirstTestIface"));
	g_assert (implements_first_iface == TRUE);

	gboolean implements_second_iface = g_type_is_a (abstract_type, g_type_from_name ("SecondTestIface"));
	g_assert (implements_second_iface == TRUE);

	gboolean implements_mixin_iface = g_type_is_a (abstract_type, g_type_from_name ("FirstTestMixin"));
	g_assert (implements_mixin_iface == TRUE);

	/* Find properties from implemented interfaces */
	GParamSpec *f_iface_f_prop = g_object_class_find_property (klass, "FirstIfaceFirstProperty");
	g_assert (f_iface_f_prop != NULL);

	GParamSpec *f_iface_s_prop = g_object_class_find_property (klass, "FirstIfaceSecondProperty");
	g_assert (f_iface_s_prop != NULL);

	GParamSpec *s_iface_f_prop = g_object_class_find_property (klass, "SecondIfaceFirstProperty");
	g_assert (s_iface_f_prop != NULL);

	GParamSpec *f_mixin_f_prop = g_object_class_find_property (klass, "FirstMixinFirstProperty");
	g_assert (f_mixin_f_prop != NULL);

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


