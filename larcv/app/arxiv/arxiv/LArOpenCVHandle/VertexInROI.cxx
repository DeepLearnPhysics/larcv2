#ifndef __VERTEXINROI_CXX__
#define __VERTEXINROI_CXX__

#include "VertexInROI.h"
#include "DataFormat/EventROI.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/LArProperties.h"

namespace larcv {

  static VertexInROIProcessFactory __global_VertexInROIProcessFactory__;

  VertexInROI::VertexInROI(const std::string name)
    : ProcessBase(name)
  {}
    
  void VertexInROI::configure(const PSet& cfg)
  {
    _truth_roi_producer      = cfg.get<std::string>("TruthROIProducer");
    _input_roi_producer      = cfg.get<std::string>("InputROIProducer");
    _output_roi_producer     = cfg.get<std::string>("OutputROIProducer");
    _planes_inside_threshold = cfg.get<uint>("NPlanesInside",2);
    _croi_idx                = cfg.get<int>("CROI_IDX",-1);
  }

  void VertexInROI::initialize()
  {}

  bool VertexInROI::process(IOManager& mgr)
  {
    LARCV_DEBUG() << "start" << std::endl;
    EventROI* ev_roi_v = nullptr;
    if (!_truth_roi_producer.empty())
      ev_roi_v       = (EventROI*)(mgr.get_data(kProductROI,_truth_roi_producer));

    auto ev_croi_v      = (EventROI*)(mgr.get_data(kProductROI,_input_roi_producer));
    auto ev_croi_true_v = (EventROI*)(mgr.get_data(kProductROI,_output_roi_producer));

    LARCV_DEBUG() << "_croi_idx=" << _croi_idx << std::endl;
    // Do you want a certain croi?
    if (_croi_idx >= 0) {
      LARCV_DEBUG() << "Requested CROI @ index " << _croi_idx << std::endl;

      if (_croi_idx >= ev_croi_v->ROIArray().size() ) {
	LARCV_WARNING() << "Requested ROI is out of range @ entry " << mgr.current_entry() << std::endl;
	LARCV_WARNING() << "SKIP!" << std::endl;
	return false;
      }
	
      ev_croi_true_v->clear();
      ev_croi_true_v->Append(ev_croi_v->ROIArray().at(_croi_idx));
      LARCV_DEBUG() << "end" << std::endl;
      return true;
    }

    
    if (ev_roi_v) { 
      LARCV_DEBUG() << "Checking true vertex against " << ev_roi_v->ROIArray().size() << " cROI in file" << std::endl;
      float tx, ty, tz;
      tx = ty = tz = -1.0*kINVALID_FLOAT;
      float scex, scey, scez;
      scex = scey = scez = -1.0*kINVALID_FLOAT;

      for(auto const& roi : ev_roi_v->ROIArray()){
	if(roi.PdgCode() == 12 || roi.PdgCode() == 14) {
	  tx = roi.X();
	  ty = roi.Y();
	  tz = roi.Z();
	  LARCV_DEBUG() << "(tx,ty,tz)=("<<tx<<","<<ty<<","<<tz<<")"<<std::endl;
	  auto const offset = _sce.GetPosOffsets(tx,ty,tz);
	  scex = tx - offset[0] + 0.7;
	  scey = ty + offset[1];
	  scez = tz + offset[2];
	}
      }

      if (scex == -1 or scey == -1 or scez == -1) return false;
    
      double xyz[3];
      xyz[0] = scex;
      xyz[1] = scey;
      xyz[2] = scez;

      auto geo = larutil::Geometry::GetME();
      auto larp = larutil::LArProperties::GetME();
      double wire_v[3];
      wire_v[0] = geo->NearestWire(xyz,0);
      wire_v[1] = geo->NearestWire(xyz,1);
      wire_v[2] = geo->NearestWire(xyz,2);
      const double tick = (scex / larp->DriftVelocity() + 4) * 2. + 3200.;

      bool one_good = false;
    
      for(size_t croi_idx = 0; croi_idx < ev_croi_v->ROIArray().size(); ++croi_idx) { 
	uint good_croi0 = 0;
	uint good_croi1 = 0;
	uint good_croi2 = 0;
      
	auto const& croi = ev_croi_v->ROIArray().at(croi_idx);
	auto const& bb_v = croi.BB();
	for(size_t plane=0; plane<bb_v.size(); ++plane) {
	  auto const& croi_meta = bb_v[plane];
	  auto const& wire = wire_v[plane];
	  if( croi_meta.min_x() <= wire && wire <= croi_meta.max_x() &&
	      croi_meta.min_y() <= tick && tick <= croi_meta.max_y() ) {
	    if(plane == 0) good_croi0 = 1;
	    if(plane == 1) good_croi1 = 1;
	    if(plane == 2) good_croi2 = 1;
	  }
	}
      
	uint good_croi = good_croi0 + good_croi1 + good_croi2;

	LARCV_DEBUG() << "Good cROI counter is " << good_croi << " as index " << croi_idx << std::endl;
      
	if (good_croi>=_planes_inside_threshold)  {
	  one_good = true;
	  ev_croi_true_v->Append(croi);
	}
      
      }

      if (!one_good) {
	LARCV_DEBUG() << "No good cROI found" << std::endl;
	return false;
      }

    }
    LARCV_DEBUG() << "Returned " << ev_croi_true_v->ROIArray().size() << " good cROI" << std::endl;
    LARCV_DEBUG() << "end" << std::endl;
    return true;
  }

  void VertexInROI::finalize() {}

}
#endif
