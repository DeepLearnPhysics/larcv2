#ifndef LARCV_CLUSTERTENSORUTILS_H
#define LARCV_CLUSTERTENSORUTILS_H

#include "Image2D.h"
#include "Voxel2D.h"
#include "Voxel3D.h"
#include "EventVoxel3D.h"
#include "EventParticle.h"
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

  /// Algorithm to create EventSparseTensor3D from meta, voxel id array, and value array
  larcv::EventSparseTensor3D as_event_sparse3d(const Voxel3DMeta& meta,
    const std::vector<larcv::VoxelID_t>& id_v,
    const std::vector<float>& val_v);

  /// Algorithm to create EventClusterVoxel3D from meta, voxel id array, and value array
  larcv::EventClusterVoxel3D as_event_cluster3d(const Voxel3DMeta& meta,
    const std::vector<std::vector<larcv::VoxelID_t> >& id_vv,
    const std::vector<std::vector<float> >& val_vv);

  /// Algorithm to create EventSparseTensor3D from meta, voxel id array, and value array
  void as_event_sparse3d(EventSparseTensor3D& data,
    const Voxel3DMeta& meta,
    const std::vector<larcv::VoxelID_t>& id_v,
    const std::vector<float>& val_v);

  /// Algorithm to create EventClusterVoxel3D from meta, voxel id array, and value array
  void as_event_cluster3d(EventClusterVoxel3D& data,
    const Voxel3DMeta& meta,
    const std::vector<std::vector<larcv::VoxelID_t> >& id_vv,
    const std::vector<std::vector<float> >& val_vv);
  
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

  /// DO NOT USE THIS
  larcv::EventSparseTensor3D generate_semantics(const EventClusterVoxel3D& clusters, const EventParticle& particles);

}
#endif
