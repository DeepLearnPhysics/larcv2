#ifndef __Cluster3DCompressor_CXX__
#define __Cluster3DCompressor_CXX__

#include "Cluster3DCompressor.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"

namespace larcv {

  static Cluster3DCompressorProcessFactory __global_Cluster3DCompressorProcessFactory__;

  Cluster3DCompressor::Cluster3DCompressor(const std::string name)
    : ProcessBase(name)
  {}

  void Cluster3DCompressor::configure_labels(const PSet& cfg)
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

  void Cluster3DCompressor::configure(const PSet& cfg)
  {
    configure_labels(cfg);

    _comp_factor_v = cfg.get<std::vector<size_t> >("CompressionFactorList", _comp_factor_v);
    if (_comp_factor_v.empty()) {
      auto comp_factor = cfg.get<size_t>("CompressionFactor", 1);
      _comp_factor_v.resize(_cluster3d_producer_v.size(), comp_factor);
    } else if (_comp_factor_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "CompressionFactorList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _scale_factor_v = cfg.get<std::vector<float> >("ScaleFactorList", _scale_factor_v);
    if (_scale_factor_v.empty()) {
      auto scale_factor = cfg.get<float>("ScaleFactor", 1.);
      _scale_factor_v.resize(_cluster3d_producer_v.size(), scale_factor);
    } else if (_scale_factor_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "ScaleFactorList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _pool_type_v = cfg.get<std::vector<unsigned short> >("PoolTypeList", _pool_type_v);
    if (_pool_type_v.empty()) {
      auto pool_type = cfg.get<unsigned short>("PoolType", (unsigned short)kSumPool);
      _pool_type_v.resize(_cluster3d_producer_v.size(), pool_type);
    } else if (_pool_type_v.size() != _cluster3d_producer_v.size()) {
      LARCV_CRITICAL() << "PoolTypeList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

  }

  void Cluster3DCompressor::initialize()
  {}

  bool Cluster3DCompressor::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _cluster3d_producer_v.size(); ++producer_index) {

      auto const& cluster3d_producer = _cluster3d_producer_v[producer_index];
      auto output_producer = _output_producer_v[producer_index];
      if (output_producer.empty()) output_producer = cluster3d_producer + "_compressed";

      auto const& ev_cluster3d = mgr.get_data<larcv::EventClusterVoxel3D>(cluster3d_producer);
      auto& ev_output = mgr.get_data<larcv::EventClusterVoxel3D>(output_producer);

      if (ev_output.meta().valid()) {
        static bool one_time_warning = true;
        if (_output_producer_v[producer_index].empty()) {
          LARCV_CRITICAL() << "Over-writing existing EventSparseTensor3D data for label "
                           << output_producer << std::endl;
          throw larbys();
        }
        else if (one_time_warning) {
          LARCV_WARNING() << "Output EventSparseTensor3D producer " << output_producer
                          << " already holding valid data will be over-written!" << std::endl;
          one_time_warning = false;
        }
      }

      auto pool_type = (PoolType_t)(_pool_type_v[producer_index]);
      auto const& scale_factor = _scale_factor_v[producer_index];
      auto const& comp_factor  = _comp_factor_v[producer_index];

      auto const& orig_meta = ev_cluster3d.meta();
      Voxel3DMeta meta;
      meta.set(orig_meta.min_x(), orig_meta.min_y(), orig_meta.min_z(),
               orig_meta.max_x(), orig_meta.max_y(), orig_meta.max_z(),
               orig_meta.num_voxel_x() / comp_factor,
               orig_meta.num_voxel_y() / comp_factor,
               orig_meta.num_voxel_z() / comp_factor,
               orig_meta.unit());

      VoxelSetArray vsa;
      vsa.resize(ev_cluster3d.as_vector().size());

      for (auto const& cluster : ev_cluster3d.as_vector()) {

        auto& out_cluster = vsa.writeable_voxel_set(cluster.id());

        for (auto const& in_vox : cluster.as_vector()) {
          auto id = meta.id(orig_meta.position(in_vox.id()));

          switch (pool_type) {

          case kSumPool: {
              out_cluster.emplace(id, in_vox.value() * scale_factor, true);
              break;
            }
          case kMaxPool: {
              auto const& out_vox = out_cluster.find(id);
              if (out_vox.id() == kINVALID_VOXELID) {
                out_cluster.emplace(id, in_vox.value() * scale_factor, true);
                break;
              } else if (out_vox.value() < (in_vox.value() * scale_factor)) {
                out_cluster.emplace(id, in_vox.value() * scale_factor, false);
                break;
              }
              break;
            }
          default: {
              LARCV_CRITICAL() << "PoolType_t " << (unsigned short)(pool_type) << " not supported!" << std::endl;
              throw larbys();
            }
          }
        }
      }
      LARCV_INFO() << "EventClusterVoxel3D " << cluster3d_producer << " compressed to " << output_producer << std::endl
                   << "Original meta:" << std::endl << ev_cluster3d.meta().dump() << std::endl
                   << "New meta:" << std::endl << meta.dump() << std::endl;
      ev_output.emplace(std::move(vsa), meta);
    }
    return true;
  }

  void Cluster3DCompressor::finalize()
  {}

}
#endif
