#ifndef __EmptyTensorFilter_CXX__
#define __EmptyTensorFilter_CXX__

#include "EmptyTensorFilter.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static EmptyTensorFilterProcessFactory __global_EmptyTensorFilterProcessFactory__;

  EmptyTensorFilter::EmptyTensorFilter(const std::string name)
    : ProcessBase(name)
  {}

  void EmptyTensorFilter::configure_labels(const PSet& cfg)
  {
    _tensor2d_producer_v.clear();
    _tensor3d_producer_v.clear();

    _tensor2d_producer_v = cfg.get<std::vector<std::string> >("Tensor2DProducerList", _tensor2d_producer_v);
    _tensor3d_producer_v = cfg.get<std::vector<std::string> >("Tensor3DProducerList", _tensor3d_producer_v);

    if (_tensor2d_producer_v.empty()) {
      auto tensor2d_producer = cfg.get<std::string>("Tensor2DProducer", "");
      if (!tensor2d_producer.empty()) {
        _tensor2d_producer_v.push_back(tensor2d_producer);
      }
    }

    if (_tensor3d_producer_v.empty()) {
      auto tensor3d_producer = cfg.get<std::string>("Tensor3DProducer", "");
      if (!tensor3d_producer.empty()) {
        _tensor3d_producer_v.push_back(tensor3d_producer);
      }
    }
  }

  void EmptyTensorFilter::configure(const PSet& cfg)
  {
    configure_labels(cfg);

    _min_voxel2d_count_v = cfg.get<std::vector<size_t> >("MinVoxel2DCountList", _min_voxel2d_count_v);
    if (_min_voxel2d_count_v.empty()) {
      auto min_voxel2d_count = cfg.get<size_t>("MinVoxel2DCount", 0);
      _min_voxel2d_count_v.resize(_tensor2d_producer_v.size(), min_voxel2d_count);
    } else if (_min_voxel2d_count_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "MinVoxel2DCount size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _min_voxel2d_value_v = cfg.get<std::vector<float> >("MinVoxel2DValueList", _min_voxel2d_value_v);
    if (_min_voxel2d_value_v.empty()) {
      auto min_voxel2d_value = cfg.get<float>("MinVoxel2DValue", 0.);
      _min_voxel2d_value_v.resize(_tensor2d_producer_v.size(), min_voxel2d_value);
    } else if (_min_voxel2d_value_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "MinVoxel2DValue size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

     _min_voxel3d_count_v = cfg.get<std::vector<size_t> >("MinVoxel3DCountList", _min_voxel3d_count_v); 
    if (_min_voxel3d_count_v.empty()) {
      auto min_voxel3d_count = cfg.get<size_t>("MinVoxel3DCount", 0);
      _min_voxel3d_count_v.resize(_tensor3d_producer_v.size(), min_voxel3d_count);
    } else if (_min_voxel3d_count_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "MinVoxel3DCount size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _min_voxel3d_value_v = cfg.get<std::vector<float> >("MinVoxel3DValueList", _min_voxel3d_value_v);
    if (_min_voxel3d_value_v.empty()) {
      auto min_voxel3d_value = cfg.get<float>("MinVoxel3DValue", 0.);
      _min_voxel3d_value_v.resize(_tensor3d_producer_v.size(), min_voxel3d_value);
    } else if (_min_voxel3d_value_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "MinVoxel3DValue size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

  }

  void EmptyTensorFilter::initialize()
  {}

  bool EmptyTensorFilter::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _tensor3d_producer_v.size(); ++producer_index) {
      auto const& producer = _tensor3d_producer_v[producer_index];
      auto const& event_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(producer);
      auto const& min_voxel_value = _min_voxel3d_value_v[producer_index];
      auto const& min_voxel_count = _min_voxel3d_count_v[producer_index];
      size_t ctr = 0;
      for (auto const& vox : event_tensor3d.as_vector()) {
        if (vox.value() < min_voxel_value) continue;
        ctr++;
      }
      if (ctr < min_voxel_count) return false;
    }

    for (size_t producer_index = 0; producer_index < _tensor2d_producer_v.size(); ++producer_index) {
      auto const& producer = _tensor2d_producer_v[producer_index];
      auto const& event_tensor2d = mgr.get_data<larcv::EventSparseTensor2D>(producer);
      auto const& min_voxel_value = _min_voxel2d_value_v[producer_index];
      auto const& min_voxel_count = _min_voxel2d_count_v[producer_index];
      for (auto const& vox_set : event_tensor2d.as_vector()) {
        size_t ctr = 0;
        for (auto const& vox : vox_set.as_vector()) {
          if (vox.value() < min_voxel_value) continue;
          ctr++;
        }
        if (ctr < min_voxel_count) return false;
      }
    }
    return true;
  }

  void EmptyTensorFilter::finalize()
  {}

}
#endif
