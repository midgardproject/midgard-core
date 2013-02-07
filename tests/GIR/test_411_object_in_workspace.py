# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

from bookstorequery import BookStoreQuery

# In this test, we depend on workspace test, which leaves
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

  def testCreate_01_ObjectInWorkspace_Foo(self):
    bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    bookstore.set_property("name", self.bookstoreName)
    bookstore.set_property("extra", self.extraFoo)
    self.assertTrue(bookstore.create())
    # set new workspace so we can query object from there
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo/Bar")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 0)
   
  def testCreate_02_ObjectInWorkspace_FooBar(self):
    # set new workspace so we can query object from there
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo/Bar")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 0)
    bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    bookstore.set_property("name", self.bookstoreName)
    bookstore.set_property("extra", self.extraFooBar)
    self.assertTrue(bookstore.create())
    # test created object
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFooBar)
    # set default workspace and get object
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFoo)

  def testCreate_03_ObjectInWorkspace_FooBarFooBar(self):
    # set new workspace so we can query object from there
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo/Bar/FooBar")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 0)
    bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    bookstore.set_property("name", self.bookstoreName)
    bookstore.set_property("extra", self.extraFooBarFooBar)
    self.assertTrue(bookstore.create())
    # test created object
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFooBarFooBar)
    # set default workspace and get object
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    self.assertEqual(bookstore.get_property("extra"), self.extraFoo)

  def testDelete_04_ObjectInWorkspace_FooBarFooBar(self):
    # set /Foo/Bar/FooBar workspace and get object
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo/Bar/FooBar")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    # purge 
    self.assertTrue(bookstore.purge(False))
    # do not expect object from /Foo/Bar/FooBar
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 0)
    # expect object from /Foo/Bar
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo/Bar")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    # expect object from /Foo
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
  
  def testDelete_05_ObjectInWorkspace_FooBar(self):
    # set /Foo/Bar workspace and get object
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo/Bar")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    # purge 
    self.assertTrue(bookstore.purge(False))
    # do not expect object from /Foo/Bar
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 0)
    # expect object from /Foo
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)

  def testDelete_06_ObjectInWorkspace_Foo(self):
    # set /Foo/Bar workspace and get object
    workspace = Midgard.Workspace()
    self.manager.get_workspace_by_path(workspace, "/Foo")
    self.mgd.set_workspace(workspace)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 1)
    bookstore = bookstores[0]
    # purge 
    self.assertTrue(bookstore.purge(False))
    # do not expect object from /Foo
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)        
    self.assertEqual(len(bookstores), 0)
    # do not expect object at all
    self.mgd.enable_workspace(False)
    bookstores = BookStoreQuery.findByName(self.mgd, self.bookstoreName)
    self.assertEqual(len(bookstores), 0)

if __name__ == "__main__":
    unittest.main()
