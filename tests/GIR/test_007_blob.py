# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard, GObject, GLib

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
    self.blob = Midgard.Blob(attachment = self.attachment)

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

  def testFileExists(self):
    self.assertFalse(self.blob.exists())
    content = "File exists?"
    self.assertTrue(self.blob.write_content(content))
    self.assertTrue(self.blob.exists())

  def testFileExists(self):
    self.assertIsNot(self.blob.get_path(), None)
    content = "File exists?"
    self.assertTrue(self.blob.write_content(content))
    self.assertIsNot(self.blob.get_path(), None)

  def testRemoveFile(self):
    self.assertFalse(self.blob.exists())
    content = "Remove file"
    self.assertFalse(self.blob.remove_file())
    self.assertTrue(self.blob.write_content(content))
    self.assertTrue(self.blob.remove_file())
    self.assertFalse(self.blob.exists())

  def testGetHandler(self):
    try: 
      handler = self.blob.get_handler("oops")
    except GObject.GError as e:
      self.assertEqual(e.message, "Invalid mode 'oops'")
    # This is new blob so fail on read attepmt */
    try:
      handler = self.blob.get_handler("r")
    except GObject.GError as e:
      self.assertEqual(e.domain, "g-file-error-quark")
      self.assertEqual(e.code, GLib.FileError.NOENT)
    valid_handler = self.blob.get_handler("w")
    self.assertIsNot(valid_handler, None)

  def testInheritance(self): 
    self.assertIsInstance(self.blob, GObject.Object)
    self.assertIsInstance(self.attachment, Midgard.DBObject)
    self.assertIsInstance(self.attachment, Midgard.Object)

if __name__ == "__main__":
    unittest.main()
