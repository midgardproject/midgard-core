# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_001_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestUser(unittest.TestCase):
  mgd = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()

  def tearDown(self):        
    self.mgd.close()
    self.mgd = None

  def getNewUser(self):
    user = Midgard.User(connection = self.mgd, login = "John", authtype = "Plaintext", active = True)
    return user

  def testNew(self):
    user = self.getNewUser()
    self.assertIsNot(user, None)
    self.assertIsInstance(user, Midgard.User)

  def testCreate(self):
    user = self.getNewUser()
    self.assertTrue(user.create())
    self.assertTrue(user.get_property("active"))
    # Try to create another user with the same.log_in and authentication type
    new_user = self.getNewUser()
    self.assertFalse(new_user.create())

    # Cleanup 
    user.delete()

  def testUpdate(self):
    user = self.getNewUser()
    self.assertTrue(user.create())
    user.set_property("active", False)
    self.assertTrue(user.update())
    self.assertFalse(user.get_property("active"))

    # Cleanup 
    user.delete()

  def testDelete(self):
    user = self.getNewUser()
    self.assertTrue(user.create())
    self.assertTrue(user.delete())

  def testQuery(self):
    # Should be deprecated 
    pass    

  def testGetSetPerson(self):
    user = self.getNewUser()
    person = Midgard.Object.factory(self.mgd, "midgard_person", None)
    self.assertTrue(person.create())
    self.assertTrue(user.create())

    self.assertTrue(user.set_person(person))
    self.assertEqual(user.get_person(), person)

    # Cleanup 
    self.assertTrue(user.delete())
    self.assertTrue(person.purge(False))

  def testLogin(self):
    user = self.getNewUser()
    self.assertFalse(user.log_in())

    self.assertTrue(user.create())
    self.assertTrue(user.log_in())
    self.assertTrue(user.log_in())

    # cleanup 
    self.assertTrue(user.delete())

  def testLogout(self):
    user = self.getNewUser()
    self.assertFalse(user.log_out())

    self.assertTrue(user.create())
    self.assertTrue(user.log_in())
    self.assertTrue(user.log_out())
    self.assertFalse(user.log_out())

    # cleanup 
    self.assertTrue(user.delete())

  def testInheritance(self):
    user = self.getNewUser()
    self.assertIsInstance(user, Midgard.DBObject)

if __name__ == "__main__":
    unittest.main()
