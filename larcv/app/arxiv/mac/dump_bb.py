from larcv import larcv
larcv.IOManager
from ROOT import TChain
import sys

ROI_PRODUCER=sys.argv[1]

roi_tree_name='partroi_%s_tree' % ROI_PRODUCER
roi_br_name='partroi_%s_branch' % ROI_PRODUCER
roi_ch = TChain(roi_tree_name)
roi_ch.AddFile(sys.argv[-1])

for entry in xrange(roi_ch.GetEntries()):
    
    roi_ch.GetEntry(entry)
    roi_br=None
    exec('roi_br=roi_ch.%s' % roi_br_name)
    print
    print roi_br.event_key()
    bb_array = roi_br.ROIArray()
    for b in bb_array:
        print b.dump()

    if len(sys.argv) == 4:
        IMG_PRODUCER = sys.argv[2]
        img_tree_name='image2d_%s_tree' % IMG_PRODUCER
        img_br_name='image2d_%s_branch' % IMG_PRODUCER
        img_ch=TChain(img_tree_name)
        img_ch.AddFile(sys.argv[-1])
        img_ch.GetEntry(entry)
        img_br=None
        exec('img_br=img_ch.%s' % img_br_name)
        img_array = img_br.Image2DArray()
        print 'Image meta...'
        for i in img_array:
            print i.meta().dump()

