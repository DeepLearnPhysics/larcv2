import ROOT,sys

k=ROOT.larcv.Image2D(20,20)
k.paint(0)
for x in xrange(20):
    for y in xrange(20):
        v=0
        if x==y: v=1
        k.set_pixel(x,y,v)
        print v,
    print
print

j=ROOT.larcv.as_ndarray(k)

for x in xrange(len(j)):
    for y in xrange(len(j[x])):
        print int(j[x][y]),
    print
print
    
for x in xrange(k.meta().cols()):
    for y in xrange(k.meta().rows()):
        print k.pixel(x,y),
    print
print

import matplotlib.pyplot as plt

img=plt.imshow(j)
img.write_png('aho.png')
sys.stdin.readline()
