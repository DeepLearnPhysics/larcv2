import ROOT,sys
import matplotlib.pyplot as plt

img=ROOT.larcv.Image2D('OK.png')
img_array=ROOT.larcv.as_ndarray(img)
plt.imshow(img_array)

roi = ROOT.larcv.ROI()
bb_v  = [ ROOT.larcv.ImageMeta(img.meta().cols()/2, img.meta().rows()/2,
                               img.meta().rows()/2, img.meta().cols()/2,
                               img.meta().min_x()+25*i,
                               img.meta().max_y()-25*i,i) for i in xrange(3) ]

for ix,bb in enumerate(bb_v):
    roi.AppendBB(bb)

    bbox = ROOT.larcv.as_bbox(roi,ix)
    rec = plt.Rectangle(**bbox)
    ax = plt.gca()
    ax.add_patch(rec)


plt.show()
