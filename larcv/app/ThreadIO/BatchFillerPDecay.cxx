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
    _num_class = 2;
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
    if (part_v.size() != 1) {
      LARCV_CRITICAL() << "Only support single particle label now: EventParticle size != 1" << std::endl;
      throw larbys();
    }
    // class
    size_t label = kINVALID_SIZE;
    int pdg = 0;
    for (auto const& part : part_v) {
      pdg = part.pdg_code();
      if(pdg == 321)
	label = 0;
      else
	label = 1;
      if (label != kINVALID_SIZE) break;
    }
    LARCV_DEBUG() << "Found PDG code " << pdg << " (class=" << label << ")" << std::endl;
    _entry_data.resize(_num_class, 0);
    for (auto& v : _entry_data) v = 0;
    _entry_data.at(label) = 1.;
    set_entry_data(_entry_data);

    return true;
  }

}
#endif
