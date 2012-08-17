# coding=utf-8

import os
import sys
import struct
import unittest
import inspect

from gi.repository import Midgard, GObject, GLib

from test_100_connection import TestConnection

class TestObjectReference(unittest.TestCase):
  mgd = None
  guid = None
  name = "TestReference"
  workspace = None
  reference = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    self.guid = Midgard.Guid.new(self.mgd)
    self.reference = Midgard.ObjectReference(id = self.guid, name = self.name)

  def tearDown(self):
    self.reference = None
    self.mgd.close()
    self.mgd = None

  def testInheritance(self):
    self.assertIsInstance(self.reference, GObject.Object)
    self.assertIsInstance(self.reference, Midgard.ModelReference)

  def testGetName(self):
    self.assertNotEqual(self.reference.get_name(), None)
    self.assertEqual(self.reference.get_name(), self.name)

  def testGetID(self):
    self.assertNotEqual(self.reference.get_id(), None)
    self.assertEqual(self.reference.get_id(), self.guid)

  def testGetIDValue(self):
    self.assertNotEqual(self.reference.get_id_value(), None)
    self.assertEqual(self.reference.get_id_value(), self.guid)
 
  def testGetWorkspace(self):
    self.assertEqual(self.reference.get_workspace(), None)

if __name__ == "__main__":
    unittest.main()
