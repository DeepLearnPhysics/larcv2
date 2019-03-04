#ifndef __BLANKIMAGE_CXX__
#define __BLANKIMAGE_CXX__

#include "BlankImage.h"
#include "DataFormat/EventImage2D.h"

namespace larcv {

  static BlankImageProcessFactory __global_BlankImageProcessFactory__;

  BlankImage::BlankImage(const std::string name)
    : ProcessBase(name)
  {}
    
  void BlankImage::configure(const PSet& cfg)
  {
    _tpc_image_producer = cfg.get<std::string>("TPCImageProducer");
    _out_image_producer = cfg.get<std::string>("OutImageProducer");
    _pix_value          = cfg.get<float>("PixValue",1.0);
    
    LARCV_INFO() << "Configured" << std::endl;
  }

  void BlankImage::initialize()
  {}

  bool BlankImage::process(IOManager& mgr)
  {
    LARCV_INFO() << "Reading-in Image2D " << _tpc_image_producer << std::endl;
    auto event_tpc_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_tpc_image_producer));  
    auto event_out_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_out_image_producer));  
    
    LARCV_INFO() << "Found number of Image2D " << event_tpc_image->Image2DArray().size() << "\n";
    for(auto const& img : event_tpc_image->Image2DArray()) {
      Image2D copy_img(img.meta());
      copy_img.paint(_pix_value);
      event_out_image->Emplace(std::move(copy_img));
      LARCV_DEBUG() << "Emplace " << event_out_image->Image2DArray().size() << std::endl;
    }
    LARCV_INFO() << "Wrote " << event_out_image->Image2DArray().size() << std::endl;
    return true;
  }

  void BlankImage::finalize()
  {}

}
#endif
