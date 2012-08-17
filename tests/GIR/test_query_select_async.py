# coding=utf-8

import sys
import struct
import unittest
import time

from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject
from gi.repository import GLib

class TestQuerySelectAsync(unittest.TestCase):
  mgd = None
  async_execution_callbacks_count = 0
  pool = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()

  def tearDown(self):
    self.pool = None
    self.mgd.close()
    self.mgd = None

  # Create three persons for *all* tests 
  @staticmethod
  def setUpClass():
    mgd = TestConnection.openConnection()
    tr = Midgard.Transaction(connection = mgd)
    tr.begin()
    # Create three persons for tests 
    a = Midgard.Object.factory(mgd, "midgard_person", None)
    a.set_property("firstname", "Alice")
    a.set_property("lastname", "Smith")
    a.create()
    b = Midgard.Object.factory(mgd, "midgard_person", None)
    b.set_property("firstname", "John")
    b.set_property("lastname", "Smith")
    b.create()
    c = Midgard.Object.factory(mgd, "midgard_person", None)
    c.set_property("firstname", "Marry")
    c.set_property("lastname", "Smith")
    c.create()
    tr.commit()

  # Purge three persons after all tests are done
  @staticmethod
  def tearDownClass():
    mgd = TestConnection.openConnection()
    tr = Midgard.Transaction(connection = mgd)
    tr.begin()
    st = Midgard.QueryStorage(dbclass = "midgard_person")
    qs = Midgard.QuerySelect(connection = mgd, storage = st)
    qs.set_constraint(
      Midgard.QueryConstraint(
        property = Midgard.QueryProperty(property = "lastname"),
        operator = "=",
        holder = Midgard.QueryValue.create_with_value("Smith")
      )
    )
    qs.execute()
    for person in qs.list_objects(): 
      person.purge(False)
    tr.commit()

  def ExecutionAsyncEndCallback(self, obj, arg):
    self.async_execution_callbacks_count += 1  


  def testSelectAsyncAllPersons(self):
    st = Midgard.QueryStorage(dbclass = "midgard_person")
    self.pool = Midgard.ExecutionPool(max_n_threads = 10)
    l = [] # Hold all executors in array to avoid implicit objects' destruction
    for i in range(0, 30):
      qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
      qs.connect("execution-end", self.ExecutionAsyncEndCallback, i)
      self.pool.push(qs)
      l.append(qs)
    # Emulate main loop - ugly   
    time.sleep(1)
    self.pool = None
    self.assertEqual(self.async_execution_callbacks_count, 30)

  def testInheritance(self):
    qs = Midgard.QuerySelect(connection = self.mgd)
    self.assertIsInstance(qs, Midgard.QueryExecutor)
    self.assertIsInstance(qs, Midgard.Executable)

if __name__ == "__main__":
    unittest.main()
