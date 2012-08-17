# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestSqlQueryColumn(unittest.TestCase):
  mgd = None
  column = None
  queryproperty = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    if self.column is None:
      self.queryproperty = Midgard.QueryProperty(property = "title")
      self.column = Midgard.SqlQueryColumn(queryproperty = self.queryproperty, name = "The title", qualifier = "t1")

  def tearDown(self):
    self.mgd.close()
    self.mgd = None

  def testQueryProperty(self):
    self.assertEqual(self.column.get_query_property(), self.queryproperty)

  def testGetName(self):
    self.assertEqual(self.column.get_name(), "The title")

  def testGetQualifier(self):
    self.assertEqual(self.column.get_qualifier(), "t1")

  def testInheritance(self): 
    self.assertIsInstance(self.column, GObject.GObject)

if __name__ == "__main__":
    unittest.main()
