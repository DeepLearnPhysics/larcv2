import os,sys
import ROOT
import numpy as np
from larcv import larcv
print larcv.Image2D

# TESTS MATRIX MULTIPLICATION FEATURE

a = np.random.rand(6,5)
b = np.random.rand(5,8)

aI = larcv.Image2D( a.shape[0], a.shape[1] )
bI = larcv.Image2D( b.shape[0], b.shape[1] )

arows = a.shape[0]
acols = a.shape[1]
brows = b.shape[0]
bcols = b.shape[1]

for r in range(0,arows):
    for c in range(0,acols):
        aI.set_pixel( r, c, a[r,c] )

for r in range(0,brows):
    for c in range(0,bcols):
        bI.set_pixel( r, c, b[r,c] )
        

C = np.dot(a,b)

CI = aI*bI

crows = CI.meta().rows()
ccols = CI.meta().cols()

print "A diff"
Adiff = np.zeros( a.shape )
for r in range(0,arows):
    for c in range(0,acols):
        Adiff[r,c] = aI.pixel(r,c)-a[r,c]
print Adiff

print "B diff"
Bdiff = np.zeros( b.shape )
for r in range(0,brows):
    for c in range(0,bcols):
        Bdiff[r,c] = bI.pixel(r,c)-b[r,c]
print Bdiff

print "CDiff"
Cdiff = np.zeros( C.shape )
for r in range(0,crows):
    for c in range(0,ccols):
        Cdiff[r,c] = CI.pixel(r,c)-C[r,c]
print Cdiff
