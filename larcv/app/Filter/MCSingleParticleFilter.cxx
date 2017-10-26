#ifndef __MCSINGLEPARTICLEFILTER_CXX__
#define __MCSINGLEPARTICLEFILTER_CXX__

#include "MCSingleParticleFilter.h"
#include "larcv/core/DataFormat/EventParticle.h"
namespace larcv {

  static MCSingleParticleFilterProcessFactory __global_MCSingleParticleFilterProcessFactory__;

  MCSingleParticleFilter::MCSingleParticleFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void MCSingleParticleFilter::configure(const PSet& cfg)
  {
    _part_producer = cfg.get<std::string>("ParticleProducer");
    _shower_min_energy = cfg.get<double>("ShowerMinEnergy");
    _track_min_energy = cfg.get<double>("TrackMinEnergy");
    _proton_min_energy = cfg.get<double>("ProtonMinEnergy");
  }

  void MCSingleParticleFilter::initialize()
  {}

  bool MCSingleParticleFilter::process(IOManager& mgr)
  {
    auto const& ev_part = mgr.get_data<larcv::EventParticle>(_part_producer);
    size_t part_ctr = 0;
    for(auto const& part : ev_part.as_vector()) {
      
      if(part.mcst_index() == kINVALID_USHORT) continue;

      if( (part.pdg_code() == 11 || part.pdg_code() == -11 || part.pdg_code() == 22 || part.pdg_code() == 111) && 
	  part.energy_deposit() < _shower_min_energy) {
	LARCV_INFO() << "Ignoring Shower (pdg_code=" << part.pdg_code() << ") with energy " << part.energy_deposit() << std::endl;
	continue;
      }

      if(part.pdg_code() == 2212 && part.energy_deposit() < _proton_min_energy) {

	LARCV_INFO() << "Ignoring Proton with energy " << part.energy_deposit() << std::endl;
	continue;

      }else if(part.shape() == kShapeTrack && part.energy_deposit() < _track_min_energy) {

	LARCV_INFO() << "Ignoring TRACK (pdg_code=" << part.pdg_code() << ") with energy " << part.energy_deposit() << std::endl;
	continue;

      }

      LARCV_INFO() << "Counting particle (pdg_code=" << part.pdg_code() << ") with energy " << part.energy_deposit() << std::endl;

      ++part_ctr;

    }
    return (part_ctr == 1);
  }

  void MCSingleParticleFilter::finalize()
  {}

}
#endif
