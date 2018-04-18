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

  ev_particle_output.set_id(ev_particle.run(), ev_particle.subrun(), ev_particle.event());

  std::vector<int> primary_pdgs;

  // std::cout << "Number of input particles: " << ev_particle.as_vector().size() << std::endl;
  // std::cout << "Input info: " << ev_particle.event_key() << std::endl;
  // std::cout << "Number of particles in output already: " << ev_particle_output.as_vector().size() << std::endl;
  // std::cout << "Output info: " << ev_particle_output.event_key() << std::endl;
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
      final_pdg = 14;
      break;
    }
    if (label ==  11){
      final_pdg = 12;
      break;
    }
    if (label ==  -12){
      final_pdg = 12;
      break;
    }
    if (label ==  -14){
      final_pdg = 14;
      break;
    }
    if (label ==  -13){
      final_pdg = 14;
      break;
    }
    if (label ==  -11){
      final_pdg = 12;
      break;
    }

  }

  larcv::Particle _output_part;
  _output_part.pdg_code(final_pdg);
  ev_particle_output.append(_output_part);
  // std::cout << "Number of output particles: " << ev_particle_output.as_vector().size() << std::endl;


  return true;
}


void EventPIDLabel::finalize() {}
}
#endif
