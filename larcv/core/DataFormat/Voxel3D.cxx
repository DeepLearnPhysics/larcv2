#ifndef __LARCV_VOXEL3D_CXX__
#define __LARCV_VOXEL3D_CXX__

#include "Voxel3D.h"
#include <iostream>
#include <algorithm>

namespace larcv {
	SparseTensor3D::SparseTensor3D(VoxelSet&& vs, Voxel3DMeta meta)
		: VoxelSet(std::move(vs))
	{ this->meta(meta); }

	void SparseTensor3D::meta(const larcv::Voxel3DMeta& meta)
	{
		for (auto const& vox : this->as_vector()) {
			if (vox.id() < meta.size()) continue;
			std::cerr << "VoxelSet contains ID " << vox.id()
			          << " which cannot exists in Voxel3DMeta with size " << meta.size()
			          << std::endl;
			throw std::exception();
		}
		_meta = meta;
	}

	void SparseTensor3D::emplace(const double x, const double y, const double z,
	                             const float val, const bool add)
	{
		auto id = _meta.id(x, y, z);
		if (id != kINVALID_VOXELID) VoxelSet::emplace(id, val, add);
	}

	const Voxel& SparseTensor3D::close(VoxelID_t id, double distance) const
	{
		const std::vector<larcv::Voxel>& voxel_v = this->as_vector();
		if(voxel_v.empty())
	      return kINVALID_VOXEL;

		const Point3D pt = _meta.position(id);
		int threshold = (int) std::ceil(distance);
		VoxelID_t max = _meta.id(std::min(pt.x + threshold, _meta.max_x()), std::min(pt.y + threshold, _meta.max_y()), std::min(pt.z + threshold, _meta.max_z()));
		VoxelID_t min = _meta.id(std::max(pt.x - threshold, _meta.min_x()), std::max(pt.y - threshold, _meta.min_y()), std::max(pt.z - threshold, _meta.min_z()));
		Voxel vox_max(max,0.);
		Voxel vox_min(min,0.);
		auto iter_max = std::lower_bound(voxel_v.begin(), voxel_v.end(), vox_max);
		auto iter_min = std::lower_bound(voxel_v.begin(), voxel_v.end(), vox_min);

		double min_distance = distance;
		VoxelID_t final_vox = kINVALID_VOXELID;
		//std::cout << kINVALID_VOXEL << std::endl;
		for (auto& i = iter_min; i != iter_max; ++i) {
			const Point3D current_point = _meta.position((*i).id());
			double d = pt.distance(current_point);
			if (d < min_distance) {
				min_distance = d;
				final_vox = (*i).id();
			}
		}
		return this->find(final_vox);
	}

	bool SparseTensor3D::within(VoxelID_t id, double distance) const {
		const std::vector<larcv::Voxel>& voxel_v = this->as_vector();
		if(voxel_v.empty())
	      return false;

		const Point3D pt = _meta.position(id);
  		int threshold = (int) std::ceil(distance);
  		VoxelID_t max = _meta.id(std::min(pt.x + threshold, _meta.max_x()), std::min(pt.y + threshold, _meta.max_y()), std::min(pt.z + threshold, _meta.max_z()));
  		VoxelID_t min = _meta.id(std::max(pt.x - threshold, _meta.min_x()), std::max(pt.y - threshold, _meta.min_y()), std::max(pt.z - threshold, _meta.min_z()));
  		Voxel vox_max(max,0.);
  		Voxel vox_min(min,0.);
  		auto iter_max = std::lower_bound(voxel_v.begin(), voxel_v.end(), vox_max);
  		auto iter_min = std::lower_bound(voxel_v.begin(), voxel_v.end(), vox_min);

  		for (auto& i = iter_min; i != iter_max; ++i) {
  			const Point3D current_point = _meta.position((*i).id());
  			double d = pt.distance(current_point);
  			if (d < distance) {
  				return true;
  			}
  		}
		return false;
	}

	Point3D SparseTensor3D::pca() const {
		size_t npts = this->size();
		//float coords[npts][3];
		double ** coords = (double**) malloc(npts * sizeof(double*));
		for (size_t i = 0; i < npts; ++i) {
			coords[i] = (double*) malloc(3 * sizeof(double));
			Voxel v = this->as_vector()[i];
			Point3D p = this->meta().position(v.id());
			coords[i][0] = p.x;
			coords[i][1] = p.y;
			coords[i][2] = p.z;
		}

		double output[3];
		compute_pca(coords, npts, output);

    // coords: free me please!
    for (size_t i = 0; i < npts; ++i) free(coords[i]);
    free(coords);

		return Point3D(output[0], output[1], output[2]);
	}

  PCA_3D SparseTensor3D::fit_pca(bool store_spread, bool use_true_coord)
  {
    PCA_3D::Points_t points(this->size());

    // fill (x,y,z) from voxel id or xyz
		for (size_t i = 0; i < points.size; ++i) {
			Voxel v = this->as_vector()[i];
      if (use_true_coord) {
        Point3D p = this->meta().position(v.id());
        points.xyz[i][0] = p.x;
        points.xyz[i][1] = p.y;
        points.xyz[i][2] = p.z;
      }
      else {
        size_t ix, iy, iz;
        this->meta().id_to_xyz_index(v.id(), ix, iy, iz);
        points.xyz[i][0] = ix;
        points.xyz[i][1] = iy;
        points.xyz[i][2] = iz;
      }
		}
    return PCA_3D(points, store_spread);
  }

	void ClusterVoxel3D::meta(const larcv::Voxel3DMeta& meta)
	{
		for (auto const& vs : this->as_vector()) {
			for (auto const& vox : vs.as_vector()) {
				if (vox.id() < meta.size()) continue;
				std::cerr << "VoxelSet contains ID " << vox.id()
				          << " which cannot exists in Voxel3DMeta with size " << meta.size()
				          << std::endl;
				throw std::exception();
			}
		}
		_meta = meta;
	}

}

#endif
