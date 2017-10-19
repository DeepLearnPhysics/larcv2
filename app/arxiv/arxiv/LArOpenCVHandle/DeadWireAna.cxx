#ifndef __DEADWIREANA_CXX__
#define __DEADWIREANA_CXX__

#include "DeadWireAna.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventROI.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/LArProperties.h"

namespace larcv {

  static DeadWireAnaProcessFactory __global_DeadWireAnaProcessFactory__;

  DeadWireAna::DeadWireAna(const std::string name)
    : ProcessBase(name),
      _tree(nullptr)
  {}
    
  void DeadWireAna::configure(const PSet& cfg)
  {
    _ev_img2d_prod  = cfg.get<std::string>("DeadWireImageProducer"); 
    _seg_roi_prod   = cfg.get<std::string>("SegmentROIProducer");
    _d_dead         = cfg.get<float>("DistanceToDead");
  }

  void DeadWireAna::initialize()
  {
    _tree = new TTree("EventDeadWireTree","");

    _tree->Branch("run"    , &_run    , "run/I");
    _tree->Branch("subrun" , &_subrun , "subrun/I");
    _tree->Branch("event"  , &_event  , "event/I");
    _tree->Branch("entry"  , &_entry  , "entry/I");

    _tree->Branch("vertex_in_dead_plane0", &_vertex_in_dead_plane0, "vertex_in_dead_plane0/I");
    _tree->Branch("vertex_in_dead_plane1", &_vertex_in_dead_plane1, "vertex_in_dead_plane1/I");
    _tree->Branch("vertex_in_dead_plane2", &_vertex_in_dead_plane2, "vertex_in_dead_plane2/I");

    _tree->Branch("vertex_near_dead_plane0", &_vertex_near_dead_plane0, "vertex_near_dead_plane0/I");
    _tree->Branch("vertex_near_dead_plane1", &_vertex_near_dead_plane1, "vertex_near_dead_plane1/I");
    _tree->Branch("vertex_near_dead_plane2", &_vertex_near_dead_plane2, "vertex_near_dead_plane2/I");
    
    _tree->Branch("nearest_wire_error", &_nearest_wire_error, "_nearest_wire_error/I");

    _tree->Branch("d_dead", &_d_dead, "d_dead/F");

  }

  bool DeadWireAna::process(IOManager& mgr)
  {

    auto dead_img = (EventImage2D*) mgr.get_data(kProductImage2D,_ev_img2d_prod);
    if (dead_img->Image2DArray().size() != 3) throw larbys("Dead wire image does not have 3 planes");

    auto seg_roi  = (EventROI*)     mgr.get_data(kProductROI,_seg_roi_prod);

    _run    = (int) seg_roi->run();
    _subrun = (int) seg_roi->subrun();
    _event  = (int) seg_roi->event();
    _entry  = (int) mgr.current_entry();

    const auto& nu_roi = seg_roi->ROIArray().front();
    
    LARCV_DEBUG() << "PDG: " << nu_roi.PdgCode() << std::endl;
    auto tx = nu_roi.X();
    auto ty = nu_roi.Y();
    auto tz = nu_roi.Z();
    auto tt = nu_roi.T();

    auto const offset = _sce.GetPosOffsets(tx,ty,tz);
    
    auto scex = tx - offset[0] + 0.7;
    auto scey = ty + offset[1];
    auto scez = tz + offset[2];
    
    LARCV_DEBUG() << "(x,y,z,t)==>sce(x,y,z,t) : ("<<tx<<","<<ty<<","<<tz<<","<<tt<<")==>("<<scex<<","<<scey<<","<<scez<<","<<tt<<")"<<std::endl;

    auto geo  = larutil::Geometry::GetME();
    auto larp = larutil::LArProperties::GetME();
    double xyz[3];
    double wire_v[3];

    const double tick = (scex / larp->DriftVelocity() + 4) * 2. + 3200.;
      
    xyz[0] = scex;
    xyz[1] = scey;
    xyz[2] = scez;
    _nearest_wire_error = 0;
    try {
      wire_v[0] = geo->NearestWire(xyz,0);
      wire_v[1] = geo->NearestWire(xyz,1);
      wire_v[2] = geo->NearestWire(xyz,2);
    } catch(const std::exception& e) {
      _tree->Fill();
      LARCV_CRITICAL() << "Could not find nearest wire" << std::endl;
      _nearest_wire_error=1;
      return true;
    }
    
    for(size_t plane=0; plane<3; ++plane) {
      LARCV_DEBUG() << "@plane="<<plane<<std::endl;
      
      const auto& img = dead_img->Image2DArray().at(plane);
      const auto& bb  = img.meta();

      auto const& wire = wire_v[plane];

      float xpixel = kINVALID_FLOAT;
      float ypixel = kINVALID_FLOAT;
      
      if( bb.min_x() <= wire && wire <= bb.max_x() &&
	  bb.min_y() <= tick && tick <= bb.max_y() ) {

	xpixel = wire - bb.min_x();
	ypixel = (tick - bb.min_y()) / 6.0;
	
	LARCV_DEBUG() << "Set (xpixel,ypixel)=("<<xpixel<<","<<ypixel<<")"<<std::endl;
      } else {
	throw larbys("Vertex outside image");
      }
      
      int in_dead = 0;
      int near_dead = 0;
      
      
      if (img.pixel((int)ypixel,(int)xpixel) == 0.0) in_dead = 1;
      
      int col_min = kINVALID_INT;
      int col_max = kINVALID_INT;

      col_min = xpixel - _d_dead;
      col_max = xpixel + _d_dead;

      if (col_max >= bb.cols()) col_max = bb.cols() - 1;
      if (col_min <= 0        ) col_min = 0;
      
      for(int col = col_min; col <= col_max; ++col) {
	if (img.pixel(ypixel,col) == 0.0)  {
	  near_dead = 1;
	}
      }

      if (plane==0) {
	_vertex_in_dead_plane0   = in_dead;
	_vertex_near_dead_plane0 = near_dead;
      }
      if (plane==1) {
	_vertex_in_dead_plane1   = in_dead;
	_vertex_near_dead_plane1 = near_dead;
      }
      if (plane==2) {
	_vertex_in_dead_plane2   = in_dead;
	_vertex_near_dead_plane2 = near_dead;
      }
    } // end plane
    
    _tree->Fill();
    return true;
  }

  void DeadWireAna::finalize()
  {
    if(has_ana_file()) {
      _tree->Write();
    }
  }

}
#endif
