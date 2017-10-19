from larcv import larcv
import sys, time
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

batch_size=3
report_cycle=2
sleep_time=0.02
consume_cycle=0.1

proc=larcv.ThreadProcessor("ThreadProcessor")
proc.configure(sys.argv[1])

start_time=time.time()
proc.start_manager(batch_size)

while 1:
    status_v = proc.storage_status_array()
    all_done=status_v.size()
    for i in xrange(status_v.size()):
        if int(status_v[i]) == 3: 
            all_done -= 1
    if all_done:
        time.sleep(1)
        continue
    break
print 'Done reading data!'

for pid in xrange(proc.storage_status_array().size()):
    print 'Processing storage id',pid
    batch_data = larcv.BatchDataStorageFactory('float').get().get_storage('BatchFillerVoxel3D').get_batch(pid)
    data = batch_data.data()
    dim  = batch_data.dim()

    print 'Process ID',pid,'data state',batch_data.state(),'size',data.size(),
    pydim  = []
    for i in xrange(dim.size()):
        pydim.append(dim[i])
    print 'dimension',pydim

    pydata = np.array(batch_data.data()).astype(np.float32)

    print 'Dumping pngs...'
    pydata = pydata.reshape(pydim[0],len(pydata)/pydim[0]).astype(np.float32)
    for entry,entry_pydata in enumerate(pydata):
        entry_pydata = entry_pydata.reshape(pydim[1:4]).astype(np.float32)
        print entry, entry_pydata.shape

        z,y,x = entry_pydata.nonzero()
        fig = plt.figure()
        ax = fig.add_subplot(111,projection='3d')
        ax.scatter(x,y,z,zdir='z',cmap='jet')
        plt.savefig('Proc%02d_BatchEntry%03d.png' % (pid,entry))
        
        sys.stdout.write('Done batch entry %-2d/%d\r' % (entry,pydim[0]))
        sys.stdout.flush()
    print
