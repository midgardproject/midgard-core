# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestObjectMetadataCrud(unittest.TestCase):
  mgd = None
  bookstore = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()
    if self.bookstore is None:
      self.bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
      self.bookstore.set_property("name", "BookStore")
      self.bookstore.create()

  def tearDown(self):
    if self.bookstore is not None:
      self.bookstore.purge(False)
    self.mgd.close()
    self.mgd = None

  def getNewBook(self):
    obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    title = "The Metadata of a Holly Grail"
    obj.set_property("title", title)
    obj.set_property("store", self.bookstore.get_property("id"))
    return obj

  def testMetadataCreate(self):
    obj = self.getNewBook()
    
    score = -1
    authors = "White rabbit team"
    hidden = False

    metadata = obj.get_property("metadata")
    metadata.set_property("score", score)
    metadata.set_property("authors", authors)
    metadata.set_property("hidden", hidden)

    self.assertTrue(obj.create())
    new_obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", obj.get_property("guid"))
    self.assertIsInstance(new_obj, Midgard.Object)
    new_metadata = new_obj.get_property("metadata")
    self.assertEqual(new_metadata.get_property("score"), score)
    self.assertEqual(new_metadata.get_property("authors"), authors)
    self.assertEqual(new_metadata.get_property("hidden"), hidden)

    # Read only properties 
    self.assertIsNot(new_metadata.get_property("created"), None)
    self.assertIsNot(new_metadata.get_property("revised"), None)
    self.assertIsNot(new_metadata.get_property("revision"), 0)

    # Cleanup 
    obj.purge(False)

  def testMetadataUpdate(self):
    obj = self.getNewBook()
    
    score = 1
    authors = "White rabbit team & sons"
    hidden = True

    self.assertTrue(obj.create())

    metadata = obj.get_property("metadata")
    metadata.set_property("score", score)
    metadata.set_property("authors", authors)
    metadata.set_property("hidden", hidden)

    self.assertTrue(obj.update())
    
    new_obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", obj.get_property("guid"))
    self.assertIsInstance(new_obj, Midgard.Object)

    metadata = new_obj.get_property("metadata")

    self.assertEqual(metadata.get_property("score"), score)
    self.assertEqual(metadata.get_property("authors"), authors)
    self.assertEqual(metadata.get_property("hidden"), hidden)

    # Read only properties 
    self.assertIsNot(metadata.get_property("created"), None)
    self.assertIsNot(metadata.get_property("revised"), None)
    self.assertIsNot(metadata.get_property("revision"), 1)

    # Cleanup 
    obj.purge(False)

  def testMetadataDelete(self):
    obj = self.getNewBook()
    
    score = 0
    authors = "White rabbit team & sons"
    hidden = True

    self.assertTrue(obj.create())
    new_obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", obj.get_property("guid"))
    self.assertIsInstance(new_obj, Midgard.Object)

    metadata = new_obj.get_property("metadata")
    metadata.set_property("score", score)
    metadata.set_property("authors", authors)
    metadata.set_property("hidden", hidden)

    self.assertTrue(obj.delete(False))

    self.assertEqual(metadata.get_property("score"), score)
    self.assertEqual(metadata.get_property("authors"), authors)
    self.assertEqual(metadata.get_property("hidden"), hidden)

    # Read only properties 
    self.assertIsNot(metadata.get_property("created"), None)
    self.assertIsNot(metadata.get_property("revised"), None)
    self.assertIsNot(metadata.get_property("revision"), 1)
    self.assertFalse(metadata.get_property("deleted"))

    # Cleanup 
    obj.purge(False)

  def testInheritance(self):
    obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    self.assertIsInstance(obj, Midgard.Object)
    self.assertIsInstance(obj, Midgard.DBObject)

if __name__ == "__main__":
    unittest.main()
