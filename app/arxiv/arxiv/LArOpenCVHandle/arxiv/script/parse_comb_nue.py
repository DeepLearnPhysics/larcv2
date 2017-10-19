#!/usr/bin/env python
import ROOT, sys
from ROOT import std
from larcv import larcv

if len(sys.argv) < 2:

   print 'Usage: python',sys.argv[0],'CONFIG_FILE [LARCV_FILE1 LARCV_FILE2 ...]'
   sys.exit(1)

proc = larcv.ProcessDriver('ProcessDriver')

print "Got argv... ",sys.argv

proc.configure(sys.argv[1])

if len(sys.argv) > 1:
   
   flist=ROOT.std.vector('std::string')()
   for x in xrange(len(sys.argv)-2):
      flist.push_back(sys.argv[x+2])

   proc.override_input_file(flist)

filter_id = proc.process_id("NuFilter")
mcinfo_id = proc.process_id("MCinfoRetriever")
reco_id   = proc.process_id("LArbysImage")
ana_id    = proc.process_id("LArbysImageAna")

filter_proc   = proc.process_ptr(filter_id)
mcinfo_proc   = proc.process_ptr(mcinfo_id)
mcinfo_proc.SetFilter(filter_proc)

larbysimg     = proc.process_ptr(reco_id)
larbysimg_ana = proc.process_ptr(ana_id)
larbysimg_ana.SetManager(larbysimg.Manager())

#evstart=int(sys.argv[2])
#step=int(3375)
step=int(1717)

#proc.override_ana_file("numu_ccqe_p00_p07_rad6_sig_bak_%05d_%05d.root"%(evstart,evstart+step))
proc.override_ana_file("nue_test_173.root")

proc.initialize()
#proc.batch_process(evstart,step)
proc.batch_process(173,1)
#proc.batch_process()

proc.finalize()
