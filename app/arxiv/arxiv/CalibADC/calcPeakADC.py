import pylab
import numpy as np
import ROOT as rt
from larcv import larcv
import os,sys

ioman = larcv.IOManager(larcv.IOManager.kREAD,"IO")

with open("ext.list",'r') as f:
    lines = f.readlines()
    for l in lines:
        ioman.add_in_file( l.strip() )

#ioman.add_in_file("supera_data_cosmics.root")
#ioman.add_in_file("supera_mc_cosmics.root")
ioman.initialize()

out = rt.TFile("test.root","recreate")

h = rt.TH1D("htest","",1000,0,200)
ttree = rt.TTree("adc","")
from array import array
peakmax = array('f',[0.0])
wireid = array('i',[0])
planeid = array('i',[0])
ttree.Branch("wireid",wireid,"wireid/I")
ttree.Branch("planeid",planeid,"planeid/I")
ttree.Branch("peak",peakmax,"peak/F")

nentries = ioman.get_n_entries()
print "NUM ENTRIES: ",nentries
num_entries = 10000

for entry in range(0,num_entries):
    ioman.read_entry(entry)

    event_images = ioman.get_data(larcv.kProductImage2D,"tpc")
    img0 = event_images.Image2DArray()[0]
    print ioman.event_id().run(),ioman.event_id().subrun(),ioman.event_id().event(),"(",entry,")"
    wfm0 = larcv.as_ndarray(img0)

    x = np.linspace(0,wfm0.shape[1],wfm0.shape[1])
    for img in event_images.Image2DArray():
        wfms = larcv.as_ndarray(img)
        if img.meta().plane()!=1:
            continue

        for w in range(0,wfms.shape[0]):
            y = wfms[w,:]
            yt = y > 40
            inpeak = False
            pmax = -1
            peakcenter = -1
            peaks = []
            for t in range(0,len(y)):
                if not inpeak:
                    if yt[t]:
                        # check if min dist away from last peak
                        if len(peaks)==0 or peaks[-1][0]+20<t:
                            inpeak = True
                            pmax = y[t]
                            peakcenter = t
                else:
                    if yt[t]:
                        # still in peak, update
                        if y[t]>pmax:
                            pmax = y[t]
                            peakcenter = t
                    else:
                      inpeak = False
                      peaks.append( (peakcenter,pmax) )
                      peakcenter = -1 
                      pmax = -1
            #print "Wire ",w,": peaks= ",peaks
            wireid[0] = w
            planeid[0] = img.meta().plane()
            for peak in peaks:
                peakmax[0] = peak[1]
                ttree.Fill()
            if len(peaks)>0:
                pylab.plot(x,y)
                pylab.show()

out.Write()

print 'done'
