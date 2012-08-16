# coding=utf-8

import sys
import struct
import unittest
from test_000_config import TestConfig
from test_100_connection import TestConnection

from gi.repository import Midgard
from gi.repository import GObject

class TestSqlQueryResultConstraints(unittest.TestCase):
  mgd = None
  select = None

  def setUp(self):
    if self.mgd is None:
      self.mgd = TestConnection.openConnection()
    if self.select is None:
      self.select = Midgard.SqlQuerySelectData(connection = self.mgd)

  def tearDown(self):
    self.mgd.close()
    self.mgd = None

  # gh-155
  def testValidateConstraintGroup(self):
    storage = Midgard.QueryStorage(dbclass = "midgard_snippet")
    columnA = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "name", storage = storage),
      name = "a",
      qualifier = "tblA"
    )
    self.select.add_column(columnA)

    columnB = Midgard.SqlQueryColumn(
      queryproperty = Midgard.QueryProperty(property = "code", storage = storage),
      name = "b",
      qualifier = "tblA"
    )
    self.select.add_column(columnB)

    cg = Midgard.QueryConstraintGroup(grouptype = "AND")
    cg.add_constraint(
      Midgard.SqlQueryConstraint(
        column = columnA,
        operator = "<>",
        holder = Midgard.QueryValue.create_with_value("")
      )
    )
    cg.add_constraint(
      Midgard.SqlQueryConstraint(
        column = columnB,
        operator = "<>",
        holder = Midgard.QueryValue.create_with_value("")
      )
    )

    self.select.set_constraint(cg)
    # Validate constraints during execution
    self.select.execute()

  def testInheritance(self):
    cg = Midgard.QueryConstraintGroup(grouptype = "AND")
    self.assertIsInstance(cg, Midgard.QueryConstraintSimple)
    self.assertIsInstance(cg, Midgard.Validable)

if __name__ == "__main__":
    unittest.main()
