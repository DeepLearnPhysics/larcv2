#ifndef __MCROIHACK_CXX__
#define __MCROIHACK_CXX__

#include "MCROIHack.h"
#include "DataFormat/EventROI.h"

namespace larcv {

  static MCROIHackProcessFactory __global_MCROIHackProcessFactory__;

  MCROIHack::MCROIHack(const std::string name)
    : ProcessBase(name)
  {}
    
  void MCROIHack::configure(const PSet& cfg)
  {
    _roi_producer = cfg.get<std::string>("ROIProducer");
  }

  void MCROIHack::initialize()
  {
    _num_processed = _num_zero_roi = _num_more_roi = _num_skipped = 0;
  }

  bool MCROIHack::process(IOManager& mgr)
  {

    // Get input ROI data
    auto event_roi = (EventROI*)(mgr.get_data(kProductROI,_roi_producer));
    if(!event_roi) {
      LARCV_CRITICAL() << _roi_producer << " not found in input!" << std::endl;
      throw larbys();
    }
    ++_num_processed;

    if(event_roi->ROIArray().size() < 1) ++_num_zero_roi;
    if(event_roi->ROIArray().size() > 1) ++_num_more_roi;

    // We'll make a new ROI!
    std::vector<larcv::ROI> roi_v;

    for(auto const& roi : event_roi->ROIArray()) {
      if(roi.TrackID() != roi.ParentTrackID()) continue;
      roi_v.push_back(roi);
      roi_v.back().MCTIndex(0);
      roi_v.back().MCSTIndex(kINVALID_USHORT);
    }
    if(roi_v.size() != 1) {
      ++_num_skipped;
      return false;
    }
    
    event_roi->Emplace(std::move(roi_v));

    return true;
  }

  void MCROIHack::finalize()
  {
    LARCV_NORMAL() << "Processed " << _num_processed << " events, skipped " << _num_skipped << " events" << std::endl;
    LARCV_NORMAL() << "Found " << _num_zero_roi << " events with 0 ROI, and " << _num_more_roi << " events with >1 ROI" << std::endl;
  }

}
#endif
