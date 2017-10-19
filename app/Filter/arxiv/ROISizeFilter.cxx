#ifndef __ROISIZEFILTER_CXX__
#define __ROISIZEFILTER_CXX__

#include "ROISizeFilter.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  static ROISizeFilterProcessFactory __global_ROISizeFilterProcessFactory__;

  ROISizeFilter::ROISizeFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void ROISizeFilter::configure(const PSet& cfg)
  {
    _roi_producer = cfg.get<std::string>("ROIProducer");
    _min_height   = cfg.get<std::vector<double> >("MinHeight");
    _min_width    = cfg.get<std::vector<double> >("MinWidth");
    _min_area     = cfg.get<std::vector<double> >("MinArea");
    if(_min_height.size() != _min_width.size() || _min_height.size() != _min_area.size()) {
      LARCV_CRITICAL() << "Configuration parameter arrays has a length mis-match!" << std::endl;
      throw larbys();
    }
  }

  void ROISizeFilter::initialize()
  {}

  bool ROISizeFilter::process(IOManager& mgr)
  {
    auto const ev_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));

    auto const& roi_v = ev_roi->ROIArray();

    for(auto const& roi : roi_v) {

      if(roi.MCSTIndex() != kINVALID_USHORT) continue;

      auto const& bb_v = roi.BB();

      if(bb_v.size() != _min_height.size()) {
	if(bb_v.empty()) {
	  LARCV_INFO() << "Skipping an event with empty images..." << std::endl;
	  return false;
	}
	LARCV_CRITICAL() << "# planes in data is " << bb_v.size()
			 << " while configuration is " << _min_height.size() << "!\n";
	throw larbys();
      }

      for(size_t i=0; i<_min_height.size(); ++i) {

	auto const& bb = bb_v[i];

	bool decision = true;

	if(bb.height() < _min_height[i]) decision = false;
	if(bb.width() < _min_width[i]) decision = false;
	if( (bb.height() * bb.width()) < _min_area[i] ) decision = false;

	LARCV_INFO() << "Store? " << (decision ? "yes ... " : "no ... ") << bb.dump();

      }
    }
    return true;
  }

  void ROISizeFilter::finalize()
  {}

}
#endif
