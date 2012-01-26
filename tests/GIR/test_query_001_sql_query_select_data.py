# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestSqlQuerySelectData(unittest.TestCase):
  mgd = None
  select = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    if self.select is None:
      self.select = Midgard.SqlQuerySelectData(connection = self.mgd)

  def testAddColumn(self): 
    column = Midgard.SqlQueryColumn(
        queryproperty = Midgard.QueryProperty(property = "title"), 
        name = "title", 
        qualifier = "t1"
    )
    # Expect exception in case of error
    self.select.add_column(column)

  def testGetColumns(self):
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "a"), 
      name = "a", 
      qualifier = "t1"
    )
    self.select.add_column(column)
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "b"), 
      name = "b", 
      qualifier = "t2"
    )
    self.select.add_column(column)
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "c"), 
      name = "c", 
      qualifier = "t3"
    )
    self.select.add_column(column)

    columns = self.select.get_columns()
    self.assertEqual(len(columns), 3)
    names = {"a", "b", "c"}
    qualifiers = {"t1", "t2", "t3"}
    for column in columns:
      self.assertIn(column.get_name(), names)
      self.assertIn(column.get_qualifier(), qualifiers)

  def testInheritance(self): 
    self.assertIsInstance(self.select, Midgard.QueryExecutor)

if __name__ == "__main__":
    unittest.main()
