# coding=utf-8

import sys
import struct
import unittest
from test_config import TestConfig
from test_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestQuerySelect(unittest.TestCase):
  def testSelectAdminPerson(self):
    mgd = TestConnection.openConnection()
    st = Midgard.QueryStorage(dbclass = "midgard_person")
    qs = Midgard.QuerySelect(connection = mgd, storage = st)
    qs.execute()
    objects = qs.list_objects()
    # Expect one person only 
    self.assertEqual(len(objects), 1);

  def testSelectInvalidType(self):
    mgd = TestConnection.openConnection()
    st = Midgard.QueryStorage(dbclass = "NotExists")
    qs = Midgard.QuerySelect(connection = mgd, storage = st)
    self.assertRaises(Midgard.ValidationError, qs.execute())

  def testOrder(self):
    mgd = TestConnection.openConnection()
    self.assertEqual("ok", "NOT IMPLEMENTED")

  def testInheritance(self):
    mgd = TestConnection.openConnection()
    qs = Midgard.QuerySelect(connection = mgd)
    self.assertIsInstance(qs, Midgard.QueryExecutor)

if __name__ == "__main__":
    unittest.main()
