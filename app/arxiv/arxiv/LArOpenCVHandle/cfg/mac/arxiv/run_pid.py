#!/usr/bin/env python
import ROOT, sys
from ROOT import std
from larcv import larcv

if len(sys.argv) < 2:
   print 'Usage: python',sys.argv[0],'CONFIG_FILE [LARCV_FILE1 LARCV_FILE2 ...]'
   sys.exit(1)

proc = larcv.ProcessDriver('ProcessDriver')

print "Loading config... ",sys.argv[1]
proc.configure(sys.argv[1])

if len(sys.argv) > 1:
   flist=ROOT.std.vector('std::string')()
   for x in xrange(len(sys.argv)-2):
      flist.push_back(sys.argv[x+2])
   proc.override_input_file(flist)

#filter_id = proc.process_id("NuFilter")
#mcinfo_id = proc.process_id("LArbysImageMC")
pid_id = proc.process_id("LArbysImagePID")

#filter_proc   = proc.process_ptr(filter_id)
#mcinfo_proc   = proc.process_ptr(mcinfo_id)
pid_proc   = proc.process_ptr(pid_id)
#mcinfo_proc.SetFilter(filter_proc)
#proc.override_ana_file(sys.argv[2] + ".root")
proc.initialize()
proc.batch_process()
proc.finalize()
