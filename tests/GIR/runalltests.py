# -*- Mode: Python -*-

import os
import glob
import sys
import shutil

import unittest

names = []
for filename in glob.iglob("test_*.py"):
  names.append(filename[:-3])

names.sort()

testLoader = unittest.TestLoader()
testSuite = testLoader.loadTestsFromNames(names)

runner = unittest.TextTestRunner(verbosity=2)
result = runner.run(testSuite)

# Cleanup
try:
  shutil.rmtree("./test_data/blobs")
except OSError:
  pass

if os.path.isfile("./test_data/test_gir.db"):
  os.remove("./test_data/test_gir.db")

if not result.wasSuccessful():
	sys.exit(1) 
