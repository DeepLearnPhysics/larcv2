#ifndef __MaskTensor3D_CXX__
#define __MaskTensor3D_CXX__

#include "MaskTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static MaskTensor3DProcessFactory __global_MaskTensor3DProcessFactory__;

  MaskTensor3D::MaskTensor3D(const std::string name)
    : ProcessBase(name)
  {}

  void MaskTensor3D::configure_labels(const PSet& cfg)
  {
    _target_producer_v.clear();
    _output_producer_v.clear();
    _target_producer_v    = cfg.get<std::vector<std::string> >("TargetProducerList",    _target_producer_v    );
    _output_producer_v    = cfg.get<std::vector<std::string> >("OutputProducerList",    _output_producer_v    );
    _reference_producer_v = cfg.get<std::vector<std::string> >("ReferenceProducerList", _reference_producer_v );
    if (_target_producer_v.empty()) {
      auto target_producer    = cfg.get<std::string>("Tensor3DProducer",  "");
      auto output_producer    = cfg.get<std::string>("OutputProducer",    "");
      auto reference_producer = cfg.get<std::string>("ReferenceProducer", "");
      if (!target_producer.empty()) {
        _target_producer_v.push_back(target_producer);
        _output_producer_v.push_back(output_producer);
        _reference_producer_v.push_back(reference_producer);
      }
    }

    if (_output_producer_v.empty()) {
      _output_producer_v.resize(_target_producer_v.size(), "");
    }
    else if (_output_producer_v.size() != _target_producer_v.size()) {
      LARCV_CRITICAL() << "Tensor3DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }

    if (_reference_producer_v.size() != _target_producer_v.size()) {
      LARCV_CRITICAL() << "Tensor3DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void MaskTensor3D::configure(const PSet& cfg)
  {
    configure_labels(cfg);

    _value_min_v = cfg.get<std::vector<float> >("MinVoxelValueList", _value_min_v);
    if (_value_min_v.empty()) {
      auto value_min = cfg.get<float>("MinVoxelValue", 0.);
      _value_min_v.resize(_target_producer_v.size(), value_min);
    } else if (_value_min_v.size() != _target_producer_v.size()) {
      LARCV_CRITICAL() << "MinVoxelValueList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

  }

  void MaskTensor3D::initialize()
  {}

  bool MaskTensor3D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _target_producer_v.size(); ++producer_index) {

      auto const& target_producer = _target_producer_v[producer_index];
      auto const& reference_producer = _reference_producer_v[producer_index];
      auto output_producer = _output_producer_v[producer_index];
      if (output_producer.empty()) output_producer = target_producer + "_masked";

      auto const& ev_target = mgr.get_data<larcv::EventSparseTensor3D>(target_producer);
      auto const& ev_ref = mgr.get_data<larcv::EventSparseTensor3D>(reference_producer);
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

      if(ev_ref.meta() != ev_target.meta()) {
        LARCV_CRITICAL() << "Target (" << target_producer 
        << ") vs. Reference (" << reference_producer
        << ") has incompatible meta!" << std::endl;
        throw larbys();
      }

      larcv::VoxelSet vs;
      auto const& value_min = _value_min_v[producer_index];
      for(auto const& vox : ev_target.as_vector()) {
        auto const& ref_vox = ev_ref.find(vox.id());
        if (ref_vox.id() == kINVALID_VOXELID) continue;
        if (ref_vox.value() < value_min) continue;
        vs.emplace(vox.id(), vox.value(), false);
      }

      ev_output.emplace(std::move(vs), ev_target.meta());
    }
    return true;
  }

  void MaskTensor3D::finalize()
  {}

}
#endif
