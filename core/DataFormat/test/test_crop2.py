import ROOT,sys
import matplotlib.pyplot as plt

img=ROOT.larcv.Image2D('Sakura.png')
img_array=ROOT.larcv.as_ndarray(img)
plt.imshow(img_array).write_png('original.png')

bb = ROOT.larcv.ImageMeta(img.meta().cols()/2, img.meta().rows()/2,
                          img.meta().rows()/2, img.meta().cols()/2,
                          img.meta().min_x(),
                          img.meta().max_y())
crop=img.crop(bb)
crop_array=ROOT.larcv.as_ndarray(crop)
plt.imshow(crop_array).write_png('cropped.png')


