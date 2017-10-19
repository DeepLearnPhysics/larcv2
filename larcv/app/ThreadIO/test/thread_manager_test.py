from larcv import larcv
import sys, time

batch_size=50
report_cycle=2
sleep_time=0.02
consume_cycle=0.1

proc=larcv.ThreadProcessor("ThreadProcessor")
proc.configure(sys.argv[1])

start_time=time.time()
proc.start_manager(batch_size)

ctr=1
while 1:
    if ctr % (int(report_cycle/sleep_time)) == 0:
        proc.status_dump()
        event_rate = proc.process_counter() / (time.time() - start_time)
        print 'Rate:',event_rate,'[event/sec] ...', event_rate * 512 * 512 * 4 / 1.e6,'[MB/s]'

    status_v = proc.storage_status_array()
    for storage_id in xrange(status_v.size()):
        status = int(status_v[storage_id])
        if status == 3:
            entries = proc.processed_entries(storage_id)
            batch_id = proc.batch_id(storage_id)
            print 'Batch',batch_id,'done, contains entries:',entries.front(),'=>',entries.back()
            print 'Releasing storage data',storage_id
            proc.release_data(storage_id)

    time.sleep(sleep_time)
    ctr+=1

