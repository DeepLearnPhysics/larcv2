#!/usr/bin/env python
import ROOT, sys, os
for l in [x for x in os.listdir(os.environ['LARCV_LIBDIR']) if x.endswith('.so')]:
   ROOT.gSystem.Load('%s/%s' % (os.environ['LARCV_LIBDIR'],l))

from ROOT import std
from larcv import larcv

if len(sys.argv) < 2:

   print 'Usage: python',sys.argv[0],'CONFIG_FILE [LARCV_FILE1 LARCV_FILE2 ...]'
   sys.exit(1)

proc = larcv.ProcessDriver('ProcessDriver')

print "config: ",sys.argv[1]
proc.configure(str(sys.argv[1]))

if len(sys.argv) > 1:
   
   flist=ROOT.std.vector('std::string')()
   for x in xrange(len(sys.argv)-2):
      flist.push_back(sys.argv[x+2])

   proc.override_input_file(flist)

proc.initialize()

#proc.batch_process(0,3)
proc.batch_process()

proc.finalize()
