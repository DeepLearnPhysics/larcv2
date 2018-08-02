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

  FlatTensorContents as_flat_arrays(const VoxelSet& tensor, const ImageMeta& meta)
  {
    FlatTensorContents res;
    res.x.resize(tensor.size());
    res.y.resize(tensor.size());
    res.z.resize(0);
    res.value.resize(tensor.size());

    size_t x,y;
    auto const& vox_array = tensor.as_vector();
    for(size_t i=0; i<tensor.as_vector().size(); ++i) {
      auto const& vox = vox_array[i];
      meta.index_to_rowcol(vox.id(),y,x);
      res.x[i] = x;
      res.y[i] = y;
      res.value[i] = vox.value();
    }
    return res;
  }

  FlatTensorContents as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta)
  {
    FlatTensorContents res;
    res.x.resize(tensor.size());
    res.y.resize(tensor.size());
    res.z.resize(tensor.size());
    res.value.resize(tensor.size());

    size_t x,y,z;
    auto const& vox_array = tensor.as_vector();
    for(size_t i=0; i<tensor.as_vector().size(); ++i) {
      auto const& vox = vox_array[i];
      meta.id_to_xyz_index(vox.id(),x,y,z);
      res.x[i] = x;
      res.y[i] = y;
      res.z[i] = z;
      res.value[i] = vox.value();
    }
    return res;
  }

  
}
#endif
