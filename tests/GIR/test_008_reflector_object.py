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

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()

  def testPropertyPrimary(self):
    self.assertEqual(Midgard.ReflectorObject.get_property_primary(self.person_type), "id")
    self.assertEqual(Midgard.ReflectorObject.get_property_primary(self.file_type), "id")
    self.assertEqual(Midgard.ReflectorObject.get_property_primary(self.resource_type), "id")

  def testPropertyUp(self):
    self.assertEqual(Midgard.ReflectorObject.get_property_up(self.person_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_up(self.file_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_up(self.resource_type), None)

  def testPropertyParent(self):
    self.assertEqual(Midgard.ReflectorObject.get_property_parent(self.person_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_parent(self.file_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_parent(self.resource_type), None)

  def testPropertyUnique(self):
    self.assertEqual(Midgard.ReflectorObject.get_property_unique(self.person_type), "firstname")
    self.assertEqual(Midgard.ReflectorObject.get_property_unique(self.file_type), None)
    self.assertEqual(Midgard.ReflectorObject.get_property_unique(self.resource_type), None)

  def testListChildren(self):
    self.assertEqual(len(Midgard.ReflectorObject.list_children(self.person_type)), 0)
    self.assertEqual(len(Midgard.ReflectorObject.list_children(self.file_type)), 0)
    self.assertEqual(len(Midgard.ReflectorObject.list_children(self.resource_type)), 0)

  def testHasMetadata(self):
    self.assertTrue(Midgard.ReflectorObject.has_metadata_class(self.person_type))
    self.assertFalse(Midgard.ReflectorObject.has_metadata_class(self.file_type))
    self.assertFalse(Midgard.ReflectorObject.has_metadata_class(self.resource_type))
 
  def testGetMetadataClass(self):
    self.assertEqual(Midgard.ReflectorObject.get_metadata_class(self.person_type), "MidgardMetadata")
    self.assertIsNone(Midgard.ReflectorObject.get_metadata_class(self.file_type))
    self.assertIsNone(Midgard.ReflectorObject.get_metadata_class(self.resource_type))

if __name__ == "__main__":
    unittest.main()
