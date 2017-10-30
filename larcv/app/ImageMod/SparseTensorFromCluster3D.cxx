#ifndef __SPARSETENSORFROMCLUSTER3D_CXX__
#define __SPARSETENSORFROMCLUSTER3D_CXX__

#include "SparseTensorFromCluster3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static SparseTensorFromCluster3DProcessFactory __global_SparseTensorFromCluster3DProcessFactory__;

  SparseTensorFromCluster3D::SparseTensorFromCluster3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void SparseTensorFromCluster3D::configure(const PSet& cfg)
  {
    _cluster3d_producer = cfg.get<std::string>("ClusterVoxel3DProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
  }

  void SparseTensorFromCluster3D::initialize()
  {}

  bool SparseTensorFromCluster3D::process(IOManager& mgr)
  {
    auto const& ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
    auto& ev_voxel3d = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);

    auto const& meta = ev_cluster3d.meta();
    ev_voxel3d.meta(meta);

    for(auto const& cluster : ev_cluster3d.as_vector()) {

      for(auto const& vox : cluster.as_vector())

	ev_voxel3d.add(vox);

    }

    return true;
  }

  void SparseTensorFromCluster3D::finalize()
  {}

}
#endif
