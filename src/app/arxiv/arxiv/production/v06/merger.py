#!/usr/bin/env python
import ROOT, sys,os
from ROOT import std
from larcv import larcv

proc = larcv.MergeTwoStream()

proc.configure(sys.argv[1])

proc.override_output_file("out.root")

proc.override_ana_file("out_ana.root")

vlist1 = ROOT.std.vector('string')()
for f in [sys.argv[2]]:
    vlist1.push_back(f)

vlist2 = ROOT.std.vector('string')()
for f in [sys.argv[3]]:
    vlist2.push_back(f)

proc.override_input_file(vlist1,vlist2)

proc.initialize()

proc.batch_process()

proc.finalize()
