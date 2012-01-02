# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

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
    # Check if we have GError 
    self.assertRaises(GObject.GError, qs.execute)
    # Check if we have correct domain 
    try:
      qs.execute()
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard-validation-error-quark")
      self.assertEqual(e.code, Midgard.ValidationError.TYPE_INVALID)

  def testOrder(self):
    mgd = TestConnection.openConnection()
    self.assertEqual("ok", "NOT IMPLEMENTED")

  def testInheritance(self):
    mgd = TestConnection.openConnection()
    qs = Midgard.QuerySelect(connection = mgd)
    self.assertIsInstance(qs, Midgard.QueryExecutor)

if __name__ == "__main__":
    unittest.main()
