# coding=utf-8

import os
import sys
import struct
import unittest

from gi.repository import Midgard, GObject

class TestConfig(Midgard.Config):
  def __init__(self):
    Midgard.Config.__init__(self)
    config_file = os.getenv("MIDGARD_TEST_CONFIGURATION_FILE")
    if config_file == "" or config_file == None:
      config_file = "./test_SQLITE.conf"
    self.read_file_at_path(config_file)

  def setProperties(self):
    self.set_property("dbdir", "/tmp")
    self.set_property("dbtype", "SQLite")
    self.set_property("database", "phpcr")
    self.set_property("loglevel", "warn")

class TestProperties(unittest.TestCase):
  def testProperties(self):
    config = TestConfig()
    config.setProperties()
    self.assertEqual(config.get_property("dbdir"), "/tmp") 
    self.assertEqual(config.get_property("dbtype"), "SQLite") 
    self.assertEqual(config.get_property("database"), "phpcr") 
    self.assertEqual(config.get_property("loglevel"), "warn") 
    self.assertTrue(hasattr(config.props, "blobdir"))
    self.assertTrue(hasattr(config.props, "dbuser"))
    self.assertTrue(hasattr(config.props, "dbpass"))
    self.assertTrue(hasattr(config.props, "dbtype"))
    # False tests
    self.assertNotEqual(config.get_property("dbdir"), "/tmp/test")
    self.assertFalse(hasattr(config.props, "notexists"))

  def testReadFileAtPath(self):
    config = Midgard.Config()
    self.assertTrue(config.read_file_at_path("./test_SQLITE.conf"));
    self.assertRaises(GObject.GError, config.read_file_at_path, "notexists")

  def testSaveFile(self):
    self.assertEqual("ok", "NOT SUPPORTED BY CORE")

  def testBlobDir(self):
    config = TestConfig()
    self.assertTrue(config.create_blobdir())

  def testInheritance(self):
    config = TestConfig()
    self.assertIsInstance(config, GObject.Object.__class__)

if __name__ == "__main__":
    unittest.main()
