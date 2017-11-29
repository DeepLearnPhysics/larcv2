#ifndef __ScaleTensor_CXX__
#define __ScaleTensor_CXX__

#include "ScaleTensor.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static ScaleTensorProcessFactory __global_ScaleTensorProcessFactory__;

  ScaleTensor::ScaleTensor(const std::string name)
    : ProcessBase(name)
  {}

  void ScaleTensor::configure_labels(const PSet& cfg)
  {
    _tensor3d_producer_v.clear();
    _output3d_producer_v.clear();
    _tensor3d_producer_v = cfg.get<std::vector<std::string> >("Tensor3DProducerList", _tensor3d_producer_v);
    _output3d_producer_v = cfg.get<std::vector<std::string> >("Output3DProducerList", _output3d_producer_v);

    if (_tensor3d_producer_v.empty()) {
      auto tensor3d_producer = cfg.get<std::string>("Tensor3DProducer", "");
      auto output3d_producer = cfg.get<std::string>("Output3DProducer", "");
      if (!tensor3d_producer.empty()) {
        _tensor3d_producer_v.push_back(tensor3d_producer);
        if (output3d_producer.empty()) output3d_producer = tensor3d_producer + "_scaled3d";
        _output3d_producer_v.push_back(output3d_producer);
      }
    }

    if(_output3d_producer_v.empty()) {
      for(auto tensor3d_producer : _tensor3d_producer_v) 
        _output3d_producer_v.push_back(tensor3d_producer + "_scaled3d");
    }
    else if (_output3d_producer_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "Tensor3DProducer and Output3DProducer must have the same array length!" << std::endl;
      throw larbys();
    }

    _tensor2d_producer_v.clear();
    _output2d_producer_v.clear();
    _tensor2d_producer_v = cfg.get<std::vector<std::string> >("Tensor2DProducerList", _tensor2d_producer_v);
    _output2d_producer_v = cfg.get<std::vector<std::string> >("Output2DProducerList", _output2d_producer_v);

    if (_tensor2d_producer_v.empty()) {
      auto tensor2d_producer = cfg.get<std::string>("Tensor2DProducer", "");
      auto output2d_producer = cfg.get<std::string>("Output2DProducer", "");
      if (!tensor2d_producer.empty()) {
        _tensor2d_producer_v.push_back(tensor2d_producer);
        if (output2d_producer.empty()) output2d_producer = tensor2d_producer + "_scaled2d";
        _output2d_producer_v.push_back(output2d_producer);
      }
    }

    if(_output2d_producer_v.empty()) {
      for(auto tensor2d_producer : _tensor2d_producer_v) 
        _output2d_producer_v.push_back(tensor2d_producer + "_scaled2d");
    }
    else if (_output2d_producer_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "Tensor2DProducer and Output2DProducer must have the same array length!" << std::endl;
      throw larbys();
    }

  }

  void ScaleTensor::configure(const PSet& cfg)
  {
    configure_labels(cfg);
    _factor3d_v = cfg.get<std::vector<float> >("Factor3DList", _factor3d_v);
    if (_factor3d_v.empty()) {
      auto factor3d = cfg.get<float>("Factor3D", 1.);
      _factor3d_v.resize(_tensor3d_producer_v.size(), factor3d);
    } else if (_factor3d_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "Factor3DList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _factor2d_v = cfg.get<std::vector<float> >("Factor2DList", _factor2d_v);
    if (_factor2d_v.empty()) {
      auto factor2d = cfg.get<float>("Factor2D", 1.);
      _factor2d_v.resize(_tensor2d_producer_v.size(), factor2d);
    } else if (_factor2d_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "Factor2DList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _offset3d_v = cfg.get<std::vector<float> >("Offset3DList", _offset3d_v);
    if (_offset3d_v.empty()) {
      auto offset3d = cfg.get<float>("Offset3D", 0.);
      _offset3d_v.resize(_tensor3d_producer_v.size(), offset3d);
    } else if (_offset3d_v.size() != _tensor3d_producer_v.size()) {
      LARCV_CRITICAL() << "Offset3DList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }

    _offset2d_v = cfg.get<std::vector<float> >("Offset2DList", _offset2d_v);
    if (_offset2d_v.empty()) {
      auto offset2d = cfg.get<float>("Offset2D", 0.);
      _offset2d_v.resize(_tensor2d_producer_v.size(), offset2d);
    } else if (_offset2d_v.size() != _tensor2d_producer_v.size()) {
      LARCV_CRITICAL() << "Offset2DList size mismatch with other input parameters!" << std::endl;
      throw larbys();
    }
  }

  void ScaleTensor::initialize(){}

  bool ScaleTensor::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _tensor3d_producer_v.size(); ++producer_index) {
      auto const& tensor3d_producer = _tensor3d_producer_v[producer_index];
      auto const& output3d_producer = _output3d_producer_v[producer_index];
      auto const& factor3d = _factor3d_v[producer_index];
      auto const& offset3d = _offset3d_v[producer_index];

       LARCV_INFO() << "Processing EventSparseTensor3D producer " << tensor3d_producer
      << " ... output " << output3d_producer << std::endl;

      auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(tensor3d_producer);
      auto const& meta = ev_tensor3d.meta();
      VoxelSet vs = (VoxelSet)(ev_tensor3d);
      vs *= factor3d;
      vs += offset3d;

      auto& ev_output3d = mgr.get_data<larcv::EventSparseTensor3D>(output3d_producer);
      ev_output3d.emplace(std::move(vs),meta);
    }

    for (size_t producer_index = 0; producer_index < _tensor2d_producer_v.size(); ++producer_index) {
      auto const& tensor2d_producer = _tensor2d_producer_v[producer_index];
      auto const& output2d_producer = _output2d_producer_v[producer_index];
      auto const& factor2d = _factor2d_v[producer_index];
      auto const& offset2d = _offset2d_v[producer_index];

       LARCV_INFO() << "Processing EventSparseTensor2D producer " << tensor2d_producer
      << " ... output " << output2d_producer << std::endl;

      auto const& ev_tensor2d = mgr.get_data<larcv::EventSparseTensor2D>(tensor2d_producer);
      std::vector<larcv::SparseTensor2D> vs_v;
      for(auto const& vs : ev_tensor2d.as_vector()) {
        auto meta = vs.meta();
        auto out_vs = vs;
        out_vs *= factor2d;
        out_vs += offset2d;
        vs_v.emplace_back(std::move(out_vs),std::move(meta));
      }

      auto& ev_output2d = mgr.get_data<larcv::EventSparseTensor2D>(output2d_producer);
      for(size_t i=0; i<vs_v.size(); ++i)
        ev_output2d.emplace(std::move(vs_v[i]));
    }

    return true;
  }

  void ScaleTensor::finalize()
  {}

}
#endif
