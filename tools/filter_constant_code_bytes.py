#!/usr/bin/python

import sys

if len(sys.argv) < 3:
  print "Usage: %s <file1-with-code-bytes> <file2-with-code-bytes>" % sys.argv[0]
  sys.exit(1)

file1 = open(sys.argv[1], 'rb')
file2 = open(sys.argv[2], 'rb')

def read_line(fd):
  for l in fd.xreadlines():
    yield l

def next_line(fd):
  return next(read_line(fd), None)

while True:
  l1 = next_line(file1)
  l2 = next_line(file2)

  if l1 == None and l2 == None: break
  #if l1 == None: l1 = ''
  #if l2 == None: l2 = ''

  if l1 == l2:
    print l1,
  else:
    print 'xx'
  #print '[', b1, b2, ']'
