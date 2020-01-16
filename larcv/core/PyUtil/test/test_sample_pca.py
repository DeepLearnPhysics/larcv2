import numpy as np
from larcv import larcv
from sklearn.decomposition import PCA
import time

#larcv.test_openmp()

# 0. Randomly sample points
# 1. Given set of points and subset of points, return list of segments
# 2. Given list of segments, compute local PCA

# Trivial test here
# Given points of coordinates (i, i, i) for i in range(0, npts)
# we sample at (almost) every point within radius `distance`
# and compute local PCA.
# Returned eigenvector should be a multiple of (1, 1, 1).

npts = 10
repeat = 1 # if you want to look at computation time, increase repeat for better precision
sample_size = 1 # how many points on each side of the fragment center we want to include
distance = np.ceil(sample_size * np.sqrt(3)) # sample size determines the distance threshold

example = np.repeat(np.arange(npts, dtype=np.double)[:, None], 3, axis=1)
samples = np.array(np.arange(sample_size, npts-sample_size), dtype=np.int32)
nsamples = samples.shape[0]
eigenvectors = np.zeros((nsamples, 3), dtype=np.double)
#print(example, samples, eigenvectors)

durations = []
for r in range(repeat):
    start = time.time()
    segments = larcv.fragment(example, samples, distance)
    #print(segments)
    eigenvectors = larcv.local_pca(segments)
    durations.append((time.time()-start))
print(eigenvectors[:10])
print('Duration (from Python side): ', np.array(durations).mean()*1e3, 'ms')
print('\n')

durations = []
for r in range(repeat):
    start = time.time()
    pca = PCA(n_components=3)
    eigenvalues = []
    eigenvectors = []
    for i in range(npts-2):
        fragment = example[i:i+sample_size*2+1]
        #print(fragment)
        pca.fit(fragment)
        eigenvalues.append(pca.singular_values_)
        eigenvectors.append(pca.components_[0])
    durations.append((time.time()-start))
print('Duration: ', np.array(durations).mean()*1e3, 'ms')
print(np.vstack(eigenvectors)[:10])
