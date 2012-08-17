# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard, GObject, GLib

class TestReflectorObject(unittest.TestCase):
  mgd = None
  reflectors = False

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()
    if self.reflectors is False:
      self.rfl_person = Midgard.ReflectorProperty(dbclass = "midgard_person")
      self.rfl_file = Midgard.ReflectorProperty(dbclass = "nt_file")
      self.rfl_resource = Midgard.ReflectorProperty(dbclass = "nt_resource")
      self.rfl_snippet = Midgard.ReflectorProperty(dbclass = "midgard_snippet")
      self.rfl_snippetdir = Midgard.ReflectorProperty(dbclass = "midgard_snippetdir")
      self.rfl_activity = Midgard.ReflectorProperty(dbclass = "midgard_activity")
      self.reflectors = True

  def tearDown(self):        
    self.mgd.close()
    self.mgd = None

  def testGetMidgardType(self):
    self.assertEqual(self.rfl_person.get_midgard_type("guid"), GObject.GType.from_name("MidgardGuid"))

  def testIsLink(self):
    self.assertFalse(self.rfl_person.is_link("id"))
    self.assertTrue(self.rfl_snippet.is_link("snippetdir"))
    self.assertTrue(self.rfl_snippetdir.is_link("up"))
    self.assertFalse(self.rfl_resource.is_link("jcr-data"))
    self.assertFalse(self.rfl_resource.is_link("notexists"))

  def testIsLinked(self):
    self.assertTrue(self.rfl_person.is_linked("id"))
    self.assertFalse(self.rfl_person.is_linked("firstname"))
    self.assertFalse(self.rfl_snippet.is_linked("snippetdir"))
    self.assertTrue(self.rfl_snippetdir.is_linked("id"))
    self.assertFalse(self.rfl_resource.is_linked("jcr-data"))
    self.assertFalse(self.rfl_resource.is_linked("notexists"))

  def testGetLinkName(self):
    self.assertEqual(self.rfl_person.get_link_name("id"), None)
    self.assertEqual(self.rfl_person.get_link_name("firstname"), None)
    self.assertEqual(self.rfl_snippet.get_link_name("snippetdir"), "midgard_snippetdir")
    self.assertEqual(self.rfl_snippetdir.get_link_name("up"), "midgard_snippetdir")
    self.assertEqual(self.rfl_resource.get_link_name("jcr-data"), None)
    self.assertEqual(self.rfl_resource.get_link_name("notexists"), None)
    self.assertEqual(self.rfl_activity.get_link_name("actor"), "midgard_person")

  def testGetLinkTarget(self):
    self.assertEqual(self.rfl_person.get_link_target("id"), None)
    self.assertEqual(self.rfl_person.get_link_target("firstname"), None)
    self.assertEqual(self.rfl_snippet.get_link_target("snippetdir"), "id")
    self.assertEqual(self.rfl_snippetdir.get_link_target("up"), "id")
    self.assertEqual(self.rfl_resource.get_link_target("jcr-data"), None)
    self.assertEqual(self.rfl_resource.get_link_target("notexists"), None)
    self.assertEqual(self.rfl_activity.get_link_target("actor"), "id")

  def testPropertyDescription(self):
    self.assertEqual(self.rfl_person.description("id"), "Local non-replication-safe database identifier")
    self.assertEqual(self.rfl_person.description("firstname"), "First name of the person")
    self.assertEqual(self.rfl_resource.description("jcr-data"), "Binary data") 
    self.assertEqual(self.rfl_resource.description("notexists"), None) 

  def testIsPrivate(self):
    self.assertFalse(self.rfl_person.is_private("id"))
    self.assertFalse(self.rfl_person.is_private("firstname"))
    self.assertFalse(self.rfl_snippet.is_private("snippetdir"))
    self.assertFalse(self.rfl_snippetdir.is_private("id"))
    self.assertFalse(self.rfl_resource.is_private("jcr-data"))
    self.assertFalse(self.rfl_resource.is_private("notexists"))

  def testIsUnique(self):
    self.assertFalse(self.rfl_person.is_unique("id"))
    self.assertTrue(self.rfl_person.is_unique("firstname"))
    self.assertFalse(self.rfl_snippet.is_unique("snippetdir"))
    self.assertFalse(self.rfl_snippetdir.is_unique("id"))
    self.assertFalse(self.rfl_resource.is_unique("jcr-data"))
    self.assertFalse(self.rfl_resource.is_unique("notexists"))

  def testIsPrimary(self):
    self.assertTrue(self.rfl_person.is_primary("id"))
    self.assertFalse(self.rfl_snippet.is_primary("snippetdir"))
    self.assertTrue(self.rfl_snippetdir.is_primary("id"))
    self.assertFalse(self.rfl_resource.is_primary("jcr-data"))
    self.assertFalse(self.rfl_resource.is_primary("notexists"))

  def testHasDefaultValue(self):
    self.assertFalse(self.rfl_person.has_default_value("id"))
    self.assertFalse(self.rfl_snippet.has_default_value("snippetdir"))
    self.assertFalse(self.rfl_snippetdir.has_default_value("id"))
    self.assertFalse(self.rfl_resource.has_default_value("notexists"))

  def testGetDefaultValue(self):
    self.assertFalse(self.rfl_person.get_default_value("id"), None)
    self.assertEqual(self.rfl_person.get_default_value("firstname"), None)
    self.assertEqual(self.rfl_snippet.get_default_value("snippetdir"), None)
    self.assertEqual(self.rfl_resource.get_default_value("notexists"), None)

  def testGetUserValue(self):
    self.assertEqual(self.rfl_person.get_user_value("id", "notexists"), None)
    self.assertEqual(self.rfl_person.get_user_value("firstname", "property"), "foaf:firstName")
    self.assertEqual(self.rfl_resource.get_user_value("notexists", "notexists"), None)
    self.assertEqual(self.rfl_resource.get_user_value("jcr-data", "RequiredType"), "BINARY")
    self.assertEqual(self.rfl_resource.get_user_value("jcr-data", "isMultiple"), "false")

if __name__ == "__main__":
    unittest.main()
