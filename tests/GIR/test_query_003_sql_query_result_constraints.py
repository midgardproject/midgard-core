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
book_title_column = "booktitle"
book_store_id = "book_store_id"

book_store_qualifier = "bookstore"


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
    sA.set_property("edition", 1)
    sA.set_property("store", idA)
    self.assertTrue(sA.create())
    
    sB = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    sB.set_property("title", "Book B")
    sB.set_property("edition", 2)
    sB.set_property("store", idA)
    self.assertTrue(sB.create())
  
    sC = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    sC.set_property("title", "Book C")
    sC.set_property("edition", 3)
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

  def addStoreColumns(self):
    storage = self.default_book_storage
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "title", storage = storage), 
      name = book_title_column, 
      qualifier = book_qualifier
    )
    self.select.add_column(column)

    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "id", storage = storage), 
      name = "book_id", 
      qualifier = book_qualifier
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
      # Do not test error message. It's provider specific.
      #self.assertEqual(e.message, "Execute error - no such column: book_q.title")

  def testGetRowsLimit(self):
    self.addColumns()
    self.select.set_limit(1)
    self.select.execute()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be one book
    self.assertEqual(len(rows), 1)

  def testGetRowsOffset(self):
    self.addColumns()
    self.select.set_limit(1)
    self.select.set_offset(1)
    self.select.execute()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be three books
    self.assertEqual(len(rows), 1)

  def testExecuteInvalidConstraint(self): 
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
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be one book
    self.assertEqual(len(rows), 1)

  def testGetRowsWithAllBooks(self):
    self.addStoreColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be three books
    self.assertEqual(len(rows), 3)

  def testGetColumnsWithAllBooks(self):
    self.addStoreColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    columns = query_result.get_columns()
    names = query_result.get_column_names()
    # There should be three columns
    self.assertEqual(len(columns), 3)
    colnames = {"booktitle", "book_id", "storename"}
    self.assertIn(names[0], colnames)
    self.assertIn(names[1], colnames)
    self.assertIn(names[2], colnames)

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

#        JOINS
#########################

  def testAddJoinAddOrder(self):
    pass

  def prepareJoin(self):
    self.addStoreColumns()

    # Add book and store id required for join
    store_storage = self.default_store_storage
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "id", storage = store_storage), 
      name = "store_id", 
      qualifier = "bookstore"
    )
    self.select.add_column(column)
  
    column = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "store", storage = self.default_book_storage), 
      name = book_store_id, 
      qualifier = book_qualifier
    )
    self.select.add_column(column)

    self.select.add_join(
      "INNER", 
      Midgard.SqlQueryColumn(
        name = book_store_id, 
        queryproperty = Midgard.QueryProperty(property = "store", storage = self.default_book_storage),
        qualifier = book_qualifier
      ),
      Midgard.SqlQueryColumn(
        name = "store_id", 
        queryproperty = Midgard.QueryProperty(property = "id", storage = self.default_store_storage),
        qualifier = book_store_qualifier
      )
    )

  def addBookTitleConstraint(self):  
    self.select.set_constraint(
      Midgard.SqlQueryConstraint(
        column = Midgard.SqlQueryColumn(
          queryproperty = Midgard.QueryProperty(property = "title", storage = self.default_book_storage),
          name = book_title_column,
          qualifier = book_qualifier
        ),
        operator = "=",
        holder = Midgard.QueryValue.create_with_value(book_a_title)
      )
    )

  def testAddJoin(self):
    self.prepareJoin()
    self.addBookTitleConstraint()

    try:
      self.select.execute()
    except GObject.GError as e:
      print self.select.get_query_string()
      raise e
    #print self.select.get_query_string()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be one book
    self.assertEqual(len(rows), 1)
    names = query_result.get_column_names()
    self.assertEqual(len(names), 5)
    colnames = {"booktitle", "book_id", "storename", "store_id", "book_store_id"}
    self.assertIn(names[0], colnames)
    self.assertEqual(rows[0].get_value(names[0]), "Book A")
    self.assertIn(names[1], colnames)
    # Ignore book_id, integer which depends on provider
    self.assertIn(names[2], colnames)
    self.assertEqual(rows[0].get_value(names[2]), "Bookstore")
    self.assertIn(names[3], colnames)
    # Ignore store_id, integer which depends on provider
    self.assertIn(names[4], colnames)
    # Ignore book_store_id, integer which depends on provider
  
    # store_id must match book_store_id
    self.assertEqual(rows[0].get_value(names[3]), rows[0].get_value(names[4]))

  def testRowGetValues(self):
    self.prepareJoin()
    self.addBookTitleConstraint()

    try:
      self.select.execute()
    except GObject.GError as e:
      print self.select.get_query_string()
      raise e
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    # There should be one book
    self.assertEqual(len(rows), 1)
    values = rows[0].get_values()
    self.assertEqual(values.n_values, 5)
    self.assertEqual("Book A", values.get_nth(0))
    self.assertNotEqual("", values.get_nth(1))
    self.assertEqual("Bookstore", values.get_nth(2))
    self.assertNotEqual(0, values.get_nth(3))
    self.assertNotEqual(" ", values.get_nth(4))

  def testAddJoinAddOrderASC(self):
    self.prepareJoin()
    self.select.add_order(
      Midgard.SqlQueryColumn(
        queryproperty = Midgard.QueryProperty(property = "title", storage = self.default_book_storage),
        name = book_title_column,
        qualifier = book_qualifier
      ),
      "ASC"
    )
    try:
      self.select.execute()
    except GObject.GError as e:
      print self.select.get_query_string()
      raise e
    #print self.select.get_query_string()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    self.assertEqual(rows[0].get_value(book_title_column), "Book A");
    self.assertEqual(rows[1].get_value(book_title_column), "Book B");
    self.assertEqual(rows[2].get_value(book_title_column), "Book C");

  def testAddJoinAddOrderDESC(self):
    self.prepareJoin()
    self.select.add_order(
      Midgard.SqlQueryColumn(
        queryproperty = Midgard.QueryProperty(property = "title", storage = self.default_book_storage),
        name = book_title_column,
        qualifier = book_qualifier
      ),
      "DESC"
    )
    try:
      self.select.execute()
    except GObject.GError as e:
      print self.select.get_query_string()
      raise e
    #print self.select.get_query_string()
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    self.assertEqual(rows[0].get_value(book_title_column), "Book C");
    self.assertEqual(rows[1].get_value(book_title_column), "Book B");
    self.assertEqual(rows[2].get_value(book_title_column), "Book A");

  def testAddJoinAddOrderASCDESC(self):
    self.prepareJoin()
    self.select.add_order(
      Midgard.SqlQueryColumn(
        queryproperty = Midgard.QueryProperty(property = "edition", storage = self.default_book_storage),
        name = "edition",
        qualifier = book_qualifier
      ),
      "DESC"
    )
    self.select.add_order(
      Midgard.SqlQueryColumn(
        queryproperty = Midgard.QueryProperty(property = "title", storage = self.default_book_storage),
        name = book_title_column,
        qualifier = book_qualifier
      ),
      "ASC"
    )

    try:
      self.select.execute()
    except GObject.GError as e:
      print self.select.get_query_string()
      raise e
    query_result = self.select.get_query_result()
    rows = query_result.get_rows()
    self.assertEqual(rows[0].get_value(book_title_column), "Book C");
    self.assertEqual(rows[1].get_value(book_title_column), "Book B");
    self.assertEqual(rows[2].get_value(book_title_column), "Book A");

  def testInheritance(self):
    self.addColumns()
    self.select.execute()
    query_result = self.select.get_query_result()
    self.assertIsInstance(query_result, Midgard.QueryResult)

if __name__ == "__main__":
    unittest.main()
