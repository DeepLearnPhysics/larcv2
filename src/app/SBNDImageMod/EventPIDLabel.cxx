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
  _output_neutrino_id_producer = cfg.get<std::string>("OutputNeutrinoIDProducer", "neutID");
  _output_proton_id_producer   = cfg.get<std::string>("OutputProtonIDProducer", "protID");
  _output_chrpion_id_producer  = cfg.get<std::string>("OutputChargedPionIDProducer", "cpiID");
  _output_ntrpion_id_producer  = cfg.get<std::string>("OutputNeutralPionIDProducer", "npiID");


  _particle_producer        = cfg.get<std::string>("ParticleProducer");
  _neutrino_producer        = cfg.get<std::string>("NeutrinoProducer");

  _proton_threshold         = cfg.get<float>("ProtonThreshold");
  _charged_pion_threshold   = cfg.get<float>("ChargedPionThreshold");
  _neutral_pion_threshold   = cfg.get<float>("NeutralPionThreshold");
}

void EventPIDLabel::initialize() {}

bool EventPIDLabel::process(IOManager& mgr) {
  // std::cout << "Enter EventPIDLabel::process " << std::endl;

  // // Read in the particles that define the pdg types:
  // auto const& ev_particle =
  //     mgr.get_data<larcv::EventParticle>(_particle_producer);

  // Read in the neutrino info:
  auto const& ev_neutrino =
      mgr.get_data<larcv::EventParticle>(_neutrino_producer);

  // std::cout << "particle producer: " << _particle_producer << std::endl;
  // std::cout << "Number of particle particles: " << ev_neutrino.as_vector().size() << std::endl;

  // std::cout << "Neutrino producer: " << _neutrino_producer << std::endl;
  // std::cout << "Number of neutrino particles: " << ev_neutrino.as_vector().size() << std::endl;



  std::vector<int> primary_pdgs;

  // // std::cout << "Number of input particles: " << ev_neutrino.as_vector().size() << std::endl;
  // // std::cout << "Input info: " << ev_neutrino.event_key() << std::endl;
  // // std::cout << "Number of particles in output already: " << ev_particle_output.as_vector().size() << std::endl;
  // // std::cout << "Output info: " << ev_particle_output.event_key() << std::endl;
  // for (auto& particle : ev_neutrino.as_vector()) {

  //   // Primary?
  //   if (particle.ancestor_track_id() == particle.track_id()){
  //     // Yes, it's a primary
  //     primary_pdgs.push_back(particle.pdg_code());
  //   }

  // }

  // Now, need to count and discriminate the final state particles
  // Need to find:
  //  Total reaction type (nueCC, numuCC, NC)
  //  Number of protons over threshold (0, 1, 2+)
  //  Number of charged pions over threshold (0, 1+)
  //  Number of neutral pions over threshold (0, 1+)

  // Get the neutrino interaction information:
  auto neut = ev_neutrino.as_vector().front();
  interaction_type_t _int_type;

  if (neut.nu_current_type() == 1){
    _int_type = kNC;
  }
  else{
    if (abs(neut.pdg_code()) == 12){
      _int_type = kNueCC;
    }
    else{
      _int_type = kNumuCC;
    }
  }

  int proton_count = 0;
  int charged_pion_count = 0;
  int neutral_pion_count = 0;

  // Loop over the final state particles to count the
  // occurences of each type:
  for (size_t i = 1; i < ev_neutrino.as_vector().size(); i++ ){
    auto & particle = ev_neutrino.as_vector().at(i);
    // std::cout << "Particle with pdg " << particle.pdg_code()
    //           << " and energy " << particle.energy_init() << std::endl;
    switch (particle.pdg_code() ){
      case 2212: // proton
        if (particle.energy_init() - 0.93827231 > _proton_threshold)
          proton_count ++;
        break;
      case 211:  // pi plus
        if (particle.energy_init() - 0.139570   > _charged_pion_threshold)
          charged_pion_count ++;
        break;
      case -211: // pi minus
        if (particle.energy_init() - 0.139570   > _charged_pion_threshold)
          charged_pion_count ++;
        break;
      case 111: //pi0
        if (particle.energy_init() - 0.1349766  > _neutral_pion_threshold)
          neutral_pion_count ++;
        break;
    }
  }
  proton_counter_t pc;
  charged_pion_counter_t cpc;
  neutral_pion_counter_t npc;

  // Determine final proton categories:
  if (proton_count == 0){
    pc = kZeroProtons;
  }
  else if (proton_count == 1){
    pc = kOneProton;
  }
  else{
    pc = kNProtons;
  }


  // Determine final charged pion categories:
  if (charged_pion_count == 0){
    cpc = kZeroChargedPions;
  }
  else{
    cpc = kNChargedPions;
  }

  // Determine final neutral pion categories:
  if (neutral_pion_count == 0){
    npc = kZeroNeutralPions;
  }
  else{
    npc = kNNeutralPions;
  }

  // Now, enumerate into the final categories.
  // There are a total of
  // kNEventCategories*kNProtonCategories*kNChargedPionCategories*kNNeutralPionCategories
  // categories.  The index is set exclusively, in an 'unraveled' way from the above categories

  int final_index = npc
                  + kNNeutralPionCategories * cpc
                  + (kNNeutralPionCategories * kNChargedPionCategories) * pc
                  + (kNNeutralPionCategories
                   * kNChargedPionCategories
                   * kNProtonCategories) * _int_type;


  auto& ev_particle_neutrino_output = mgr.get_data<larcv::EventParticle>(_output_neutrino_id_producer);
  auto& ev_particle_proton_output   = mgr.get_data<larcv::EventParticle>(_output_proton_id_producer);
  auto& ev_particle_chrpion_output  = mgr.get_data<larcv::EventParticle>(_output_chrpion_id_producer);
  auto& ev_particle_ntrpion_output  = mgr.get_data<larcv::EventParticle>(_output_ntrpion_id_producer);
  auto& ev_particle_all_output      = mgr.get_data<larcv::EventParticle>("all");

  ev_particle_neutrino_output.clear();
  ev_particle_proton_output.clear();
  ev_particle_chrpion_output.clear();
  ev_particle_ntrpion_output.clear();
  ev_particle_all_output.clear();

  ev_particle_neutrino_output.set_id(ev_neutrino.run(), ev_neutrino.subrun(), ev_neutrino.event());
  ev_particle_proton_output.set_id(ev_neutrino.run(), ev_neutrino.subrun(), ev_neutrino.event());
  ev_particle_chrpion_output.set_id(ev_neutrino.run(), ev_neutrino.subrun(), ev_neutrino.event());
  ev_particle_ntrpion_output.set_id(ev_neutrino.run(), ev_neutrino.subrun(), ev_neutrino.event());
  ev_particle_all_output.set_id(ev_neutrino.run(), ev_neutrino.subrun(), ev_neutrino.event());



  larcv::Particle _output_neutrino_part;
  _output_neutrino_part.pdg_code(_int_type);
  ev_particle_neutrino_output.append(_output_neutrino_part);

  larcv::Particle _output_proton_part;
  _output_proton_part.pdg_code(pc);
  ev_particle_proton_output.append(_output_proton_part);

  larcv::Particle _output_chrpion_part;
  _output_chrpion_part.pdg_code(cpc);
  ev_particle_chrpion_output.append(_output_chrpion_part);

  larcv::Particle _output_ntrpion_part;
  _output_ntrpion_part.pdg_code(npc);
  ev_particle_ntrpion_output.append(_output_ntrpion_part);

  larcv::Particle _output_all_part;
  _output_all_part.pdg_code(final_index);
  ev_particle_all_output.append(_output_all_part);



  // std::cout << "Category: " << _int_type
  //           << "\t Np: "   << proton_count
  //           << "\t Npi: "  << charged_pion_count
  //           << "\t Npi0: " << neutral_pion_count << std::endl;



  // std::cout << "Exit EventPIDLabel::process " << std::endl;

  return true;
}


void EventPIDLabel::finalize() {}
}
#endif
