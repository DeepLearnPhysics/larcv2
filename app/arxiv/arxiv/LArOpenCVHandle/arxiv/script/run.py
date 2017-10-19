import os,sys
import numpy as np

#data=[str(3434*i) for i in xrange(8)]
data=[str(int(1717)*i) for i in xrange(8)]
#data=[str(int(215)*i) for i in xrange(8)]

from multiprocessing.dummy import Pool as ThreadPool

def work(d):
    SS="nice -n 19 python parse_comb.py comb_vars.cfg %s ~/Desktop/numu_ccqe_p00_p07.root &" % d
    print SS
    os.system(SS)

# Make the Pool of workers
pool = ThreadPool(8)

results = pool.map(work, data)

pool.close()
pool.join()
