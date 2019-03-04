from larcv import larcv
larcv.IOManager
from ROOT import TChain
import sys

PIXEL2D_PRODUCER=sys.argv[1]

pixel_tree_name='pixel2d_%s_tree' % PIXEL2D_PRODUCER
pixel_ch = TChain(pixel_tree_name)
pixel_ch.AddFile(sys.argv[2])


for entry in xrange(pixel_ch.GetEntries()):

    pixel_br=None
    pixel_ch.GetEntry(entry)

    
    exec('pixel_br=pixel_ch.pixel2d_%s_branch' % PIXEL2D_PRODUCER)
    print pixel_br

    print 'Pixel...',PIXEL2D_PRODUCER
    for p in xrange(0,3):
        print "  plane=",p,
        bb_array = pixel_br.Pixel2DClusterArray(p)
        for pc in bb_array:
            print "  cluster num pixels=",pc.size()
            for ipixel2d in xrange(0,pc.size()):
                pixel2d = pc.at(ipixel2d)
                print pixel2d.X(),pixel2d.Y()
    print

