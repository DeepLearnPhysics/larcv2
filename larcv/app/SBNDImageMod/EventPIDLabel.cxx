#ifndef __EVENTPIDLABEL_CXX__
#define __EVENTPIDLABEL_CXX__

#include "EventPIDLabel.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larcv/core/DataFormat/EventParticle.h"
#include "larcv/core/DataFormat/EventVoxel2D.h"

namespace larcv {

static EventPIDLabelProcessFactory
    __global_EventPIDLabelProcessFactory__;

EventPIDLabel::EventPIDLabel(const std::string name)
    : ProcessBase(name) {}

void EventPIDLabel::configure(const PSet& cfg) {
  _output_producer = cfg.get<std::string>("OutputProducer");
  _particle_producer = cfg.get<std::string>("ParticleProducer");
}

void EventPIDLabel::initialize() {}

bool EventPIDLabel::process(IOManager& mgr) {

  // Read in the particles that define the pdg types:
  auto const& ev_particle =
      mgr.get_data<larcv::EventParticle>(_particle_producer);

  // Also output a corresponding particle 2d to match the clusters:
  auto& ev_particle_output =
      mgr.get_data<larcv::EventParticle>(_output_producer);

  std::vector<int> primary_pdgs;

  for (auto& particle : ev_particle.as_vector()) {

    // Primary?
    if (particle.ancestor_track_id() == particle.track_id()){
      // Yes, it's a primary
      primary_pdgs.push_back(particle.pdg_code());
    }

  }

  // Look over the pdgs labels for the appropriate keys:
  int final_pdg = -999;
  for (auto & label : primary_pdgs){
    if (label ==  12){
      final_pdg = 12;
      break;
    }
    if (label ==  14){
      final_pdg = 14;
      break;
    }
    if (label ==  13){
      final_pdg = 13;
      break;
    }
    if (label ==  11){
      final_pdg = 11;
      break;
    }
    if (label ==  -12){
      final_pdg = -12;
      break;
    }
    if (label ==  -14){
      final_pdg = -14;
      break;
    }
    if (label ==  -13){
      final_pdg = -13;
      break;
    }
    if (label ==  -11){
      final_pdg = -11;
      break;
    }

  }

  larcv::Particle _output_part;
  _output_part.pdg_code(final_pdg);
  ev_particle_output.append(_output_part);


  return true;
}


void EventPIDLabel::finalize() {}
}
#endif
