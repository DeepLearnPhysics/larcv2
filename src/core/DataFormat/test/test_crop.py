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

bb = ROOT.larcv.ImageMeta(10,10,
                          10,10,
                          img.meta().min_x(),
                          img.meta().max_y()-1)
crop=img.crop(bb)

img_array=ROOT.larcv.as_ndarray(img)
print len(img_array),len(img_array[0])
print
for row in xrange(img.meta().rows()):
    for col in xrange(img.meta().cols()):
        print img.pixel(row,col),
    print
print

crop_array=ROOT.larcv.as_ndarray(crop)
print len(crop_array),len(crop_array[0])
print
for row in xrange(crop.meta().rows()):
    for col in xrange(crop.meta().cols()):
        print crop.pixel(row,col),
    print
print

