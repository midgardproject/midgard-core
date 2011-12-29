# coding=utf-8

import sys
import struct
import unittest

from gi.repository import Midgard
from gi.repository import GObject

class TestConfig(Midgard.Config):
  def __init__(self):
    Midgard.Config.__init__(self)
    self.setProperties()

  def setProperties(self):
    self.set_property("dbdir", "/tmp")
    self.set_property("dbtype", "SQLite")
    self.set_property("database", "phpcr")
    self.set_property("loglevel", "warn")

class TestProperties(unittest.TestCase):
  def testProperties(self):
    config = TestConfig()
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

  def testObject(self):
    config = TestConfig()
    self.assertIsInstance(config, GObject.Object)

if __name__ == "__main__":
    unittest.main()
