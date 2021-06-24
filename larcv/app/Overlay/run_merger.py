import ROOT, sys
from ROOT import std
from larcv import larcv

if len(sys.argv) < 2:

   print('Usage: python',sys.argv[0],'CONFIG_FILE [LARCV_FILE1 LARCV_FILE2 ...]')
   sys.exit(1)

proc = larcv.MergeStreams()
#cfg = larcv.CreatePSetFromFile(sys.argv[1], "MergeStreams")
#print(cfg)
cfg = open(sys.argv[1]).read()
print cfg
#proc.configure(cfg)
proc.configure(sys.argv[1])
proc.initialize()
proc.process()
#proc.batch_process(0,5)

proc.finalize()
