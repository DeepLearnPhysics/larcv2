#ifndef __ThresholdCluster3D_CXX__
#define __ThresholdCluster3D_CXX__

#include "ThresholdCluster3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static ThresholdCluster3DProcessFactory __global_ThresholdCluster3DProcessFactory__;

  ThresholdCluster3D::ThresholdCluster3D(const std::string name)
    : ProcessBase(name)
  {}

  void ThresholdCluster3D::configure_labels(const PSet& cfg)
  {
    _target_producer_v.clear();
    _output_producer_v.clear();
    _target_producer_v = cfg.get<std::vector<std::string> >("TargetProducerList", _target_producer_v);
    _output_producer_v   = cfg.get<std::vector<std::string> >("OutputProducerList", _output_producer_v);

    if (_target_producer_v.empty()) {
      auto Cluster3D_producer = cfg.get<std::string>("TargetProducer", "");
      auto output_producer   = cfg.get<std::string>("OutputProducer", "");
      if (!Cluster3D_producer.empty()) {
        _target_producer_v.push_back(Cluster3D_producer);
        _output_producer_v.push_back(output_producer);
      }
    }

    if (_output_producer_v.empty()) {
      _output_producer_v.resize(_target_producer_v.size(), "");
    }
    else if (_output_producer_v.size() != _target_producer_v.size()) {
      LARCV_CRITICAL() << "TargetProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void ThresholdCluster3D::configure(const PSet& cfg)
  {
    configure_labels(cfg);

    _voxel_value_min_v = cfg.get<std::vector<float> >("MinThresholdList", _voxel_value_min_v);
    if (_voxel_value_min_v.empty()) {
      auto voxel_value_min = cfg.get<float>("MinThreshold", 0.);
      _voxel_value_min_v.resize(_target_producer_v.size(), voxel_value_min);
    } else if (_voxel_value_min_v.size() != _target_producer_v.size()) {
      LARCV_CRITICAL() << "MinThresholdList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _voxel_value_max_v = cfg.get<std::vector<float> >("MaxThresholdList", _voxel_value_max_v);
    if (_voxel_value_max_v.empty()) {
      auto voxel_value_max = cfg.get<float>("MaxThreshold", std::numeric_limits<float>::max());
      _voxel_value_max_v.resize(_target_producer_v.size(), voxel_value_max);
    } else if (_voxel_value_max_v.size() != _target_producer_v.size()) {
      LARCV_CRITICAL() << "MaxThresholdList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
  }

  void ThresholdCluster3D::initialize()
  {}

  bool ThresholdCluster3D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _target_producer_v.size(); ++producer_index) {

      auto const& target_producer = _target_producer_v[producer_index];
      auto output_producer = _output_producer_v[producer_index];
      if (output_producer.empty()) output_producer = target_producer + "_threshold";

      auto const& ev_cluster3D = mgr.get_data<larcv::EventClusterVoxel3D>(target_producer);
      auto& ev_output = mgr.get_data<larcv::EventClusterVoxel3D>(output_producer);

      if (ev_output.meta().valid()) {
        static bool one_time_warning = true;
        if (_output_producer_v[producer_index].empty()) {
          LARCV_CRITICAL() << "Over-writing existing EventSparseCluster3D data for label "
                           << output_producer << std::endl;
          throw larbys();
        }
        else if (one_time_warning) {
          LARCV_WARNING() << "Output EventSparseCluster3D producer " << output_producer
                          << " already holding valid data will be over-written!" << std::endl;
          one_time_warning = false;
        }
      }

      ev_output = ev_cluster3D;
      auto const& voxel_value_min = _voxel_value_min_v[producer_index];
      auto const& voxel_value_max = _voxel_value_max_v[producer_index];
      ev_output.threshold(voxel_value_min, voxel_value_max);
    }
    return true;
  }

  void ThresholdCluster3D::finalize()
  {}

}
#endif