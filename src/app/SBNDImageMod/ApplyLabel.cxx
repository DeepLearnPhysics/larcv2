#ifndef __APPLYLABEL_CXX__
#define __APPLYLABEL_CXX__

#include "ApplyLabel.h"
#include "larcv/core/DataFormat/EventParticle.h"

namespace larcv {

static ApplyLabelProcessFactory
    __global_ApplyLabelProcessFactory__;

ApplyLabel::ApplyLabel(const std::string name)
    : ProcessBase(name) {}

void ApplyLabel::configure(const PSet& cfg) {
  _output_producer = cfg.get<std::string>("OutputLabel", "eventlabel");
  _output_id       = cfg.get<int>("OutputID");
}

void ApplyLabel::initialize() {}

bool ApplyLabel::process(IOManager& mgr) {

  // Read in the neutrino info:
  auto & ev_label =
      mgr.get_data<larcv::EventParticle>(_output_producer);


  larcv::Particle _output_part;
  _output_part.pdg_code(_output_id);
  ev_label.append(_output_part);


  return true;
}


void ApplyLabel::finalize() {}
}
#endif
