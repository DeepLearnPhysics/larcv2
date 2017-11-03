#ifndef __BatchFillerMultiLabel_CXX__
#define __BatchFillerMultiLabel_CXX__

#include "BatchFillerMultiLabel.h"
#include <random>
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"

namespace larcv {

static BatchFillerMultiLabelProcessFactory
    __global_BatchFillerMultiLabelProcessFactory__;

BatchFillerMultiLabel::BatchFillerMultiLabel(const std::string name)
    : BatchFillerTemplate<float>(name) {}

void BatchFillerMultiLabel::configure(const PSet& cfg) {
  _part_producer = cfg.get<std::string>("ParticleProducer");

  _pdg_list = cfg.get<std::vector<int> >("PdgClassList");

  if (_pdg_list.empty()) {
    LARCV_CRITICAL() << "PdgClassList needed to define classes!" << std::endl;
    throw larbys();
  }
  _num_class = _pdg_list.size();
}

void BatchFillerMultiLabel::initialize() {}

void BatchFillerMultiLabel::_batch_begin_() {}

void BatchFillerMultiLabel::_batch_end_() {
  if (logger().level() <= msg::kINFO) {
    LARCV_INFO() << "Total data size: " << batch_data().data_size()
                 << std::endl;

    std::vector<size_t> ctr_v;
    for (auto const& v : batch_data().data()) {
      if (v >= ctr_v.size()) ctr_v.resize(v + 1, 0);
      ctr_v[v] += 1;
    }
    std::stringstream ss;
    ss << "Used: ";
    for (size_t i = 0; i < ctr_v.size(); ++i)
      ss << ctr_v[i] << " of class " << i << " ... ";
    LARCV_INFO() << ss.str() << std::endl;
  }
}

void BatchFillerMultiLabel::finalize() {}

bool BatchFillerMultiLabel::process(IOManager& mgr) {
  auto const& event_part = mgr.get_data<larcv::EventParticle>(_part_producer);

  if (batch_data().dim().empty()) {
    std::vector<int> dim(2);
    dim[0] = batch_size();
    dim[1] = _num_class;
    set_dim(dim);
  }

  // labels
  auto const& part_v = event_part.as_vector();

  // Prepare output:
  _entry_data.resize(_num_class, 0);
  for (auto& v : _entry_data) v = 0;  

  // Check presence of each class:
  for (auto const& part : part_v) {
    std::cout << part.pdg_code() << std::endl;
    std::cout << "pdg size: " << _pdg_list.size() << std::endl;
    for (size_t class_idx = 0; class_idx < _pdg_list.size(); ++class_idx) {
      std::cout << "pdg_list " << _pdg_list[class_idx] << std::endl;
      // if (part.pdg_code() != _pdg_list[class_idx]) continue;
      if (part.pdg_code() == _pdg_list[class_idx]) {
        _entry_data[class_idx] = 1;
      }
    }
    std::cout << "part loop" << std::endl;
  }
  std::cout << std::endl;

  for (auto entry : _entry_data){
    std::cout << entry << std::endl;
  }

  set_entry_data(_entry_data);

  return true;
}
}
#endif
