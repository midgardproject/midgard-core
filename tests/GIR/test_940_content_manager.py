# coding=utf-8

import sys
import struct
import unittest
import time
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestContentManagerJobCreate(unittest.TestCase):
  mgd = None
  bookstore = None
  reference = None
  manager = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()
    if self.bookstore is None:
      self.bookstore = Midgard.Object.factory(self.mgd, "gir_test_book_store", None)
    if self.reference is None:
      self.reference = Midgard.ObjectReference(id = Midgard.Guid.new(self.mgd), name = "TestReferenceOne")
    if self.manager is None:
      self.manager = Midgard.SqlContentManager(connection = self.mgd)

  def tearDown(self):
    self.bookstore = None
    self.reference = None
    self.job = None
    self.manager = None
    self.mgd.close()
    self.mgd = None

  def testInheritance(self):
    self.assertIsInstance(self.manager, GObject.Object)
    self.assertIsInstance(self.manager, Midgard.ContentManager)

  def testGetConnection(self):
    self.assertNotEqual(self.manager.get_connection(), None)
    self.assertEqual(self.manager.get_connection(), self.mgd)    

  def testCreateLoadJob(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.LOAD, self.bookstore, self.reference, None)
    self.assertIsInstance(job, Midgard.SqlContentManagerJobLoad)

  def testJobLoadObjects(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.LOAD, self.bookstore, self.reference, None)
    self.assertEqual(job.get_content_object(), self.bookstore)
    self.assertEqual(job.get_model(), None)
    self.assertEqual(job.get_reference(), self.reference)

  def testCreateCreateJob(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.CREATE, self.bookstore, self.reference, None)
    self.assertIsInstance(job, Midgard.SqlContentManagerJobCreate)

  def testJobCreateObjects(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.LOAD, self.bookstore, self.reference, None)
    self.assertEqual(job.get_content_object(), self.bookstore)
    self.assertEqual(job.get_model(), None)
    self.assertEqual(job.get_reference(), self.reference)

  def testCreateUpdateJob(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.UPDATE, self.bookstore, self.reference, None)
    self.assertIsInstance(job, Midgard.SqlContentManagerJobUpdate)

  def testJobUpdateObjects(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.LOAD, self.bookstore, self.reference, None)
    self.assertEqual(job.get_content_object(), self.bookstore)
    self.assertEqual(job.get_model(), None)
    self.assertEqual(job.get_reference(), self.reference)

  def testCreateDeleteJob(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.DELETE, self.bookstore, self.reference, None)
    self.assertIsInstance(job, Midgard.SqlContentManagerJobDelete)

  def testJobDeleteObjects(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.LOAD, self.bookstore, self.reference, None)
    self.assertEqual(job.get_content_object(), self.bookstore)
    self.assertEqual(job.get_model(), None)
    self.assertEqual(job.get_reference(), self.reference)

  def testCreatePurgeJob(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.PURGE, self.bookstore, self.reference, None)
    self.assertIsInstance(job, Midgard.SqlContentManagerJobPurge)

  def testJobPurgeObjects(self):
    job = self.manager.create_job(Midgard.ContentManagerJobType.LOAD, self.bookstore, self.reference, None)
    self.assertEqual(job.get_content_object(), self.bookstore)
    self.assertEqual(job.get_model(), None)
    self.assertEqual(job.get_reference(), self.reference)

if __name__ == "__main__":
    unittest.main()
