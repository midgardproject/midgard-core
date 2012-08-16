# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard, GObject, GLib

class TestReflectorObject(unittest.TestCase):
  mgd = None
  person_type = "midgard_person"
  file_type = "nt_file"
  resource_type = "nt_resource"
  snippet_type = "midgard_snippet"
  snippetdir_type = "midgard_snippetdir"

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()

  def tearDown(self):
    self.mgd.close()
    self.mgd = None

  def testPropertyPrimary(self):
    self.assertEqual(Midgard.ReflectorObject.get_property_primary(self.person_type), "id")
    self.assertEqual(Midgard.ReflectorObject.get_property_primary(self.file_type), "id")
    self.assertEqual(Midgard.ReflectorObject.get_property_primary(self.resource_type), "id")

  def testPropertyUp(self):
    self.assertEqual(Midgard.ReflectorObject.get_property_up(self.person_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_up(self.file_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_up(self.resource_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_up(self.snippetdir_type), "up")

  def testPropertyParent(self):
    self.assertEqual(Midgard.ReflectorObject.get_property_parent(self.person_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_parent(self.file_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_parent(self.resource_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_parent(self.snippet_type), "snippetdir")

  def testPropertyUnique(self):
    self.assertEqual(Midgard.ReflectorObject.get_property_unique(self.person_type), "firstname")
    self.assertEqual(Midgard.ReflectorObject.get_property_unique(self.file_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_unique(self.resource_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_unique(self.snippet_type), "name")
    self.assertEqual(Midgard.ReflectorObject.get_property_unique(self.snippetdir_type), "name")

  def testListChildren(self):
    self.assertEqual(len(Midgard.ReflectorObject.list_children(self.person_type)), 0)
    self.assertEqual(len(Midgard.ReflectorObject.list_children(self.file_type)), 0)
    self.assertEqual(len(Midgard.ReflectorObject.list_children(self.resource_type)), 0)
    self.assertEqual(len(Midgard.ReflectorObject.list_children(self.snippetdir_type)), 1)
    self.assertIn("midgard_snippet", Midgard.ReflectorObject.list_children(self.snippetdir_type))

  def testHasMetadata(self):
    self.assertTrue(Midgard.ReflectorObject.has_metadata_class(self.person_type))
    self.assertFalse(Midgard.ReflectorObject.has_metadata_class(self.file_type))
    self.assertFalse(Midgard.ReflectorObject.has_metadata_class(self.resource_type))
    self.assertTrue(Midgard.ReflectorObject.has_metadata_class(self.snippet_type))
    self.assertTrue(Midgard.ReflectorObject.has_metadata_class(self.snippetdir_type))

  def testGetMetadataClass(self):
    self.assertEqual(Midgard.ReflectorObject.get_metadata_class(self.person_type), "MidgardMetadata")
    self.assertIsNone(Midgard.ReflectorObject.get_metadata_class(self.file_type))
    self.assertIsNone(Midgard.ReflectorObject.get_metadata_class(self.resource_type))
    self.assertEqual(Midgard.ReflectorObject.get_metadata_class(self.snippet_type), "MidgardMetadata")

  @staticmethod
  def getSchemaValue(schema_type, name):
    return Midgard.ReflectorObject.get_schema_value(schema_type, name)

  def testGetSchemaValueSuperType(self):
    self.assertEqual(self.getSchemaValue(self.file_type, "Supertypes"), "nt:hierarchyNode")
    self.assertEqual(self.getSchemaValue(self.resource_type, "SuperTypes"), None)
    self.assertEqual(self.getSchemaValue(self.person_type, "NotExists"), None)

  def testGetSchemaValueIsMixin(self):
    self.assertEqual(self.getSchemaValue(self.file_type, "isMixin"), "false")
    self.assertEqual(self.getSchemaValue(self.resource_type, "isMixin"), "false")
    self.assertEqual(self.getSchemaValue("mix_title", "isMixin"), "true")

  def testGetSchemaValueOnParentVersion(self):
    self.assertEqual(self.getSchemaValue(self.file_type, "OnParentVersion"), "COPY")
    self.assertEqual(self.getSchemaValue(self.resource_type, "OnParentVersion"), "VERSION")
    self.assertEqual(self.getSchemaValue("mix_title", "OnParentVersion"), None)

if __name__ == "__main__":
    unittest.main()
