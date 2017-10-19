#ifndef __GOODCROIFILTER_CXX__
#define __GOODCROIFILTER_CXX__

#include "GoodcROIFilter.h"


#include "LArUtil/Geometry.h"
#include "LArUtil/LArProperties.h"

namespace larcv {

  static GoodcROIFilterProcessFactory __global_GoodcROIFilterProcessFactory__;

  GoodcROIFilter::GoodcROIFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void GoodcROIFilter::configure(const PSet& cfg)
  {
    _true_roi_producer = cfg.get<std::string>("TrueROIProducer");
    _reco_roi_producer = cfg.get<std::string>("RecoROIProducer");
  }

  void GoodcROIFilter::initialize()
  {}

  bool GoodcROIFilter::process(IOManager& mgr)
  {
    EventROI* ev_croi = nullptr;
    if (!_reco_roi_producer.empty()) {
      ev_croi = (EventROI*)(mgr.get_data(kProductROI,_reco_roi_producer));
      if (!ev_croi) throw larbys("Invalid reco ROI producer provided");
      if (ev_croi->ROIArray().empty()) return false;
    }

    EventROI* ev_roi = nullptr;
    if (!_true_roi_producer.empty()) {
      ev_roi = (EventROI*)(mgr.get_data(kProductROI,_true_roi_producer));
      if (!ev_roi) throw larbys("Invalid true ROI producer provided");
      if (ev_roi->ROIArray().empty()) throw larbys("Invalid true ROI producer provided");
    }
    
    double tx,ty,tz,tt,te;
    double scex,scey,scez;
    
    tx = ty = tz = tt = te = -1.;
    scex = scey = scez = -1.;
    auto geo = larutil::Geometry::GetME();
    auto larp = larutil::LArProperties::GetME();
    double xyz[3];
    double wire_v[3];

    bool nu_found = false;
    for(auto const& roi : ev_roi->ROIArray()){
      if(std::abs(roi.PdgCode()) == 12 || std::abs(roi.PdgCode()) == 14) {
	tx = roi.X();
	ty = roi.Y();
	tz = roi.Z();
	tt = roi.T();
	te = roi.EnergyInit();
	auto const offset = _sce.GetPosOffsets(tx,ty,tz);
	scex = tx - offset[0] + 0.7;
	scey = ty + offset[1];
	scez = tz + offset[2];
	nu_found = true;
	break;
      }
    }

    if (!nu_found) throw larbys("No neutrino in this event");

    xyz[0] = scex;
    xyz[1] = scey;
    xyz[2] = scez;

    try {
      wire_v[0] = geo->NearestWire(xyz,0);
      wire_v[1] = geo->NearestWire(xyz,1);
      wire_v[2] = geo->NearestWire(xyz,2);
    } catch(const std::exception& e) {
      LARCV_WARNING()<<"Cannot find nearest wire for ("<<xyz[0]<<","<<xyz[1]<<","<<xyz[2]<<")"<<std::endl;
      wire_v[0] = wire_v[1] = wire_v[2] = kINVALID_DOUBLE;
      return false;
    }

    const double tick = (scex / larp->DriftVelocity() + 4) * 2. + 3200.;

    for(auto const& croi : ev_croi->ROIArray()) {
      auto const& bb_v = croi.BB();

      size_t this_good_croi_ctr = 0;

      for(size_t plane=0; plane<bb_v.size(); ++plane) {
	auto const& croi_meta = bb_v[plane];
	auto const& wire = wire_v[plane];
	if( croi_meta.min_x() <= wire && wire <= croi_meta.max_x() &&
	    croi_meta.min_y() <= tick && tick <= croi_meta.max_y() ) {
	  ++this_good_croi_ctr;
	}
      }

      if (this_good_croi_ctr >= 2) return true;
	
    } // end check of true vertex in cROI

    return false;
  }

  void GoodcROIFilter::finalize()
  {}

}
#endif
