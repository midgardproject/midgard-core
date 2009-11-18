
#ifndef MIDGARD_TEST_REPLICATOR_H
#define MIDGARD_TEST_REPLICATOR_H

#include "midgard_test.h"
#include "midgard_test_object.h"

#define MIDGARD_TEST_REPLICATOR_DEPENDENT_DIR "/tmp/midgard_test_replicator/dependent_objects"
#define MIDGARD_TEST_REPLICATOR_INDEPENDENT_DIR "/tmp/midgard_test_replicator/independent_objects"

/* tests */
void midgard_test_replicator_new(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_serialize(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_created(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_updated(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_purged(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_serialize_blob(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_blob(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_by_guid(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_media(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_unserialize(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_import_object_already_imported(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_import_from_xml(MgdObjectTest *mot, gconstpointer data);
void midgard_test_replicator_xml_is_valid(MgdObjectTest *mot, gconstpointer data);

void midgard_test_replicator_update_object_links(MgdObjectTest *mot, gconstpointer data);

void midgard_test_replicator_prepare_dirs (void);
void midgard_test_replicator_export_archive (MgdObjectTest *mot, gconstpointer data);

#endif /* MIDGARD_TEST_REPLICATOR_H */
