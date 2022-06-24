#ifndef __RescaleChargeTensor3D_CXX__
#define __RescaleChargeTensor3D_CXX__

#include "RescaleChargeTensor3D.h"
#include "larcv/core/DataFormat/EventVoxel3D.h"
namespace larcv {

  static RescaleChargeTensor3DProcessFactory __global_RescaleChargeTensor3DProcessFactory__;

  RescaleChargeTensor3D::RescaleChargeTensor3D(const std::string name)
    : ProcessBase(name)
  {}

  void RescaleChargeTensor3D::configure_labels(const PSet& cfg)
  {
    _hit_charge_producer_v.clear();
    _hit_key_producer_v.clear();
    _hit_charge_producer_v   = cfg.get<std::vector<std::string> >("HitChargeProducerList", _hit_charge_producer_v );
    _hit_key_producer_v      = cfg.get<std::vector<std::string> >("HitKeyProducerList",    _hit_key_producer_v );
    auto _reference_producer = cfg.get<std::string>("ReferenceProducer", "" );
    auto _output_producer    = cfg.get<std::string>("OutputProducer", "" );

    if (_hit_charge_producer_v.size() != 3) {
      throw larbys();
      LARCV_CRITICAL() << "HitChargeProducerList must contain 3 producers (one per plane)" << std::endl;
    }
    if (_hit_key_producer_v.size() != 3) {
      LARCV_CRITICAL() << "HitKeyProducerList must contain 3 producers (one per plane)" << std::endl;
      throw larbys();
    }
    if (_reference_producer.empty()) {
      LARCV_CRITICAL() << "ReferenceProducer must be specified" << std::endl;
      throw larbys();
    }
    if (_output_producer.empty()) {
      LARCV_CRITICAL() << "OutputProducer must be specified" << std::endl;
      throw larbys();
    }
  }

  void RescaleChargeTensor3D::configure(const PSet& cfg)
  {
    configure_labels(cfg);
  }

  void RescaleChargeTensor3D::initialize()
  {}

  bool RescaleChargeTensor3D::process(IOManager& mgr)
  {
    // Check if the output tensor already exist, warn
    auto& ev_output    = mgr.get_data<larcv::EventSparseTensor3D>(_output_producer);
    if (ev_output.meta().valid()) {
      LARCV_WARNING() << "Output EventSparseTensor3D producer " << _output_producer
                      << " already holding valid data will be over-written!" << std::endl;
    }
    auto const& ev_ref = mgr.get_data<larcv::EventSparseTensor3D>(_reference_producer);

    // Load the hit tensors
    std::vector<larcv::EventSparseTensor3D> ev_hit_charges;
    std::vector<larcv::EventSparseTensor3D> ev_hit_keys;
    for (size_t plane_index = 0; plane_index < 3; ++plane_index) {
      auto const& hit_charge_producer = _hit_charge_producer_v[plane_index];
      auto const& hit_key_producer    = _hit_key_producer_v[plane_index];
      ev_hit_charges.push_back( mgr.get_data<larcv::EventSparseTensor3D>(hit_charge_producer) );
      ev_hit_keys.push_back(    mgr.get_data<larcv::EventSparseTensor3D>(hit_key_producer) );
    }

    // Count how many times each hit key appears in the reference tensor
    std::map<size_t, size_t> hit_counts;
    for(auto const& ref_vox : ev_ref.as_vector()) {
      for (size_t plane_index = 0; plane_index < 3; ++plane_index) {
        auto const& vox = ev_hit_keys[plane_index].find(ref_vox.id());
        if ( hit_counts.find(vox.value()) == hit_counts.end() ) {
          hit_counts[vox.value()] = 1;
        } else {
          hit_counts[vox.value()] += 1;
        }
      }
    }

    // Create a new voxel set, store the rescaled charge for each of them
    larcv::VoxelSet vs;
    for(auto const& ref_vox : ev_ref.as_vector()) {
      double value    = 0.;
      size_t n_planes = 0;
      for (size_t plane_index = 0; plane_index < 3; ++plane_index) {
        auto const& hit_key_vox    = ev_hit_keys[plane_index].find(ref_vox.id());
        auto const& hit_charge_vox = ev_hit_charges[plane_index].find(ref_vox.id());
        if ( hit_key_vox.value() > -1 ) {
          value   += hit_charge_vox.value()/hit_counts[hit_key_vox.value()];
          n_planes += 1;
        }
      }
      value /= n_planes;
      vs.emplace(ref_vox.id(), value, false);
    }

    ev_output.emplace(std::move(vs), ev_hit_keys[0].meta());

    // Done
    return true;
  }

  void RescaleChargeTensor3D::finalize()
  {}

}
#endif
