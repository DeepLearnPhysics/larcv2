#ifndef __LARCV_VOXEL3D_CXX__
#define __LARCV_VOXEL3D_CXX__

#include "Voxel3D.h"
#include <iostream>

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
