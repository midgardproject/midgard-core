# coding=utf-8

import sys
import struct
import unittest
import inspect
import gobject

from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestStorage(unittest.TestCase):
  def testBaseStorage(self):
    mgd = TestConnection.openConnection()
    self.assertTrue(Midgard.Storage.create_base_storage(mgd))

  def getDBObjectDerived(self, typename, types):
    children = GObject.type_children(typename)
    for gtype in children:
      childname = GObject.type_name(gtype)
      types.append(childname)
      self.getDBObjectDerived(childname, types)

  def testCreateTypeStorage(self):
    mgd = TestConnection.openConnection()
    types = []
    self.getDBObjectDerived("MidgardDBObject", types)
    for typename in types:
      nsname = "Midgard." + typename[7:]
      #gtype = gobject.type.from_name(typename)
      if (GObject.type_is_a(GObject.type_from_name(typename), GObject.type_from_name("MidgardBaseInterface"))
          or gobject.GObject.__gtype__.is_abstract("Midgard." + nsname)
          or typename == "MidgardMetadata"):
        continue
      #print(inspect.isabstract(typename))
      # FIXME, core should support some routines to check if type could have storage
      Midgard.Storage.create(mgd, typename)
      self.assertEqual(Midgard.Storage.exists(mgd, typename), True, "Failed to create " + nsname + " storage")

  def testUpdateTypeStorage(self):
    self.assertEqual("ok", "NOT IMPLEMENTED")

if __name__ == "__main__":
    unittest.main()
