#ifndef __SEGMENTMAKER_CXX__
#define __SEGMENTMAKER_CXX__

#include "SegmentMaker.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static SegmentMakerProcessFactory __global_SegmentMakerProcessFactory__;

  SegmentMaker::SegmentMaker(const std::string name)
    : ProcessBase(name)
  {}
    
  void SegmentMaker::configure(const PSet& cfg)
  {
    _tpc_image_producer = cfg.get<std::string>("TPCImageProducer");
    _seg_image_producer = cfg.get<std::string>("SegmentImageProducer");
    _adc_threshold = cfg.get<float>("ADCThreshold");
    
    LARCV_INFO() << "Configured" << std::endl;
  }

  void SegmentMaker::initialize()
  {
    LARCV_INFO() << "Initialized" << std::endl;
  }
  
  bool SegmentMaker::process(IOManager& mgr)
  {

    LARCV_INFO() << "Reading-in TPC Image2D " << _tpc_image_producer << std::endl;
    auto event_tpc_image   = (EventImage2D*)(mgr.get_data(kProductImage2D,_tpc_image_producer));  
    auto event_segment_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_seg_image_producer));  
    
    LARCV_INFO() << "Found number of TPC Image2D " << event_tpc_image->Image2DArray().size() << "\n";
    for(auto const& img : event_tpc_image->Image2DArray()) {
      auto copy_img  = img;
      copy_img.binary_threshold(_adc_threshold,(float)kROIUnknown,(float)kROICosmic);
      event_segment_image->Emplace(std::move(copy_img));
    }

    return true;
  }

  void SegmentMaker::finalize()
  {}

}
#endif
