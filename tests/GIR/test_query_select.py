# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestQuerySelect(unittest.TestCase):
  mgd = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()

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

  def testSelectAllPersons(self):
    st = Midgard.QueryStorage(dbclass = "midgard_person")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    qs.execute()
    objects = qs.list_objects()
    # Expect admin person and Smith family
    self.assertEqual(len(objects), 4);

  def getQSForSmiths(self):
    st = Midgard.QueryStorage(dbclass = "midgard_person")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    qs.set_constraint(
      Midgard.QueryConstraint(
        property = Midgard.QueryProperty(property = "lastname"),
        operator = "=",
        holder = Midgard.QueryValue.create_with_value("Smith")
      )
    )
    return qs

  def testSelectSmithFamily(self):
    qs = self.getQSForSmiths()
    qs.execute()
    # Expect Smith family - 3 persons
    self.assertEqual(qs.get_results_count(), 3);

  def testSelectNothing(self):
    st = Midgard.QueryStorage(dbclass = "midgard_person")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    qs.set_constraint(
      Midgard.QueryConstraint(
        property = Midgard.QueryProperty(property = "firstname"),
        operator = "=",
        holder = Midgard.QueryValue.create_with_value("Sir Lancelot")
      )
    )
    qs.execute()
    # Do not expect persons
    self.assertEqual(qs.get_results_count(), 0);

  def testSelectInvalidType(self): 
    st = Midgard.QueryStorage(dbclass = "NotExists")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    # Check if we have GError 
    self.assertRaises(GObject.GError, qs.execute)
    # Check if we have correct domain 
    try:
      qs.execute()
    except GObject.GError as e:
      self.assertEqual(e.domain, "midgard-validation-error-quark")
      self.assertEqual(e.code, Midgard.ValidationError.TYPE_INVALID)

  def testOrderASC(self):
    qs = self.getQSForSmiths()
    qs.add_order(Midgard.QueryProperty(property = "firstname"), "ASC")
    qs.execute()
    l = qs.list_objects()
    self.assertEqual(l[0].get_property("firstname"), "Alice")
    self.assertEqual(l[1].get_property("firstname"), "John")
    self.assertEqual(l[2].get_property("firstname"), "Marry")

  def testOrderDESC(self):
    qs = self.getQSForSmiths()
    qs.add_order(Midgard.QueryProperty(property = "firstname"), "DESC")
    qs.execute()
    l = qs.list_objects()
    self.assertEqual(l[0].get_property("firstname"), "Marry")
    self.assertEqual(l[1].get_property("firstname"), "John")
    self.assertEqual(l[2].get_property("firstname"), "Alice")

  def testInheritance(self):
    qs = Midgard.QuerySelect(connection = self.mgd)
    self.assertIsInstance(qs, Midgard.QueryExecutor)

if __name__ == "__main__":
    unittest.main()
