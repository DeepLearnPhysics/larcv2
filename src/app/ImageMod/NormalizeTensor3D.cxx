#ifndef __NORMALIZETENSOR3D_CXX__
#define __NORMALIZETENSOR3D_CXX__

#include "NormalizeTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static NormalizeTensor3DProcessFactory __global_NormalizeTensor3DProcessFactory__;

  NormalizeTensor3D::NormalizeTensor3D(const std::string name)
    : ProcessBase(name)
  {}

  void NormalizeTensor3D::configure_labels(const PSet& cfg)
  {
    _tensor3d_producer_v.clear();
    _output_producer_v.clear();
    _tensor3d_producer_v = cfg.get<std::vector<std::string> >("Tensor3DProducerList", _tensor3d_producer_v);
    _output_producer_v    = cfg.get<std::vector<std::string> >("OutputProducerList", _output_producer_v);

    if (_tensor3d_producer_v.empty()) {
      auto tensor3d_producer = cfg.get<std::string>("Tensor3DProducer", "");
      auto output_producer    = cfg.get<std::string>("OutputProducer", "");
      if (!tensor3d_producer.empty()) {
        _tensor3d_producer_v.push_back(tensor3d_producer);
        _output_producer_v.push_back(output_producer);
      }
    }

    if (_output_producer_v.empty()) {
      _output_producer_v.resize(_tensor3d_producer_v.size(), "");
    }
    else if (_output_producer_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "Tensor3DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void NormalizeTensor3D::configure(const PSet& cfg)
  {
    configure_labels(cfg);

    _norm_v.clear();
    _norm_type_v.clear();

    _norm_v = cfg.get<std::vector<float> >("NormalizationList", _norm_v);
    if (_norm_v.empty()) {
      auto norm = cfg.get<float>("Normalization", -1.);
      _norm_v.resize(_tensor3d_producer_v.size(), norm);
    } else if (_norm_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "NormalizationList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _norm_type_v = cfg.get<std::vector<unsigned short> >("NormTypeList", _norm_type_v);
    if (_norm_type_v.empty()) {
      auto norm_type = cfg.get<unsigned short>("NormType", (unsigned short)kNormalizeByArea);
      _norm_type_v.resize(_tensor3d_producer_v.size(), norm_type);
    } else if (_norm_type_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "NormTypeList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
  }

  void NormalizeTensor3D::initialize()
  {}

  bool NormalizeTensor3D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _tensor3d_producer_v.size(); ++producer_index) {

      auto const& tensor3d_producer = _tensor3d_producer_v[producer_index];
      auto output_producer = _output_producer_v[producer_index];
      if (output_producer.empty()) output_producer = tensor3d_producer + "_normalized";

      auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(tensor3d_producer);
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

      float norm = _norm_v[producer_index];
      NormType_t norm_type = (NormType_t)(_norm_type_v[producer_index]);
      if (norm < 0.) {
        if (norm_type == kNormalizeByArea) {
          norm = 0.;
          for (auto const& vox : ev_tensor3d.as_vector()) {norm += vox.value();}
        }
        if (norm_type == kNormalizeByAmplitude) {
          norm = 0.;
          for (auto const& vox : ev_tensor3d.as_vector()) { if (norm < vox.value()) norm = vox.value(); }
        }
      }
      larcv::VoxelSet vs;
      for (auto const& vox : ev_tensor3d.as_vector())
        vs.emplace(vox.id(), vox.value() / norm, false);

      auto const meta = ev_tensor3d.meta();
      ev_output.emplace(std::move(vs), meta);

      LARCV_INFO() << "EventSparseTensor3D " << tensor3d_producer << " thresholded to " << output_producer << std::endl
                   << "Original meta:" << std::endl << meta.dump() << std::endl
                   << "New meta:" << std::endl << ev_output.meta().dump() << std::endl;

    }
    return true;
  }

  void NormalizeTensor3D::finalize()
  {}

}
#endif
