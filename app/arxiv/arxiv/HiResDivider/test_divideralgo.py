import os,sys
import numpy as np
import ROOT as rt
from array import array

from larcv import larcv 
from ROOT import divalgo
from ROOT import std

rt.gStyle.SetOptStat(0)

divider = divalgo.DividerAlgo()

uwires = std.vector('int')()
vwires = std.vector('int')()
ywires = std.vector('int')()

# Draw TPC projection
c = rt.TCanvas("c","c",1400,400)
hblank = rt.TH2D("h2","",1000,-50,1050,600,-150,150)
hblank.Draw()

tpc = rt.TBox(0,-115,1036,117)
tpc.SetLineWidth(2)
tpc.SetLineColor(rt.kBlack)
tpc.SetFillColor(0)
tpc.SetFillStyle(0)
tpc.Draw()

c.Update()

while True:

    z1 = np.random.random()*1100-50
    z2 = np.random.random()*1100-50
    if z2<z1:
        z0 = z1
        z1 = z2
        z2 = z0
    y1 = np.random.random()*300 - 150
    y2 = np.random.random()*300 - 150
    if y2<y1:
        y0 = y1
        y1 = y2
        y2 = y0
    bbox = [ z1, y1, z2, y2 ]

    # INJECT OWN BOX HERE
    #bbox = [ 103.7, -38, 207.4, 65.7 ]

    tbbox = rt.TBox( bbox[0], bbox[1], bbox[2], bbox[3] )
    tbbox.SetLineColor(rt.kBlack)
    tbbox.SetLineWidth(1)
    tbbox.SetFillStyle(0)
    tbbox.Draw()

    divider.getregionwires( bbox[0], bbox[1], bbox[2], bbox[3], ywires, uwires, vwires)

    if uwires.size()<2 or vwires.size()<2 or ywires.size()<2:
        print "invalid box: ",bbox
        print "[ENTER] to continue."
        raw_input()
        continue

    # draw U wires
    print "U bounds: [",uwires.at(0),",",uwires.back(),"]"
    print "V bounds: [",vwires.at(0),",",vwires.back(),"]"
    print "Y bounds: [",ywires.at(0),",",ywires.back(),"]"

    uwire0 = uwires.at(0)
    uwire0_start = std.vector('float')()
    uwire0_end   = std.vector('float')()
    divalgo.getUstart(uwire0,uwire0_start)
    divalgo.getUend(uwire0,uwire0_end)
    
    uwire1 = uwires.back()
    uwire1_start = std.vector('float')()
    uwire1_end   = std.vector('float')()
    divalgo.getUstart(uwire1,uwire1_start)
    divalgo.getUend(uwire1,uwire1_end)

    ustart = rt.TLine( uwire0_start.at(2), uwire0_start.at(1), uwire0_end.at(2), uwire0_end.at(1) )
    uend   = rt.TLine( uwire1_start.at(2), uwire1_start.at(1), uwire1_end.at(2), uwire1_end.at(1) )
    ustart.SetLineColor(rt.kBlue)
    uend.SetLineColor(rt.kBlue)

    ustart.Draw()
    uend.Draw()

    # V wires
    
    vwire0 = vwires.at(0)
    vwire0_start = std.vector('float')()
    vwire0_end   = std.vector('float')()
    divalgo.getVstart(vwire0,vwire0_start)
    divalgo.getVend(vwire0,vwire0_end)
    
    vwire1 = vwires.back()
    vwire1_start = std.vector('float')()
    vwire1_end   = std.vector('float')()
    divalgo.getVstart(vwire1,vwire1_start)
    divalgo.getVend(vwire1,vwire1_end)
    
    vstart = rt.TLine( vwire0_start.at(2), vwire0_start.at(1), vwire0_end.at(2), vwire0_end.at(1) )
    vend   = rt.TLine( vwire1_start.at(2), vwire1_start.at(1), vwire1_end.at(2), vwire1_end.at(1) )
    vstart.SetLineColor(rt.kRed)
    vend.SetLineColor(rt.kRed)

    vstart.Draw()
    vend.Draw()

    # Y wires
    
    ywire0 = ywires.at(0)
    ywire0_start = std.vector('float')()
    ywire0_end   = std.vector('float')()
    divalgo.getYstart(ywire0,ywire0_start)
    divalgo.getYend(ywire0,ywire0_end)
    
    ywire1 = ywires.back()
    ywire1_start = std.vector('float')()
    ywire1_end   = std.vector('float')()
    divalgo.getYstart(ywire1,ywire1_start)
    divalgo.getYend(ywire1,ywire1_end)
    
    ystart = rt.TLine( ywire0_start.at(2), ywire0_start.at(1), ywire0_end.at(2), ywire0_end.at(1) )
    yend   = rt.TLine( ywire1_start.at(2), ywire1_start.at(1), ywire1_end.at(2), ywire1_end.at(1) )
    ystart.SetLineColor(rt.kGreen-4)
    yend.SetLineColor(rt.kGreen-4)

    ystart.Draw()
    yend.Draw()


    c.Update()

    print "[ENTER] to continue."
    raw_input()
