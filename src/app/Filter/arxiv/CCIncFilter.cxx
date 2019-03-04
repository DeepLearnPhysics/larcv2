#ifndef __CCINCFILTER_CXX__
#define __CCINCFILTER_CXX__

#include "CCIncFilter.h"
#include "DataFormat/EventROI.h"
namespace larcv {

  static CCIncFilterProcessFactory __global_CCIncFilterProcessFactory__;

  CCIncFilter::CCIncFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void CCIncFilter::configure(const PSet& cfg)
  {
    _roi_producer = cfg.get<std::string>("ROIProducer");
    _shower_min_energy = cfg.get<double>("ShowerMinEnergyMeV");
    _track_min_energy = cfg.get<double>("TrackMinEnergyMeV");
    _proton_min_energy = cfg.get<double>("ProtonMinEnergyMeV");
    _fiducial_cut_cm   = cfg.get<double>("FiducialCutCM");
    if ( _fiducial_cut_cm<0 )
      _fiducial_cut_cm = 0.0;
  }

  void CCIncFilter::initialize()
  {}

  bool CCIncFilter::process(IOManager& mgr)
  {

    // get the event roi
    auto ev_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
    
    // track the number of particles that satisfy our conditions


    // goal: looking for "clean" CC-inclusive events
    // Event must have
    //  (1) muon present
    //  (2) proton or shower with minium energy deposition
    //  (3) vertex in fiducial volume

    bool has_muon = false;
    bool has_proton = false;
    bool has_shower = false;
    bool fv_vertex = false;
    //bool has_mc = false;

    for(auto const& roi : ev_roi->ROIArray()) {


      if ( roi.Type()==larcv::kROIBNB ) {
	// BNB ROI
	double x = roi.X();
	double y = roi.Y();
	double z = roi.Z();
	LARCV_INFO() << "Vertex: " << x << ", " << y << ", " << z << std::endl;
	if ( (x>(0+_fiducial_cut_cm) && x<(250-_fiducial_cut_cm) )
	     && (y!=0 && y>(-117+_fiducial_cut_cm) && y<(117-_fiducial_cut_cm))
	     && ( z>(0+_fiducial_cut_cm) && z<(1025-_fiducial_cut_cm) ) ) {
	  fv_vertex = true;
	}
      }

      // not an MC particle ROI
      if(roi.MCSTIndex() == kINVALID_USHORT) continue;

      // shower ROI
      if( roi.PdgCode() == 11 || roi.PdgCode() == -11 || roi.PdgCode() == 22 || roi.PdgCode() == 111 ) {
	if ( roi.EnergyDeposit() < _shower_min_energy ) {
	  LARCV_INFO() << "Ignoring Shower (PdgCode=" << roi.PdgCode() << ") with energy " << roi.EnergyDeposit() << std::endl;
	  continue;
	}
	else {
	  has_shower = true;
	  LARCV_INFO() << "Accepting Shower (PdgCode=" << roi.PdgCode() << ") with energy " << roi.EnergyDeposit() << std::endl;
	}
      }
      // proton ROI
      else if(roi.PdgCode() == 2212) {
	if ( roi.EnergyDeposit() < _proton_min_energy) {
	  LARCV_INFO() << "Ignoring Proton with energy " << roi.EnergyDeposit() << std::endl;
	  continue;
	}
	else {
	  has_proton = true;
	  LARCV_INFO() << "Accepting Proton with energy " << roi.EnergyDeposit() << std::endl;
	}
      }
      else if ( roi.PdgCode()==13 || roi.PdgCode()==-13 ) {
	if ( roi.EnergyDeposit()<_track_min_energy ) {
	  LARCV_INFO() << "Ignoring muon with energy " << roi.EnergyDeposit() << std::endl;
	  continue;
	}
	else {
	  has_muon = true;
	  LARCV_INFO() << "Accepting muon with energy " << roi.EnergyDeposit() << std::endl;
	}
      }
      // use track for moun replacement? // covers proton, muon, pion
      // else if(roi.Shape() == kShapeTrack && roi.EnergyDeposit() < _track_min_energy) {

      // 	LARCV_INFO() << "Ignoring TRACK (PdgCode=" << roi.PdgCode() << ") with energy " << roi.EnergyDeposit() << std::endl;
      // 	continue;

      // }

      //LARCV_INFO() << "Counting particle (PdgCode=" << roi.PdgCode() << ") with energy " << roi.EnergyDeposit() << std::endl;

      //++part_ctr;

    }

    bool keep = (has_muon && (has_proton || has_shower) && fv_vertex );
    if ( keep ) {
      LARCV_INFO() << "Keeping Event" << std::endl;
    }
    else {
      LARCV_INFO() << "Reject Event (mu=" << has_muon << ", proton=" << has_proton << ", shower=" << has_shower << ", vtx=" << fv_vertex << ")" << std::endl;
    }

    return keep;
  }

  void CCIncFilter::finalize()
  {}

}
#endif
