#ifndef LARCV_EVENTVOXEL2D_CXX
#define LARCV_EVENTVOXEL2D_CXX

#include "EventVoxel2D.h"

namespace larcv {

  /// Global larcv::EventClusterPixel2DFactory to register EventClusterPixel2D
  static EventClusterPixel2DFactory __global_EventClusterPixel2DFactory__;

  /// Global larcv::EventSparseTensor2DFactory to register EventSparseTensor2D
  static EventSparseTensor2DFactory __global_EventSparseTensor2DFactory__;

  //
  // EventClusterPixel2D
  //
  const larcv::ClusterPixel2D&
  EventClusterPixel2D::cluster_pixel_2d(const ProjectionID_t id) const
  {
    if(id >= _cluster_v.size()) {
      std::cerr << "EventClusterPixel2D does not hold any ClusterPixel2D for ProjectionID_t " << id << std::endl;
      throw larbys();
    }
    return _cluster_v[id];
  }

  void EventClusterPixel2D::emplace(larcv::ClusterPixel2D&& clusters)
  {
    if(_cluster_v.size() <= clusters.meta().id())
      _cluster_v.resize(clusters.meta().id()+1);
    _cluster_v[clusters.meta().id()] = std::move(clusters);
  }

  void EventClusterPixel2D::set(const larcv::ClusterPixel2D& clusters) 
  {
    if(_cluster_v.size() <= clusters.meta().id())
      _cluster_v.resize(clusters.meta().id()+1);
    _cluster_v[clusters.meta().id()] = clusters;
    
  }
  
  void EventClusterPixel2D::emplace(larcv::VoxelSetArray&& clusters, larcv::ImageMeta&& meta)
  {
    larcv::ClusterPixel2D source(std::move(clusters),std::move(meta));
    emplace(std::move(source));
  }

  void EventClusterPixel2D::set(const larcv::VoxelSetArray& clusters, const larcv::ImageMeta& meta)
  {
    larcv::ClusterPixel2D source(clusters);
    source.meta(meta);
    emplace(std::move(source));
  }

  //
  // EventSparseTensor2D
  //
  const larcv::SparseTensor2D&
  EventSparseTensor2D::sparse_tensor_2d(const ProjectionID_t id) const
  {
    if(id >= _tensor_v.size()) {
      std::cerr << "EventSparseTensor2D does not hold any SparseTensor2D for ProjectionID_t " << id << std::endl;
      throw larbys();
    }
    return _tensor_v[id];
  }

  void EventSparseTensor2D::emplace(larcv::SparseTensor2D&& cluster)
  {
    if(_tensor_v.size() <= cluster.meta().id())
      _tensor_v.resize(cluster.meta().id()+1);
    _tensor_v[cluster.meta().id()] = std::move(cluster);
  }

  void EventSparseTensor2D::set(const larcv::SparseTensor2D& cluster) 
  {
    if(_tensor_v.size() <= cluster.meta().id())
      _tensor_v.resize(cluster.meta().id()+1);
    _tensor_v[cluster.meta().id()] = cluster;
  }
  
  void EventSparseTensor2D::emplace(larcv::VoxelSet&& cluster, larcv::ImageMeta&& meta)
  {
    larcv::SparseTensor2D source(std::move(cluster),std::move(meta));
    emplace(std::move(source));
  }

  void EventSparseTensor2D::set(const larcv::VoxelSet& cluster, const larcv::ImageMeta& meta)
  {
    larcv::SparseTensor2D source(cluster);
    source.meta(meta);
    emplace(std::move(source));
  }

  
}

#endif
