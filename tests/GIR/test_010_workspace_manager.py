# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestWorkspaceManager(unittest.TestCase):
  mgd = None
  manager = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    if self.manager is None:
      self.manager = Midgard.WorkspaceManager(connection = self.mgd)

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
    wa = Midgard.Workspace()
    wa.set_property("name", "devel")
    self.assertTrue(self.manager.create_workspace(wa, ""))
 
  def testCreateWorkspaceDuplicate(self):
    wa = Midgard.Workspace()
    wa.set_property("name", "devel")
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

  def testInheritance(self): 
    self.assertIsInstance(self.manager, GObject.GObject)

if __name__ == "__main__":
    unittest.main()
