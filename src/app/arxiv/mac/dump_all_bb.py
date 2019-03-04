from larcv import larcv
larcv.IOManager
from ROOT import TChain
import sys

IMG_PRODUCER=sys.argv[1]
ROI_PRODUCER=sys.argv[2]
SEG_PRODUCER=sys.argv[3]

img_tree_name='image2d_%s_tree' % IMG_PRODUCER
img_br_name='image2d_%s_branch' % IMG_PRODUCER
img_ch = TChain(img_tree_name)
img_ch.AddFile(sys.argv[4])

if ROI_PRODUCER:
    roi_tree_name='partroi_%s_tree' % ROI_PRODUCER
    roi_br_name='partroi_%s_branch' % ROI_PRODUCER
    roi_ch = TChain(roi_tree_name)
    roi_ch.AddFile(sys.argv[4])

if SEG_PRODUCER:
    seg_tree_name='image2d_%s_tree' % SEG_PRODUCER
    seg_br_name='image2d_%s_branch' % SEG_PRODUCER
    seg_ch = TChain(seg_tree_name)
    seg_ch.AddFile(sys.argv[4])

for entry in xrange(img_ch.GetEntries()):

    roi_br=img_br=seg_br=None
    img_ch.GetEntry(entry)

    exec('img_br=img_ch.%s' % img_br_name)
    if ROI_PRODUCER:
        roi_ch.GetEntry(entry)
        exec('roi_br=roi_ch.%s' % roi_br_name)

        print 'ROI...',ROI_PRODUCER
        print roi_br.event_key()
        bb_array = roi_br.ROIArray()
        for b in bb_array:
            print b.dump(),
        print

    if SEG_PRODUCER:
        seg_ch.GetEntry(entry)
        exec('seg_br=seg_ch.%s' % seg_br_name)

        print 'Seg...',SEG_PRODUCER
        bb_array = seg_br.Image2DArray()
        for b in bb_array:
            print b.meta().dump(),
        print
        
    print 'Image...',IMG_PRODUCER
    bb_array = img_br.Image2DArray()
    for b in bb_array:
        print b.meta().dump(),
    print
    break
