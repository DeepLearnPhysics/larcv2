#!/usr/bin/env python
import ROOT, sys,os
from ROOT import std
from larcv import larcv

if len(sys.argv) not in [3,5]:

   print 'Usage: python',sys.argv[0],'CONFIG_FILE OUTPUT_FILE_NAME [NU_FILE_LIST COSMIC_FILE_LIST]'
   sys.exit(1)

proc = larcv.MergeTwoStream()

proc.configure(sys.argv[1])

proc.override_output_file(sys.argv[2])

if len(sys.argv) == 5:
   flist1,flist2 = (None,None)
   if not os.path.isfile(sys.argv[3]): 
      print 'NU_FILELIST does not exist: ', sys.argv[2]
      sys.exit(1)

   if not os.path.isfile(sys.argv[4]): 
      print 'NU_FILELIST does not exist: ', sys.argv[4]
      sys.exit(1)

   vlist1 = ROOT.std.vector('string')()
   vlist2 = ROOT.std.vector('string')()

   if sys.argv[3].endswith('.root'):
      vlist1.push_back(sys.argv[3])
   else:
      flist1 = [x for x in open(sys.argv[3],'r').read().split() if x.endswith('.root')]
      vlist1.reserve(len(flist1))
      for f in flist1:
         vlist1.push_back(f)

   if sys.argv[4].endswith('.root'):
      vlist2.push_back(sys.argv[4])
   else:
      flist2 = [x for x in open(sys.argv[4],'r').read().split() if x.endswith('.root')]
      vlist2.reserve(len(flist2))
      for f in flist2:
         vlist2.push_back(f)
   
   proc.override_input_file(vlist1,vlist2)

proc.initialize()

proc.batch_process()

proc.finalize()
