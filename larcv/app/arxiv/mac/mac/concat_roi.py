from larcv import larcv
import numpy as np

#TWO different files from two ``scans"
files = ["../god.root","../god2.root"]
ioms  = [larcv.IOManager() for _ in xrange(len(files))]

out_iom = larcv.IOManager(1)
out_iom.set_out_file("god3.root")
out_iom.set_verbosity(0)
out_iom.initialize()

for file_,iom in zip(files,ioms):
    iom.add_in_file(file_)
    iom.initialize()

entries = np.array([iom.get_n_entries()-1 for iom in ioms])
max_entries = entries.max()
for entry in xrange(max_entries+1):

    evids  = []
    evrois = []
    print "entry",entry
    for ix,iom in enumerate(ioms):
        
        if entry > entries[ix]:
            evids.append((0,0,0))
            evrois.append(None)
            continue
        
        iom.read_entry(entry)
        evroi = iom.get_data(larcv.kProductROI,"god")

        evrois.append(evroi)
        evids.append((evroi.run(),evroi.subrun(),evroi.event()))

        
    hasroi = -1
    print evids
    for ix,id_ in enumerate(evids):

        if id_[1] > 0:
            if hasroi >= 0:
                raise Exception("TWO USER ROIS FOUND! Which one do I pick?")
            else:
                hasroi = ix

    evroi = out_iom.get_data(larcv.kProductROI,"god")

    if hasroi < 0:
        out_iom.set_id(1,0,entry)
        out_iom.save_entry()
        continue
    
    out_iom.set_id(1,1,entry)
    evroi.Set(evrois[hasroi].ROIArray())
    out_iom.save_entry()
    
out_iom.finalize()
