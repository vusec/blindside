#!/usr/bin/python

import fileinput

def print_nice(i, barr):
  s = ('0x%04x'%i) + ': ' + ' '.join(barr[0:8]) + '  ' + ' '.join(barr[8:16]) + ' : '

  x1 = ''
  for i in reversed(barr[0:8]):
    if i == 'xx':
      x1 += '??'
    else:
      x1 += '%02x' % int(i,16)
  if x1 != '': x1 = '0x' + x1

  x2 = ''
  for i in reversed(barr[8:16]):
    if i == 'xx':
      x2 += '??'
    else:
      x2 += '%02x' % int(i,16)
  if x2 != '': x2 = '0x' + x2

  s += x1 + ' ' + x2

  print s

for line in fileinput.input():
  spl = line.split()
  for i in range(0, len(spl), 16):
    print_nice(i, spl[i:i+16])
