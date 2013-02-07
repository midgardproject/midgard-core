# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

from bookstorequery import BookStoreQuery

# In this test, we depend on workspace context test, which leaves
# undeleted workspaces (and thus contexts) in test database.

class TestObjectInWorkspaceContext(unittest.TestCase):
  mgd = None
  manager = None
  bookstoreName = "BookStore In Foo"
  extraFoo = "extra Foo"
  extraFooBar = "extra FooBar"
  extraFooBarFooBar = "extra FooBarFooBar"

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
      self.mgd.enable_workspace(True)
    if self.manager is None:
      self.manager = Midgard.WorkspaceManager(connection = self.mgd)
    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/Foo")
    self.mgd.set_workspace(ws)

  def tearDown(self):        
    self.mgd.close()
    self.mgd = None

  def testCreate_01_ObjectInWorkspaceContext_Foo(self):
    bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    bookstore.set_property("name", self.bookstoreName)
    bookstore.set_property("extra", self.extraFoo)
    self.assertTrue(bookstore.save())
    # set new context so we can query object from there
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Bar")
    self.mgd.set_workspace(context)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
   
  def testCreate_02_ObjectInWorkspaceContext_FooBar(self):
    # set new context so we can query object from there
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Bar")
    self.mgd.set_workspace(context)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    bookstore.set_property("extra", self.extraFooBar)
    self.assertTrue(bookstore.save())
    # test updated object
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFooBar)
    # set default context and get object
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo")
    self.mgd.set_workspace(context)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFoo)

  def testCreate_03_ObjectInWorkspaceContext_FooBarFooBar(self):
    # set new context so we can query object from there
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Bar/FooBar")
    self.mgd.set_workspace(context)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    bookstore.set_property("extra", self.extraFooBarFooBar)
    self.assertTrue(bookstore.save())
    # test updated object
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFooBarFooBar)
    # set default context and get object
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo")
    self.mgd.set_workspace(context)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFoo)

  def testDelete_04_ObjectInWorkspaceContext_FooBarFooBar(self):
    # set /Foo/Bar/FooBar context and get object
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Bar/FooBar")
    self.mgd.set_workspace(context)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertTrue(bookstore.purge(False))
    # expect object from /Foo/Bar
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFooBar)

  def testDelete_05_ObjectInWorkspaceContext_FooBar(self):
    # set /Foo/Bar context and get object
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Bar")
    self.mgd.set_workspace(context)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertTrue(bookstore.purge(False))
    # expect object from /Foo/Bar
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFoo)

  def testDelete_06_ObjectInWorkspaceContext_FooBarFooBar(self):
    # set /Foo context and get object
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo")
    self.mgd.set_workspace(context)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertTrue(bookstore.purge(False))
    # expect object from /Foo/Bar
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 0)

if __name__ == "__main__":
    unittest.main()
