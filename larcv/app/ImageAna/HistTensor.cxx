#ifndef __HistTensor_CXX__
#define __HistTensor_CXX__

#include "HistTensor.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static HistTensorProcessFactory __global_HistTensorProcessFactory__;

  HistTensor::HistTensor(const std::string name)
    : ProcessBase(name)
  {}

  void HistTensor::configure(const PSet& cfg)
  {
    _tensor3d_producer_v.clear();
    _tensor2d_producer_v.clear();

    _tensor3d_producer_v = cfg.get<std::vector<std::string> >("Tensor3DProducerList", _tensor3d_producer_v);
    _tensor2d_producer_v = cfg.get<std::vector<std::string> >("Tensor2DProducerList", _tensor3d_producer_v);

    if (_tensor3d_producer_v.empty()) {
      auto tensor3d_producer = cfg.get<std::string>("Tensor3DProducer", "");
      if (!tensor3d_producer.empty()) {
        _tensor3d_producer_v.push_back(tensor3d_producer);
      }
    }

    if (_tensor2d_producer_v.empty()) {
      auto tensor2d_producer = cfg.get<std::string>("Tensor2DProducer", "");
      if (!tensor2d_producer.empty()) {
        _tensor2d_producer_v.push_back(tensor2d_producer);
      }
    }
  }

  void HistTensor::initialize()
  {
    _tree_v.clear();
    for (auto producer : _tensor3d_producer_v) {
      producer = producer + "_3d";
      auto tree = new TTree(producer.c_str(), "");
      tree->Branch("x", &_x, "x/F");
      tree->Branch("y", &_y, "y/F");
      tree->Branch("z", &_z, "z/F");
      tree->Branch("v", &_v, "v/F");
      _tree_v.push_back(tree);
    }
    for (auto producer : _tensor2d_producer_v) {
      producer = producer + "_2d";
      auto tree = new TTree(producer.c_str(), "");
      tree->Branch("x", &_x, "x/F");
      tree->Branch("y", &_y, "y/F");
      tree->Branch("v", &_v, "v/F");
      tree->Branch("projection", &_projection, "projection/I");
      _tree_v.push_back(tree);
    }
  }

  bool HistTensor::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _tensor3d_producer_v.size(); ++producer_index) {
      LARCV_INFO() << "Processing EventSparseTensor3D producer " << _tensor3d_producer_v[producer_index] << std::endl;
      auto const& producer = _tensor3d_producer_v[producer_index];
      auto tree = _tree_v[producer_index];
      auto const& ev_tensor3d = mgr.get_data<larcv::EventSparseTensor3D>(producer);
      auto const& meta = ev_tensor3d.meta();
      for (auto const& vox : ev_tensor3d.as_vector()) {
        auto const pos = meta.position(vox.id());
        _x = pos.x;
        _y = pos.y;
        _z = pos.z;
        _v = vox.value();
        tree->Fill();
      }
    }

    for (size_t producer_index = 0; producer_index < _tensor2d_producer_v.size(); ++producer_index) {
      LARCV_INFO() << "Processing EventSparseTensor2D producer " << _tensor2d_producer_v[producer_index] << std::endl;
      auto const& producer = _tensor2d_producer_v[producer_index];
      auto tree = _tree_v[producer_index+_tensor3d_producer_v.size()];
      auto const& ev_tensor2d = mgr.get_data<larcv::EventSparseTensor2D>(producer);

      for (auto const& vs : ev_tensor2d.as_vector()) {
        auto const& meta = vs.meta();
        _projection = meta.id();
        for (auto const& vox : vs.as_vector()) {
          auto const pos = meta.position(vox.id());
          _x = pos.x;
          _y = pos.y;
          _v = vox.value();
          tree->Fill();
        }
      }
    }

    return true;
  }

  void HistTensor::finalize()
  {
    if (has_ana_file()) {
      for(auto tree : _tree_v) { tree->Write(); }
    }
  }

}
#endif
