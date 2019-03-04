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

  void Cluster2DFromCluster3D::configure_labels(const PSet& cfg)
  {
    _cluster3d_producer_v.clear();
    _output_producer_v.clear();
    _cluster3d_producer_v = cfg.get<std::vector<std::string> >("Cluster3DProducerList", _cluster3d_producer_v);
    _output_producer_v    = cfg.get<std::vector<std::string> >("OutputProducerList", _output_producer_v);

    if (_cluster3d_producer_v.empty()) {
      auto cluster3d_producer = cfg.get<std::string>("Cluster3DProducer", "");
      auto output_producer    = cfg.get<std::string>("OutputProducer", "");
      if (!cluster3d_producer.empty()) {
        _cluster3d_producer_v.push_back(cluster3d_producer);
        if (output_producer.empty()) output_producer = cluster3d_producer + "_cluster2d";
        _output_producer_v.push_back(output_producer);
      }
    }

    if (_output_producer_v.empty()) {
      _output_producer_v.resize(_cluster3d_producer_v.size(), "");
    }
    else if (_output_producer_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "Cluster3DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void Cluster2DFromCluster3D::configure(const PSet& cfg)
  {
    configure_labels(cfg);

    _xy_v = cfg.get<std::vector<bool> >("XYList", _xy_v);
    if (_xy_v.empty()) {
      auto xy = cfg.get<bool>("XY", true);
      _xy_v.resize(_cluster3d_producer_v.size(), xy);
    } else if (_xy_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "XYList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _yz_v = cfg.get<std::vector<bool> >("YZList", _yz_v);
    if (_yz_v.empty()) {
      auto yz = cfg.get<bool>("YZ", true);
      _yz_v.resize(_cluster3d_producer_v.size(), yz);
    } else if (_yz_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "YZList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _zx_v = cfg.get<std::vector<bool> >("ZXList", _zx_v);
    if (_zx_v.empty()) {
      auto zx = cfg.get<bool>("ZX", true);
      _zx_v.resize(_cluster3d_producer_v.size(), zx);
    } else if (_zx_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "ZXList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

  }

  void Cluster2DFromCluster3D::initialize()
  {}

  bool Cluster2DFromCluster3D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _cluster3d_producer_v.size(); ++producer_index) {
      auto const& cluster3d_producer = _cluster3d_producer_v[producer_index];
      auto const& output_producer = _output_producer_v[producer_index];
      auto const& xy = _xy_v[producer_index];
      auto const& yz = _yz_v[producer_index];
      auto const& zx = _zx_v[producer_index];

      auto const& ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(cluster3d_producer);
      auto const& meta3d = ev_cluster3d.meta();
      auto const& cluster3d_v = ev_cluster3d.as_vector();

      ProjectionID_t current_id = 0;

      ImageMeta meta_xy(meta3d.min_x(), meta3d.min_y(), meta3d.max_x(), meta3d.max_y(),
                        meta3d.num_voxel_x(), meta3d.num_voxel_y(),
                        (xy ? current_id : larcv::kINVALID_PROJECTIONID), meta3d.unit());
      if (xy) current_id += 1;

      ImageMeta meta_yz(meta3d.min_y(), meta3d.min_z(), meta3d.max_y(), meta3d.max_z(),
                        meta3d.num_voxel_y(), meta3d.num_voxel_z(),
                        (yz ? current_id : larcv::kINVALID_PROJECTIONID),  meta3d.unit());
      if (yz) current_id += 1;

      ImageMeta meta_zx(meta3d.min_z(), meta3d.min_x(), meta3d.max_z(), meta3d.max_x(),
                        meta3d.num_voxel_z(), meta3d.num_voxel_x(),
                        (zx ? current_id : larcv::kINVALID_PROJECTIONID), meta3d.unit());

      LARCV_DEBUG() << "Processing " << cluster3d_v.size()
                    << " voxel 3D clusters" << std::endl;

      VoxelSetArray vsa_xy; vsa_xy.resize(cluster3d_v.size());
      VoxelSetArray vsa_yz; vsa_yz.resize(cluster3d_v.size());
      VoxelSetArray vsa_zx; vsa_zx.resize(cluster3d_v.size());

      for (size_t cluster_index = 0; cluster_index < cluster3d_v.size(); ++cluster_index) {

        auto const& cluster3d = cluster3d_v[cluster_index];

        for (auto const& vox : cluster3d.as_vector()) {

          auto const pos = meta3d.position(vox.id());
          LARCV_DEBUG() << "Voxel " << vox.id() << " value " << vox.value() << std::endl;
          if (xy) {
            auto& vs_xy = vsa_xy.writeable_voxel_set(cluster_index);
            LARCV_DEBUG() << "(x,y) = (" << pos.x << "," << pos.y << ")" << std::endl
                          << meta_xy.dump() << std::endl;
            vs_xy.emplace(meta_xy.index(meta_xy.row(pos.y), meta_xy.col(pos.x)), vox.value(), true);
          }
          if (yz) {
            auto& vs_yz = vsa_yz.writeable_voxel_set(cluster_index);
            LARCV_DEBUG() << "(y,z) = (" << pos.y << "," << pos.z << ")" << std::endl
                          << meta_yz.dump() << std::endl;
            vs_yz.emplace(meta_yz.index(meta_yz.row(pos.z), meta_yz.col(pos.y)), vox.value(), true);
          }
          if (zx) {
            auto& vs_zx = vsa_zx.writeable_voxel_set(cluster_index);
            LARCV_DEBUG() << "(z,x) = (" << pos.z << "," << pos.x << ")" << std::endl
                          << meta_zx.dump() << std::endl;
            vs_zx.emplace(meta_zx.index(meta_zx.row(pos.x), meta_zx.col(pos.z)), vox.value(), true);
          }
        }
      }

      auto& out_cluster2d = mgr.get_data<larcv::EventClusterPixel2D>(output_producer);

      if (xy) { out_cluster2d.emplace(std::move(vsa_xy), std::move(meta_xy)); }
      if (yz) { out_cluster2d.emplace(std::move(vsa_yz), std::move(meta_yz)); }
      if (zx) { out_cluster2d.emplace(std::move(vsa_zx), std::move(meta_zx)); }
    }
    return true;
  }

  void Cluster2DFromCluster3D::finalize()
  {}

}
#endif
