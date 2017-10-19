#ifndef __MCFILTER_CXX__
#define __MCFILTER_CXX__

#include "MCFilter.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  static MCFilterProcessFactory __global_MCFilterProcessFactory__;

  MCFilter::MCFilter(const std::string name)
    : ProcessBase(name)
  {}
    
  void MCFilter::configure(const PSet& cfg)
  {
    _select_ccqe  = cfg.get<bool>("SelectCCQE");
    _select_cc    = cfg.get<bool>("SelectCC");
    _roi_producer = cfg.get<std::string>("ROIProducer");
  }

  void MCFilter::initialize()
  {}

  bool MCFilter::process(IOManager& mgr)
  {
    if(!_roi_producer.empty()) {
      bool ccqe_found=false;
      bool cc_found=false;
      auto ev_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
      for(auto const& roi : ev_roi->ROIArray()) {
	if(roi.MCSTIndex() != kINVALID_USHORT) continue;
	if(roi.NuInteractionType() == 1001) ccqe_found = true;
	if(roi.NuCurrentType() == 0) cc_found = true;
      }
      if(cc_found && _select_cc) return true;
      if(ccqe_found && _select_ccqe) return true;
    }
    return false;
  }

  void MCFilter::finalize()
  {}

}
#endif
