import os,sys
import numpy as np

data=[str(3434*i) for i in xrange(8)]
#data=[str(int(1717)*i) for i in xrange(8)]
#data=[str(int(215)*i) for i in xrange(8)]

from multiprocessing.dummy import Pool as ThreadPool

def work(d):
    SS="python run.py trk_vtx.cfg %s ~/Desktop/numu_ccqe_p00_p07.root &" % d
    #SS="run.py shr_vtx.cfg %s ~/Desktop/nue_track_shower.root& " %d
    print SS
    os.system(SS)

# Make the Pool of workers
pool = ThreadPool(8)

results = pool.map(work, data)

pool.close()
pool.join()
