# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig

from gi.repository import Midgard, GObject

class TestConnection(Midgard.Connection):
  def __init__(self):
    Midgard.init()
    Midgard.Connection.__init__(self)

  @staticmethod
  def openConnection():
    config = TestConfig()
    mgd = Midgard.Connection()
    mgd.open_config(config)
    return mgd

class TestMethods(unittest.TestCase):
  def testOpenConfig(self):
    config = TestConfig()
    mgd = TestConnection()
    self.assertEqual(mgd.get_error_string(), "MGD_ERR_OK")
    self.assertTrue(mgd.open_config(config))
    self.assertEqual(mgd.get_error_string(), "MGD_ERR_OK")

  def testInheritance(self):
    mgd = TestConnection()
    self.assertIsInstance(mgd, GObject.GObject)

if __name__ == "__main__":
    unittest.main()
