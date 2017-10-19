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
OUT_FNAME="iotest.root"
NUM_EVENT=1000

ERROR_FILE_EXIST      = 1
ERROR_READ_INIT       = 2
ERROR_WRITE_INIT      = 3
ERROR_ENTRY_MISSING   = 4
ERROR_PRODUCT_MISSING = 5

if os.path.isfile(OUT_FNAME):
    cmsg.error("Test output file (%s) already exists..." % OUT_FNAME)
    sys.exit(ERROR_FILE_EXIST)
#
# Test Write
#

o=larcv.IOManager(larcv.IOManager.kWRITE)
o.reset()
o.set_verbosity(MSG_LEVEL)
o.set_out_file(OUT_FNAME)
if not o.initialize():
    sys.exit(ERROR_WRITE_INIT)

product_names = larcv.DataProductFactory.get().product_names()
    
for idx in xrange(NUM_EVENT):

    for name_idx in xrange(product_names.size()):

        o.get_data(product_names[name_idx],"product_type%02d" % name_idx)

    o.set_id(0,0,idx)
    o.save_entry()

o.finalize()

#
# Test Read
#

i=larcv.IOManager(larcv.IOManager.kREAD)
i.reset()
i.set_verbosity(MSG_LEVEL)
i.add_in_file(OUT_FNAME)
if not i.initialize():
    sys.exit(ERROR_READ_INIT)

product_ctr={}
entry_ctr=0
for idx in xrange(NUM_EVENT):

    if not i.read_entry(idx):
        break
    
    entry_ctr += 1

    for name_idx in xrange(product_names.size()):

        name = product_names[name_idx]
        
        if name not in product_ctr:
            product_ctr[name] = 0

        if i.get_data(name,"product_type%02d" % name_idx):
            product_ctr[name] += 1

i.finalize()

if not entry_ctr == NUM_EVENT:
    cmsg.error("Read-back only found %d/%d events!" % (entry_ctr,NUM_EVENT))
    sys.exit(ERROR_ENTRY_MISSING)
    
for t,ctr in product_ctr.iteritems():
    if not ctr == NUM_EVENT:
        cmsg.error("Product type %d (name %s) only has %d/%d count!" % (t,larcv.ProductName(t),ctr,NUM_EVENT))
        sys.exit(ERROR_PRODUCT_MISSING)
        break
print product_ctr
if os.path.isfile(OUT_FNAME):
    os.remove(OUT_FNAME)

sys.exit(0)
