import ROOT,sys

img1=ROOT.larcv.Image2D(20,20)
img1.paint(0)
for x in xrange(20):
    for y in xrange(20):
        v=0
        if x==y: v=1
        img1.set_pixel(x,y,v)
        print v,
    print
print

img2=ROOT.larcv.Image2D(img1.meta())
img2.copy(0,0,img1.as_vector())

arr1=ROOT.larcv.as_ndarray(img1)
arr2=ROOT.larcv.as_ndarray(img2)

for x in xrange(len(arr1)):
    for y in xrange(len(arr1[x])):
        print int(arr1[x][y]),
    print
print

for x in xrange(len(arr2)):
    for y in xrange(len(arr2[x])):
        print int(arr2[x][y]),
    print
print
