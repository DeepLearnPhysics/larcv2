#ifndef __LARCV_UTILS_H__
#define __LARCV_UTILS_H__
#include <iostream>
#include <vector>
#include <cmath>

std::vector< std::vector<int> > sample(float ** carray, int * csamples, int npts, int num_samples, double threshold);

// Compute PCA on coords with shape (nfrag, 3) and stores
// eigenvector with largest eigenvalue in output
void pca(float ** coords, int nfrag, float * output);

#endif
