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

if __name__ == "__main__":
    unittest.main()
