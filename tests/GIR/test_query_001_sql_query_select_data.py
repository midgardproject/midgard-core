# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

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

  def tearDown(self):
    self.mgd.close()
    self.mgd = None

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


  def testExecute(self):
    # SELECT p.firstname AS firstname, p.lastname AS lastname FROM person AS p
    self.addColumns()
    self.select.execute()
    # Do not test unpredictable query string. If must be tested, move it to particular test
    #query = "SELECT \n\tp.firstname AS firstname, \n\tp.lastname AS lastname\nFROM person AS p\nWHERE 1=1 AND 0<1"
    #self.assertEqual(self.select.get_query_string(), query)

  @unittest.skip("To implement")
  def testExecuteInvalid(self):
    pass

  def testGetQueryResult(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    self.assertIsInstance(query_result, Midgard.SqlQueryResult)

  def testInheritance(self): 
    self.assertIsInstance(self.select, Midgard.QueryExecutor)

if __name__ == "__main__":
    unittest.main()
