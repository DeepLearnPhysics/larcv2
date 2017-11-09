import numpy
from larcv import larcv

l2 = numpy.array([[1.0,2.0,3.0], [4.0,5.0,6.0], [7.0,8.0,9.0], [3.0, 5.0, 0.0]],dtype="float32")

print 'Input Matrix (numpy array)'
print l2

k=larcv.as_image2d(l2)
print k.meta().rows(),k.meta().cols()
print
print '1D array rep. of Image2D'
for x in k.as_vector(): print x,
print
print
print 'row/col representation of Image2D'
for r in xrange(k.meta().rows()):
    for c in xrange(k.meta().cols()):
        print k.pixel(r,c),
    print
        
ar = larcv.as_ndarray(k)
print ar
