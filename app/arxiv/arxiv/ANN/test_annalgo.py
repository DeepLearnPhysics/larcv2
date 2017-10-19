import os
from larcv import larcv
from ROOT import ann,std
import numpy as np

npoints = 1000
ndims = 2

gauscenters = [(10,10),(2,2)]
gaussig     = [(2,3),  (1,3)]

mean = [5,5]
cov  = [ [1,0],[0,1] ]
points1 = np.random.multivariate_normal(mean, cov, npoints)


algo = ann.ANNAlgo( npoints, ndims )

for ipt,pt in enumerate(points1):
    print pt,type(pt),pt[0],pt[1]
    v = std.vector("double")(ndims,0.0)
    v[0] = pt[0]
    v[1] = pt[1]
    algo.setPoint( ipt, v )

algo.initialize()

# PRINT
algo.printdata()
# DUMP
algo.dump( "anntest.dmp" )

print "Made it!"
