from larcv import larcv
import sys, time
import numpy as np
import matplotlib.pyplot as plt

batch_size=50
report_cycle=2
sleep_time=0.02
consume_cycle=0.1

proc=larcv.ThreadProcessor("ThreadProcessor")
proc.configure(sys.argv[1])

start_time=time.time()
proc.start_manager(batch_size)

while 1:
    status_v = proc.storage_status_array()
    if not int(status_v[0]) == 3:
        time.sleep(0.1)
        continue
    break

for pid in xrange(proc.storage_status_array().size()):
    batch_data = larcv.BatchDataStorageFactory('float').get().get_storage('BatchFillerImage2D').get_batch(pid)

    data = batch_data.data()
    dim  = batch_data.dim()

    print 'Process ID',pid,'data state',batch_data.state(),'size',data.size(),

    pydata = np.array(batch_data.data()).astype(np.float32)
    pydim  = []
    for i in xrange(dim.size()):
        pydim.append(dim[i])
    print 'dimension',pydim
    print 'Dumping pngs...'
    pydata = pydata.reshape(pydim[0],len(pydata)/pydim[0]).astype(np.float32)
    for entry,entry_pydata in enumerate(pydata):
        entry_pydata = entry_pydata.reshape(pydim[2],pydim[3]).astype(np.float32)
        print entry, entry_pydata.shape
        fig,ax=plt.subplots(figsize=(12,8),facecolor='w')
        fmappng=plt.imshow(entry_pydata,cmap='jet',interpolation='none')
        fmappng.write_png('Proc%02d_BatchEntry%03d.png' % (pid,entry))
        plt.close(fig)
        sys.stdout.write('Done batch entry %-2d/%d\r' % (entry,pydim[0]))
        sys.stdout.flush()
    print
