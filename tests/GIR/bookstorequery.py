# coding=utf-8

import os
import sys

from gi.repository import Midgard, GObject, GLib

class BookStoreQuery():

  @staticmethod
  def findByName(mgd, name):
    storage = Midgard.QueryStorage(dbclass = "gir_test_book_store")
    qs = Midgard.QuerySelect(connection = mgd, storage = storage)
    qs.toggle_read_only(False)
    qs.set_constraint(
      Midgard.QueryConstraint(
        property = Midgard.QueryProperty(property = "name"),
        operator = "=",
        holder = Midgard.QueryValue.create_with_value(name)
      )
    )
    qs.execute()
    return qs.list_objects()
