# -*- Mode: Python -*-

import os
import glob
import sys

import unittest

names = []
for filename in glob.iglob("test_*.py"):
  names.append(filename[:-3])

names.sort()

testLoader = unittest.TestLoader()
testSuite = testLoader.loadTestsFromNames(names)

runner = unittest.TextTestRunner(verbosity=2)
result = runner.run(testSuite)
if not result.wasSuccessful():
	sys.exit(1) 
