# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestWorkspaceContext(unittest.TestCase):
  mgd = None
  manager = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    if self.manager is None:
      self.manager = Midgard.WorkspaceManager(connection = self.mgd)

  def tearDown(self):        
    self.mgd.close()
    self.mgd = None

  def createWorkspaces(self):
    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/Foo/Bar")
    ws_bar = Midgard.Workspace(name = "FooBar")
    self.manager.create_workspace(ws_bar, "/Foo/Bar")
    ws_baz = Midgard.Workspace(name = "FooBaz")
    self.manager.create_workspace(ws_baz, "/Foo/Baz")

  def testAListWorkspacesNames(self):
    self.createWorkspaces()
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Bar/FooBar")
    names = context.list_workspace_names()
    self.assertEqual(len(names), 3)
    self.assertIn("Bar", names)
    self.assertIn("Foo", names)
    self.assertIn("FooBar", names)

    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Baz/FooBaz")
    names = context.list_workspace_names()
    self.assertEqual(len(names), 3)
    self.assertIn("Baz", names)
    self.assertIn("Foo", names)
    self.assertIn("FooBaz", names)

  def testListChildren(self):
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Bar/FooBar")
    children = context.list_children()
    names = {"Foo", "Bar", "FooBar"}
    self.assertEqual(len(children), 3)
    self.assertIn(children[0].get_property("name"), names)
    self.assertIn(children[1].get_property("name"), names)
    self.assertIn(children[1].get_property("name"), names)

  def testHasWorkspace(self):
    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/Foo")
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/Foo/Baz/FooBaz")
    self.assertTrue(context.has_workspace(ws))

  def testInheritance(self): 
    context = Midgard.WorkspaceContext()
    self.assertIsInstance(context, GObject.GObject)
    self.assertIsInstance(context, Midgard.WorkspaceContext)
    self.assertIsInstance(context, Midgard.WorkspaceStorage)

if __name__ == "__main__":
    unittest.main()
