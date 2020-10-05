#!/usr/bin/python

import subprocess
import os
import sys
from time import sleep
import signal
import errno

class BashCmd(object):
  def __init__(self, workingDir):
    self.cmd = ""
    self.out = ""
    self.err = ""
    assert workingDir != "", "Error: BashCmd requires a working dir"
    self.workingDir = workingDir
    os.chdir(self.workingDir)
  
  def run(self, cmd, pipe=True):
    if os.getcwd() != self.workingDir:
      os.chdir(self.workingDir)

    self.cmd = cmd
    
    if pipe:
      proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    else:
      proc = subprocess.Popen(cmd, shell=True)

    (self.out,self.err) = proc.communicate()
    
    return self.err == "" or self.err == None

  def run_forked(self, cmd, pipe=False):
    # do the UNIX double-fork magic, see Stevens' "Advanced 
    # Programming in the UNIX Environment" for details (ISBN 0201563177)
    try: 
        pid = os.fork() 
        if pid > 0:
            # parent process, return and keep running
            #sleep(5)
            return
    except OSError, e:
        print >>sys.stderr, "fork #1 failed: %d (%s)" % (e.errno, e.strerror) 
        sys.exit(1)

    #os.setsid()
    #
    # do second fork
    #try: 
    #    pid = os.fork() 
    #    print "xxx", pid
    #    if pid > 0:
    #        # exit from second parent
    #        sys.exit(0) 
    #except OSError, e: 
    #    print >>sys.stderr, "fork #2 failed: %d (%s)" % (e.errno, e.strerror) 
    #    sys.exit(1)

    # do stuff
    self.run(cmd,pipe)

    # all done
    os._exit(os.EX_OK)    

  def getcmd(self):
    return self.cmd

  def stdout(self):
    return self.out
  
  def stderr(self):
    return self.err
