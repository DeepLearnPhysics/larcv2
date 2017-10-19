import matplotlib
import matplotlib.pyplot as plt
from ROOT import geo2d
from ROOT import larocv
import numpy as np
import cv2

import ROOT, sys, os
from ROOT import std
from larcv import larcv

proc = larcv.ProcessDriver('ProcessDriver')

proc.configure(sys.argv[1])

flist=ROOT.std.vector('std::string')()
for x in xrange(len(sys.argv)-4):
   flist.push_back(sys.argv[x+4])
proc.override_input_file(flist)


ana_id     = proc.process_id("LArbysImageAna")
extract_id = proc.process_id("LArbysImageExtract")
filter_id  = proc.process_id("LArbysImageFilter")
larbysimg_ana     = proc.process_ptr(ana_id)
larbysimg_extract = proc.process_ptr(extract_id)
larbysimg_filter  = proc.process_ptr(filter_id)

larbysimg_ana.SetInputLArbysFile(sys.argv[2])
larbysimg_filter.SetLArbysImageAna(larbysimg_ana)
larbysimg_filter.SetLArbysImageExtract(larbysimg_extract)

proc.override_ana_file(sys.argv[3] + ".root")

proc.initialize()
proc.batch_process()
proc.finalize()
