#ifndef __LARCV_UTILS_CXX__
#define __LARCV_UTILS_CXX__
#include "utils.h"
#include "eigen.h"

namespace larcv {

std::vector< std::vector<int> > sample(double ** carray, int * csamples, int npts, int num_samples, double threshold) {
    std::vector< std::vector<int> > all_fragments;
    for (size_t i = 0; i < num_samples; ++i)
    {
        double x = carray[csamples[i]][0];
        double y = carray[csamples[i]][1];
        double z = carray[csamples[i]][2];
        std::vector<int> fragment_idx;
        // Find all points in this fragment around the sample point
        for (size_t j = 0; j < npts; ++j) {
            double distance = std::sqrt(std::pow(carray[j][0]-x, 2) + std::pow(carray[j][1]-y, 2) + std::pow(carray[j][2]-z, 2));
            if (distance <= threshold) {
                fragment_idx.push_back(j);
            }
        }
        int nfrag = fragment_idx.size();
        if (nfrag <= 0) continue;
        all_fragments.push_back(fragment_idx);
    }
    return all_fragments;
}

void compute_pca(double ** coords, int nfrag, double * output) {
    double mean[3];
    for (size_t k = 0; k < nfrag; ++k) {
        mean[0] += coords[k][0];
        mean[1] += coords[k][1];
        mean[2] += coords[k][2];
    }
    mean[0] /= nfrag;
    mean[1] /= nfrag;
    mean[2] /= nfrag;

    // Compute local PCA
    // covariance matrix
    double M[3][3];
    for (size_t i1 = 0; i1 < 3; ++i1) {
        for (size_t i2 = 0; i2 < 3; ++i2) {
            M[i1][i2] = 0.;
            for (size_t k = 0; k < nfrag; ++k) {
                M[i1][i2] = M[i1][i2] + (coords[k][i1]-mean[i1]) * (coords[k][i2]-mean[i2]);
            }
        }
    }
    // eigenvectors of cov matrix
    double eigenvectors[3][3];
    double eigenvalues[3];
    eigen_decomposition(M, eigenvectors, eigenvalues);
    //std::cout << eigenvalues[0] << " " << eigenvalues[1] << " " << eigenvalues[2] << std::endl;
    size_t best_idx;
    if (eigenvalues[1] > eigenvalues[0]) {
        best_idx = (eigenvalues[2] > eigenvalues[1]) ? 2 : 1;
    }
    else {
        best_idx = (eigenvalues[2] > eigenvalues[0]) ? 2 : 0;
    }
    output[0] = eigenvectors[0][best_idx];
    output[1] = eigenvectors[1][best_idx];
    output[2] = eigenvectors[2][best_idx];
}
}
#endif
