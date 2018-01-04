#ifndef LARCV_CLUSTERTENSORUTILS_CXX
#define LARCV_CLUSTERTENSORUTILS_CXX

#include "ClusterTensorUtils.h"

namespace larcv {

  larcv::Image2D as_image2d(const SparseTensor2D& tensor)
  { return as_image2d((VoxelSet)tensor,tensor.meta()); }
  
  larcv::Image2D as_image2d(const VoxelSet& tensor, const ImageMeta& meta)
  {
    std::vector<float> data(meta.size(),0.);
    for(auto const& voxel : tensor.as_vector())
      data[voxel.id()] = voxel.value();
    
    return Image2D(std::move(ImageMeta(meta)),std::move(data));
  }

  larcv::SparseTensor2D as_sparse_tensor2d(const ClusterPixel2D& clusters)
  {
    VoxelSet vs;
    for(auto const& cluster : clusters.as_vector()) {
      for(auto const& voxel : cluster.as_vector())
	{ vs.add(voxel); }
    }
    return SparseTensor2D(std::move(vs),std::move(ImageMeta(clusters.meta())));
  }

  larcv::Image2D as_image2d(const ClusterPixel2D& clusters)
  {
    std::vector<float> data(clusters.meta().size(),0.);
    for(auto const& cluster : clusters.as_vector()) {
      for(auto const& voxel : cluster.as_vector())
	{ data[voxel.id()] = voxel.value(); }
    }

    return Image2D(std::move(ImageMeta(clusters.meta())),std::move(data));
  }

  larcv::SparseTensor3D as_sparse_tensor3d(const ClusterVoxel3D& clusters)
  {
    VoxelSet vs;
    for(auto const& cluster : clusters.as_vector()) {
      for(auto const& voxel : cluster.as_vector())
	{ vs.add(voxel); }
    }
    return SparseTensor3D(std::move(vs),std::move(Voxel3DMeta(clusters.meta())));
  }

  
}
#endif
