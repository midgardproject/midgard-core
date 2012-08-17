# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_020_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestSchemaObjectTree(unittest.TestCase):
  mgd = None

  def setUp(self):
    if self.mgd == None:
      self.mgd = TestConnection.openConnection()

  def purgeSnippets(self):
    tr = Midgard.Transaction(connection = self.mgd)
    tr.begin()
    st = Midgard.QueryStorage(dbclass = "midgard_snippet")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    qs.execute()
    for s in qs.list_objects():
      s.purge(False)
    st = Midgard.QueryStorage(dbclass = "midgard_snippetdir")
    qs = Midgard.QuerySelect(connection = self.mgd, storage = st)
    qs.execute()
    for s in qs.list_objects():
      s.purge(False)
    tr.commit()

  def tearDown(self):        
    self.purgeSnippets()
    self.mgd.close()
    self.mgd = None

  def testObjectList(self):
    # Create object and two child ones
    sdirA = Midgard.Object.factory(self.mgd, "midgard_snippetdir", None)
    sdirA.set_property("name", "List A")
    self.assertTrue(sdirA.create())
    idA = sdirA.get_property("id")

    sdirB = Midgard.Object.factory(self.mgd, "midgard_snippetdir", None)
    sdirB.set_property("name", "List B")
    sdirB.set_property("up", idA)
    self.assertTrue(sdirB.create())
    
    sdirC = Midgard.Object.factory(self.mgd, "midgard_snippetdir", None)
    sdirC.set_property("name", "List C")
    sdirC.set_property("up", idA)
    self.assertTrue(sdirC.create())

    # check if we can get child ones
    ret = Midgard.SchemaObjectTree.list_objects(sdirA)
    self.assertEqual(len(ret), 2)
    names = {"List B", "List C"}
    self.assertIn(ret[0].get_property("name"), names)
    self.assertIn(ret[1].get_property("name"), names)

  def testObjectListChildren(self):
    # Create object and two child ones
    sdirA = Midgard.Object.factory(self.mgd, "midgard_snippetdir", None)
    sdirA.set_property("name", "Dir with snippets")
    self.assertTrue(sdirA.create())
    idA = sdirA.get_property("id")

    sA = Midgard.Object.factory(self.mgd, "midgard_snippet", None)
    sA.set_property("name", "A")
    sA.set_property("snippetdir", idA)
    self.assertTrue(sA.create())
    
    sB = Midgard.Object.factory(self.mgd, "midgard_snippet", None)
    sB.set_property("name", "B")
    sB.set_property("snippetdir", idA)
    self.assertTrue(sB.create())

    sC = Midgard.Object.factory(self.mgd, "midgard_snippet", None)
    sC.set_property("name", "C")
    sC.set_property("snippetdir", idA)
    self.assertTrue(sC.create())

    # check if we can get child ones
    ret = Midgard.SchemaObjectTree.list_children_objects(sdirA, "midgard_snippet")
    self.assertEqual(len(ret), 3)
    names = {"A", "B", "C"}
    self.assertIn(ret[0].get_property("name"), names)
    self.assertIn(ret[1].get_property("name"), names)
    self.assertIn(ret[2].get_property("name"), names)

  def testGetParentName(self):
    snippet = Midgard.Object.factory(self.mgd, "midgard_snippet", None)
    self.assertEqual(Midgard.SchemaObjectTree.get_parent_name(snippet), "midgard_snippetdir")
    snippetdir = Midgard.Object.factory(self.mgd, "midgard_snippetdir", None)
    self.assertEqual(Midgard.SchemaObjectTree.get_parent_name(snippetdir), None)

  def testGetParentObjectOfTheSameType(self):
    snippetdir = Midgard.Object.factory(self.mgd, "midgard_snippetdir", None)
    snippetdir.set_property("name", "Parent A")
    self.assertTrue(snippetdir.create())
    child = Midgard.Object.factory(self.mgd, "midgard_snippetdir", None)
    child.set_property("name", "Parent B")
    child.set_property("up", snippetdir.get_property("id"))
    self.assertTrue(child.create())
    parent = Midgard.SchemaObjectTree.get_parent_object(child)
    self.assertEqual(parent.get_property("guid"), snippetdir.get_property("guid"))

if __name__ == "__main__":
    unittest.main()
