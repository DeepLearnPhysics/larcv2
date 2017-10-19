from larcv import larcv
larcv.IOManager
import matplotlib.pyplot as plt
from ROOT import TChain
import sys

IMAGE_PRODUCER=sys.argv[1]

img_tree_name='image2d_%s_tree' % IMAGE_PRODUCER
img_br_name='image2d_%s_branch' % IMAGE_PRODUCER
img_ch = TChain(img_tree_name)
img_ch.AddFile(sys.argv[2])

start=0
cutoff=0
if len(sys.argv) > 3:
    cutoff = int(sys.argv[3])
if len(sys.argv) > 4:
    start = int(sys.argv[3])
    cutoff = int(sys.argv[4])

for entry in xrange(img_ch.GetEntries()):
    if entry<start: continue
    img_ch.GetEntry(entry)
    img_br=None
    exec('img_br=img_ch.%s' % img_br_name)
    event_key = img_br.event_key()
    index=0
    for img in img_br.Image2DArray():
        mat=larcv.as_ndarray(img)
        mat_display=plt.imshow(mat)
        mat_display.write_png('%s_plane%d_%d.png' % (event_key,img.meta().plane(),index))
        index+=1
    if cutoff and cutoff <= entry:
        break 


