
#ifndef MIDGARD_TEST_WORKSPACE_H
#define MIDGARD_TEST_WORKSPACE_H

#include "midgard_test.h"

typedef struct {
	MidgardWorkspace *mw;
	MidgardConnection *mgd;
	MidgardObject *object;
} MidgardObjectWorkspaceTest;

void 	midgard_test_object_workspace_create			(MidgardObjectWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_object_workspace_select_created		(MidgardObjectWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_object_workspace_context_select_created	(MidgardObjectWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_object_workspace_update			(MidgardObjectWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_object_workspace_context_update		(MidgardObjectWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_object_workspace_select_updated		(MidgardObjectWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_object_workspace_delete			(MidgardObjectWorkspaceTest *mwct, gconstpointer data);

#endif /* MIDGARD_TEST_WORKSPACE_H */
