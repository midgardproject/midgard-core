
#ifndef MIDGARD_TEST_OBJECT_H
#define MIDGARD_TEST_OBJECT_H

#include <midgard/midgard.h>

typedef struct {
	MidgardConnection *mgd;
	MgdObject *object;
	guint id;
	gchar *guid;
	gchar *created;
	gchar *creator;
	gchar *classname;
}MgdObjectTest;

#endif /* MIDGARD_TEST_OBJECT */
