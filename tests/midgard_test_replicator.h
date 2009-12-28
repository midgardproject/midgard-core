
#ifndef MIDGARD_TEST_REPLICATOR_H
#define MIDGARD_TEST_REPLICATOR_H

#include "midgard_test.h"
#include "midgard_test_object.h"

#define MIDGARD_TEST_REPLICATOR_DEPENDENT_DIR "/tmp/midgard_test_replicator/dependent_objects"
#define MIDGARD_TEST_REPLICATOR_INDEPENDENT_DIR "/tmp/midgard_test_replicator/independent_objects"

/* tests */
void midgard_test_replicator_new(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_serialize(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_created(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_updated(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_purged(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_serialize_blob(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_blob(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_by_guid(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_export_media(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_unserialize(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_import_object_already_imported(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_import_from_xml(MidgardObjectTest *mot, gconstpointer data);
void midgard_test_replicator_xml_is_valid(MidgardObjectTest *mot, gconstpointer data);

void midgard_test_replicator_update_object_links(MidgardObjectTest *mot, gconstpointer data);

void midgard_test_replicator_prepare_dirs (void);
void midgard_test_replicator_export_archive (MidgardObjectTest *mot, gconstpointer data);

#endif /* MIDGARD_TEST_REPLICATOR_H */
