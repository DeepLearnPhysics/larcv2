import ROOT,sys
ROOT.larcv.load_pycvutil

img=ROOT.larcv.Image2D(20,20)
img.paint(0)
for row in xrange(20):
    for col in xrange(20):
        v=0
        if row==col: v=1
        img.set_pixel(row,col,v)
        print v,
    print
print

print img.meta().dump()

bb = ROOT.larcv.ImageMeta(10,10,
                          10,10,
                          img.meta().min_x(),
                          img.meta().max_y()-1)
print bb.dump()

crop=img.crop(bb)
print crop.meta().dump()
crop_array=ROOT.larcv.as_ndarray(crop)
print len(crop_array),len(crop_array[0])
print
for row in xrange(crop.meta().rows()):
    for col in xrange(crop.meta().cols()):
        print crop.pixel(row,col),
    print
print

img.overlay(crop)
img_array=ROOT.larcv.as_ndarray(img)
print len(img_array),len(img_array[0])
print
for row in xrange(img.meta().rows()):
    for col in xrange(img.meta().cols()):
        print img.pixel(row,col),
    print
print
