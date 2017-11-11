#ifndef __CLUSTER2DFROMCLUSTER3D_CXX__
#define __CLUSTER2DFROMCLUSTER3D_CXX__

#include "Cluster2DFromCluster3D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static Cluster2DFromCluster3DProcessFactory __global_Cluster2DFromCluster3DProcessFactory__;

  Cluster2DFromCluster3D::Cluster2DFromCluster3D(const std::string name)
    : ProcessBase(name)
  {}
    
  void Cluster2DFromCluster3D::configure(const PSet& cfg)
  {
    _cluster3d_producer = cfg.get<std::string>("Cluster3DProducer");
    _output_producer = _cluster3d_producer + "_xyz";
    _output_producer = cfg.get<std::string>("OutputProducer", _output_producer);
    _xy = cfg.get<int>("XY", 1);
    _yz = cfg.get<int>("YZ", 1);
    _zx = cfg.get<int>("ZX", 1);
  }

  void Cluster2DFromCluster3D::initialize()
  {}

  bool Cluster2DFromCluster3D::process(IOManager& mgr)
  {
    auto const& ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);
    auto const& meta3d = ev_cluster3d.meta();
    auto const& cluster3d_v = ev_cluster3d.as_vector();

    ProjectionID_t current_id = 0;

    ImageMeta meta_xy(meta3d.min_x(), meta3d.min_y(), meta3d.max_x(), meta3d.max_y(),
                      meta3d.num_voxel_x(), meta3d.num_voxel_y(),
                      (_xy ? current_id : larcv::kINVALID_PROJECTIONID), meta3d.unit());
    if (_xy) current_id += 1;

    ImageMeta meta_yz(meta3d.min_y(), meta3d.min_z(), meta3d.max_y(), meta3d.max_z(),
                      meta3d.num_voxel_y(), meta3d.num_voxel_z(),
                      (_yz ? current_id : larcv::kINVALID_PROJECTIONID),  meta3d.unit());
    if (_yz) current_id += 1;

    ImageMeta meta_zx(meta3d.min_z(), meta3d.min_x(), meta3d.max_z(), meta3d.max_x(),
                      meta3d.num_voxel_z(), meta3d.num_voxel_x(),
                      (_zx ? current_id : larcv::kINVALID_PROJECTIONID), meta3d.unit());

    LARCV_DEBUG() << "Processing " << cluster3d_v.size()
                  << " voxel 3D clusters" << std::endl;

    VoxelSetArray vsa_xy; vsa_xy.resize(cluster3d_v.size());
    VoxelSetArray vsa_yz; vsa_yz.resize(cluster3d_v.size());
    VoxelSetArray vsa_zx; vsa_zx.resize(cluster3d_v.size());

    for(size_t cluster_index=0; cluster_index<cluster3d_v.size(); ++cluster_index) {

      auto const& cluster3d = cluster3d_v[cluster_index];

      for(auto const& vox : cluster3d.as_vector()) {

        auto const pos = meta3d.position(vox.id());
        LARCV_DEBUG() << "Voxel " << vox.id() << " value " << vox.value() << std::endl;
        if (_xy) {
          auto& vs_xy = vsa_xy.writeable_voxel_set(cluster_index);
          LARCV_DEBUG() << "(x,y) = (" << pos.x << "," << pos.y << ")" << std::endl
                        << meta_xy.dump() << std::endl;
          vs_xy.emplace(meta_xy.index(meta_xy.row(pos.y), meta_xy.col(pos.x)), vox.value(), true);
        } 
        if (_yz) {
          auto& vs_yz = vsa_yz.writeable_voxel_set(cluster_index);
          LARCV_DEBUG() << "(y,z) = (" << pos.y << "," << pos.z << ")" << std::endl
                        << meta_yz.dump() << std::endl;
          vs_yz.emplace(meta_yz.index(meta_yz.row(pos.z), meta_yz.col(pos.y)), vox.value(), true);
        }
        if (_zx) {
          auto& vs_zx = vsa_zx.writeable_voxel_set(cluster_index);
          LARCV_DEBUG() << "(z,x) = (" << pos.z << "," << pos.x << ")" << std::endl
                        << meta_zx.dump() << std::endl;
          vs_zx.emplace(meta_zx.index(meta_zx.row(pos.x), meta_zx.col(pos.z)), vox.value(), true);
        }
      } 
    }

    auto& out_cluster2d = mgr.get_data<larcv::EventClusterPixel2D>(_output_producer);

    if (_xy) { out_cluster2d.emplace(std::move(vsa_xy),std::move(meta_xy)); }
    if (_yz) { out_cluster2d.emplace(std::move(vsa_yz),std::move(meta_yz)); }
    if (_zx) { out_cluster2d.emplace(std::move(vsa_zx),std::move(meta_zx)); }

    return true;
  }

  void Cluster2DFromCluster3D::finalize()
  {}

}
#endif
