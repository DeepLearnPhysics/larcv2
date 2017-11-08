#ifndef __Cluster3DCompressor_CXX__
#define __Cluster3DCompressor_CXX__

#include "Cluster3DCompressor.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static Cluster3DCompressorProcessFactory __global_Cluster3DCompressorProcessFactory__;

  Cluster3DCompressor::Cluster3DCompressor(const std::string name)
    : ProcessBase(name)
  {}

  void Cluster3DCompressor::configure(const PSet& cfg)
  {
    _cluster3d_producer  = cfg.get<std::string>("TargetProducer");
    _output_producer = cfg.get<std::string>("OutputProducer");
    _comp_factor     = cfg.get<size_t>("CompressionFactor");
    _scale_factor    = cfg.get<float>("ScaleFactor", 1.);
    _pool_type       = (PoolType_t)(cfg.get<unsigned short>("PoolType", (unsigned short)kSumPool));
  }

  void Cluster3DCompressor::initialize()
  {}

  bool Cluster3DCompressor::process(IOManager& mgr)
  {
    auto const& event_cluster = mgr.get_data<larcv::EventClusterVoxel3D>(_cluster3d_producer);

    auto const& orig_meta = event_cluster.meta();
    Voxel3DMeta meta;
    meta.set(orig_meta.min_x(), orig_meta.min_y(), orig_meta.min_z(),
             orig_meta.max_x(), orig_meta.max_y(), orig_meta.max_z(),
             orig_meta.num_voxel_x() / _comp_factor,
             orig_meta.num_voxel_y() / _comp_factor,
             orig_meta.num_voxel_z() / _comp_factor,
             orig_meta.unit());

    auto output_cluster = (larcv::EventClusterVoxel3D*)(mgr.get_data("cluster3d", _output_producer));
    output_cluster->meta(meta);
    output_cluster->resize(event_cluster.as_vector().size());

    for (auto const& cluster : event_cluster.as_vector()) {

      auto& out_cluster = ((VoxelSetArray*)output_cluster)->writeable_voxel_set(cluster.id());

      for (auto const& in_vox : cluster.as_vector()) {
        auto id = meta.id(orig_meta.position(in_vox.id()));

        switch (_pool_type) {

        case kSumPool: {
            out_cluster.emplace(id, in_vox.value(), true);
            break;
          }
        case kMaxPool: {
            auto const& out_vox = out_cluster.find(id);
            if (out_vox.id() == kINVALID_VOXELID) {
              out_cluster.emplace(id, in_vox.value(), true);
              break;
            } else if (out_vox.value() < in_vox.value()) {
              out_cluster.emplace(id, in_vox.value(), false);
              break;
            }
            break;
          }
        default: {
            LARCV_CRITICAL() << "PoolType_t " << (unsigned short)(_pool_type) << " not supported!" << std::endl;
            throw larbys();
          }
        }
      }
    }
    return true;
  }

  void Cluster3DCompressor::finalize()
  {}

}
#endif
