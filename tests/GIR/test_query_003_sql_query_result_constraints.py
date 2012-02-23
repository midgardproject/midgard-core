# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

book_qualifier = "book_q"
book_title = "btitle"
book_a_title = "Book A"

class TestSqlQueryResultConstraints(unittest.TestCase):
  mgd = None
  select = None
  default_book_storage = None
  default_store_storage = None

  def createStore(self):
    tr = Midgard.Transaction(connection = self.mgd)
    tr.begin()

    sdirA = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    sdirA.set_property("name", "Bookstore")
    self.assertTrue(sdirA.create())
    idA = sdirA.get_property("id")
    
    sA = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    sA.set_property("title", book_a_title)
    sA.set_property("store", idA)
    self.assertTrue(sA.create())
    
    sB = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    sB.set_property("title", "Book B")
    sB.set_property("store", idA)
    self.assertTrue(sB.create())
  
    sC = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    sC.set_property("title", "Book C")
    sC.set_property("store", idA)
    self.assertTrue(sC.create())

    tr.commit()

  def purgeStore(self):
    tr = Midgard.Transaction(connection = self.mgd)
    tr.begin()
    st = Midgard.QueryStorage(dbclass = "gir_test_book_crud")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    qs.execute()
    for s in qs.list_objects():
      s.purge(False)
    st = Midgard.QueryStorage(dbclass = "gir_test_book_store")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    qs.execute()
    for s in qs.list_objects():
      s.purge(False)
    tr.commit()

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    if self.select is None:
      self.select = Midgard.SqlQuerySelectData(connection = self.mgd)
    self.default_book_storage = Midgard.QueryStorage(dbclass = "gir_test_book_crud")
    self.default_store_storage = Midgard.QueryStorage(dbclass = "gir_test_book_store")
    self.createStore()

  def tearDown(self):
    self.purgeStore()
    self.mgd.close()
    self.mgd = None

  def addColumns(self):
    storage = self.default_book_storage
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "title", storage = storage), 
      name = "btitle", 
      qualifier = book_qualifier
    )
    self.select.add_column(column)
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "id", storage = storage), 
      name = "sid", 
      qualifier = book_qualifier
    )
    self.select.add_column(column)

  def addSDirColumns(self):
    storage = self.default_book_storage
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "title", storage = storage), 
      name = "bookname", 
      qualifier = "b"
    )
    self.select.add_column(column)
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "id", storage = storage), 
      name = "sid", 
      qualifier = "b"
    )
    self.select.add_column(column)
    store_storage = self.default_store_storage
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "name", storage = store_storage), 
      name = "storename", 
      qualifier = "bookstore"
    )
    self.select.add_column(column)

  def testExecuteInvalidQuery(self):
    self.addColumns()
    self.select.set_constraint(
      Midgard.SqlQueryConstraint(
        column = Midgard.SqlQueryColumn(
          queryproperty = Midgard.QueryProperty(property = "title"),
          qualifier = book_qualifier
        ),
        operator = "=",
        holder = Midgard.QueryValue.create_with_value(book_a_title)
      )
    )
    try:
      self.select.execute()
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard-execution-error-quark")
      self.assertEqual(e.code, Midgard.ValidationError.TYPE_INVALID)
      self.assertEqual(e.message, "Execute error - no such column: book_q.title")

  def testGetRowsLimit(self):
    self.addColumns()
    self.select.set_limit(1)
    self.select.execute()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be three snippets 
    self.assertEqual(len(rows), 1)

  def testExecuteInvalidConstraint(self):
    pass 
    self.addColumns()
    # SqlQueryConstraint expected
    self.select.set_constraint(
      Midgard.QueryConstraint(
        property = Midgard.QueryProperty(property = "name"),
        operator = "=",
        holder = Midgard.QueryValue.create_with_value("InvalidConstraint")
      )
    )
    try:
      self.select.execute()
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard-validation-error-quark")
      self.assertEqual(e.code, Midgard.ValidationError.TYPE_INVALID)
      self.assertEqual(e.message, "Invalid constraint type 'MidgardQueryConstraint'. Expected SqlQueryConstraint or SqlQueryConstraintGroup")

  def testGetRowsWithABook(self):
    self.addColumns()
    self.select.set_constraint(
      Midgard.SqlQueryConstraint(
        column = Midgard.SqlQueryColumn(
          queryproperty = Midgard.QueryProperty(property = "title", storage = self.default_book_storage),
          qualifier = book_qualifier
        ),
        operator = "=",
        holder = Midgard.QueryValue.create_with_value(book_a_title)
      )
    )
    try:
      self.select.execute()
    except GObject.GError as e:
      print self.select.get_query_string()
      raise e
    print self.select.get_query_string()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be one snippet
    self.assertEqual(len(rows), 1)

  def testGetRowsSnippetsAndDir(self):
    self.addSDirColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be three snippets 
    self.assertEqual(len(rows), 3)

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
      self.assertEqual(column.get_qualifier(), book_qualifier)

  def testGetColumnPropertyName(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    columns = query_result.get_columns()
    names = {"title", "id"}
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
    colnames = {book_title, "sid"}
    self.assertIn(names[0], colnames)
    self.assertIn(names[1], colnames)

  def testInheritance(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    self.assertIsInstance(query_result, Midgard.QueryResult)

if __name__ == "__main__":
    unittest.main()
