#ifndef __UNIONROI_CXX__
#define __UNIONROI_CXX__

#include "UnionROI.h"

namespace larcv {

  static UnionROIProcessFactory __global_UnionROIProcessFactory__;

  UnionROI::UnionROI(const std::string name)
    : ProcessBase(name)
  {}
    
  void UnionROI::configure(const PSet& cfg)
  {
    _in_roi_producer = cfg.get<std::string>("InProducer");
    _out_roi_producer = cfg.get<std::string>("OutProducer");
  }

  bool UnionROI::process(IOManager& mgr)
  {

    const auto in_evroi = (EventROI*)mgr.get_data(kProductROI,_in_roi_producer);
    if(!in_evroi) {
      LARCV_CRITICAL() << "Could not get input ROI producer " << _in_roi_producer << std::endl;
      throw larbys();
    }

    const auto& roi_v = in_evroi->ROIArray();

    LARCV_DEBUG() << "Read in " << roi_v.size() << " ROIs from producer " << _in_roi_producer << std::endl;
    auto union_roi = Union(roi_v);

    if (union_roi.BB().size()<3) return false;

    auto out_evroi = (EventROI*)mgr.get_data(kProductROI,_out_roi_producer);
    out_evroi->Emplace(std::move(union_roi));
    LARCV_DEBUG() << "Stored " << out_evroi->ROIArray().size() << " ROIs from producer " << _out_roi_producer << std::endl;

    return true;
  }
  
  ROI UnionROI::Union(const std::vector<ROI>& roi_v) {
    
    LARCV_DEBUG() << "start: got " << roi_v.size() << " rois" << std::endl;
    std::vector<ImageMeta> union_bb_v(3);

    if(roi_v.front().BB().size()<3) {
      LARCV_CRITICAL() << "First ROI only has " << roi_v.front().BB().size() << " planes" << std::endl;
      return ROI();
    }
    
    for(size_t plane=0; plane<3; ++plane) 
      union_bb_v[plane] = roi_v.front().BB(plane);
    
    bool first = false;
    for(auto const& roi : roi_v) {
	  
      if (!first) { first = true; continue; }

      auto const& bb_v = roi.BB();

      for(size_t plane=0; plane<bb_v.size(); ++plane) {
	auto const& bb = bb_v[plane];
	auto& union_bb = union_bb_v[plane];
	union_bb = union_bb.inclusive(bb);
      }
      
    }

    ROI ret_roi;
    for(size_t plane=0; plane<3; ++plane) 
      ret_roi.AppendBB(union_bb_v.at(plane));

    LARCV_DEBUG() << "end" << std::endl;
    return ret_roi;
  }
  

}
#endif
