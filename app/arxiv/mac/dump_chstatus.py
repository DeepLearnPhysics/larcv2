from larcv import larcv
larcv.IOManager
from ROOT import TChain
import sys

CHSTATUS_PRODUCER='bnb_mc'

chstatus_tree_name='chstatus_%s_tree' % CHSTATUS_PRODUCER
chstatus_br_name='chstatus_%s_branch' % CHSTATUS_PRODUCER
chstatus_ch = TChain(chstatus_tree_name)
chstatus_ch.AddFile(sys.argv[1])

for entry in xrange(chstatus_ch.GetEntries()):
    chstatus_ch.GetEntry(entry)
    chstatus_br=None
    exec('chstatus_br=chstatus_ch.%s' % chstatus_br_name)
    print
    print chstatus_br.event_key()
    print chstatus_br.dump()



