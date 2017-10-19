#ifndef __SEGMENTFILTER_CXX__
#define __SEGMENTFILTER_CXX__

#include "SegmentFilter.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/UtilFunc.h"

namespace larcv {

  static SegmentFilterProcessFactory __global_SegmentFilterProcessFactory__;

  SegmentFilter::SegmentFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void SegmentFilter::configure(const PSet& cfg)
  {
    auto allowed_par_v = cfg.get<std::vector<std::string> >("Allowed");
    _seg_producer = cfg.get<std::string>("SegProducer");
    
    _allowed_segment_v.resize((size_t)kROITypeMax,false);
    
    for(const auto& par : allowed_par_v) {
      auto roi_type = String2ROIType(par);
      _allowed_segment_v[(size_t)roi_type] = true;
    }

    _allowed_segment_v.front() = true;
  }

  void SegmentFilter::initialize()
  {}

  bool SegmentFilter::process(IOManager& mgr)
  {
    auto ev_seg = (EventImage2D*)mgr.get_data(kProductImage2D,_seg_producer);

    for(size_t plane=0; plane<3; ++plane) {
      for(auto px : ev_seg->Image2DArray()[plane].as_vector()) {
	if (!_allowed_segment_v[(size_t)px]) {
	  LARCV_DEBUG() << "... saw px value for px="<<(int)px<< " a " << ROIType2String((ROIType_t)px) << " @ entry= " << mgr.current_entry() << std::endl;
	  return false;
	}
      }
    }
    
    return true;
  }

  void SegmentFilter::finalize()
  {}

}
#endif
