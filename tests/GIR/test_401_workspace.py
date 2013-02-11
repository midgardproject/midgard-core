# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestWorkspace(unittest.TestCase):
  mgd = None
  manager = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    self.mgd.beginTransaction()  
    if self.manager is None:
      self.manager = Midgard.WorkspaceManager(connection = self.mgd)

  def tearDown(self):        
    self.mgd.commitTransaction()
    self.mgd.close()
    self.mgd = None

  def createWorkspaces(self):
    ws_foo = Midgard.Workspace(name = "Foo")
    self.manager.create_workspace(ws_foo, "")
    ws_bar = Midgard.Workspace(name = "Bar")
    self.manager.create_workspace(ws_bar, "/Foo")
    ws_baz = Midgard.Workspace(name = "Baz")
    self.manager.create_workspace(ws_baz, "/Foo")

  def testAListWorkspacesNames(self):
    self.createWorkspaces()
    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/Foo")
    names = ws.list_workspace_names()
    self.assertEqual(len(names), 2)
    self.assertIn("Bar", names)
    self.assertIn("Baz", names)

  def testListChildren(self):
    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/Foo")
    children = ws.list_children()
    names = {"Bar", "Baz"}
    self.assertEqual(len(children), 2)
    self.assertIn(children[0].get_property("name"), names)
    self.assertIn(children[1].get_property("name"), names)

  def testGetPath(self):
    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/Foo")
    self.assertEqual(ws.get_path(), "/Foo");

    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/Foo/Bar")
    self.assertEqual(ws.get_path(), "/Foo/Bar");

    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/Foo/Baz")
    self.assertEqual(ws.get_path(), "/Foo/Baz");

  def testInheritance(self): 
    ws = Midgard.Workspace()
    self.assertIsInstance(ws, GObject.GObject)
    self.assertIsInstance(ws, Midgard.Workspace)
    self.assertIsInstance(ws, Midgard.WorkspaceStorage)

if __name__ == "__main__":
    unittest.main()
