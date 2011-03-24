
/* Define structures, keep it to make legacy and new code 
 * existance in better shape */

#ifndef MIDGARD_DEFS_H
#define MIDGARD_DEFS_H

#include <glib.h>
#include "midgard_config_auto.h"

typedef struct MidgardTypeHolder MidgardTypeHolder;
typedef struct _MidgardConnection MidgardConnection;
typedef struct _MidgardObjectClass MidgardObjectClass;
typedef struct _MidgardUser MidgardUser; 
typedef struct _MidgardMetadata MidgardMetadata;
typedef struct _MidgardMetadataClass MidgardMetadataClass;
typedef struct _MidgardWorkspace MidgardWorkspace;
typedef struct _MidgardWorkspacePrivate MidgardWorkspacePrivate;
typedef struct _MidgardWorkspaceContext MidgardWorkspaceContext;

typedef enum {
	MIDGARD_AUTHTYPE_NORMAL = 0,
	MIDGARD_AUTHTYPE_PAM,
	MIDGARD_AUTHTYPE_TRUST
} MidgardTypeAuth;

#define MIDGARD_PACKAGE_NAME "midgard2"

#endif /* MIDGARD_DEFS_H */
