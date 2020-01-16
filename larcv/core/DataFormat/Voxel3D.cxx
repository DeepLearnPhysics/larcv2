#ifndef __LARCV_VOXEL3D_CXX__
#define __LARCV_VOXEL3D_CXX__

#include "Voxel3D.h"
#include <iostream>
#include "utils.h"

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

	const Voxel& SparseTensor3D::close(VoxelID_t id, double distance, const larcv::Voxel3DMeta& meta) const
	{
		const std::vector<larcv::Voxel>& voxel_v = this->as_vector();
		if(voxel_v.empty())
	      return kINVALID_VOXEL;

		const Point3D pt = meta.position(id);
		int threshold = (int) std::ceil(distance);
		VoxelID_t max = meta.id(std::min(pt.x + threshold, meta.max_x()), std::min(pt.y + threshold, meta.max_y()), std::min(pt.z + threshold, meta.max_z()));
		VoxelID_t min = meta.id(std::max(pt.x - threshold, meta.min_x()), std::max(pt.y - threshold, meta.min_y()), std::max(pt.z - threshold, meta.min_z()));
		Voxel vox_max(max,0.);
		Voxel vox_min(min,0.);
		auto iter_max = std::lower_bound(voxel_v.begin(), voxel_v.end(), vox_max);
		auto iter_min = std::lower_bound(voxel_v.begin(), voxel_v.end(), vox_min);

		double min_distance = distance;
		Voxel final_vox = kINVALID_VOXEL;
		for (auto i = iter_min; i < iter_max; ++i) {
			const Point3D current_point = meta.position((*i).id());
			double d = pt.distance(current_point);
			if (d < min_distance) {
				min_distance = d;
				final_vox = (*i);
			}
		}
		return final_vox;
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
		std::cout << "starting" << std::endl;
		compute_pca(coords, npts, output);
		return Point3D(output[0], output[1], output[2]);
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
