# coding=utf-8

import sys
import struct
import unittest
import time

from bookstorequery import BookStoreQuery
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestExecutionPool(unittest.TestCase):
  mgd = None
  bookstore = None
  job = None
  reference = None
  pool = None
  max_threads = 5

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()
    if self.bookstore is None:
      self.bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
      self.bookstore.set_property("name", "BookStore One")
      self.bookstore.create()
    if self.reference is None:
      self.reference = Midgard.ObjectReference(id = self.bookstore.get_property("guid"), name = "guid")
    if self.job is None:
      self.job = Midgard.SqlContentManagerJobLoad(
        connection = self.mgd,
        contentobject = self.bookstore,
        reference = self.reference)
    if self.pool is None:
      self.pool = Midgard.ExecutionPool(max_n_threads = self.max_threads);

  def tearDown(self):
    if self.bookstore is not None:
        self.bookstore.purge(False)
    self.reference = None
    self.job = None
    self.pool = None
    self.mgd.close()
    self.mgd = None

  def testInheritance(self):
    self.assertIsInstance(self.pool, GObject.Object)
    self.assertIsInstance(self.pool, Midgard.Pool)

  def testASetMaxNResources(self):
    self.pool.set_max_n_resources(self.max_threads)

  def testZGetMaxNResources(self):
    threads = self.pool.get_max_n_resources()
    self.assertNotEqual(0, self.max_threads)
    self.assertEqual(threads, self.max_threads)

  def testZPush(self): 
    self.pool.push(self.job)
    time.sleep(1)
    pool = None
    time.sleep(1)

  def testZPushInvalid(self):
    try:
      self.pool.push(self.bookstore)
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard-validation-error-quark")
      self.assertEqual(e.code, Midgard.ValidationError.TYPE_INVALID)

if __name__ == "__main__":
    unittest.main()
