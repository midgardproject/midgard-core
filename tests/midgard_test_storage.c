
#include "midgard_test.h"

void 
midgard_test_storage_create_test_storage (MidgardConnection *mgd)
{
	g_return_if_fail (mgd != NULL);

	gboolean base_storage_created = midgard_storage_create_base_storage (mgd);
	g_assert (base_storage_created != FALSE);

	guint n_types;
	guint i;
	const gchar *typename;
	GType *all_types = g_type_children (MIDGARD_TYPE_OBJECT, &n_types);

	for (i = 0; i < n_types; i++) {

		typename = g_type_name (all_types[i]);
		gboolean class_storage_created = midgard_storage_create (mgd, typename);
		g_assert (class_storage_created != FALSE);
	}
}
