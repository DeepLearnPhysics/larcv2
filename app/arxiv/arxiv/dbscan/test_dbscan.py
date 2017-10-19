import os,time,sys
import ROOT as rt
from larcv import larcv
from ROOT import dbscan,std
import numpy as np

colors = [ rt.kRed, rt.kBlue, rt.kMagenta, rt.kGreen, rt.kCyan ]

npoints = 1000
ndims = 2

gauscenters = [(10,10),(2,2)]
gauscovs    = [ 
    [[1,0],[0,1]],
    [[1,0.7],[0.7,1]]
]

# gen pts and put them into a vector. Also, graph them
g = rt.TGraph( npoints )

algo = dbscan.DBSCANAlgo()
data = dbscan.dbPoints()
for ipt in xrange(0,npoints):
    ig = np.random.randint(0,len(gauscenters))
    mean = gauscenters[ig]
    cov  = gauscovs[ig]
    points = np.random.multivariate_normal(mean, cov, 1)
    g.SetPoint(ipt,points[0][0],points[0][1])
    v = std.vector("double")(ndims,0.0)
    v[0] = points[0][0]
    v[1] = points[0][1]
    data.push_back(v)

c = rt.TCanvas("c","c",1400,600)
c.Divide(2,1)
c.cd(1)
g.SetMarkerStyle(20)
g.Draw("AP")
c.Update()

xmin = g.GetXaxis().GetXmin()
xmax = g.GetXaxis().GetXmax()
ymin = g.GetYaxis().GetXmin()
ymax = g.GetYaxis().GetXmax()


c.cd(2)

s = time.time()
output = algo.scan( data, 3, 1.0 )
print "cluster time: ",time.time()-s," sec"

print "Number of clusters: ",output.clusters.size()
haxis = rt.TH2D("hout","",100,xmin,xmax,100,ymin,ymax)
haxis.Draw()
gclusters = []
for icluster in xrange(0,output.clusters.size()):
    npts = output.clusters.at(icluster).size()
    gc = rt.TGraph( npts )
    for ipt in range(0,npts):
        idx = output.clusters.at(icluster).at(ipt)
        gc.SetPoint(ipt, data.at(idx).at(0), data.at(idx).at(1) )
    gclusters.append(gc)
    gc.Draw("P")

    if npts==0:
        gc.SetMarkerStyle(24)
        gc.SetMarkerColor( rt.kBlack )
    else:
        gc.SetMarkerStyle( 20 + icluster%4 )
        gc.SetMarkerColor( colors[icluster%len(colors)] )

c.Update()

# testing point
testpoint = std.vector("double")(2,0.0)
testpoint[0] = gauscenters[0][0]
testpoint[1] = gauscenters[0][1]
match = output.findMatchingCluster( testpoint, data, 10.0 )
print "matching cluster index=",match

print "How'd we do?"
raw_input()


#algo.initialize()

# PRINT
#algo.printdata()
# DUMP
#algo.dump( "anntest.dmp" )


