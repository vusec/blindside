#!/usr/bin/python

import sys
import os
from lib.bashcmd import BashCmd
from capstone import *

if len(sys.argv) < 3:
  print "Usage: %s <kvaddr> <num-ins>" % sys.argv[0]
  sys.exit(1)

kvaddr = int(sys.argv[1], 16)
num_ins = int(sys.argv[2])

extract_bytes_cmd = os.path.dirname(os.path.realpath(__file__))+'/lib/print_kernel_bytes'
num_bytes = 15
bc = BashCmd('.')
md = Cs(CS_ARCH_X86, CS_MODE_64)
#md.syntax = CS_OPT_SYNTAX_ATT

count = 0
while count < num_ins:

  cmd = extract_bytes_cmd + (' 0x%x %d' % (kvaddr,num_bytes))
  if not bc.run(cmd):
    print "bc.run(%s) failed"
    print "bc.stderr()=%s" % bc.stderr().strip()
    sys.exit(1)

  code_bytes_str = bc.stdout().split()
  code_bytes = ''.join([chr(int(b,16)) for b in code_bytes_str])

  g = md.disasm(code_bytes, kvaddr)
  ins_disas = next(g, None)
  if not ins_disas:
    #print "Error, no ins:", ("%x" % kvaddr)
    print "%3d: 0x%x-0x%x:\t%s\t%-25s\t%d:%x" % (count+1, kvaddr, kvaddr+1, "(bad)", "", 1, ord(code_bytes[0]))
    kvaddr += 1
    count += 1
    continue


  print "%3d: 0x%x-0x%x:\t%s\t%-25s\t%d:%s" % (count+1, ins_disas.address, ins_disas.address + ins_disas.size, ins_disas.mnemonic,
            ins_disas.op_str, ins_disas.size, ' '.join(code_bytes_str[:ins_disas.size]))

  kvaddr = kvaddr + ins_disas.size
  count += 1
