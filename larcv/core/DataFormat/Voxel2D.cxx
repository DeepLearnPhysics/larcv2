#ifndef __LARCV_VOXEL2D_CXX__
#define __LARCV_VOXEL2D_CXX__

#include "Voxel2D.h"
#include <iostream>

namespace larcv {
    SparseTensor2D::SparseTensor2D(larcv::VoxelSet&& vs, larcv::ImageMeta&& meta)
        : VoxelSet(vs)
    { this->meta(meta); }

    void SparseTensor2D::meta(const larcv::ImageMeta& meta)
    {
        auto const max_id = meta.size();
        for (auto const& vox : this->as_vector()) {
            if (vox.id() < max_id) continue;
            std::cerr << "VoxelSet contains ID " << vox.id()
                      << " which cannot exists in ImageMeta with size " << max_id
                      << std::endl;
            throw std::exception();
        }
        _meta = meta;
    }

    const Voxel& SparseTensor2D::close(VoxelID_t id, double distance, const larcv::ImageMeta& meta) const
	{
		const std::vector<larcv::Voxel>& voxel_v = this->as_vector();
		if(voxel_v.empty())
	      return kINVALID_VOXEL;

		const Point2D pt = meta.position(id);
		int threshold = (int) std::ceil(distance);
		VoxelID_t max = meta.id(std::min(pt.x + threshold, meta.max_x()), std::min(pt.y + threshold, meta.max_y()));
		VoxelID_t min = meta.id(std::max(pt.x - threshold, meta.min_x()), std::max(pt.y - threshold, meta.min_y()));
		Voxel vox_max(max,0.);
		Voxel vox_min(min,0.);
		auto iter_max = std::lower_bound(voxel_v.begin(), voxel_v.end(), vox_max);
		auto iter_min = std::lower_bound(voxel_v.begin(), voxel_v.end(), vox_min);

		double min_distance = distance;
		Voxel final_vox = kINVALID_VOXEL;
		for (auto i = iter_min; i < iter_max; ++i) {
			const Point2D current_point = meta.position((*i).id());
			double d = pt.distance(current_point);
			if (d < min_distance) {
				min_distance = d;
				final_vox = (*i);
			}
		}
		return final_vox;
	}

    ClusterPixel2D::ClusterPixel2D(larcv::VoxelSetArray&& vsa, larcv::ImageMeta meta)
        : VoxelSetArray(vsa)
    {this->meta(meta);}

    void ClusterPixel2D::meta(const larcv::ImageMeta& meta)
    {
        auto const max_id = meta.size();
        for (auto const& vs : this->as_vector()) {
            for (auto const& vox : vs.as_vector()) {
                if (vox.id() < max_id) continue;
                std::cerr << "VoxelSet contains ID " << vox.id()
                          << " which cannot exists in ImageMeta with size " << max_id
                          << std::endl;
                throw std::exception();
            }
        }
        _meta = meta;
    }

}

#endif
