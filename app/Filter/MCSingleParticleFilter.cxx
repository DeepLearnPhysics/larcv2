#ifndef __MCSINGLEPARTICLEFILTER_CXX__
#define __MCSINGLEPARTICLEFILTER_CXX__

#include "MCSingleParticleFilter.h"
#include "DataFormat/EventParticle.h"
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
    for(auto const& part : ev_part) {
      
      if(part.MCSTIndex() == kINVALID_USHORT) continue;

      if( (part.PdgCode() == 11 || part.PdgCode() == -11 || part.PdgCode() == 22 || part.PdgCode() == 111) && 
	  part.EnergyDeposit() < _shower_min_energy) {
	LARCV_INFO() << "Ignoring Shower (PdgCode=" << part.PdgCode() << ") with energy " << part.EnergyDeposit() << std::endl;
	continue;
      }

      if(part.PdgCode() == 2212 && part.EnergyDeposit() < _proton_min_energy) {

	LARCV_INFO() << "Ignoring Proton with energy " << part.EnergyDeposit() << std::endl;
	continue;

      }else if(part.Shape() == kShapeTrack && part.EnergyDeposit() < _track_min_energy) {

	LARCV_INFO() << "Ignoring TRACK (PdgCode=" << part.PdgCode() << ") with energy " << part.EnergyDeposit() << std::endl;
	continue;

      }

      LARCV_INFO() << "Counting particle (PdgCode=" << part.PdgCode() << ") with energy " << part.EnergyDeposit() << std::endl;

      ++part_ctr;

    }
    return (part_ctr == 1);
  }

  void MCSingleParticleFilter::finalize()
  {}

}
#endif
