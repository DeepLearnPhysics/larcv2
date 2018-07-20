#ifndef LARCV_CLUSTERTENSORUTILS_H
#define LARCV_CLUSTERTENSORUTILS_H

#include "Image2D.h"
#include "Voxel2D.h"
#include "Voxel3D.h"

namespace larcv {

  /// Algorithm to convert SparseTensor2D into Image2d
  larcv::Image2D as_image2d(const SparseTensor2D& tensor);

  /// Algorithm to convert VoxelSet and ImagMeta into Image2d
  larcv::Image2D as_image2d(const VoxelSet& tensor, const ImageMeta& meta);

  /// Algorithm to convert ClusterPixel2D into SparseTensor2D
  larcv::SparseTensor2D as_sparse_tensor2d(const ClusterPixel2D& clusters);

  /// Algorithm to convert ClusterPixel2D into Image2D
  larcv::Image2D as_image2d(const ClusterPixel2D& clusters);

  /// Algorithm to convert ClusterVoxel3D into SparseTensor3D
  larcv::SparseTensor3D as_sparse_tensor3d(const ClusterVoxel3D& clusters);
  
  /**
     \class FlatTensorContents
     A simple class to store sparse tensor (VoxelSet)'s coordinte (x,y,z) and value using flat arrays
  */
  class FlatTensorContents{
  public:
    std::vector<int> x, y, z; ///< XYZ coordinates
    std::vector<float> value; ///< Values
  };

  /// Algorithm to extract three 1D arrays (x y and value) from 2D VoxelSet
  FlatTensorContents as_flat_arrays(const VoxelSet& tensor, const ImageMeta& meta);

  /// Algorithm to extract four 1D arrays (x y z and value) from 3D VoxelSet
  FlatTensorContents as_flat_arrays(const VoxelSet& tensor, const Voxel3DMeta& meta);

}
#endif
