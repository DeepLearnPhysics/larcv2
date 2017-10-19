#!/usr/bin/env python
import ROOT, sys
from ROOT import std
from larcv import larcv

if len(sys.argv) < 2:

   print 'Usage: python',sys.argv[0],'CONFIG_FILE [LARCV_FILE1 LARCV_FILE2 ...]'
   sys.exit(1)

proc = larcv.ProcessDriver('ProcessDriver')
cfg=ROOT.std.string(sys.argv[1])
print "Loading config... ",cfg
proc.configure(cfg)

print "Specifying input..."
if len(sys.argv) > 1:
   flist=ROOT.std.vector('std::string')()
   for x in xrange(len(sys.argv)-2):
      flist.push_back(sys.argv[x+2])
      print "> ",sys.argv[x+2]

   proc.override_input_file(flist)

filter_id = proc.process_id("NuFilter")
mcinfo_id = proc.process_id("LArbysImageMC")
reco_id   = proc.process_id("LArbysImage")

filter_proc   = proc.process_ptr(filter_id)
mcinfo_proc   = proc.process_ptr(mcinfo_id)
mcinfo_proc.SetFilter(filter_proc)

larbysimg     = proc.process_ptr(reco_id)
proc.override_ana_file("track_shower.root")
proc.initialize()
proc.batch_process(0,100)
proc.finalize()

