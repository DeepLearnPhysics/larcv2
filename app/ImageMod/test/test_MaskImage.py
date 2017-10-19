from larcv import larcv
from colored_msg import colored_msg as cmsg
import os, sys
larcv.logger.force_level(0)

#
# Constants
#
MSG_LEVEL=larcv.msg.kERROR
if 'debug' in sys.argv:
    MSG_LEVEL = larcv.msg.kDEBUG
if 'info' in sys.argv:
    MSG_LEVEL = larcv.msg.kINFO

OUT_FNAME="maskimage.root"
NUM_EVENT=1

ERROR_FILE_EXIST      = 1
ERROR_WRITE_INIT      = 2

if os.path.isfile(OUT_FNAME):
    cmsg.error("Test output file (%s) already exists..." % OUT_FNAME)
    sys.exit(ERROR_FILE_EXIST)

from larcv import larcv
o=larcv.IOManager(larcv.IOManager.kWRITE)
o.reset()
o.set_verbosity(MSG_LEVEL)
o.set_out_file(OUT_FNAME)

p = larcv.MaskImage()
cfg = larcv.CreatePSetFromFile(sys.argv[1],"MaskImage")
p.configure(cfg)
p.initialize()

if not o.initialize():
    sys.exit(ERROR_WRITE_INIT)
for idx in xrange(NUM_EVENT):

    img1 = larcv.Image2D(9,9)
    img2 = larcv.Image2D(9,9)

    img1.paint(1)

    for x in xrange(img2.as_vector().size()):
        if x%2 == 0: img2.set_pixel(x,1)
        else: img2.set_pixel(x,0)
        
    event_image1 = o.get_data(larcv.kProductImage2D,"original")
    event_image1.Append(img1)
    event_image2 = o.get_data(larcv.kProductImage2D,"reference")
    event_image2.Append(img2)
    event_image3 = o.get_data(larcv.kProductImage2D,"target")
    event_image3.Append(img1)
    o.set_id(0,0,idx)
    p.process(o)
    o.save_entry()

    idx+=1
p.finalize()
o.finalize()
