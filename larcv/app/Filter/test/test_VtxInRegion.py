from larcv import larcv
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

OUT_FNAME="merger.root"
NUM_EVENT=1

ERROR_FILE_EXIST      = 1
ERROR_WRITE_INIT      = 2

# if os.path.isfile(OUT_FNAME):
#     print "Test output file (%s) already exists..." % OUT_FNAME
#     sys.exit(ERROR_FILE_EXIST)

o=larcv.IOManager(larcv.IOManager.kWRITE)
o.reset()
o.set_verbosity(MSG_LEVEL)
o.set_out_file(OUT_FNAME)

if not o.initialize():
    sys.exit(ERROR_WRITE_INIT)

p = larcv.VtxInRegion()
p.set_verbosity(2)
cfg=larcv.CreatePSetFromFile(sys.argv[1],"VtxInRegion");
p.configure(cfg)

x=[]
y=[]
for idx1 in xrange(0,1000,10):
    for idx2 in xrange(-100,100,2):
        event_roi1 = o.get_data(larcv.kProductROI,"vtxinregion")
        bb1 = larcv.ImageMeta(2,2,2,2,2,2,0) #doesn't matter
        roi1= larcv.ROI()
        roi1.Type(2)
        roi1.Position(0,idx2,idx1,0);
        roi1.AppendBB(bb1)
        event_roi1.clear()
        event_roi1.Append(roi1)

        if(p.process(o)==True):
            x.append(idx1)
            y.append(idx2)
        
import numpy as np
x=np.array(x)
y=np.array(y)
import matplotlib.pyplot as plt
plt.plot(x,y,'o')
plt.show()
p.finalize()
o.finalize()
