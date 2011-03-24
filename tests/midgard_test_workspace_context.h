
#ifndef MIDGARD_TEST_WORKSPACE_CONTEXT_H
#define MIDGARD_TEST_WORKSPACE_CONTEXT_H

#include "midgard_test.h"

typedef struct {
	MidgardWorkspaceContext *mwc;
	MidgardConnection *mgd;
} MidgardWorkspaceContextTest;

void 	midgard_test_workspace_context_exists			(MidgardWorkspaceContextTest *mwct, gconstpointer data);
void 	midgard_test_workspace_context_create			(MidgardWorkspaceContextTest *mwct, gconstpointer data);
void 	midgard_test_workspace_context_update			(MidgardWorkspaceContextTest *mwct, gconstpointer data);
void 	midgard_test_workspace_context_purge			(MidgardWorkspaceContextTest *mwct, gconstpointer data);
void 	midgard_test_workspace_context_get_by_path		(MidgardWorkspaceContextTest *mwct, gconstpointer data);

void 	midgard_test_workspace_context_get_path			(MidgardWorkspaceContextTest *mwct, gconstpointer data);
void 	midgard_test_workspace_context_list_workspace_names	(MidgardWorkspaceContextTest *mwct, gconstpointer data);
void 	midgard_test_workspace_context_list_children		(MidgardWorkspaceContextTest *mwct, gconstpointer data);
void 	midgard_test_workspace_context_get_workspace_by_name	(MidgardWorkspaceContextTest *mwct, gconstpointer data);

void 	midgard_test_workspace_context_has_workspace		(MidgardWorkspaceContextTest *mwct, gconstpointer data);

#endif /* MIDGARD_TEST_WORKSPACE_CONTEXT_H */
