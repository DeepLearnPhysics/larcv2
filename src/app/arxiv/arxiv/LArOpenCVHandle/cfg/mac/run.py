from larcv import larcv
import ROOT, sys
from ROOT import std

if len(sys.argv) < 2:
   print 'Usage: python',sys.argv[0],'CONFIG_FILE [LARCV_FILE1 LARCV_FILE2 ...]'
   sys.exit(1)

proc = larcv.ProcessDriver('ProcessDriver')
print "ARGS: ",str(sys.argv)
print "Loading config... ",sys.argv[1]
proc.configure(sys.argv[1])
print "Loaded"
print sys.argv
if len(sys.argv) > 1:
   flist=ROOT.std.vector('std::string')()
   for x in xrange(len(sys.argv)-4):
      print "Pushing back...",sys.argv[x+4]
      flist.push_back(sys.argv[x+4])
   
   proc.override_input_file(flist)

proc.override_ana_file(sys.argv[2] + ".root")
proc.override_output_file(sys.argv[3] + ".root")
proc.initialize()

proc.batch_process()


proc.finalize()
