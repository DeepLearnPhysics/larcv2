from __future__ import print_function
import ROOT
from larcv import larcv
from larcv.dataloader3 import larcv_threadio
import numpy, time
import matplotlib.pyplot as plt

# instantiate dataloader2
proc = larcv_threadio()

io_config = \
"""
MainIO: {
Verbosity:    2
EnableFilter: false
RandomAccess: 2
RandomSeed:   123
InputFiles:   ["test_10k.root"]
ProcessType:  ["BatchFillerImage2D","BatchFillerImage2D"]
ProcessName:  ["main_data","main_label"]
NumThreads: 6
NumBatchStorage: 6

ProcessList: {
main_data: {
Verbosity: 3
ImageProducer: "data"
Channels: [0,1,2]
}
main_label: {
Verbosity: 3
ImageProducer: "segment"
Channels: [0,1,2]
}
}
}      
"""

import tempfile
cfg_file = tempfile.NamedTemporaryFile('w')
cfg_file.write(io_config)
cfg_file.flush()

# construct configuration dictionary
filler_cfg={}
filler_cfg["filler_name"] = 'MainIO'
filler_cfg["verbosity"]   = 0
filler_cfg['filler_cfg']  = cfg_file.name
filler_cfg['make_copy']   = False
proc.configure(filler_cfg)

batch_size=100
report_cycle=1
sleep_time=0.0001
stop_time=120

proc.start_manager(batch_size)
# ignore the 1st data loading
proc.next()
data_read = 0
start_time=time.time()
last_time = 0
proc.next()
while 1:
  elapsed_time = time.time() - start_time
  if elapsed_time > stop_time:
    break
  if not proc.is_reading():
    for name in ['main_data','main_label']:
      data_read += proc.fetch_data(name).data().size
    proc.next()
  if (elapsed_time - last_time) > report_cycle:
    print( 'Time',elapsed_time,'Rate:', data_read / 1.e6 / elapsed_time,'[MB/s]')
    last_time = elapsed_time
  time.sleep(sleep_time)

proc.reset()


