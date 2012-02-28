# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestSqlQueryResult(unittest.TestCase):
  mgd = None
  select = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    if self.select is None:
      self.select = Midgard.SqlQuerySelectData(connection = self.mgd)

  def tearDown(self):
    self.mgd.close()
    self.mgd = None

  def addColumns(self):
    storage = Midgard.QueryStorage(dbclass = "midgard_person")
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "firstname", storage = storage), 
      name = "name", 
      qualifier = "p"
    )
    self.select.add_column(column)
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "lastname", storage = storage), 
      name = "lastname", 
      qualifier = "p"
    )
    self.select.add_column(column)

  @unittest.skip("To implement")
  def testExecuteInvalid(self):
    pass

  def testGetRows(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be default, administrator person only 
    self.assertEqual(len(rows), 1)

  def testGetColumns(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    columns = query_result.get_columns()
    self.assertEqual(len(columns), 2)

  def testGetColumnsQualifier(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    columns = query_result.get_columns()
    for column in columns:
      self.assertEqual(column.get_qualifier(), "p")

  def testGetColumnPropertyName(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    columns = query_result.get_columns()
    names = {"firstname", "lastname"}
    for column in columns:
      query_property = column.get_query_property()
      prop = query_property.get_property("property")
      self.assertIn(prop, names)

  def testGetColumnNames(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    names = query_result.get_column_names()
    self.assertEqual(len(names), 2)
    colnames = {"name", "lastname"}
    self.assertIn(names[0], colnames)
    self.assertIn(names[1], colnames)

  def testInheritance(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    self.assertIsInstance(query_result, Midgard.QueryResult)

if __name__ == "__main__":
    unittest.main()
