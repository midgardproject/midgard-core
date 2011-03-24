
#ifndef MIDGARD_TEST_WORKSPACE_H
#define MIDGARD_TEST_WORKSPACE_H

#include "midgard_test.h"

typedef struct {
	MidgardWorkspace *mw;
	MidgardConnection *mgd;
} MidgardWorkspaceTest;

void 	midgard_test_workspace_new			(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_create			(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_update			(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_exists			(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_purge			(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_list_children		(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_get_by_path		(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_get_path			(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_list_workspace_names	(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_get_workspace_by_name	(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_get_context		(MidgardWorkspaceTest *mwct, gconstpointer data);
void 	midgard_test_workspace_is_in_context		(MidgardWorkspaceTest *mwct, gconstpointer data);

#endif /* MIDGARD_TEST_WORKSPACE_H */
