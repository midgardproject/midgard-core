# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestWorkspaceManager(unittest.TestCase):
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

  def testCreateInvalidWorkspace(self):
    # Attempt to create nameless workspace should fail 
    fake_ws = Midgard.Workspace()
    try:
      self.manager.create_workspace(fake_ws, "/devel/testing/stable")
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard_workspace_storage_error-quark")
      self.assertEqual(e.code, Midgard.WorkspaceStorageError.INVALID_VALUE)
      self.assertEqual(e.message, "Invalid (empty or null) workspace's name")

  def testCreateWorkspace(self):
    wa = Midgard.Workspace(name = "devel")
    self.assertTrue(self.manager.create_workspace(wa, ""))
 
  def testCreateWorkspaceDuplicate(self):
    wa = Midgard.Workspace(name = "devel")
    try:
      self.manager.create_workspace(wa, "")
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard_workspace_storage_error-quark")
      self.assertEqual(e.code, Midgard.WorkspaceStorageError.NAME_EXISTS)
      self.assertEqual(e.message, "WorkspaceStorage at path '/devel' already exists")

  def testCreateWorkspaceAtPath(self):
    # If element in path is missed, manager should create it
    wa = Midgard.Workspace(name = "testing")
    self.assertTrue(self.manager.create_workspace(wa, "/devel"))

    wb = Midgard.Workspace(name = "stable")
    self.assertTrue(self.manager.create_workspace(wb, "/devel/testing"))

  def testGetObjectWorkspaceInvalid(self):
    workspace =self.manager.get_object_workspace(self.manager)
    self.assertEqual(workspace, None)

  def testGetObjectWorkspaceWithoutWorkspace(self):
    bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    workspace = self.manager.get_object_workspace(bookstore)
    self.assertEqual(workspace, None)

  def testGetObjectWorkspace(self):
    #bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    context = Midgard.WorkspaceContext()
    self.manager.get_workspace_by_path(context, "/devel/testing")
    self.mgd.enable_workspace(True)
    self.mgd.set_workspace(context)
    bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    bookstore.create()
    workspace = self.manager.get_object_workspace(bookstore)
    ws = Midgard.Workspace()
    self.manager.get_workspace_by_path(ws, "/devel/testing")
    self.assertEqual(workspace.get_property("name"), ws.get_property("name"))
    self.assertEqual(workspace.get_property("guid"), ws.get_property("guid"))
    bookstore.purge(False)

  def testGetWorkspaceByPathInvalid(self):
    wa = Midgard.Workspace()
    try:
      self.manager.get_workspace_by_path(wa, "/invalid/path")
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard_workspace_storage_error-quark")
      self.assertEqual(e.code, Midgard.WorkspaceStorageError.OBJECT_NOT_EXISTS)
      self.assertEqual(e.message, "WorkspaceStorage doesn't exists at given path")

  def testGetWorkspaceByPathNotExists(self):
    wa = Midgard.Workspace()
    try:
      self.manager.get_workspace_by_path(wa, "/devel/testing/notexists")
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard_workspace_storage_error-quark")
      self.assertEqual(e.code, Midgard.WorkspaceStorageError.OBJECT_NOT_EXISTS)
      self.assertEqual(e.message, "WorkspaceStorage doesn't exists at given path")

  def testGetWorkspaceByPath(self):
    wa = Midgard.Workspace()
    self.assertTrue(self.manager.get_workspace_by_path(wa, "/devel"))
    wb = Midgard.Workspace()
    self.assertTrue(self.manager.get_workspace_by_path(wb, "/devel/testing"))
    wc = Midgard.Workspace()
    self.assertTrue(self.manager.get_workspace_by_path(wc, "/devel/testing/stable"))

  def testUpdateWorkspace(self):
    wa = Midgard.Workspace()
    self.manager.get_workspace_by_path(wa, "/devel")
    self.assertTrue(self.manager.update_workspace(wa))
    wb = Midgard.Workspace()
    self.manager.get_workspace_by_path(wb, "/devel/testing")
    self.assertTrue(self.manager.update_workspace(wb))
    wc = Midgard.Workspace()
    self.manager.get_workspace_by_path(wc, "/devel/testing/stable")
    self.assertTrue(self.manager.update_workspace(wc))
    try:
      wd = Midgard.Workspace(name = "invalidname")
      self.manager.update_workspace(wd)
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard_workspace_storage_error-quark")
      self.assertEqual(e.code, Midgard.WorkspaceStorageError.INVALID_VALUE)
      self.assertEqual(e.message, "Invalid value ID for workspace")

  def testPathExists(self):
    self.assertTrue(self.manager.path_exists("/devel"))
    self.assertTrue(self.manager.path_exists("/devel/testing"))
    self.assertTrue(self.manager.path_exists("/devel/testing/stable"))
    self.assertFalse(self.manager.path_exists("/the/path/not/exists"))

  @unittest.skip("Not implemented in core")
  def testPurgeWorkspace(self):
    wa = Midgard.Workspace(name = "a")
    self.assertTrue(self.manager.create_workspace(wa, ""))
    wb = Midgard.Workspace(name = "b")
    self.assertTrue(self.manager.create_workspace(wb, "/a"))
    wc = Midgard.Workspace(name = "c")
    self.assertTrue(self.manager.create_workspace(wc, "/a/b"))
    # purge
    self.assertTrue(self.manager.purge_workspace(wc))
    self.assertTrue(self.manager.purge_workspace(wb))
    self.assertTrue(self.manager.purge_workspace(wa))
    
    # Invalid workspace
    wd = Midgard.Workspace(name = "d")
    try:
      self.manager.purge_workspace(wd)
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard_workspace_storage_error-quark")
      self.assertEqual(e.code, Midgard.WorkspaceStorageError.INVALID_VALUE)
      self.assertEqual(e.message, "Invalid value ID for workspace")

  def testInheritance(self): 
    self.assertIsInstance(self.manager, GObject.GObject)

if __name__ == "__main__":
    unittest.main()
