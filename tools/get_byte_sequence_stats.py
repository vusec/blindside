#!/usr/bin/python

import sys

def print_usage_exit():
  print "Usage: %s <file>" % sys.argv[0]
  sys.exit(1)

if len(sys.argv) < 2:
  print_usage_exit()

file1 = open(sys.argv[1], 'rb')

byte_freq = {}

def mask_byte(x):
  if x == 'xx':
    return x
  return x[0]+str(int(x[1],16)&8)

prev_byte = None
total_bytes = 0
for l in file1.xreadlines():
  curr_byte = l.strip()
  if not curr_byte:
    continue

  total_bytes += 1

  if curr_byte not in byte_freq:
    byte_freq[curr_byte] = [0, {}]
  byte_freq[curr_byte][0] += 1

  if prev_byte != None:
    if prev_byte not in byte_freq[curr_byte][1]:
      byte_freq[curr_byte][1][prev_byte] = 0
    byte_freq[curr_byte][1][prev_byte] += 1

  #prev_byte = curr_byte
  prev_byte = mask_byte(curr_byte)

print 'Number of processed bytes:', total_bytes

fname = 'code_bytes_table.h'
out = open(fname, 'wb')
out.write('#ifndef __CSTRUCT_HEADER__\n')
out.write('#define __CSTRUCT_HEADER__\n')

out.write('\ntypedef struct {\n')
out.write('  unsigned char code_byte;\n')
out.write('  unsigned char percentage;\n')
out.write('} code_byte_info;\n')

out.write('\ncode_byte_info code_byte_table[256][32] = {\n');

for b in range(256):
  bstr = '%02X' % b
  binfo = byte_freq.get(bstr, [0, {}])
  b_freq = binfo[0]
  sub_total = 0
  count = 0
  out.write(' /* 0x%02X: */ {' % b);
  prev_bytes_sorted_by_freq = reversed(sorted([ (byte_freq[bstr][1][pb], pb) for pb in binfo[1] ]))
  for (pb_freq, pb) in prev_bytes_sorted_by_freq:
    if pb == 'xx':
      continue
    sub_total += pb_freq

    if b_freq == 0:
      percentage = 0
    else:
      percentage = int(sub_total*100.0 / b_freq)
    out.write('{0x%s,%d},' % (pb, percentage))
    count += 1
  for i in range(32-count):
    out.write('{0x00,0},')

  out.write('},\n')

out.write('};\n');

out.write('\n#endif\n')
out.close()

print 'Wrote code_byte_tables to:', fname
print ''
