#ifndef __MaskTensor2D_CXX__
#define __MaskTensor2D_CXX__

#include "MaskTensor2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
namespace larcv {

  static MaskTensor2DProcessFactory __global_MaskTensor2DProcessFactory__;

  MaskTensor2D::MaskTensor2D(const std::string name)
    : ProcessBase(name)
  {}

  void MaskTensor2D::configure_labels(const PSet& cfg)
  {
    _target_producer_v.clear();
    _output_producer_v.clear();
    _target_producer_v    = cfg.get<std::vector<std::string> >("TargetProducerList",    _target_producer_v    );
    _output_producer_v    = cfg.get<std::vector<std::string> >("OutputProducerList",    _output_producer_v    );
    _reference_producer_v = cfg.get<std::vector<std::string> >("ReferenceProducerList", _reference_producer_v );
    if (_target_producer_v.empty()) {
      auto target_producer    = cfg.get<std::string>("Tensor2DProducer",  "");
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
      LARCV_CRITICAL() << "Tensor2DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }

    if (_reference_producer_v.size() != _target_producer_v.size()) {
      LARCV_CRITICAL() << "Tensor2DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void MaskTensor2D::configure(const PSet& cfg)
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

  void MaskTensor2D::initialize()
  {}

  bool MaskTensor2D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _target_producer_v.size(); ++producer_index) {

      auto const& target_producer = _target_producer_v[producer_index];
      auto const& reference_producer = _reference_producer_v[producer_index];
      auto output_producer = _output_producer_v[producer_index];
      if (output_producer.empty()) output_producer = target_producer + "_masked";

      auto const& ev_target = mgr.get_data<larcv::EventSparseTensor2D>(target_producer);
      auto const& ev_ref = mgr.get_data<larcv::EventSparseTensor2D>(reference_producer);

      if(ev_target.as_vector().size() != ev_ref.as_vector().size()) {
	LARCV_CRITICAL() << "Target tensor2d count ("
			 << target_producer << "," << ev_target.as_vector().size()
			 << ") != reference tensor2d count (" 
			 << reference_producer << "," << ev_ref.as_vector().size() << ")" << std::endl;
	throw larcv::larbys();
      }

      std::vector<larcv::SparseTensor2D> tensor2d_v;

      for(size_t i=0; i<ev_target.as_vector().size(); ++i) {

	auto const& target_vs = ev_target.as_vector()[i];
	auto const& ref_vs    = ev_ref.as_vector()[i];

	auto target_meta = target_vs.meta();
	auto ref_meta    = ref_vs.meta();
	if(ref_meta != target_meta) {
	  LARCV_CRITICAL() << "Target (" << target_producer 
			   << ") vs. Reference (" << reference_producer
			   << ") has incompatible meta!" << std::endl;
	  throw larbys();
	}
	
	larcv::VoxelSet vs;
	auto const& value_min = _value_min_v[producer_index];
	for(auto const& vox : target_vs.as_vector()) {
	  auto const& ref_vox = ref_vs.find(vox.id());
	  if (ref_vox.id() == kINVALID_VOXELID) continue;
	  if (ref_vox.value() < value_min) continue;
	  vs.emplace(vox.id(), vox.value(), false);
	}
	larcv::ImageMeta meta(target_meta);
	larcv::SparseTensor2D tensor2d(std::move(vs),std::move(meta));
	tensor2d_v.emplace_back(std::move(tensor2d));
      }

      auto& ev_output = mgr.get_data<larcv::EventSparseTensor2D>(output_producer);
      for(size_t i=0; i<tensor2d_v.size(); ++i) 
	ev_output.emplace(std::move(tensor2d_v[i]));
    }
    return true;
  }

  void MaskTensor2D::finalize()
  {}

}
#endif
