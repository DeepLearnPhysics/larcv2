from __future__ import print_function
from larcv import larcv
import sys, time

batch_size=100
report_cycle=1
sleep_time=0.0001
stop_time=120
record=True

proc=larcv.ThreadProcessor("ThreadProcessor")
proc.configure(sys.argv[1])

# list of batch filler IDs
ids = proc.batch_fillers()
names = [proc.storage_name(i) for i in ids]
dtype = [larcv.BatchDataTypeName(proc.batch_types()[i]) for i in ids]

fout=None
if record:
    fout = open('data.csv','w')
    fout.write('time,entries,datasize\n')

data_read = 0
ctr=1
start_time=time.time()
proc.start_manager(batch_size)
while 1:

    elapsed_time = time.time() - start_time
    if elapsed_time > stop_time:
        break
    
    if ctr % (int(report_cycle/sleep_time)) == 0:
        proc.status_dump()
        event_read   = proc.process_counter()
        print( 'Rate:',event_read / elapsed_time,'[event/sec] ...', data_read / 1.e6 / elapsed_time,'[MB/s]')
        fout.write('{:g},{:d},{:d}\n'.format(elapsed_time,event_read,data_read))
        
    status_v = proc.storage_status_array()
    for storage_id in xrange(status_v.size()):
        status = int(status_v[storage_id])
        if status == 3:
            for idx, name in enumerate(names):
                data_read += larcv.BatchDataStorageFactory(dtype[idx]).get().get_storage(name).get_batch(storage_id).data().size()
            #entries    = proc.processed_entries(storage_id)
            #batch_id   = proc.batch_id(storage_id)
            #msg = 'Batch {:02d} done contains entries: '.format(batch_id)
            #print( 'Batch',batch_id,'done, contains entries:',)
            #for entry in range(entries.size()):
            #    msg += '{:d} '.format(entries[entry])
            #print(msg)
            #print('Releasing storage data',storage_id)
            proc.release_data(storage_id)

    time.sleep(sleep_time)
    ctr+=1
proc.stop_manager()
proc.reset()
fout.close()
