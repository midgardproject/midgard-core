# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestStorage(unittest.TestCase):
  def testBaseStorage(self):
    mgd = TestConnection.openConnection()
    self.assertTrue(Midgard.Storage.create_base_storage(mgd))

  def testCreateTypeStorage(self):
    self.assertEqual("ok", "NOT IMPLEMENTED")

  def testUpdateTypeStorage(self):
    self.assertEqual("ok", "NOT IMPLEMENTED")

if __name__ == "__main__":
    unittest.main()
