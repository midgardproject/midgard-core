# coding=utf-8

import sys
import struct
import unittest
import inspect

from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestStorage(unittest.TestCase):
  mgd = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()

  def tearDown(self):
    self.mgd.close()
    self.mgd = None

  def testBaseStorage(self):
    types = []
    self.getDBObjectDerived("MidgardDBObject", types)
    for typename in types:
      nsname = "Midgard." + typename[7:]
      self.assertEqual(Midgard.Storage.exists(self.mgd, typename), False, "Storage for " + nsname + " shouldn't exist")
    self.assertTrue(Midgard.Storage.create_base_storage(self.mgd))

  def getDBObjectDerived(self, typename, types):
    children = GObject.type_children(typename)
    for gtype in children:
      childname = GObject.type_name(gtype)
      self.getDBObjectDerived(childname, types)
      # FIXME, core should support some routines to check if type could have storage 
      if (GObject.type_is_a(gtype, GObject.type_from_name("MidgardBaseInterface"))
          or gtype.is_abstract()
          or childname == "MidgardMetadata"):
        continue
      types.append(childname)

  def testCreateTypeStorage(self):
    types = []
    self.getDBObjectDerived("MidgardDBObject", types)
    for typename in types:
      nsname = "Midgard." + typename[7:]
      self.assertEqual(Midgard.Storage.create(self.mgd, typename), True, "Failed to create " + nsname + " storage")
      self.assertEqual(Midgard.Storage.exists(self.mgd, typename), True, "Storage for " + nsname + " doesn't exists. Previosuly created.")

  def testUpdateTypeStorage(self):
    types = []
    self.getDBObjectDerived("MidgardDBObject", types)
    for typename in types:
      nsname = "Midgard." + typename[7:]
      self.assertEqual(Midgard.Storage.exists(self.mgd, typename), True, "Storage for " + nsname + " doesn't exists. Already created.")
      self.assertEqual(Midgard.Storage.update(self.mgd, typename), True, "Failed to update " + nsname + " storage")
      self.assertEqual(Midgard.Storage.exists(self.mgd, typename), True, "Storage for " + nsname + " doesn't exists. Previosuly updated.")

if __name__ == "__main__":
    unittest.main()
