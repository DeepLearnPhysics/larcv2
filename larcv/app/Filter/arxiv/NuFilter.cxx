#ifndef __NUFILTER_CXX__
#define __NUFILTER_CXX__

#include "NuFilter.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  static NuFilterProcessFactory __global_NuFilterProcessFactory__;

  NuFilter::NuFilter(const std::string name)
    : ProcessBase(name), _event_tree(nullptr)
  {
    _n_calls = 0;
    _n_fail_ccqe =0;
    _n_fail_nupdg = 0;
    _n_fail_nuE = 0;
    _n_fail_lepton_dep = 0;
    _n_fail_proton_dep = 0;
    _n_pass = 0;
    _n_fail_unknowns=0;
    _n_fail_inter=0;
  }
    
  void NuFilter::configure(const PSet& cfg)
  {
    this->set_verbosity((msg::Level_t)cfg.get<uint>("Verbosity",2));
    _nu_pdg = cfg.get<unsigned int>("NuPDG");
    _interaction_mode = cfg.get<int>("InteractionMode");
    _dep_sum_lepton = 0.0;
    _dep_sum_proton = 0.0;
    
    _min_nu_init_e  = cfg.get<double>("MinNuEnergy");
    _max_nu_init_e  = cfg.get<double>("MaxNuEnergy");
    _dep_sum_lepton = cfg.get<double>("MinEDepSumLepton");
    _dep_sum_proton = cfg.get<double>("MinEDepSumProton");
    
    _select_signal     = cfg.get<bool>("SelectSignal");
    _select_background = cfg.get<bool>("SelectBackground");

    _roi_producer_name = cfg.get<std::string>("ROIProducer");

    _event_tree = new TTree("NuFilterTree","");
    _event_tree->Branch("run"      , &_run      , "run/i");
    _event_tree->Branch("subrun"   , &_subrun   , "subrun/i");
    _event_tree->Branch("event"    , &_event    , "event/i");
    _event_tree->Branch("entry"    , &_entry    , "entry/i");
    _event_tree->Branch("selected1L1P" , &_selected , "selected1L1P/i");
  }

  //from rui an.
  bool NuFilter::MCSelect(const EventROI* ev_roi) {

    bool pdg_b = false;
    bool interaction_mode_b = false;
    bool energy_init_b = false;
    bool vis_lepton_b = false;
    bool vis_one_proton_b = false;
    bool vis_no_unknowns_b = false;
    
    uint nlepton  = 0;
    uint nproton  = 0;
    
    uint unknown_ctr=0.0;
    
    // Get neutrino ROI
    auto roi = ev_roi->at(0);
    
    uint pdgcode          = roi.PdgCode();
    uint interaction_mode = roi.NuInteractionType();
    double energy_init    = roi.EnergyInit();

    std::vector<aparticle> protons_v;
    std::vector<aparticle> leptons_v;
    std::vector<aparticle> unknown_v;

    bool first = false;
    
    for(const auto& roi : ev_roi->ROIArray()) {

      if(!first) { first=true; continue; }
      
      int pdgcode = std::abs(roi.PdgCode());
      
      if (pdgcode==2212) {
	aparticle thispro;
	thispro.pdg           = pdgcode;
	thispro.trackid       = roi.TrackID();
	thispro.ptrackid      = roi.ParentTrackID();
	thispro.depeng        = roi.EnergyDeposit();
	thispro.primary       = ( roi.TrackID() == roi.ParentTrackID() );
	protons_v.push_back(std::move(thispro));
      }
      
      else if (pdgcode==11 or pdgcode==13) {
	aparticle thislep;
	thislep.pdg           = pdgcode;
	thislep.trackid       = roi.TrackID();
	thislep.ptrackid      = roi.ParentTrackID();
	thislep.depeng        = roi.EnergyDeposit();
	thislep.primary       = ( roi.TrackID() == roi.ParentTrackID() );
	leptons_v.push_back(std::move(thislep));
      }

      else {
	aparticle thisunknown;
	thisunknown.pdg           = pdgcode;
	thisunknown.trackid       = roi.TrackID();
	thisunknown.ptrackid      = roi.ParentTrackID();
	thisunknown.depeng        = roi.EnergyDeposit();
	thisunknown.primary       = ( roi.TrackID() == roi.ParentTrackID() );
	unknown_v.push_back(std::move(thisunknown));
      }
      
    }
    
    //calculate the visible lepton energy
    int lepton_engs_ctr = 0 ;
    for (size_t p1=0;p1 < leptons_v.size() ; p1++ ){
      const auto& lepton1 = leptons_v[p1];
      if (! lepton1.primary ) continue;
      nlepton+=1;
      float this_lepton_eng = lepton1.depeng;
      for (size_t p2=0;p2 < leptons_v.size() ; p2++ ){
      	if (p1==p2) continue;
      	const auto& lepton2 = leptons_v[p2];
      	if (lepton2.ptrackid != lepton1.trackid) continue;
      	this_lepton_eng+=lepton2.depeng;
      }
      if ( this_lepton_eng > _dep_sum_lepton ) lepton_engs_ctr ++;
    }

    //calculate the visible proton energy
    int proton_engs_ctr = 0 ;
    for (size_t p1=0;p1 < protons_v.size() ; p1++ ){
      const auto& proton1 = protons_v[p1];
      if (! proton1.primary ) continue;
      nproton+=1;
      float this_proton_eng = proton1.depeng;
      for (size_t p2=0;p2 < protons_v.size() ; p2++ ){
	if (p1==p2) continue;
	const auto& proton2 = protons_v[p2];
	if (proton2.ptrackid != proton1.trackid) continue;
	this_proton_eng+=proton2.depeng;
      }
      if ( this_proton_eng > _dep_sum_proton ) proton_engs_ctr ++;
    }


    for(const auto& unknown : unknown_v) {
      if (!unknown.primary) continue;
      unknown_ctr+=1;
    }

    
    // requested nu PDG code
    if (pdgcode == _nu_pdg) {
      pdg_b = true;
    }

    // requested nu interaction mode
    if ((interaction_mode == _interaction_mode) or (_interaction_mode < 0)) {
      interaction_mode_b = true;
    } 

    // neutrino energy range
    if (energy_init >= _min_nu_init_e && energy_init <= _max_nu_init_e) {
      energy_init_b = true;
    }

    // interaction must have visible leptons
    if (lepton_engs_ctr == 1) {
      vis_lepton_b = true;
    }

    // There must be 1 visible proton
    if (proton_engs_ctr == 1) {
      vis_one_proton_b = true;
    }

    // There must be no primary unknowns
    if (unknown_ctr==0) {
      vis_no_unknowns_b = true;
    }
    
    bool selected =
      pdg_b               &&
      interaction_mode_b  &&
      energy_init_b       &&
      vis_lepton_b        &&
      vis_one_proton_b    &&
      vis_no_unknowns_b;
    
    if (! pdg_b)              _n_fail_nupdg      += 1;
    if (! interaction_mode_b) _n_fail_inter      += 1;
    if (! energy_init_b)      _n_fail_nuE        += 1;
    if (! vis_lepton_b)       _n_fail_lepton_dep += 1;
    if (! vis_one_proton_b)   _n_fail_proton_dep += 1;
    if (! vis_no_unknowns_b)  _n_fail_unknowns   += 1;
    
    return selected;
  }

  void NuFilter::initialize()
  {
    LARCV_DEBUG() << "start" << std::endl;
    LARCV_DEBUG() << "end" << std::endl;
  }

  bool NuFilter::process(IOManager& mgr)
  {

    LARCV_DEBUG() << "start" << std::endl;
    _run      = kINVALID_UINT;
    _subrun   = kINVALID_UINT;
    _event    = kINVALID_UINT;
    _entry    = kINVALID_UINT;
    _selected = kINVALID_UINT;
    
    if(_roi_producer_name.empty()) {
      _event_tree->Fill();
      return true;
    }

    auto ev_roi = (EventROI*) mgr.get_data(kProductROI, _roi_producer_name);

    _run    = ev_roi->run();
    _subrun = ev_roi->subrun();
    _event  = ev_roi->event();
    _entry  = mgr.current_entry();
    
    bool signal_selected = MCSelect(ev_roi);
    LARCV_DEBUG() << "selected: " << signal_selected << std::endl;
    _selected = (uint) signal_selected;
    
    // if atleast 1 of config selection is false, then test against signal selected
    if ( !_select_signal or !_select_background) {
      if ( _select_signal     and !signal_selected ) {
	_event_tree->Fill();
	LARCV_DEBUG() << "false" << std::endl;
	return false;
      }
      if ( _select_background and  signal_selected ) {
	_event_tree->Fill();
	LARCV_DEBUG() << "false" << std::endl;
	return false;
      }
    }

    _event_tree->Fill();
    LARCV_DEBUG() << "true" << std::endl;
    LARCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
  void NuFilter::finalize()
  {
    _event_tree->Write();
  }

}
#endif
