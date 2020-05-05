#ifndef __LARCV_UTILS_H__
#define __LARCV_UTILS_H__
#include <iostream>
#include <vector>
#include <cmath>

namespace larcv {
	std::vector< std::vector<int> > sample(double ** carray, int * csamples, int npts, int num_samples, double threshold);

	// Compute PCA on coords with shape (nfrag, 3) and stores
	// eigenvector with largest eigenvalue in output
	void compute_pca(double ** coords, int nfrag, double * output);

  // PCA class for 3-dim points
  class PCA_3D {
    public:
      // proper handling of 2d array to avoid memory leaks
      struct Points_t
      {
        double **xyz = nullptr;
        size_t size = 0;

        Points_t(size_t n) { resize(n); }

        Points_t (const Points_t&) = delete;
        Points_t& operator= (const Points_t&) = delete;

        ~Points_t() { _del(); }

        void resize(size_t n)
        {
          if (n == size) return;
          _del();
          size = n;
          xyz = new double*[size];
          for (size_t i = 0; i < size; ++i) xyz[i] = new double[3];
        }

        private:
          void _del()
          {
            if (xyz) {
              for (size_t i = 0 ; i < size; ++i) delete[] xyz[i];
              delete[] xyz;
              size = 0;
            }
          }
      }; 

      PCA_3D(const Points_t& points, bool store_spread=false);
      PCA_3D(double **coords, size_t n, bool store_spread=false);

      void transform(double **input, double **output, size_t n);
      void transform(const Points_t& input, Points_t& output);

      std::vector<double>
      find_spread(double** input, size_t n, bool do_transform=false);

      std::vector<double>
      find_spread(const Points_t& pts, bool do_transform=false);

      const std::vector<double>& get_axis(size_t i) const { return _components.at(i); }
      const std::vector<double>& get_mean() const { return _mean; }
      std::vector<double> get_spread() const { return _spread; }

    private:
      void _fit(double **coords, size_t n, bool store_spread);
      std::vector<std::vector<double>> _components;
      std::vector<double> _mean;
      std::vector<double> _spread;
  };
}
#endif
