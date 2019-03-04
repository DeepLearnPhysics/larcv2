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
   for x in xrange(len(sys.argv)-3):
      flist.push_back(sys.argv[x+3])
   proc.override_input_file(flist)

filter_id  = proc.process_id("NuFilter")
mcinfo_id  = proc.process_id("LArbysImageMC")
reco_id    = proc.process_id("LArbysImage")
out_id     = proc.process_id("LArbysImageOut")
result_id  = proc.process_id("LArbysImageResult")

filter_proc   = proc.process_ptr(filter_id)

mcinfo_proc   = proc.process_ptr(mcinfo_id)
mcinfo_proc.SetFilter(filter_proc)

larbysimg     = proc.process_ptr(reco_id)

larbysimg_out = proc.process_ptr(out_id)
larbysimg_out.SetManager(larbysimg.Manager())

larbysimg_result = proc.process_ptr(result_id)
larbysimg_result.SetManager(larbysimg.Manager())

proc.override_ana_file(sys.argv[2] + ".root")
proc.initialize()
proc.batch_process(0,50)
proc.finalize()

