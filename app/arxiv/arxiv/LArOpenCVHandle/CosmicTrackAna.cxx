#ifndef __COSMICTRACKANA_CXX__
#define __COSMICTRACKANA_CXX__

#include "CosmicTrackAna.h"
#include "DataFormat/EventPixel2D.h"
#include <cassert>

namespace larcv {

  static CosmicTrackAnaProcessFactory __global_CosmicTrackAnaProcessFactory__;

  CosmicTrackAna::CosmicTrackAna(const std::string name)
    : ProcessBase(name), _tree(nullptr)
  {}
    
  void CosmicTrackAna::configure(const PSet& cfg)
  {
    //
    // ThruMu and StopMu producers (Pixel2D)
    //
    _thrumu_px_prod = cfg.get<std::string>("ThruMuProducer");
    _stopmu_px_prod = cfg.get<std::string>("StopMuProducer");  
    
    //
    // End points producers (Pixel2D)
    //
    _top_px_prod = cfg.get<std::string>("TopPxProducer");
    _bot_px_prod = cfg.get<std::string>("BotPxProducer");  

    _up_px_prod   = cfg.get<std::string>("UpPxProducer");
    _down_px_prod = cfg.get<std::string>("DownPxProducer");  

    _anode_px_prod   = cfg.get<std::string>("AnodePxProducer");
    _cathode_px_prod = cfg.get<std::string>("CathodePxProducer");  

  }

  void CosmicTrackAna::initialize()
  {
    
    //
    // RSEE
    //

    _tree = new TTree("EventCosmicTrackTree","");
    _tree->Branch("run",&_run,"run/I");
    _tree->Branch("subrun",&_subrun,"subrun/I");
    _tree->Branch("event",&_event,"event/I");
    _tree->Branch("entry",&_entry,"entry/I");
    
    //
    // ThruMu & StopMu
    //
    _tree->Branch("n_thru_mu_trk", &_n_thru_mu_trk,"n_thru_mu_trk/I");
    _tree->Branch("n_thru_mu_pix_p0", &_n_thru_mu_pix_p0,"n_thru_mu_pix_p0/I");
    _tree->Branch("n_thru_mu_pix_p1", &_n_thru_mu_pix_p1,"n_thru_mu_pix_p1/I");
    _tree->Branch("n_thru_mu_pix_p2", &_n_thru_mu_pix_p2,"n_thru_mu_pix_p2/I");
    _tree->Branch("avg_thru_mu_pix",  &_avg_thru_mu_pix,"avg_thru_mu_pix/F");


    _tree->Branch("n_stop_mu_trk",    &_n_stop_mu_trk,"n_stop_mu_trk/I");
    _tree->Branch("n_stop_mu_pix_p0", &_n_stop_mu_pix_p0,"n_stop_mu_pix_p0/I");
    _tree->Branch("n_stop_mu_pix_p1", &_n_stop_mu_pix_p1,"n_stop_mu_pix_p1/I");
    _tree->Branch("n_stop_mu_pix_p2", &_n_stop_mu_pix_p2,"n_stop_mu_pix_p2/I");
    _tree->Branch("avg_stop_mu_pix",  &_avg_stop_mu_pix,"avg_stop_mu_pix/F");

    //
    // End points
    //
    _tree->Branch("n_top_pts", &_n_top_pts , "n_top_pts/I");
    _tree->Branch("n_bot_pts", &_n_bot_pts , "n_bot_pts/I");
    _tree->Branch("n_up_pts"  , &_n_up_pts   , "n_up_pts/I");
    _tree->Branch("n_down_pts", &_n_down_pts , "n_down_pts/I");
    _tree->Branch("n_anode_pts"  , &_n_anode_pts   , "n_anode_pts/I");
    _tree->Branch("n_cathode_pts", &_n_cathode_pts , "n_cathode_pts/I");

  }

  bool CosmicTrackAna::process(IOManager& mgr)
  {
    
    LARCV_DEBUG() << "start" << std::endl;
    
    clear();

    auto const ev_thrumu = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_thrumu_px_prod));
    if (!ev_thrumu) throw larbys("Invalid ThruMu producer provided!");
    
    auto const ev_stopmu = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_stopmu_px_prod));
    if (!ev_stopmu) throw larbys("Invalid StopMu producer provided!");

    auto const ev_top_pts = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_top_px_prod));
    if (!ev_top_pts) throw larbys("Invalid top_pts producer provided!");

    auto const ev_bot_pts = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_bot_px_prod));
    if (!ev_bot_pts) throw larbys("Invalid bot_pts producer provided!");

    auto const ev_up_pts = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_up_px_prod));
    if (!ev_up_pts) throw larbys("Invalid up_pts producer provided!");

    auto const ev_down_pts = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_down_px_prod));
    if (!ev_down_pts) throw larbys("Invalid down_pts producer provided!");

    auto const ev_cathode_pts = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_cathode_px_prod));
    if (!ev_cathode_pts) throw larbys("Invalid cathode_pts producer provided!");

    auto const ev_anode_pts = (EventPixel2D*)(mgr.get_data(kProductPixel2D,_anode_px_prod));
    if (!ev_anode_pts) throw larbys("Invalid anode_pts producer provided!");

    _run    = (int) ev_thrumu->run();
    _subrun = (int) ev_thrumu->subrun();
    _event  = (int) ev_thrumu->event();
    _entry  = (int) mgr.current_entry();


    //
    // Count the number of points @ each side
    //
    assert(ev_top_pts->Pixel2DArray().size());

    _n_top_pts     = (int) ev_top_pts->Pixel2DArray(0).size();
    _n_bot_pts     = (int) ev_bot_pts->Pixel2DArray(0).size();
    _n_up_pts      = (int) ev_up_pts->Pixel2DArray(0).size();
    _n_down_pts    = (int) ev_down_pts->Pixel2DArray(0).size();
    _n_anode_pts   = (int) ev_anode_pts->Pixel2DArray(0).size();
    _n_cathode_pts = (int) ev_cathode_pts->Pixel2DArray(0).size();
    
    // 
    // Count the number of stopmu
    // 

    _avg_thru_mu_pix   = 0.0;
    _avg_stop_mu_pix = 0.0;

    for(size_t plane=0; plane<3; ++plane) {

      const auto& thrumu_v = ev_thrumu->Pixel2DClusterArray(plane);
      const auto& stopmu_v = ev_stopmu->Pixel2DClusterArray(plane);

      if (plane==0) {
	_n_thru_mu_trk = (int)thrumu_v.size();
	_n_stop_mu_trk = (int)stopmu_v.size();
	
	_n_thru_mu_pix_p0 = 0;
	for(const auto& v : thrumu_v) _n_thru_mu_pix_p0 += (int)v.size();

	_n_stop_mu_pix_p0 = 0;
	for(const auto& v : stopmu_v) _n_stop_mu_pix_p0 += (int)v.size();

	_avg_thru_mu_pix  += _n_thru_mu_pix_p0;
	_avg_stop_mu_pix  += _n_stop_mu_pix_p0;
      }


      if (plane==1) {
	_n_thru_mu_pix_p1 = 0;
	for(const auto& v : thrumu_v) _n_thru_mu_pix_p1 += (int)v.size();

	_n_stop_mu_pix_p1 = 0;
	for(const auto& v : stopmu_v) _n_stop_mu_pix_p1 += (int)v.size();

	_avg_thru_mu_pix  += _n_thru_mu_pix_p1;
	_avg_stop_mu_pix  += _n_stop_mu_pix_p1;
      }


      if (plane==1) {
	_n_thru_mu_pix_p2 = 0;
	for(const auto& v : thrumu_v) _n_thru_mu_pix_p2 += (int)v.size();

	_n_stop_mu_pix_p2 = 0;
	for(const auto& v : stopmu_v) _n_stop_mu_pix_p2 += (int)v.size();

	_avg_thru_mu_pix  += _n_thru_mu_pix_p2;
	_avg_stop_mu_pix  += _n_stop_mu_pix_p2;
      }
    }

    _avg_thru_mu_pix /= 3.0;
    _avg_stop_mu_pix /= 3.0;

    _tree->Fill();
    LARCV_DEBUG() << "end" << std::endl;
    return true;
  }

  void CosmicTrackAna::finalize()
  { _tree->Write(); }

  void CosmicTrackAna::clear()  {

    _run=kINVALID_INT;
    _subrun=kINVALID_INT;
    _event=kINVALID_INT;
    _entry=kINVALID_INT;

    _n_thru_mu_trk=kINVALID_INT;
    _n_thru_mu_pix_p0=kINVALID_INT;
    _n_thru_mu_pix_p1=kINVALID_INT;
    _n_thru_mu_pix_p2=kINVALID_INT;
    _avg_thru_mu_pix=kINVALID_INT;

    _n_stop_mu_trk=kINVALID_INT;
    _n_stop_mu_pix_p0=kINVALID_INT;
    _n_stop_mu_pix_p1=kINVALID_INT;
    _n_stop_mu_pix_p2=kINVALID_INT;
    _avg_stop_mu_pix=kINVALID_INT;

    _n_top_pts=kINVALID_INT;
    _n_bot_pts=kINVALID_INT;
    _n_up_pts=kINVALID_INT;
    _n_down_pts=kINVALID_INT;
    _n_anode_pts=kINVALID_INT;
    _n_cathode_pts=kINVALID_INT;

  }


}
#endif
