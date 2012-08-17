# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestContentManagerJobCreate(unittest.TestCase):
  mgd = None
  bookstore_one = None
  bookstore_two = None
  job_one = None
  job_two = None
  reference_one = None
  reference_two = None
  callback_msg_start = None 
  callback_msg_end = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()
    if self.bookstore_one is None:
      self.bookstore_one = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
      self.bookstore_one.set_property("name", "BookStore One")
    if self.bookstore_two is None:  
      self.bookstore_two = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
      self.bookstore_two.set_property("name", "BookStore Two")
    if self.reference_one is None:
      self.reference_one = Midgard.ObjectReference(id = Midgard.Guid.new(self.mgd), name = "TestReferenceOne")
    if self.reference_two is None:
      self.reference_two = Midgard.ObjectReference(id = Midgard.Guid.new(self.mgd), name = "TestReferenceTwo")
    if self.job_one is None:
      self.job_one = Midgard.SqlContentManagerJobCreate(
          connection = self.mgd,
          contentobject = self.bookstore_one,
          reference = self.reference_one)
    if self.job_two is None:
      self.job_two = Midgard.SqlContentManagerJobCreate(
          connection = self.mgd,
          contentobject = self.bookstore_two,
          reference = self.reference_two)
    self.callback_msg_start = "TODO START"
    self.callback_msg_end = "TODO END"

  def tearDown(self):
    if self.bookstore_one is not None:
        self.bookstore_one.purge(False)
    if self.bookstore_two is not None:
        self.bookstore_two.purge(False)
    self.reference_one = None
    self.reference_two = None
    self.job_one = None
    self.job_two = None
    self.mgd.close()
    self.mgd = None

  def testInheritance(self):
    self.assertIsInstance(self.job_one, GObject.Object)
    self.assertIsInstance(self.job_one, Midgard.Validable)
    self.assertIsInstance(self.job_one, Midgard.Executable)
    self.assertIsInstance(self.job_one, Midgard.Job)
    self.assertIsInstance(self.job_one, Midgard.ContentManagerJob)
    self.assertIsInstance(self.job_one, Midgard.SqlContentManagerJob)
    self.assertIsInstance(self.job_two, GObject.Object)
    self.assertIsInstance(self.job_two, Midgard.Validable)
    self.assertIsInstance(self.job_two, Midgard.Executable)
    self.assertIsInstance(self.job_two, Midgard.Job)
    self.assertIsInstance(self.job_two, Midgard.ContentManagerJob)
    self.assertIsInstance(self.job_two, Midgard.SqlContentManagerJob)

  def testAIsValid(self):
    self.assertFalse(self.job_one.is_valid())    
    self.assertFalse(self.job_two.is_valid())    

  def testAIsRunning(self):
    self.assertFalse(self.job_one.is_running())    
    self.assertFalse(self.job_two.is_running())    

  def testAIsExecuted(self):
    self.assertFalse(self.job_one.is_executed())    
    self.assertFalse(self.job_two.is_executed())    

  def testAIsFailed(self):
    self.assertFalse(self.job_one.is_failed())    
    self.assertFalse(self.job_two.is_failed())    

  def testGetConnection(self):
    self.assertEqual(self.job_one.get_connection(), self.mgd)    
    self.assertEqual(self.job_two.get_connection(), self.mgd)    

  def testGetContentObject(self):
    self.assertEqual(self.job_one.get_content_object(), self.bookstore_one)    
    self.assertEqual(self.job_two.get_content_object(), self.bookstore_two)    

  def testGetReference(self):
    self.assertEqual(self.job_one.get_reference(), self.reference_one)    
    self.assertEqual(self.job_two.get_reference(), self.reference_two)    

  def testGetModel(self):
    self.assertEqual(self.job_one.get_model(), None)    
    self.assertEqual(self.job_two.get_model(), None)    

  def executionStartCallback(self, obj, arg):
    self.callback_msg_start = "DONE START"

  def executionEndCallback(self, obj, arg):
    self.callback_msg_end = "DONE END"

  def testZExecute(self):
    self.job_one.connect("execution-start", self.executionStartCallback, None)
    self.job_one.connect("execution-end", self.executionEndCallback, None)
    self.job_one.execute()
    self.assertTrue(self.job_one.is_valid())
    self.assertNotEqual(self.callback_msg_start, None)
    self.assertEqual(self.callback_msg_start, "DONE START")
    self.assertNotEqual(self.callback_msg_end, None)
    self.assertEqual(self.callback_msg_end, "DONE END")
    # job two
    self.job_two.connect("execution-start", self.executionStartCallback, None)
    self.job_two.connect("execution-end", self.executionEndCallback, None)
    self.job_two.execute()
    self.assertTrue(self.job_two.is_valid())
    self.assertNotEqual(self.callback_msg_start, None)
    self.assertEqual(self.callback_msg_start, "DONE START")
    self.assertNotEqual(self.callback_msg_end, None)
    self.assertEqual(self.callback_msg_end, "DONE END")


if __name__ == "__main__":
    unittest.main()
