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

  def testJoin(self):
    storage_one = Midgard.QueryStorage(dbclass = "midgard_person")
    storage_two = Midgard.QueryStorage(dbclass = "midgard_person")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = storage_one)
    group = Midgard.QueryConstraintGroup(grouptype = "AND")
    constraint_one = Midgard.QueryConstraint(
      property = Midgard.QueryProperty(property = "firstname"),
      operator = "=",
      holder = Midgard.QueryValue.create_with_value("Alice")
    )
    constraint_two = Midgard.QueryConstraint(
      property = Midgard.QueryProperty(property = "firstname", storage = storage_two),
      operator = "=",
      holder = Midgard.QueryValue.create_with_value("John")
    )
    group.add_constraint(constraint_one)
    group.add_constraint(constraint_two)
    qs.set_constraint(group)
    qs.add_join(
      "LEFT",
      Midgard.QueryProperty(property = "lastname"),
      Midgard.QueryProperty(property = "lastname", storage = storage_two)
    )
    qs.execute()
    # We expect Alice person only
    self.assertEqual(qs.get_results_count(), 1)
    objects = qs.list_objects()
    self.assertEqual(objects[0].get_property("firstname"), "Alice")

  def testConstraintGroupType(self):
    group = Midgard.QueryConstraintGroup(grouptype = "AND")
    self.assertEqual(group.get_property("grouptype"), "AND")
    self.assertEqual(group.get_group_type(), "AND")
    self.assertTrue(group.set_group_type("OR"))
    self.assertFalse(group.set_group_type("INVALID"))

  def testConstraint(self):
    person_storage = Midgard.QueryStorage(dbclass = "midgard_person")
    prop = Midgard.QueryProperty(property = "firstname", storage = person_storage) 
    constraint = Midgard.QueryConstraint(
        storage = person_storage, 
        operator = "=", 
        property = prop
    )
    # test if we get the same 
    self.assertEqual(constraint.get_storage(), person_storage)
    self.assertEqual(constraint.get_operator(), "=")
    self.assertEqual(constraint.get_property(), prop)
    # then set new ones and test again
    new_storage = Midgard.QueryStorage(dbclass = "midgard_person")
    new_prop = Midgard.QueryProperty(property = "firstname", storage = person_storage) 
    new_operator = "<>"
    constraint.set_storage(new_storage)
    constraint.set_operator(new_operator)
    constraint.set_property(new_prop)
    self.assertEqual(constraint.get_storage(), new_storage)
    self.assertEqual(constraint.get_operator(), new_operator)
    self.assertEqual(constraint.get_property(), new_prop)

  def testProperty(self):
    storage = Midgard.QueryStorage(dbclass = "midgard_person")
    prop = Midgard.QueryProperty(property = "firstname", storage = storage)
    self.assertEqual(prop.get_property("storage"), storage)
    self.assertEqual(prop.get_property("property"), "firstname")

  def testStorage(self):
    storage = Midgard.QueryStorage(dbclass = "midgard_person")
    self.assertEqual(storage.get_property("dbclass"), "midgard_person")
    storage.set_property("dbclass", "midgard_snippet")
    self.assertEqual(storage.get_property("dbclass"), "midgard_snippet")

  def testValue(self):
    self.assertEqual("ok", "Not supported by core")

  def testInheritance(self):
    qs = Midgard.QuerySelect(connection = self.mgd)
    self.assertIsInstance(qs, Midgard.QueryExecutor)

if __name__ == "__main__":
    unittest.main()
