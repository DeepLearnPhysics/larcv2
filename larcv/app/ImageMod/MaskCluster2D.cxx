#ifndef __MaskCluster2D_CXX__
#define __MaskCluster2D_CXX__

#include "MaskCluster2D.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"
namespace larcv {

  static MaskCluster2DProcessFactory __global_MaskCluster2DProcessFactory__;

  MaskCluster2D::MaskCluster2D(const std::string name)
    : ProcessBase(name)
  {}

  void MaskCluster2D::configure_labels(const PSet& cfg)
  {
    _target_producer_v.clear();
    _output_producer_v.clear();
    _target_producer_v    = cfg.get<std::vector<std::string> >("TargetProducerList",    _target_producer_v    );
    _output_producer_v    = cfg.get<std::vector<std::string> >("OutputProducerList",    _output_producer_v    );
    _reference_producer_v = cfg.get<std::vector<std::string> >("ReferenceProducerList", _reference_producer_v );
    if (_target_producer_v.empty()) {
      auto target_producer    = cfg.get<std::string>("Cluster2DProducer",  "");
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
      LARCV_CRITICAL() << "Cluster2DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }

    if (_reference_producer_v.size() != _target_producer_v.size()) {
      LARCV_CRITICAL() << "Cluster2DProducer and OutputProducer must have the same array length!" << std::endl;
      throw larbys();
    }
  }

  void MaskCluster2D::configure(const PSet& cfg)
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

  void MaskCluster2D::initialize()
  {}

  bool MaskCluster2D::process(IOManager& mgr)
  {
    for (size_t producer_index = 0; producer_index < _target_producer_v.size(); ++producer_index) {

      auto const& target_producer = _target_producer_v[producer_index];
      auto const& reference_producer = _reference_producer_v[producer_index];
      auto output_producer = _output_producer_v[producer_index];
      if (output_producer.empty()) output_producer = target_producer + "_masked";

      auto const& ev_target = mgr.get_data<larcv::EventClusterPixel2D>(target_producer);
      auto const& ev_ref = mgr.get_data<larcv::EventSparseTensor2D>(reference_producer);

      std::vector<larcv::ClusterPixel2D> vsa2d_v(ev_target.as_vector().size());

      for(size_t data_idx=0; data_idx<ev_target.as_vector().size(); ++data_idx) {

	auto const& vsa_target = ev_target.as_vector()[data_idx];
	auto const& vs_ref    = ev_ref.as_vector()[data_idx];
	auto meta = vsa_target.meta();

	auto& vsa_out = vsa2d_v[data_idx];
	vsa_out.meta(meta);
	vsa_out.resize(vsa_target.as_vector().size());
	
	if(vs_ref.meta() != vsa_target.meta()) {
	  LARCV_CRITICAL() << "Target (" << target_producer 
			   << ") vs. Reference (" << reference_producer
			   << ") has incompatible meta!" << std::endl;
	  throw larbys();
	}
	
	auto const& value_min = _value_min_v[producer_index];

	for(size_t vs_idx=0; vs_idx<vsa_target.as_vector().size(); ++vs_idx) {
	  auto const& target = vsa_target.as_vector()[vs_idx];
	  auto& vs_out = vsa_out.writeable_voxel_set(vs_idx);

	  for(auto const& vox : target.as_vector()) {
	    auto const& ref_vox = vs_ref.find(vox.id());
	    if (ref_vox.id() == kINVALID_VOXELID) continue;
	    if (ref_vox.value() < value_min) continue;
	    vs_out.emplace(vox.id(), vox.value(), false);
	  }
	}
      }

      auto ev_output = (EventClusterPixel2D*)(mgr.get_data("cluster2d",output_producer));
      ev_output->clear();
      for(size_t data_idx=0; data_idx<vsa2d_v.size(); ++data_idx)
	ev_output->emplace(std::move(vsa2d_v[data_idx]));
    }
    return true;
  }

  void MaskCluster2D::finalize()
  {}

}
#endif
