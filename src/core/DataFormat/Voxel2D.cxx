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
