#ifndef __BADREGIONANA_CXX__
#define __BADREGIONANA_CXX__

#include "BadRegionAna.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include "DataFormat/EventPGraph.h"
#include "LArbysUtils.h"



namespace larcv {

  static BadRegionAnaProcessFactory __global_BadRegionAnaProcessFactory__;

  BadRegionAna::BadRegionAna(const std::string name)
    :
    ProcessBase(name),
    _tree(nullptr)
  {}
    
  void BadRegionAna::configure(const PSet& cfg)
  {
    _pgraph_prod    = cfg.get<std::string>("PGraphProducer");
    _reco_roi_prod  = cfg.get<std::string>("RecoROIProducer");
    _adc_img_prod   = cfg.get<std::string>("ADCImageProducer");

    // clock wise in YZ plane
    geo2d::Vector<float> t0,t1,t2,t3;
    t0=t1=t2=t3=geo2d::Vector<float>(kINVALID_FLOAT,kINVALID_FLOAT);

    std::array<geo2d::Vector<float>, 4 > pts_v;


    //
    // YZ plane
    //

    
    // region 1 (left)
    t0 = geo2d::Vector<float>(  0,-98);
    t1 = geo2d::Vector<float>(  0, 18);
    t2 = geo2d::Vector<float>(167, 111);
    t3 = geo2d::Vector<float>(341, 111);

    pts_v[0] = t0;
    pts_v[1] = t1;
    pts_v[2] = t2;
    pts_v[3] = t3;

    _yz_bound_v.push_back(pts_v);
    
    // region 2 (middle left)
    t0 = geo2d::Vector<float>(134,-111);
    t1 = geo2d::Vector<float>(467, 111);
    t2 = geo2d::Vector<float>(520, 111);
    t3 = geo2d::Vector<float>(192,-111);

    pts_v[0] = t0;
    pts_v[1] = t1;
    pts_v[2] = t2;
    pts_v[3] = t3;

    _yz_bound_v.push_back(pts_v);
    
    // region 3 (up and down)
    t0 = geo2d::Vector<float>(740,-111);
    t1 = geo2d::Vector<float>(700,-111);
    t2 = geo2d::Vector<float>(700, 111);
    t3 = geo2d::Vector<float>(740, 111);

    pts_v[0] = t0;
    pts_v[1] = t1;
    pts_v[2] = t2;
    pts_v[3] = t3;

    _yz_bound_v.push_back(pts_v);

    // region 4 (right down to left)
    t0 = geo2d::Vector<float>(941,-111);
    t1 = geo2d::Vector<float>(887,-111);
    t2 = geo2d::Vector<float>(556, 111);
    t3 = geo2d::Vector<float>(610, 111);

    pts_v[0] = t0;
    pts_v[1] = t1;
    pts_v[2] = t2;
    pts_v[3] = t3;

    _yz_bound_v.push_back(pts_v);

    //
    // XZ plane
    //

    // region 0 (bottom)
    t0 = geo2d::Vector<float>(0   ,50);
    t1 = geo2d::Vector<float>(1037,50);
    t2 = geo2d::Vector<float>(1037,50);
    t3 = geo2d::Vector<float>(0   ,0);

    pts_v[0] = t0;
    pts_v[1] = t1;
    pts_v[2] = t2;
    pts_v[3] = t3;

    _xz_bound_v.push_back(pts_v);
    
    // region 0 (bottom)
    t0 = geo2d::Vector<float>(700,  0);
    t1 = geo2d::Vector<float>(700,256);
    t2 = geo2d::Vector<float>(740,256);
    t3 = geo2d::Vector<float>(740,  0);

    pts_v[0] = t0;
    pts_v[1] = t1;
    pts_v[2] = t2;
    pts_v[3] = t3;

    _xz_bound_v.push_back(pts_v);
    
  }

  void BadRegionAna::initialize()
  {
    _tree = new TTree("BadRegionAna","");
    _tree->Branch("run"   , &_run    , "run/I");
    _tree->Branch("subrun", &_subrun , "subrun/I");
    _tree->Branch("event" , &_event  , "event/I");
    _tree->Branch("entry" , &_entry  , "entry/I");
    _tree->Branch("roid"  , &_roid   , "roid/I");
    _tree->Branch("vtxid" , &_vtxid  , "vtxid/I");

    _tree->Branch("in_region_0"   , &_in_region_0    , "in_region_0/I");
    _tree->Branch("in_region_1"   , &_in_region_1    , "in_region_1/I");
    _tree->Branch("in_region_2"   , &_in_region_2    , "in_region_2/I");
    _tree->Branch("in_region_3"   , &_in_region_3    , "in_region_3/I");
    _tree->Branch("in_region_4"   , &_in_region_4    , "in_region_4/I");
    _tree->Branch("in_region_5"   , &_in_region_5    , "in_region_5/I");

    _tree->Branch("in_region_yz" , &_in_region_yz  , "in_region_yz/I");
    _tree->Branch("in_region_xz" , &_in_region_xz  , "in_region_xz/I");		    
    
    _tree->Branch("in_region" , &_in_region  , "in_region/I");		    

  }

  bool BadRegionAna::process(IOManager& mgr)
  {

    _run    = kINVALID_INT;
    _subrun = kINVALID_INT;
    _event  = kINVALID_INT;
    _entry  = kINVALID_INT;
    _roid   = kINVALID_INT;
    _vtxid  = kINVALID_INT;


    _in_region_0  = kINVALID_INT;
    _in_region_1  = kINVALID_INT;
    _in_region_2  = kINVALID_INT;
    _in_region_3  = kINVALID_INT;
    _in_region_4  = kINVALID_INT;
    _in_region_5  = kINVALID_INT;
    _in_region_yz = kINVALID_INT;
    _in_region_xz = kINVALID_INT;
    _in_region    = kINVALID_INT;    

    
    EventImage2D* ev_img = nullptr;
    if (!_adc_img_prod.empty()) {
      ev_img = (EventImage2D*)(mgr.get_data(kProductImage2D,_adc_img_prod));
      if (!ev_img) throw larbys("Could not get Image2D from producer name");
    } else throw larbys("Specify image producer");
    
    EventPGraph* ev_pgraph = nullptr;
    if (!_pgraph_prod.empty()) {
      ev_pgraph = (EventPGraph*)(mgr.get_data(kProductPGraph,_pgraph_prod));
      if (!ev_pgraph) throw larbys("Could not get PGraph from producer name");
    } else throw larbys("Specify pgraph producer");

    EventROI* ev_croi = nullptr;
    if (!_reco_roi_prod.empty()) {
      ev_croi = (EventROI*)(mgr.get_data(kProductROI,_reco_roi_prod));
      if (!ev_croi) throw larbys("Could not get ROI from producer name");
    } else throw larbys("Specify reco ROI name");


    _run    = ev_pgraph->run();
    _subrun = ev_pgraph->subrun();
    _event  = ev_pgraph->event();
    _entry  = mgr.current_entry();

    LARCV_DEBUG() << "Got RSEE=("<<_run<<","<<_subrun<<","<<_event<<","<<_entry<<")"<<std::endl;

    const auto& adc_img_v = ev_img->Image2DArray();
    
    std::vector<std::vector<ImageMeta> > roid_v;
    roid_v.reserve(ev_croi->ROIArray().size());

    for(auto const& croi : ev_croi->ROIArray()) {
      auto const& bb_v = croi.BB();
      roid_v.push_back(crop_metas(adc_img_v,bb_v));
    }

    _vtxid=-1;
    auto vtx_counts = ev_pgraph->PGraphArray().size();
    LARCV_DEBUG() << "Got " << vtx_counts << " vertices" << std::endl;
    
    for (int vtx_idx = 0; vtx_idx < (int)vtx_counts; ++vtx_idx) {
      _vtxid += 1;

      auto pgraph = ev_pgraph->PGraphArray().at(vtx_idx);

      auto const& roi_v = pgraph.ParticleArray();
      auto const& bb_v  = roi_v.front().BB();

      auto iter = std::find(roid_v.begin(),roid_v.end(),bb_v);
      if (iter == roid_v.end()) throw larbys("Unknown image meta");

      auto roid = iter - roid_v.begin();

      if (roid != _roid) _vtxid = 0;

      _roid  = roid;

      auto const& a_roi = roi_v.front();
      
      auto x = a_roi.X();
      auto y = a_roi.Y();
      auto z = a_roi.Z();

      geo2d::Vector<float> pt(z,y);

      _in_region_0 = InsideRegion(pt,_yz_bound_v[0]);
      _in_region_1 = InsideRegion(pt,_yz_bound_v[1]);
      _in_region_2 = InsideRegion(pt,_yz_bound_v[2]);
      _in_region_3 = InsideRegion(pt,_yz_bound_v[3]);
      
      _in_region_yz = (_in_region_0 or
		       _in_region_1 or
		       _in_region_2 or
		       _in_region_3);

      pt = geo2d::Vector<float>(z,x);
      
      _in_region_4 = InsideRegion(pt,_xz_bound_v[0]);
      _in_region_5 = InsideRegion(pt,_xz_bound_v[1]);
      
      _in_region_xz = (_in_region_4 or
		       _in_region_5);


      _in_region = _in_region_yz or _in_region_xz;
      
      _tree->Fill();
    }

    return true;
  }

  void BadRegionAna::finalize()
  {
    if(has_ana_file()) {
      _tree->Write();
    }
  }

}
#endif
