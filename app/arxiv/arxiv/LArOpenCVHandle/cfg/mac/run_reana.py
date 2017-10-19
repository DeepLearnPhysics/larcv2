from larcv import larcv
import ROOT, sys
from ROOT import std

if len(sys.argv) < 2:
   print "SYS.ARGV"
   print "1->cfg"
   print "2->ana"
   print "3->out"
   print "4->event"
   print "5->vtxid"
   sys.exit(1)

proc = larcv.ProcessDriver('ProcessDriver')
proc.configure(sys.argv[1])
if len(sys.argv) > 1:
   flist=ROOT.std.vector('std::string')()
   for x in xrange(len(sys.argv)-6):
      print "Pushing back...",sys.argv[x+6]
      flist.push_back(sys.argv[x+6])
   
   proc.override_input_file(flist)

proc.override_ana_file(sys.argv[2] + ".root")
proc.override_output_file(sys.argv[3] + ".root")
proc.initialize()

larbys_reana_id = proc.process_id("LArbysImageReAna")
larbys_reana = proc.process_ptr(larbys_reana_id)
print "GOT: ",larbys_reana,"@ id=",larbys_reana_id

print "Set IO",proc.io()
larbys_reana.SetIOManager(proc.io())

pgraph_prod = 'test'
print "Set pgraph producer",pgraph_prod
larbys_reana.SetPGraphProducer(pgraph_prod)

pixel2d_prod = 'test_ctor'
print "Set pixel2d producer",pixel2d_prod
larbys_reana.SetPixel2DProducer(pixel2d_prod)

vertexid = int(sys.argv[5])
if vertexid >= 0:
   print "Set vertex id",vertexid
   larbys_reana.SetVertexID(vertexid)

event = int(sys.argv[4])
print "Set event",event
if event >= 0:
   proc.batch_process(event,1)
else:
   assert vertexid == -1;
   proc.batch_process()

proc.finalize()
