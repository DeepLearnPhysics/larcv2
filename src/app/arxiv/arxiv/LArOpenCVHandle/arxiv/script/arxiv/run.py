import os,sys
import numpy as np

#data=[str(3375*i) for i in xrange(8)]
data=[str(1654*i) for i in xrange(8)]

from multiprocessing.dummy import Pool as ThreadPool

def work(d):
    #SS="python parse_comb.py comb_vars.cfg %s ~/Desktop/ccqe.root &"%d
    SS="python parse_comb.py comb_vars.cfg %s ~/Desktop/numu_p00_p03.root &"%d
    print SS
    os.system(SS)

# Make the Pool of workers
pool = ThreadPool(8)

results = pool.map(work, data)

pool.close()
pool.join()

                        
