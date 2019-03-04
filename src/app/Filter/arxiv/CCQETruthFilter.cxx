#ifndef __CCQETRUTHFILTER_CXX__
#define __CCQETRUTHFILTER_CXX__

#include "CCQETruthFilter.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/ParticleMass.h"
namespace larcv {

  static CCQETruthFilterProcessFactory __global_CCQETruthFilterProcessFactory__;

  CCQETruthFilter::CCQETruthFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void CCQETruthFilter::configure(const PSet& cfg)
  {
    _roi_producer  = cfg.get<std::string>("ROIProducer");
    _nu_energy_max = cfg.get<double>("NuEnergyMax");
    _nu_energy_min = cfg.get<double>("NuEnergyMin");
  }

  void CCQETruthFilter::initialize()
  {
    _tree = new TTree("tree","");
    _tree->Branch("pdg",&_pdg,"pdg/I");
    _tree->Branch("energy",&_energy,"energy/D");
    _tree->Branch("x",&_x,"x/D");
    _tree->Branch("y",&_y,"y/D");
    _tree->Branch("z",&_z,"z/D");
    _tree->Branch("itype",&_type,"itype/I");
    _tree->Branch("energy_init",&_energy_init,"energy_init/D");
    _tree->Branch("energy_deposit",&_energy_deposit,"energy_deposit/D");
  }

  bool CCQETruthFilter::process(IOManager& mgr)
  {

    auto const evroi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
    bool keep=false;
    _pdg = 0;
    _energy = 0;
    _x = _y = _z = -1e20;
    _type = -1;
    _energy_init = _energy_deposit = 0;
    bool nu_found=false;
    std::stringstream tmp_ss;
    for(auto const& roi : evroi->ROIArray()) {
      tmp_ss <<"Pdg: " << roi.PdgCode() << " track id " << roi.TrackID() << " parent pdg " << roi.ParentPdgCode() << " parent track id " << roi.ParentTrackID() 
	     << " energy init " << roi.EnergyInit() << " energy deposit " << roi.EnergyDeposit()
	     << " interaction " << roi.NuInteractionType()
	     << " ("<<roi.X()<<","<<roi.Y()<<","<<roi.Z()<<")"<<std::endl;


      if(roi.MCSTIndex() != kINVALID_INDEX) {
	_energy_init += roi.EnergyInit();
	if(roi.PdgCode() != 111)
	  _energy_init -= ParticleMass(roi.PdgCode());
	_energy_deposit += roi.EnergyDeposit();
	continue;
      }

      if(roi.PdgCode() != 12 && roi.PdgCode() != 14 && roi.PdgCode() != -12 && roi.PdgCode() != -14) continue;
      if(nu_found) {
	LARCV_WARNING() << "2nd nu found..." << std::endl << tmp_ss.str() << std::endl;
	return false;
      }	
      _x = roi.X();
      _y = roi.Y();
      _z = roi.Z();
      _energy = roi.EnergyInit();
      _pdg = roi.PdgCode();
      _type = roi.NuInteractionType();
      nu_found = true;

      if(roi.EnergyInit() < _nu_energy_min || roi.EnergyInit() > _nu_energy_max) continue;
      if(roi.NuInteractionType() != 1001) continue;
      if( roi.X() < 5.     || roi.X() > 251.35 ) continue;
      if( roi.Y() < -111.5 || roi.Y() > 111.5  ) continue;
      if( roi.Z() < 5.     || roi.Z() > 1031.8 ) continue;

      keep=true;
    }
    if(_energy_deposit/_energy_init > 1.01)
      LARCV_WARNING() << "Energy deposit/init > 1.01..." << std::endl << tmp_ss.str() << std::endl;
    /*
    if(keep) std::cout<<"Kept!"<<std::endl;
    else std::cout<<"... skipped" << std::endl;
    */
    _tree->Fill();
    return keep;
  }

  void CCQETruthFilter::finalize()
  {
    if(this->has_ana_file()) {
      this->ana_file().cd();
      _tree->Write();
    }
  }

}
#endif
