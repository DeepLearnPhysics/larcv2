#ifndef __BatchFillerPDecay_CXX__
#define __BatchFillerPDecay_CXX__

#include "BatchFillerPDecay.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include <random>

namespace larcv {

  static BatchFillerPDecayProcessFactory __global_BatchFillerPDecayProcessFactory__;

  BatchFillerPDecay::BatchFillerPDecay(const std::string name)
    : BatchFillerTemplate<float>(name)
  {}

  void BatchFillerPDecay::configure(const PSet& cfg)
  {
    _part_producer = cfg.get<std::string>("ParticleProducer");
    _num_class = 5;
  }

  void BatchFillerPDecay::initialize()
  {}

  void BatchFillerPDecay::_batch_begin_()
  {
    if (!batch_data().dim().empty() && (int)(batch_size()) != batch_data().dim().front()) {
      auto dim = batch_data().dim();
      dim[0] = batch_size();
      this->set_dim(dim);
    }
  }

  void BatchFillerPDecay::_batch_end_()
  {
    if (logger().level() <= msg::kINFO) {
      LARCV_INFO() << "Total data size: " << batch_data().data_size() << std::endl;

      std::vector<size_t> ctr_v(_num_class, 0);
      auto const& data = batch_data().data();
      for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] < 1.) continue;
        ctr_v[i % _num_class] += 1;
      }
      std::stringstream ss;
      ss << "Class fractions (0";
      for (size_t i = 1; i < _num_class; ++i) ss << "," << i;
      ss << ") ... (" << ctr_v[0];
      for (size_t i = 1; i < _num_class; ++i) ss << "," << ctr_v[i];
      ss << ")";
      LARCV_INFO() << ss.str() << std::endl;
    }
  }

  void BatchFillerPDecay::finalize()
  {}

  bool BatchFillerPDecay::process(IOManager & mgr)
  {
    auto const& event_part = mgr.get_data<larcv::EventParticle>(_part_producer);

    if (batch_data().dim().empty()) {
      std::vector<int> dim(2);
      dim[0] = batch_size();
      dim[1] = _num_class;
      set_dim(dim);
    }

    // labels
    auto const& part_v = event_part.as_vector();
    /*
    if (part_v.size() != 1) {
      LARCV_CRITICAL() << "Only support single particle label now: EventParticle size != 1" << std::endl;
      throw larbys();
    }
    */

    size_t label = kINVALID_SIZE;
    // Check if this is not kaon decay
    for (auto const& part : part_v) {
      if(part.parent_track_id() != part.track_id()) continue;
      if(part.pdg_code() != 321) label = 0;
      break;
    }
    if(label == kINVALID_SIZE) {
      // Now categorize into 3 cases
      // 1) mu+ ... mono-energetic muon ... 63.55%
      // 2) pi/mu + pi0 (+pi0) ... track + shower ... 20.66 + 3.353 + 1.761 = 25.774%
      // 3) pi + pi + pi ... three tracks ... 5.59%
      // 4) e + pi0 ... three shower ... 5.07%
      int positron_ctr = 0;
      int pion_ctr = 0;
      int piminus_ctr = 0;
      int neutral_pion_ctr = 0;
      int muon_ctr = 0;
      // First, count neutral pions by counting decay gamma
      int gamma_ctr = 0;
      for(auto const& part : part_v) {
	if(part.parent_pdg_code() != 111) continue;
	if(part.pdg_code() != 22) continue;
	++gamma_ctr;
      }
      if(gamma_ctr > 2) neutral_pion_ctr = 2;
      else if(gamma_ctr > 0) neutral_pion_ctr = 1;
      // Second, count others
      for(auto const& part : part_v) {
	if(part.parent_pdg_code() != 321) continue;
	if(part.track_id() == part.parent_track_id()) continue;
	if(part.pdg_code() == -11) ++positron_ctr;
	else if(part.pdg_code() == -13) ++muon_ctr;
	else if(abs(part.pdg_code()) == 211) ++pion_ctr;
	if(part.pdg_code() == -211) ++piminus_ctr;
      }
      // Assign label
      if      (positron_ctr == 0 && pion_ctr == 0 && muon_ctr == 1 && neutral_pion_ctr == 0)
	label = 1;
      else if (positron_ctr == 0 && pion_ctr == 1 && muon_ctr == 0 && neutral_pion_ctr >  0)
	label = 2;
      else if (positron_ctr == 0 && pion_ctr == 0 && muon_ctr == 1 && neutral_pion_ctr >  0)
	label = 2;
      else if (positron_ctr == 0 && pion_ctr == 3 && muon_ctr == 0 && neutral_pion_ctr == 0)
	label = 3;
      else if (positron_ctr == 1 && pion_ctr == 0 && muon_ctr == 0 && neutral_pion_ctr >  0)
	label = 4;

      // In case some particles are missing, key only on unique particle count
      if(label == kINVALID_SIZE) {
	LARCV_WARNING() << "Keying on partial particle info (should be rare)..." << std::endl;
	if      (positron_ctr == 1) label = 4;
	else if (neutral_pion_ctr > 0) label = 2;
	else if (piminus_ctr > 0) label = 3;
      }
      if(label == kINVALID_SIZE) {
	LARCV_CRITICAL() << "Cannot classify this event! " << event_part.event_key() << std::endl;
	throw larbys();
      }
    }

    _entry_data.resize(_num_class, 0);
    for (auto& v : _entry_data) v = 0;
    _entry_data.at(label) = 1.;
    set_entry_data(_entry_data);

    return true;
  }

}
#endif
