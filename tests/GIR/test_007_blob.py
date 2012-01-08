# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestBlob(unittest.TestCase):
  mgd = None
  attachment = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()
    self.attachment = Midgard.Object.factory(self.mgd, "midgard_attachment", None)
    self.attachment.set_property("title", "TestTitle")
    self.attachment.set_property("name", "TestName")
    self.attachment.create()
    self.blob = Midgard.Blob.create_blob(self.attachment, None)

  def tearDown(self):
    if self.blob is not None:
      if self.blob.exists():
        self.blob.remove_file()
    self.attachment.purge(False)

  def testBlobNew(self):
    self.assertIsNot(self.blob, None)
    self.assertIsNot(self.attachment, None)

  def testWriteContent(self):
    self.assertTrue(self.blob.write_content("Bździna wybżdziła"))
   
  def testReadContent(self):
    content = "Bździna wybżdziła"
    self.assertTrue(self.blob.write_content(content))
    c = self.blob.read_content()
    self.assertEqual(c[0], content)

  def testInheritance(self): 
    self.assertIsInstance(self.blob, GObject.Object)
    self.assertIsInstance(self.attachment, Midgard.DBObject)
    self.assertIsInstance(self.attachment, Midgard.Object)

if __name__ == "__main__":
    unittest.main()
