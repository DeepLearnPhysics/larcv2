#ifndef __ThresholdTensor3D_CXX__
#define __ThresholdTensor3D_CXX__

#include "ThresholdTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static ThresholdTensor3DProcessFactory __global_ThresholdTensor3DProcessFactory__;

  ThresholdTensor3D::ThresholdTensor3D(const std::string name)
    : ProcessBase(name)
  {}

  void ThresholdTensor3D::configure_labels(const PSet& cfg)
  {
    _target_producer_v.clear();
    _output_producer_v.clear();
    _target_producer_v = cfg.get<std::vector<std::string> >("TargetProducerList", _target_producer_v);
    _output_producer_v   = cfg.get<std::vector<std::string> >("OutputProducerList", _output_producer_v);

    if (_target_producer_v.empty()) {
      auto tensor3d_producer = cfg.get<std::string>("TargetProducer", "");
      auto output_producer   = cfg.get<std::string>("OutputProducer", "");
      if (!tensor3d_producer.empty()) {
        _target_producer_v.push_back(tensor3d_producer);
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

  void ThresholdTensor3D::configure(const PSet& cfg)
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

  void ThresholdTensor3D::initialize()
  {}

  bool ThresholdTensor3D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _target_producer_v.size(); ++producer_index) {

      auto const& target_producer = _target_producer_v[producer_index];
      auto output_producer = _output_producer_v[producer_index];
      if (output_producer.empty()) output_producer = target_producer + "_threshold";

      auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(target_producer);
      auto& ev_output = mgr.get_data<larcv::EventSparseTensor3D>(output_producer);

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

      ev_output = ev_tensor3d;
      auto const& voxel_value_min = _voxel_value_min_v[producer_index];
      auto const& voxel_value_max = _voxel_value_max_v[producer_index];
      ev_output.threshold(voxel_value_min, voxel_value_max);
    }
    return true;
  }

  void ThresholdTensor3D::finalize()
  {}

}
#endif