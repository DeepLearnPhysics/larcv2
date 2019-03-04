from larcv import larcv
import sys, time

batch_size=400
report_cycle=2
sleep_time=0.02

proc=larcv.ThreadProcessor("ThreadProcessor")
proc.configure(sys.argv[1])

start_time=time.time()
proc.batch_process(batch_size)

ctr=1
while 1:
    if ctr % (int(report_cycle/sleep_time)) == 0:
        proc.status_dump()
        event_rate = proc.process_counter() / (time.time() - start_time)
        print 'Rate:',event_rate,'[event/sec] ...', event_rate * 512 * 512 * 4 / 1.e6,'[MB/s]'
    status_v = proc.storage_status_array()
    for storage_id in xrange(status_v.size()):
        if int(status_v[storage_id]) == 3 or int(status_v[storage_id]) == 1:
            proc.release_data(storage_id)
            proc.batch_process(batch_size)
            break
    time.sleep(sleep_time)
    ctr+=1

