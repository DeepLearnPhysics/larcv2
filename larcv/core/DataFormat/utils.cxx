#ifndef __LARCV_UTILS_CXX__
#define __LARCV_UTILS_CXX__
#include <algorithm>
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

PCA_3D::PCA_3D(const Points_t& points, bool store_spread) 
{
  _fit(points.xyz, points.size, store_spread);
}

PCA_3D::PCA_3D(double **coords, size_t n, bool store_spread)
{
  _fit(coords, n, store_spread);
}

void PCA_3D::_fit(double **coords, size_t n, bool store_spread)
{
  _mean.resize(3, 0); 
  for (size_t k = 0; k < n; ++k) {
      _mean[0] += coords[k][0];
      _mean[1] += coords[k][1];
      _mean[2] += coords[k][2];
  }
  _mean[0] /= n;
  _mean[1] /= n;
  _mean[2] /= n;

  // Compute local PCA
  // covariance matrix
  double M[3][3];
  for (size_t i1 = 0; i1 < 3; ++i1) {
      for (size_t i2 = 0; i2 < 3; ++i2) {
          M[i1][i2] = 0.;
          for (size_t k = 0; k < n; ++k) {
              M[i1][i2] = M[i1][i2] + (coords[k][i1] - _mean[i1]) * (coords[k][i2] - _mean[i2]);
          }
      }
  }

  double ev[3];    // unordered eigenvalue
  double vh[3][3]; // transpose of eig_vec
  eigen_decomposition(M, vh, ev);

  // argsort eigenvalue
  std::vector<size_t> idx({0, 1, 2});
  std::sort(idx.begin(), idx.end(), [&](size_t i, size_t j){return ev[i] > ev[j];});
    
  // fill eigenvectors with descending eigenvalues
  _components.reserve(3);
  double eig_val[3];
  for (size_t i = 0; i < 3; ++i){
    size_t ii = idx[i];
    eig_val[i] = ev[ii];
    std::vector<double> vec(3);
    for (size_t j = 0; j < 3; ++j)
      vec[j] = vh[j][ii]; // transpose of vh sorted by eigenvalue
    _components.push_back(std::move(vec));
  }

  if (store_spread)
    _spread = find_spread(coords, n, true);
}

std::vector<double> PCA_3D::find_spread(const Points_t& pts, bool do_transform)
{
  return find_spread(pts.xyz, pts.size, do_transform);
}

std::vector<double> PCA_3D::find_spread(double **input, size_t n, bool do_transform)
{

  std::vector<double> spread;
  if (n == 0) return spread;

  Points_t pts(do_transform ? n : 0);
  if (do_transform) transform(input, pts.xyz, n);

  double** xyz = do_transform ? pts.xyz : input;

  double min_pt[3], max_pt[3];
  std::copy_n(xyz[0], 3, min_pt);
  std::copy_n(xyz[0], 3, max_pt);

  for (size_t i_pt = 1; i_pt < n; ++i_pt) {
    auto& pt = xyz[i_pt];
    for (size_t i = 0; i < 3; ++i) {
      if (pt[i] < min_pt[i]) min_pt[i] = pt[i]; 
      if (pt[i] > max_pt[i]) max_pt[i] = pt[i]; 
    } //for xyz
  } // for i_pt

  spread.reserve(3);
  for (size_t i = 0; i < 3; ++i) 
    spread.emplace_back(max_pt[i] - min_pt[i]);

  return spread;
}

void PCA_3D::transform(double **input, double **output, size_t n)
{
  for (size_t i_pt = 0; i_pt < n; ++i_pt) {
    auto& pt = output[i_pt];
    std::fill_n(pt, 3, 0.);
    // matrix multiplication
    for (size_t i = 0; i < 3; ++i)
      for (size_t j = 0; j < 3; ++j)
        pt[i] += _components[i][j] * (input[i_pt][j] - _mean[j]);
  }
}

void PCA_3D::transform(const Points_t& input, Points_t& output)
{
  output.resize(input.size);
  transform(input.xyz, output.xyz, input.size);
}
} // namespace larcv
#endif
