
#include "midgard_test_reflector_object.h"

#define MIXIN_NAME "FirstTestMixin"
#define FIRST_IFACE_NAME "FirstTestIface"
#define SECOND_IFACE_NAME "SecondTestIface"
#define ABSTRACT_NAME "AbstractTypeWithInterfaces"

void 	
midgard_test_reflector_object_is_mixin (MidgardReflectorObjectTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	g_assert (mgd != NULL);

	GType mixin_type = g_type_from_name (MIXIN_NAME);
	g_assert (mixin_type != G_TYPE_INVALID);

	g_assert (G_TYPE_IS_INTERFACE (mixin_type));

	g_assert (g_type_is_a (mixin_type, G_TYPE_INTERFACE));
	g_assert (g_type_is_a (mixin_type, MIDGARD_TYPE_BASE_INTERFACE));
	g_assert (g_type_is_a (mixin_type, MIDGARD_TYPE_BASE_MIXIN));

	gboolean is_mixin = midgard_reflector_object_is_mixin (mgd, MIXIN_NAME);
	g_assert (is_mixin == TRUE);

	gboolean is_interface = midgard_reflector_object_is_interface (mgd, MIXIN_NAME);
	g_assert (is_interface == TRUE);

	gboolean is_abstract = midgard_reflector_object_is_abstract (mgd, MIXIN_NAME);
	g_assert (is_abstract == FALSE);
}

void 	
midgard_test_reflector_object_is_interface (MidgardReflectorObjectTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	g_assert (mgd != NULL);

	GType f_iface_type = g_type_from_name (FIRST_IFACE_NAME);
	g_assert (f_iface_type != G_TYPE_INVALID);

	g_assert (G_TYPE_IS_INTERFACE (f_iface_type));
	g_assert (g_type_is_a (f_iface_type, G_TYPE_INTERFACE));
	g_assert (g_type_is_a (f_iface_type, MIDGARD_TYPE_BASE_INTERFACE));

	gboolean is_interface = midgard_reflector_object_is_interface (mgd, FIRST_IFACE_NAME);
	g_assert (is_interface == TRUE);

	gboolean is_abstract = midgard_reflector_object_is_abstract (mgd, FIRST_IFACE_NAME);
	g_assert (is_abstract == FALSE);

	gboolean is_mixin = midgard_reflector_object_is_mixin (mgd, FIRST_IFACE_NAME);
	g_assert (is_mixin == FALSE);

	GType s_iface_type = g_type_from_name (SECOND_IFACE_NAME);
	g_assert (s_iface_type != G_TYPE_INVALID);

	g_assert (G_TYPE_IS_INTERFACE (s_iface_type));
	g_assert (g_type_is_a (s_iface_type, G_TYPE_INTERFACE));
	g_assert (g_type_is_a (s_iface_type, MIDGARD_TYPE_BASE_INTERFACE));

	is_interface = midgard_reflector_object_is_interface (mgd, SECOND_IFACE_NAME);
	g_assert (is_interface == TRUE);

	is_abstract = midgard_reflector_object_is_abstract (mgd, SECOND_IFACE_NAME);
	g_assert (is_abstract == FALSE);

	is_mixin = midgard_reflector_object_is_mixin (mgd, SECOND_IFACE_NAME);
	g_assert (is_mixin == FALSE);
}

void 	
midgard_test_reflector_object_is_abstract (MidgardReflectorObjectTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	g_assert (mgd != NULL);

	GType abstract_type = g_type_from_name (ABSTRACT_NAME);
	g_assert (abstract_type != G_TYPE_INVALID);

	g_assert (G_TYPE_IS_ABSTRACT (abstract_type) == TRUE);

	gboolean is_abstract = midgard_reflector_object_is_abstract (mgd, ABSTRACT_NAME);
	g_assert (is_abstract == TRUE);

	gboolean is_mixin = midgard_reflector_object_is_mixin (mgd, ABSTRACT_NAME);
	g_assert (is_mixin == FALSE);

	gboolean is_interface = midgard_reflector_object_is_interface (mgd, ABSTRACT_NAME);
	g_assert (is_interface == FALSE);
}

#define N_PROPS 4
const gchar *abstract_type_properties = "FirstIfaceFirstProperty, FirstIfaceSecondProperty, SecondIfaceFirstProperty, FirstMixinFirstProperty";

void 	
midgard_test_reflector_object_list_defined_properties (MidgardReflectorObjectTest *mwct, gconstpointer data)
{
	MidgardConnection *mgd = mwct->mgd;
	g_assert (mgd != NULL);

	GType abstract_type = g_type_from_name (ABSTRACT_NAME);
	g_assert (abstract_type != G_TYPE_INVALID);

	GObjectClass *klass = g_type_class_ref (abstract_type);
	g_assert (klass != NULL);

	guint i;
	guint n_prop;
	GParamSpec **pspecs = g_object_class_list_properties (klass, &n_prop);
	g_assert (pspecs != NULL);
	g_assert_cmpuint (n_prop, ==, N_PROPS);
	for (i = 0; i < n_prop; i++) {
		const gchar *abstract_property_name = pspecs[i]->name;
		gchar *implemented = g_strstr_len (abstract_type_properties, -1, abstract_property_name);
		g_assert_cmpstr (implemented, !=, NULL);
	}

	gchar **properties = midgard_reflector_object_list_defined_properties (mgd, ABSTRACT_NAME, &n_prop);
	g_assert (properties != NULL);
	g_assert_cmpuint (n_prop, ==, N_PROPS);
	for (i = 0; i < n_prop; i++) {
		gchar *implemented = g_strstr_len (abstract_type_properties, -1, properties[i]);
		g_assert_cmpstr (implemented, !=, NULL);
	}

	g_free (properties);
	g_free (pspecs);
	g_type_class_unref (klass);
}
