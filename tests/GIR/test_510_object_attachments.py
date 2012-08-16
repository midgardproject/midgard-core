# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_100_connection import TestConnection

from gi.repository import Midgard, GObject

class TestAttachment(unittest.TestCase):
  mgd = None
  obj = None
  bookstore = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    if self.bookstore is None:
      self.bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
      self.bookstore.set_property("name", "BookStore")
      self.bookstore.create()
    if self.obj is None:
      self.obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
      self.obj.set_property("title", "The Hoy Grail and Attachments")
      self.obj.set_property("store", self.bookstore.get_property("id"))
      self.assertTrue(self.obj.create())

  def tearDown(self):        
    if self.bookstore is not None:
      self.bookstore.purge(False)
    self.mgd.close()
    self.mgd = None

  def getBook(self):
    st = Midgard.QueryStorage(dbclass = "gir_test_book_crud")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    qs.set_constraint(
      Midgard.QueryConstraint(
          property = Midgard.QueryProperty(property = "title"),
          operator = "=",
          holder = Midgard.QueryValue.create_with_value("The Hoy Grail and Attachments")
          )
        )
    qs.execute()
    res = qs.list_objects()
    return res[0]

  def testCreateAttachment(self):
    attA = self.obj.create_attachment("A_Name", "A_Title", "image/png")
    self.assertIsInstance(attA, Midgard.Object)
    attB = self.obj.create_attachment("B_Name", "B_Title", "text/plain")
    self.assertIsInstance(attB, Midgard.Object)
    attC = self.obj.create_attachment("C_Name", "C_Title", "")
    self.assertIsInstance(attC, Midgard.Object)
    self.assertTrue(self.obj.has_attachments())

  def testListAttachments(self):
    obj = self.getBook()
    atts = obj.list_attachments()
    self.assertEqual(len(atts), 3)
    names = {"A_Name", "B_Name", "C_Name"}
    for att in atts:
      self.assertIn(att.get_property("name"), names)

  @unittest.skip("Fix RuntimeError for GObject.Parameter")
  def testFindAttachments(self):
    param = GObject.Parameter()
    param.name = "name"
    param.value = "A_Name"
    atts = self.obj.find_attachments(1, param)
    self.assertEqual(len(atts), 1)

  def testHasAttachments(self):
    obj = self.getBook()
    self.assertTrue(obj.has_attachments())

  def testZDeleteAttachments(self):
    obj = self.getBook()
    self.assertTrue(obj.delete_attachments(0, None))

  def testZPurgeAttachments(self):
    attA = self.obj.create_attachment("A_Name", "A_Purge", "image/png")
    self.assertIsInstance(attA, Midgard.Object)
    attB = self.obj.create_attachment("B_Name", "B_Purge", "text/plain")
    self.assertIsInstance(attA, Midgard.Object)
    self.assertTrue(self.obj.purge_attachments(False, 0, None))

  def testInheritance(self):
    att = Midgard.Object.factory(self.mgd, "midgard_attachment", None)
    self.assertIsInstance(att, Midgard.Object)
    self.assertIsInstance(att, Midgard.DBObject)

if __name__ == "__main__":
    unittest.main()
