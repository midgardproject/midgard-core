# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestObjectCrud(unittest.TestCase):
  mgd = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()

  def createGuid(self):
    try:
      guid = Midgard.Guid.new(self.mgd)
    except AttributeError:
      guid = Midgard.guid_new(self.mgd)
    return guid

  def getNewBook(self):
    obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    title = "The Holly Grail"
    author = "Sir Lancelot"
    price = 999.999
    serial = self.createGuid()
    edition = 1
    sold = False
    description = "The true story of white rabbit"

    obj.set_property("title", title)
    obj.set_property("author", author)
    obj.set_property("price", price)
    obj.set_property("serial", serial)
    obj.set_property("edition", edition)
    obj.set_property("sold", sold)
    obj.set_property("description", description)

    return obj

  def testCreate(self):
    obj = self.getNewBook()
   
    title = "The Holly Grail"
    author = "Sir Lancelot"
    price = 999.999
    serial = self.createGuid()
    edition = 1
    sold = False
    description = "The true story of white rabbit"

    obj.set_property("title", title)
    obj.set_property("author", author)
    obj.set_property("price", price)
    obj.set_property("serial", serial)
    obj.set_property("edition", edition)
    obj.set_property("sold", sold)
    obj.set_property("description", description)

    self.assertTrue(obj.create())
    self.assertTrue(Midgard.is_guid(obj.get_property("guid")))
    new_obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", obj.get_property("guid"))
    self.assertIsInstance(new_obj, Midgard.Object)
    self.assertEqual(new_obj.get_property("title"), title)
    self.assertEqual(new_obj.get_property("author"), author)
    self.assertEqual(round(new_obj.get_property("price"), 3), price) # FIXME , it should be fixed in core
    self.assertEqual(new_obj.get_property("serial"), serial)
    self.assertEqual(new_obj.get_property("edition"), edition)
    self.assertEqual(new_obj.get_property("sold"), sold)
    self.assertEqual(new_obj.get_property("description"), description)

    # TODO, metadata

    # Cleanup 
    obj.purge(False)

  def testUpdate(self):
    obj = self.getNewBook()

    title = "The Holly Grail 2"
    author = "Sir Lancelot"
    price = 09.099
    serial = self.createGuid()
    edition = 2
    sold = True
    description = "The true story of white rabbit. Part 2"

    obj.set_property("title", title)
    obj.set_property("author", author)
    obj.set_property("price", price)
    obj.set_property("serial", serial)
    obj.set_property("edition", edition)
    obj.set_property("sold", sold)
    obj.set_property("description", description)

    self.assertTrue(obj.create())
    self.assertTrue(Midgard.is_guid(obj.get_property("guid")))

    self.assertTrue(obj.update())

    new_obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", obj.get_property("guid"))
    self.assertIsInstance(new_obj, Midgard.Object)
    self.assertEqual(new_obj.get_property("title"), title)
    self.assertEqual(new_obj.get_property("author"), author)
    self.assertEqual(round(new_obj.get_property("price"), 3), price) # FIXME , it should be fixed in core
    self.assertEqual(new_obj.get_property("serial"), serial)
    self.assertEqual(new_obj.get_property("edition"), edition)
    self.assertEqual(new_obj.get_property("sold"), sold)
    self.assertEqual(new_obj.get_property("description"), description)

    # TODO, metadata

    # Cleanup 
    obj.purge(False)

  def testDelete(self):
    obj = self.getNewBook()    
    obj.set_property("title", "The book to delete")

    self.assertTrue(obj.create())
    self.assertTrue(Midgard.is_guid(obj.get_property("guid")))
    self.assertTrue(obj.delete(False))

    # TODO metadata

    new_obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", obj.get_property("guid"))
    self.assertIsNone(new_obj)

    # Cleanup 
    obj.purge(False)

  def testPurge(self):
    obj = self.getNewBook()    
    obj.set_property("title", "The book to delete")

    self.assertTrue(obj.create())
    self.assertTrue(Midgard.is_guid(obj.get_property("guid")))
    self.assertTrue(obj.purge(False))

    # TODO metadata
    new_obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", obj.get_property("guid"))
    self.assertIsNone(new_obj)


  def testInheritance(self):
    obj = Midgard.Object.factory(self.mgd, "gir_test_book_crud", None)
    self.assertIsInstance(obj, Midgard.Object)
    self.assertIsInstance(obj, Midgard.DBObject)

if __name__ == "__main__":
    unittest.main()
