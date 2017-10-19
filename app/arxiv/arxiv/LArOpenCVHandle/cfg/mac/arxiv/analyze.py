from larcv import larcv
import ROOT, sys
from ROOT import std


if len(sys.argv) < 2:
   print 'Usage: python',sys.argv[0],'CONFIG_FILE [LARCV_FILE1 LARCV_FILE2 ...]'
   sys.exit(1)

proc = larcv.ProcessDriver('ProcessDriver')

print "Loading config... ",sys.argv[1]
proc.configure(sys.argv[1])
print "Loaded"
print sys.argv
if len(sys.argv) > 1:
   flist=ROOT.std.vector('std::string')()
   for x in xrange(len(sys.argv)-6):
      print "Pushing back...",sys.argv[x+6]
      flist.push_back(sys.argv[x+6])
   
   proc.override_input_file(flist)

proc.override_ana_file(sys.argv[2] + ".root")
proc.override_output_file(sys.argv[3] + ".root")

ana_id  = proc.process_id("LArbysImageAna")
ext_id  = proc.process_id("LArbysImageExtract")
out_id  = proc.process_id("LArbysImageOut")

ana_proc = proc.process_ptr(ana_id)
ext_proc = proc.process_ptr(ext_id)
out_proc = proc.process_ptr(out_id)

out_proc.SetLArbysImageAna(ana_proc)
out_proc.SetLArbysImageExtract(ext_proc)

ana_proc.SetInputLArbysMCFile(sys.argv[4]);
ana_proc.SetInputLArbysRecoFile(sys.argv[5])

proc.initialize()
proc.batch_process()
proc.finalize()
